#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

namespace Aero::RT::Simd {
	struct trianglePack {
		constexpr size_t N = HWY_MAX_LNAES(double);

        // Completely scalable SoA lanes
        alignas(32) double v0_x[N], v0_y[N], v0_z[N];
        alignas(32) double e1_x[N], e1_y[N], e1_z[N];
        alignas(32) double e2_x[N], e2_y[N], e2_z[N];

        int material_ids[N];
        size_t size = 0; // How many triangles are actually stored in this pack (up to N)
	};
}