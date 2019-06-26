#pragma once
#include "reflection.h"

#include <cstdint>
#include <string>

namespace Orz {



namespace MM {

	class Meta() Mesh {
	public:
		static Mesh* create();
		void *dump_geom();
		int id2;
		MetaExclude() float qq;
	private:
		int id;
		int num_verts;
		float weight;
	};

} // namespace Test
} // namespace Orz
