#include "AssetTool.h"
#include "Base/AssetFile.h"

#if defined max
	#undef max
#endif

#if defined min
	#undef min
#endif

#include <vcg/complex/complex.h>
#include <vcg/complex/used_types.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include "vcgOBJImporter.hpp"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

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

bool LoadingCallback(const int32_t progress, const char *pMessage)
{
	Base::Log("Loading obj mesh in progress-%s %d%%", pMessage, progress);
	return true;
}

bool LoadOBJ(
	AssetFile &asset,
	__out std::vector<Geometry::Vertex> &vertices,
	__out std::vector<uint32_t> &indices,
	__out Geometry::Box &boundingBox)
{
	PMesh mesh;
	int32_t loadMask = 0;
	std::vector<OBJImpoter::ObjIndexedFace> indexedFace;

	auto err = OBJImpoter::Open(mesh, asset.GetPath().c_str(), loadMask, indexedFace, LoadingCallback);
	if (err != OBJImpoter::E_NOERROR)
	{
		Base::Log("Failed to load obj- %s, error= %s.", asset.GetPath().c_str(), OBJImpoter::ErrorMsg(err));
		assert(0);
	}

	///vcg::tri::UpdateTopology<PMesh>::VertexFace(mesh);
	vcg::tri::UpdateBounding<PMesh>::Box(mesh);
	Vec3 min(mesh.bbox.min.X(), mesh.bbox.min.Y(), mesh.bbox.min.Z());
	Vec3 max(mesh.bbox.max.X(), mesh.bbox.max.Y(), mesh.bbox.max.Z());
	boundingBox = Geometry::Box(min, max);

	vertices.resize((size_t)mesh.vn);
	size_t index = 0U;
	for (auto it = mesh.vert.cbegin(); it != mesh.vert.cend(); ++it)
	{
		if (it->HasCoord())
		{
			auto &position = it->P();
			vertices[index].Position = Vec3(position.X(), position.Y(), position.Z());
		}

		if (it->HasNormal())
		{
			auto &normal = it->N();
			vertices[index].Normal = Vec3(normal.X(), normal.Y(), normal.Z());
		}

		///if (it->HasTexCoord())
		///{
		///	///auto &uv = it->T();
		///	vertices[index].UV = Vec2();
		///}

		++index;
	}

	index = 0U;
	indices.resize(indexedFace.size() * 3U);
	for (auto it = indexedFace.cbegin(); it < indexedFace.cend(); ++it)
	{
		assert(it->v.size() == 3U);
		indices[index++] = it->v[0];
		indices[index++] = it->v[1];
		indices[index++] = it->v[2];
	}

	///for (auto it = mesh.face.cbegin(); it < mesh.face.cend(); ++it)
	///{
	///	for (int32_t i = 0; i < it->VN(); ++i)
	///	{
	///		const auto vertex = it->V(i);
	///		auto point = vertex->P();
	
	///		if (it->HasNormal())
	///		{
	///			auto normal = vertex->N();
	///		}
	///	}
	///}

	return true;
}

NamespaceEnd(AssetTool)