// University campuses.  SIU is modelled in detail, the other three are simpler.
#include "Kit.h"
#include <cmath>

using namespace glm;
using namespace col;
typedef std::vector<std::string> Lines;
static const float PI = 3.14159265f;

// lawn, boundary wall, gate with name arch, guard booth, perimeter trees.  Gate faces west (-X).
static void shell(Kit& k, float hw, float hd, const char* shortName) {
    k.lawn(2 * hw, 2 * hd);
    k.wall(-hw, -hd, hw, -hd); k.wall(hw, -hd, hw, hd); k.wall(-hw, hd, hw, hd);
    k.wall(-hw, -hd, -hw, -6); k.wall(-hw, 6, -hw, hd);
    for (int s = -1; s <= 1; s += 2) k.box(-hw, 0.3f, s * 6.8f, 1.6f, 5, 1.6f, BRICK);
    k.box(-hw, 5.3f, 0, 1.8f, 1.2f, 15.6f, BRICK);
    float H = props::signHeight(Lines{ shortName }, 7.f);
    k.sign(Lines{ shortName }, -hw - 0.1f, 6.5f + H / 2, 0, -PI / 2, 7.f);
    k.sign(Lines{ shortName }, -hw + 0.1f, 6.5f + H / 2, 0, PI / 2, 7.f);
    k.box(-hw + 3, 0.3f, 8.5f, 2.5f, 2.6f, 2.5f, vec3(0.85f, 0.85f, 0.8f));
    for (float z = -hd + 4; z <= hd - 4; z += 12) if (std::fabs(z) > 9) k.tree(-hw + 3.5f, z, 0.8f);
    for (float x = -hw + 14; x <= hw - 4; x += 12) { k.tree(x, -hd + 2.5f, 0.8f); k.tree(x, hd - 2.5f, 0.8f); }
}

