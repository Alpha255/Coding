#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"

#include "path_util.h"
#include "reflection.h"

#pragma optimize("", off)

static llvm::cl::OptionCategory MyToolCategory("dumb_meta options");

using namespace clang::tooling;
using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;

static cl::extrahelp MoreHelp("\n-src_root <source path>"
	"\n-single_meta_storage_path [meta file path]");
static cl::opt<std::string> project_src_root("src_root", cl::cat(MyToolCategory));
static cl::opt<std::string> meta_storage_path("single_meta_storage_path", cl::cat(MyToolCategory));


namespace TypeInterface {
struct Type {
  std::string name;
  std::string qualified_name;
  std::string source_loc;// for debug
  int64_t size; // in byte
  uint32_t alignment;
  bool is_container;
};

enum AccessLevel { E_public, E_protected, E_private, E_invalid };
struct Field {
  std::string name;
  std::unique_ptr<Type> type;
  uint64_t offset;
  AccessLevel acc;
};

struct Method {
  std::string name;
  //  std::vector<Type> params; TODO: don't need this time
  bool is_static;
  bool is_ctor;
  AccessLevel acc;
};

struct TypeContainer : public Type {
  TypeContainer() { is_container = true; }
  std::vector<Field> fields;
  std::vector<Method> methods;
};
} // namespace TypeInterface

class ClassMatcher : public MatchFinder::MatchCallback {
private:
  // recursive visitor?

  std::vector<std::unique_ptr<TypeInterface::Type>> &types_;

  bool dump_type(const clang::ASTContext *ctx,
                 const clang::CXXMethodDecl *method,
                 TypeInterface::Method &ret) {

    // I don't count on default ctor/operators
    if (method->isImplicit() || method->isCopyAssignmentOperator() || clang::dyn_cast<CXXDestructorDecl>(method) != nullptr)
      return false;

    ret.name = method->getNameAsString();
    ret.is_static = method->isStatic();
	ret.is_ctor = clang::dyn_cast<CXXConstructorDecl>(method) != nullptr;

    return true;
  }

  TypeInterface::Field dump_type(const clang::ASTContext *ctx,
                                 const clang::DeclaratorDecl *var) {
    TypeInterface::Field field_type{};

    clang::LangOptions langopts{};
    // langopts.CPlusPlus = true;

    clang::PrintingPolicy policy(langopts);
    policy.SuppressTagKeyword = true;

    // strip off typedefs
    // auto ast_type = field->getType().getDesugaredType(*ctx);
    auto qual_type = var->getType().getCanonicalType();
    auto qual_type_no_qualifier = qual_type.getUnqualifiedType();
    auto ast_type = qual_type.getTypePtr();

    TypeInterface::Type *tc = new TypeInterface::Type();
    tc->qualified_name = qual_type_no_qualifier.getAsString(policy);

    policy.SuppressScope = true;
    tc->name = qual_type_no_qualifier.getAsString(policy);
    tc->size = ctx->getTypeSize(qual_type_no_qualifier) / CHAR_BIT;
    tc->alignment = ctx->getTypeAlign(qual_type_no_qualifier) / CHAR_BIT;

    field_type.acc = static_cast<TypeInterface::AccessLevel>(var->getAccess());
    field_type.name = var->getNameAsString();
    field_type.type.reset(tc);

    // field_out.is_const = qual_type.isConstQualified();
    // field_out.is_volatile = qual_type.isVolatileQualified();
    // field_out.is_reference = ast_type->isReferenceType();

    return field_type;
  }

