#include "AssetTool.h"
#include "Base/AssetFile.h"

#if defined max
	#undef max
#endif

#include <vcg/complex/complex.h>
#include <vcg/complex/used_types.h>
#include <vcglib/wrap/io_trimesh/import_obj.h>

NamespaceBegin(AssetTool)

class TFace;
class TVertex;

class PFace;
class PVertex;
class PHEdge;
class PEdge;

struct TUsedTypes : public vcg::UsedTypes<vcg::Use<TVertex>::AsVertexType, vcg::Use<TFace>::AsFaceType>
{
};

class TVertex : public vcg::Vertex<TUsedTypes, vcg::vertex::BitFlags, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Mark> 
{
};

class TFace : public vcg::Face<TUsedTypes, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::BitFlags, vcg::face::FFAdj> 
{
};

class TMesh : public vcg::tri::TriMesh<std::vector<TVertex>, std::vector<TFace>> 
{
};

struct PUsedTypes : public vcg::UsedTypes<vcg::Use<PVertex>::AsVertexType, vcg::Use<PEdge>::AsEdgeType, vcg::Use<PHEdge>::AsHEdgeType, vcg::Use<PFace>::AsFaceType> 
{
};

class PVertex : public vcg::Vertex<PUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Mark, vcg::vertex::BitFlags, vcg::vertex::VHAdj> 
{
};

class PEdge : public vcg::Edge<PUsedTypes> 
{
};

class PHEdge : public vcg::HEdge<PUsedTypes, vcg::hedge::BitFlags, vcg::hedge::HEdgeData> 
{
};

class PFace :public vcg::Face<PUsedTypes, vcg::face::PolyInfo, vcg::face::PFVAdj, vcg::face::PFVAdj, vcg::face::PFFAdj, vcg::face::PFHAdj, vcg::face::BitFlags, vcg::face::Normal3f> 
{
};

class PMesh : public vcg::tri::TriMesh<std::vector<PVertex>, std::vector<PFace>, std::vector<PHEdge>, std::vector<PEdge>> 
{
};

typedef vcg::tri::io::ImporterOBJ<PMesh> OBJImpoter;

bool LoadOBJ(
	const std::string &filePath,
	__out std::vector<Geometry::Vertex> &vertices,
	__out std::vector<uint32_t> &indices)
{
	PMesh mesh;
	int32_t loadMask = 0;
	auto err = OBJImpoter::Open(mesh, filePath.c_str(), loadMask);
	if (err != OBJImpoter::E_NOERROR)
	{
		Base::Log("Failed to load obj- %s, error= %s.", filePath.c_str(), OBJImpoter::ErrorMsg(err));
		assert(0);
	}

	return false;
}

NamespaceEnd(AssetTool)