static void buildSIU(Kit& k) {
    const float hw = 50, hd = 40;
    shell(k, hw, hd, "SIU");
    const vec3 PATH = PATHC;

    // paths
    k.slab(-34, 0, 32, 5, PATH);  k.slab(16, -11, 4, 8, PATH);  k.slab(10, 18.5f, 4, 5, PATH);  k.slab(-30, -8, 4, 12, PATH);

    // A: main academic building (north), 6 floors, glass atrium, big rooftop name sign
    float hA = 6 * 3.6f + 0.4f;
    k.box(16, 0.3f, -26, 60, hA, 18, vec3(0.90f, 0.88f, 0.82f), MAT_WINDOWS);
    k.box(16, 0.3f + hA, -26, 61, 0.5f, 19, STONE);
    k.box(16, 0.3f, -15.5f, 12, hA * 0.9f, 3, GLASSB);
    Lines name = { "SYLHET INTERNATIONAL", "UNIVERSITY" };
    float H = props::signHeight(name, 54.f);
    for (int s = -1; s <= 1; s += 2) k.box(16 + s * 22.f, 0.3f + hA + 0.5f, -19, 0.6f, 1.5f, 0.6f, DARKG);
    k.sign(name, 16, 0.3f + hA + 0.5f + 1.5f + H / 2, -19, 0, 54);

    // B: library with dome (south)
    k.box(10, 0.3f, 28, 44, 14.8f, 14, vec3(0.85f, 0.80f, 0.70f), MAT_WINDOWS);
    k.cyl(10, 15.1f, 28, 6, 2, CREAM, 20);
    k.sph(10, 17.1f, 28, 6.2f, 5.2f, 6.2f, PATINA, 20, 10);
    k.cyl(10, 22.0f, 28, 0.2f, 2.5f, GOLD, 6);
    k.sign(Lines{ "CENTRAL LIBRARY" }, 10, 9, 35.2f, 0, 20);
    k.sign(Lines{ "CENTRAL LIBRARY" }, 10, 9, 20.8f, PI, 20);

    // C: engineering block (east)
    k.box(38, 0.3f, 0, 14, 18.4f, 30, vec3(0.78f, 0.66f, 0.55f), MAT_WINDOWS);
    k.sign(Lines{ "ENGINEERING", "BLOCK" }, 30.85f, 12, 0, -PI / 2, 14);

    // D: student centre / cafeteria
    k.box(-30, 0.3f, -22, 22, 7.6f, 14, vec3(0.92f, 0.78f, 0.60f), MAT_WINDOWS);
    k.sign(Lines{ "STUDENT CENTER" }, -30, 5.2f, -14.85f, 0, 14);

    // mosque
    k.box(-33, 0.3f, 28, 12, 7, 12, WHITE);
    k.sph(-33, 7.3f, 28, 5, 4, 5, vec3(0.95f), 16, 8);
    k.cyl(-40, 0.3f, 22, 0.8f, 16, WHITE, 10);
    k.cyl(-40, 16.3f, 22, 1.1f, 0.6f, GOLD, 10);

    // football ground, fountain, flag
    k.pitch(2, 4, 40, 24);
    k.cyl(-36, 0.3f, 10, 3.4f, 0.5f, STONE, 20);
    k.cyl(-36, 0.35f, 10, 3.0f, 0.5f, WATER, 20);
    k.cone(-36, 0.8f, 10, 0.5f, 2.2f, vec3(0.7f, 0.85f, 0.95f), 8);
    k.flag(-40, -8);

    // parking: bikes, cars, CNG
    k.slab(42, 29, 14, 18, ASPHALT, cfg::LAWN_TOP, 0.05f);
    const vec3 cols[] = { vec3(0.8f, 0.2f, 0.2f), vec3(0.2f, 0.4f, 0.8f), vec3(0.15f), vec3(0.9f, 0.7f, 0.1f) };
    for (int i = 0; i < 10; ++i)
        k.vehicle(VType::Bike, 36.5f + 1.2f * i, 24, PI, cols[i % 4], cols[(i + 1) % 4]);
    k.vehicle(VType::Car, 37.5f, 32, 0, vec3(0.9f), vec3(0));
    k.vehicle(VType::Car, 42.f, 32, 0, vec3(0.1f, 0.2f, 0.5f), vec3(0));
    k.vehicle(VType::Car, 46.5f, 32, 0, vec3(0.6f, 0.1f, 0.1f), vec3(0));
    k.vehicle(VType::CNG, 39, 36, 0, vec3(0.1f, 0.55f, 0.25f), vec3(0.08f));
    k.vehicle(VType::CNG, 44, 36, 0, vec3(0.1f, 0.55f, 0.25f), vec3(0.08f));
    // trees on the lawn
    k.tree(-20, -8, 0.9f); k.tree(-20, 16, 0.9f); k.tree(26, 4, 0.9f); k.tree(26, -8, 0.9f);
    k.tree(-45, -12, 0.8f); k.tree(-45, 14, 0.8f); k.tree(24, 20, 0.9f); k.tree(-14, 30, 0.9f);
}

static void buildLeading(Kit& k) {
    const float hw = 50, hd = 40;
    shell(k, hw, hd, "LU");
    const vec3 roofc(0.30f, 0.30f, 0.32f);
    k.box(0, 0.3f, -28, 60, 15.2f, 14, BRICK, MAT_WINDOWS);
    k.box(-26, 0.3f, -14, 12, 15.2f, 20, BRICK, MAT_WINDOWS);
    k.box(26, 0.3f, -14, 12, 15.2f, 20, BRICK, MAT_WINDOWS);
    for (int s = -1; s <= 1; s += 2) {
        k.box(s * 32.f, 0.3f, -28, 8, 26, 8, BRICK * 1.1f, MAT_WINDOWS);
        k.cone(s * 32.f, 26.3f, -28, 6.4f, 8, roofc, 4, PI / 4);
    }
    Lines name = { "LEADING UNIVERSITY" };
    float H = props::signHeight(name, 34.f);
    for (int s = -1; s <= 1; s += 2) k.box(s * 14.f, 15.5f, -30, 0.6f, 1.2f, 0.6f, DARKG);
    k.sign(name, 0, 15.5f + 1.2f + H / 2, -30, 0, 34);
    k.slab(-34, 0, 32, 5, PATHC); k.slab(0, -6, 5, 12, PATHC);
    k.minar(0, -6);
    k.pitch(0, 22, 40, 24);
    for (int s = -1; s <= 1; s += 2) { k.tree(s * 14.f, 4); k.tree(s * 36.f, 12); }
}