  std::unique_ptr<TypeInterface::Type>
  dump_type(const clang::ASTContext *ctx, const clang::CXXRecordDecl *cls) {
    auto ast_type_cls = cls->getTypeForDecl();
    auto ast_qual_type_cls = ast_type_cls->getCanonicalTypeInternal();

    TypeInterface::TypeContainer *tc = new TypeInterface::TypeContainer();
    tc->name = cls->getNameAsString();
    tc->qualified_name = cls->getQualifiedNameAsString();
    tc->size = ctx->getTypeSize(ast_type_cls) / CHAR_BIT;
    tc->alignment = ctx->getTypeAlign(ast_type_cls) / CHAR_BIT;

	auto loc = ctx->getFullLoc(cls->getBeginLoc());
	if (loc.isValid())
	{
		auto path = loc.getFileLoc().getFileEntry()->tryGetRealPathName();
		tc->source_loc = path;
	}

    for (auto method = cls->method_begin(); method != cls->method_end();
         ++method) {
		if (!check_attr(*method))
			continue;

      TypeInterface::Method m{};
      if (dump_type(ctx, *method, m)) {
        tc->methods.push_back(m);
      }
    }

    const clang::ASTRecordLayout &layout = ctx->getASTRecordLayout(cls);

    for (auto field = cls->field_begin(); field != cls->field_end(); ++field) {
		if (!check_attr(*field))
			continue;
      bool std = field->isInStdNamespace();
      bool canonical = field->isCanonicalDecl();

      TypeInterface::Field field_type = dump_type(ctx, *field);
      field_type.offset = layout.getFieldOffset(field->getFieldIndex());
      tc->fields.push_back(std::move(field_type));
    }

    return std::unique_ptr<TypeInterface::Type>(tc);
  }

  bool check_attr(const clang::CXXRecordDecl *stmt) {
    for (auto attr : stmt->getAttrs()) {
      auto annotate = dyn_cast<AnnotateAttr>(attr);
      if (annotate == nullptr ||
          !annotate->getAnnotation().startswith(_META_KEYWORD ","))
        return false;
    }
    return true;
  };

  // only for exclude annotate
  bool check_attr(const clang::DeclaratorDecl *var) {
	  for (auto attr : var->getAttrs()) {
		  auto annotate = dyn_cast<AnnotateAttr>(attr);
		  if (annotate != nullptr &&
			  annotate->getAnnotation().startswith(_META_EXCLUDE_KEYWORD))
			  return false;
	  }
	  return true;
  };

public:
  ClassMatcher(std::vector<std::unique_ptr<TypeInterface::Type>> &types)
      : types_{types} {}

  void run(const MatchFinder::MatchResult &Result) override {
    auto ast_context = Result.Context;	

	//TODO: meta data
    if (const clang::CXXRecordDecl *cls =
            Result.Nodes.getNodeAs<clang::CXXRecordDecl>("cls")) {
      if (cls && check_attr(cls)) {
        auto t = dump_type(Result.Context, cls);
        types_.push_back(std::move(t));
      }
    } else if (const clang::VarDecl *var =
                   Result.Nodes.getNodeAs<clang::VarDecl>("var")) {
		if (check_attr(var)) {
			static_cast<TypeInterface::TypeContainer *>(types_.back().get())
				->fields.push_back(dump_type(Result.Context, var));
		}
    }
  }

  void onStartOfTranslationUnit() override{}

  void onEndOfTranslationUnit() override{}

};

int main(int argc, const char *argv[]) {

  llvm::EnablePrettyStackTrace();
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);

  std::vector<std::string> source_list{ begin(OptionsParser.getSourcePathList()), end(OptionsParser.getSourcePathList())};

  if (!project_src_root.empty())
  {
	  list_dir(source_list, project_src_root, "*.h", true);
  }

  ClangTool tool(OptionsParser.getCompilations(),
                 source_list);

  std::vector<std::unique_ptr<TypeInterface::Type>> types;

  ClassMatcher matcher(types);
  MatchFinder finder;

  // doc: http://clang.llvm.org/docs/LibASTMatchersReference.html
  // match cxx record which has attributes and bind it to "cls" and bind it's
  // variables to "var"
  auto cls_matcher_expr =
      cxxRecordDecl(hasAttr(clang::attr::Annotate)).bind("cls");

  // static member
  auto var_matcher_expr = cxxRecordDecl(hasAttr(clang::attr::Annotate),
                                        forEach(varDecl().bind("var")));

  finder.addMatcher(cls_matcher_expr, &matcher);
  finder.addMatcher(var_matcher_expr, &matcher);

  int result = tool.run(newFrontendActionFactory(&finder).get());

  return result;
}
