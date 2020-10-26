#include "Colorful/Public/GfxResource.h"

namespace tinyxml2
{
	class XMLNode;
}

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(Material)
class EXPORT_API Material
{
public:
	Material(const std::string& name);
protected:
	void loadInputLayouts(const tinyxml2::XMLNode* root);
	void loadShaders(const tinyxml2::XMLNode* root);
private:
	std::string m_Name;
};

NAMESPACE_END(Gfx)