static void buildMetro(Kit& k) {
    const float hw = 50, hd = 28;
    shell(k, hw, hd, "MU");
    k.box(8, 0.3f, -12, 56, 11.6f, 20, vec3(0.80f, 0.82f, 0.86f), MAT_WINDOWS);
    k.box(-6, 0.3f, -14, 22, 40, 16, GLASSB, MAT_WINDOWS);
    k.cyl(-6, 40.3f, -14, 0.3f, 8, DARKG, 6);
    k.sign(Lines{ "METROPOLITAN", "UNIVERSITY" }, 20, 6.2f, -1.85f, 0, 24);
    k.slab(-34, 0, 32, 5, PATHC); k.slab(-6, 0, 5, 6, PATHC);
    // basketball court with hoops
    k.slab(14, 10, 22, 14, vec3(0.2f, 0.35f, 0.6f));
    for (int s = -1; s <= 1; s += 2) {
        k.cyl(14 + s * 10.5f, 0.34f, 10, 0.1f, 3.2f, DARKG, 6);
        k.box(14 + s * 10.f, 3.0f, 10, 0.1f, 1.2f, 1.8f, WHITE);
    }
    k.slab(40, 18, 18, 8, ASPHALT, cfg::LAWN_TOP, 0.05f);
    k.vehicle(VType::Car, 35, 18, PI / 2, vec3(0.9f), vec3(0)); k.vehicle(VType::Car, 40, 18, PI / 2, vec3(0.1f), vec3(0));
    k.vehicle(VType::Car, 45, 18, PI / 2, vec3(0.7f, 0.1f, 0.1f), vec3(0));
    for (int i = 0; i < 4; ++i) k.tree(-30.f + 10.f * i, 10, 0.9f);
}

static void buildNortheast(Kit& k) {
    const float hw = 50, hd = 40;
    shell(k, hw, hd, "NEU");
    const vec3 ochre(0.93f, 0.85f, 0.65f);
    k.box(0, 0.3f, -14, 30, 11.2f, 16, ochre, MAT_WINDOWS);
    for (int s = -1; s <= 1; s += 2) k.box(s * 28.f, 0.3f, -14, 26, 7.6f, 14, ochre * 0.95f, MAT_WINDOWS);
    for (int i = 0; i < 6; ++i) k.cyl(-10.f + 4.f * i, 0.3f, -4, 0.6f, 8.4f, WHITE, 12);
    k.box(0, 8.7f, -5.4f, 24, 1, 4.6f, CREAM);
    mat4 Pg = k.P * translate(mat4(1.f), vec3(0, 9.7f, -5.4f)) * rotate(mat4(1.f), PI / 2, vec3(0, 1, 0));
    k.b.gable(vec3(0), vec3(4.6f, 3, 24), CREAM, Pg);
    k.cyl(0, 11.5f, -14, 5, 2.5f, CREAM, 20);
    k.sph(0, 14, -14, 5.2f, 4.5f, 5.2f, PATINA, 20, 10);
    k.cyl(0, 18.2f, -14, 0.2f, 2.5f, GOLD, 6);
    k.slab(-25, 0, 50, 5, PATHC); k.slab(0, -2, 5, 4, PATHC);
    k.post(Lines{ "NORTHEAST UNIVERSITY", "BANGLADESH" }, 16, 4, 0, 30);
    k.pitch(0, 24, 40, 24);
    k.flag(-14, 6);
    for (int s = -1; s <= 1; s += 2) { k.tree(s * 20.f, 8); k.tree(s * 38.f, 8); }
}

void buildCampus(Batch& b, const Landmark& lm, std::mt19937& rng) {
    Kit k(b, lm, rng);
    switch (lm.type) {
    case LType::SIU:         buildSIU(k); break;
    case LType::Leading:     buildLeading(k); break;
    case LType::Metropolitan: buildMetro(k); break;
    default:                 buildNortheast(k); break;
    }
}
