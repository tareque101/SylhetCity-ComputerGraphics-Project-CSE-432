#pragma once
// World units: 1 unit = 1 metre.  +X = east, -Z = north, +Y = up.
namespace cfg {
constexpr float ROAD_W       = 12.f;
constexpr float ROAD_TOP     = 0.15f;   // asphalt surface
constexpr float MARK_TOP     = 0.19f;   // lane markings
constexpr float WALK_TOP     = 0.40f;   // pavement
constexpr float PLATE_TOP    = 0.25f;   // block ground plate
constexpr float LAWN_TOP     = 0.30f;
constexpr float PATH_TOP     = 0.34f;
constexpr float RIVER_N      = -200.f;  // Surma north bank edge
constexpr float RIVER_S      = -135.f;  // Surma south bank edge
constexpr float NORTH_BANK_Z = -270.f;
}
