#include "Landmarks.h"
#include "Kit.h"
#include "RoadNet.h"
#include <algorithm>
#include <cmath>

using namespace glm;
using namespace col;
using props::rf;

static const float PI = 3.14159265f;
typedef std::vector<std::string> Lines;

bool isCampus(LType t) {
    return t == LType::SIU || t == LType::Leading || t == LType::Metropolitan || t == LType::Northeast;
}

// ---------------------------------------------------------------------------------------------
// LANDMARK TABLE: name, map label, type, centre (x,z), footprint, variant, category.
// Change positions here; each place sits inside one block of the road grid (see RoadNet.h).
// ---------------------------------------------------------------------------------------------
std::vector<Landmark> makeLandmarks() {
    using namespace grid;
    std::vector<Landmark> L;
    auto add = [&](const char* n, const char* lb, LType t, float x, float z, float sx, float sz, int v, LCat c) {
        Landmark l;
        l.name = n; l.label = lb; l.type = t; l.pos = vec2(x, z); l.size = vec2(sx, sz); l.variant = v; l.cat = c;
        L.push_back(l);
    };
    add("Bandar Bazar Point",                   "BANDAR BAZAR", LType::Point, X[0], Z[0], 0, 0, 0, LCat::Junction);
    add("Zindabazar Point",                     "ZINDABAZAR",         LType::Point, X[1], Z[1], 0, 0, 1, LCat::Junction);
    add("Ambarkhana Point",                     "AMBARKHANA",         LType::Point, X[2], Z[1], 0, 0, 2, LCat::Junction);
    add("Medical Point",                        "MEDICAL POINT",      LType::Point, X[1], Z[3], 0, 0, 3, LCat::Junction);
    // Bridges: Keane = pedestrians only (variant 0); Shah Jalal + East Surma = toll bridges for vehicles
    add("Keane Bridge (pedestrian only)",       "KEANE BRIDGE",       LType::Bridge, X[0], -167.5f, 0, 0, 0, LCat::Landmark);
    add("Shah Jalal Bridge (toll)",             "SHAH JALAL BRIDGE",  LType::Bridge, X[2], -167.5f, 0, 0, 1, LCat::Landmark);
    add("East Surma Bridge (toll)",             "EAST SURMA BRIDGE",  LType::Bridge, X[4], -167.5f, 0, 0, 2, LCat::Landmark);
    // Surma riverside, city bank (same side as Keane Bridge, Bandar Bazar and the Circuit House)
    add("Ali Amjad's Clock Tower",              "ALI AMJAD CLOCK",    LType::Clock, -142, -121, 22, 22, 0, LCat::Landmark);
    add("Sylhet Circuit House",                 "CIRCUIT HOUSE",      LType::Circuit, -20, -122, 62, 24, 0, LCat::Admin);
    // Bandar Bazar area
    add("Bandar Bazar Market",                  "BANDAR MARKET",      LType::Market, -50, -50, 50, 38, 0, LCat::Landmark);
    add("Karimullah Market",                    "KARIMULLAH MKT",     LType::Mall, -92, -78, 30, 22, 0, LCat::Landmark);
    add("Hasan Market",                         "HASAN MARKET",       LType::Mall, -40, -17, 38, 16, 1, LCat::Landmark);
    add("Sylhet Court",                         "SYLHET COURT",       LType::Court, -60, 50, 58, 38, 0, LCat::Admin);
    add("Sylhet DC Office (Deputy Commissioner)", "DC OFFICE",        LType::DCOffice, -60, 140, 58, 32, 0, LCat::Admin);
    add("Sylhet City Corporation (SCC)",        "SCC",   LType::CityCorp, 55, -50, 58, 34, 0, LCat::Admin);
    add("Sylhet Govt. Pilot High School",       "PILOT SCHOOL",       LType::School, 55, 50, 52, 34, 0, LCat::School);
    add("Sylhet Govt. Girls' High School",      "GIRLS SCHOOL",       LType::School, 170, -50, 52, 34, 1, LCat::School);
    add("MAG Osmani Medical College Hospital",  "MEDICAL COLLEGE",    LType::Hospital, 50, 140, 66, 34, 0, LCat::Health);
    add("BRTA Sylhet Office",                   "BRTA OFFICE",        LType::BRTA, 170, 140, 46, 30, 0, LCat::Admin);
    add("Northeast University Bangladesh",      "NORTHEAST UNI",     LType::Northeast, 170, 50, 100, 80, 0, LCat::University);
    add("Leading University",                   "LEADING UNIV",       LType::Leading, 290, -50, 100, 80, 0, LCat::University);
    add("Sylhet International University (SIU)", "SIU",  LType::SIU, 290, 50, 100, 80, 0, LCat::University);
    add("Metropolitan University",              "METRO UNIV",         LType::Metropolitan, 290, 140, 100, 56, 0, LCat::University);
    return L;
}

// --- junction points --------------------------------------------------------------------------
static void buildPoint(Kit& k, const Landmark& lm) {
    if (lm.variant == 0) {          // Bandar Bazar gateway arch over the bridge approach
        float z = -22.f;
        for (int s = -1; s <= 1; s += 2) {
            k.box(s * 9.8f, 0.4f, z, 2, 9, 2, BRICK);
            k.sph(s * 9.8f, 9.4f, z, 1.6f, 1.4f, 1.6f, GOLD, 10, 6);
        }
        k.box(0, 9.4f, z, 22, 2.4f, 2.4f, BRICK);
        k.sign(Lines{ "BANDAR BAZAR" }, 0, 10.6f, z + 1.4f, 0, 16);
        k.sign(Lines{ "BANDAR BAZAR" }, 0, 10.6f, z - 1.4f, PI, 16);
        return;
    }
    Lines txt;
    vec3 roof;
    if (lm.variant == 1)      { txt = { "ZINDABAZAR", "POINT" }; roof = vec3(0.80f, 0.60f, 0.15f); }
    else if (lm.variant == 2) { txt = { "AMBARKHANA", "POINT" }; roof = vec3(0.15f, 0.55f, 0.30f); }
    else                      { txt = { "MEDICAL", "POINT" };    roof = vec3(0.75f, 0.12f, 0.15f); }
    k.cyl(0, 0.15f, 0, 1.2f, 0.45f, STONE, 20);
    k.cyl(0, 0.6f, 0, 0.6f, 8.8f, STONE, 12);
    k.box(0, 9.4f, 0, 14, 5.5f, 14, NAVY);
    for (int i = 0; i < 4; ++i) {
        float yaw = i * PI / 2;
        k.sign(txt, std::sin(yaw) * 7.15f, 12.15f, std::cos(yaw) * 7.15f, yaw, 11.5f);
    }
    k.sph(0, 14.9f, 0, 6.f, 2.4f, 6.f, roof, 16, 6);
    k.cyl(0, 17.0f, 0, 0.15f, 2.f, GOLD, 6);
    if (lm.variant == 3)            // hospital cross
        for (int i = 0; i < 2; ++i) {
            mat4 Pk = place(k.C, i * PI / 2);
            k.b.box(vec3(-3, 19.5f, 0), vec3(6, 1.8f, 0.5f), RED, Pk, MAT_EMISSIVE);
            k.b.box(vec3(-0.9f, 17.4f, 0), vec3(1.8f, 6, 0.5f), RED, Pk, MAT_EMISSIVE);
        }
}

// --- bridges over the Surma river -----------------------------------------------------------------
// The river sits in a deep channel (water at y = -7), so the decks are ~6 m above the water and
// steamers / boats pass underneath.  Piers stand at z = -178 and -157 (absolute), the boat lanes avoid them.
static const vec3 RED_STEEL(0.70f, 0.07f, 0.08f);

// bar between two points in the YZ plane at lateral offset x (landmark-relative coordinates)
static void yzBar(Kit& k, float x, float y0, float z0, float y1, float z1, float t, const vec3& c) {
    float segLen = std::sqrt((z1 - z0) * (z1 - z0) + (y1 - y0) * (y1 - y0));
    mat4 Ps = k.P * translate(mat4(1.f), vec3(x, (y0 + y1) / 2, (z0 + z1) / 2)) *
              rotate(mat4(1.f), -std::atan2(y1 - y0, z1 - z0), vec3(1, 0, 0));
    k.b.box(vec3(0, -t / 2, 0), vec3(t, t, segLen + 0.05f), c, Ps);
}

static void buildKeane(Kit& k, float z0, float z1) {
    const float W = 10.f, len = z1 - z0, zc = (z0 + z1) * 0.5f, H = 12.f, AX = 4.6f;
    // piers (brick) and red steel girders
    for (int i = -1; i <= 1; i += 2) {
        k.box(0, -9.5f, i * 10.5f, W - 0.6f, 8.45f, 3.0f, BRICK);
        k.box(0, -1.15f, i * 10.5f, W - 0.2f, 0.2f, 3.4f, STONE);
    }
    k.box(0, -1.05f, zc, W, 1.19f, len, RED_STEEL);
    k.box(0, 0.12f, zc, W - 1.2f, 0.04f, len, vec3(0.64f, 0.62f, 0.58f));   // walking surface
    // steel through-arches with hangers and diagonals
    const int N = 14;
    auto archY = [&](float t) { return 0.4f + H * (1 - (2 * t - 1) * (2 * t - 1)); };
    for (int s = -1; s <= 1; s += 2) {
        float x = s * AX;
        for (int i = 0; i < N; ++i) {
            float t0 = (float)i / N, t1 = (float)(i + 1) / N;
            yzBar(k, x, archY(t0), z0 + len * t0, archY(t1), z0 + len * t1, 0.75f, RED_STEEL);   // arch chord
            // lattice: alternating diagonals between deck and arch
            if (i % 2 == 0) yzBar(k, x, 0.4f, z0 + len * t0, archY(t1), z0 + len * t1, 0.22f, RED_STEEL);
            else            yzBar(k, x, archY(t0), z0 + len * t0, 0.4f, z0 + len * t1, 0.22f, RED_STEEL);
        }
        for (int i = 1; i < N; ++i) {
            float t = (float)i / N;
            k.box(x, 0.4f, z0 + len * t, 0.28f, archY(t) - 0.4f, 0.28f, RED_STEEL);          // hanger
        }
        // handrail just inside the arch
        float rx = s * (AX - 0.7f);
        k.box(rx, 1.05f, zc, 0.1f, 0.1f, len, RED_STEEL);
        for (float z = z0 + 1; z < z1; z += 2.f) k.box(rx, 0.16f, z, 0.07f, 0.95f, 0.07f, RED_STEEL);
    }
    for (int i = 2; i < N - 1; i += 2) {                                                         // top bracing
        float t = (float)i / N;
        k.box(0, archY(t) - 0.15f, z0 + len * t, 2 * AX - 0.4f, 0.3f, 0.3f, RED_STEEL);
    }
    // name boards on both arches
    for (int s = -1; s <= 1; s += 2)
        k.sign(Lines{ "KEANE BRIDGE" }, s * (AX + 0.55f), H * 0.55f, zc, s * PI / 2, 9.f, vec3(0.30f, 0.04f, 0.04f));
    // lamps, warm light at night
    for (int s = -1; s <= 1; s += 2)
        for (int i = 1; i < N; i += 3) {
            float z = z0 + len * i / N + 2.3f;
            k.cyl(s * 3.4f, 0.16f, z, 0.06f, 3.3f, DARKG, 6);
            k.b.sphere(k.C + vec3(s * 3.4f, 3.55f, z), vec3(0.3f), vec3(1.f, 0.92f, 0.65f), mat4(1.f), 6, 4, MAT_EMISSIVE);
        }
    // pedestrian plazas at both ends, with bollards that keep vehicles off the bridge
    float zs0 = z1, zs1 = z1 + 29.f;             // city end (south bank)
    float zn1 = z0, zn0 = z0 - 26.f;             // far bank
    k.box(0, 0.f, (zs0 + zs1) / 2, 14.f, 0.16f, zs1 - zs0, PATHC);
    k.box(0, 0.f, (zn0 + zn1) / 2, 14.f, 0.16f, zn1 - zn0, PATHC);
    for (float x = -6.f; x <= 6.01f; x += 2.f) {
        k.cyl(x, 0.16f, zs1 - 1.2f, 0.18f, 0.95f, (x < 0 ? RED : WHITE), 8);
        k.cyl(x, 0.16f, zn0 + 1.2f, 0.18f, 0.95f, (x < 0 ? RED : WHITE), 8);
    }
    k.post(Lines{ "KEANE BRIDGE", "PEDESTRIANS ONLY" }, 8.5f, zs1 - 3.f, 0, 11);
    k.post(Lines{ "KEANE BRIDGE", "PEDESTRIANS ONLY" }, -8.5f, zn0 + 3.f, PI, 11);
    // far bank: tea stalls and trees
    for (int s = -1; s <= 1; s += 2) {
        k.box(s * 9.f, 0.16f, zn0 + 8.f, 3.2f, 2.4f, 2.4f, vec3(0.55f, 0.38f, 0.22f));
        k.box(s * 9.f, 2.6f, zn0 + 8.f, 4.0f, 0.15f, 3.2f, s > 0 ? vec3(0.85f, 0.2f, 0.15f) : vec3(0.2f, 0.45f, 0.8f));
        k.tree(s * 12.f, zn0 + 14.f);
    }
}

static void buildTollBridge(Kit& k, const Landmark& lm, float z0, float z1) {
    const float len = z1 - z0, zc = (z0 + z1) * 0.5f;
    for (int i = -1; i <= 1; i += 2) {
        k.box(0, -9.5f, i * 10.5f, 17.f, 8.45f, 3.2f, vec3(0.62f, 0.62f, 0.60f));
        k.box(0, -1.15f, i * 10.5f, 19.4f, 0.2f, 3.6f, STONE);
    }
    k.box(0, -1.05f, zc, 19, 1.19f, len, STEEL);
    for (int s = -1; s <= 1; s += 2) {
        k.box(s * 9.1f, 1.35f, zc, 0.15f, 0.12f, len, STEEL);
        for (float z = z0 + 1; z < z1; z += 4.f) k.box(s * 9.1f, 0.4f, z, 0.2f, 1.0f, 0.2f, STEEL);
    }
    // portal signs over the approach roads
    const char* nm = lm.variant == 1 ? "SHAH JALAL BRIDGE" : "EAST SURMA BRIDGE";
    for (int s = -1; s <= 1; s += 2) {
        k.box(s * 9.4f, 0.14f, zc, 0.5f, 7.0f, 0.5f, STEEL);
    }
    k.box(0, 7.1f, zc, 19.4f, 0.8f, 0.8f, STEEL);
    k.sign(Lines{ nm }, 0, 8.6f, zc + 0.5f, 0, 16.f);
    k.sign(Lines{ nm }, 0, 8.6f, zc - 0.5f, PI, 16.f);
}

static void buildBridge(Kit& k, const Landmark& lm) {
    float z0 = cfg::RIVER_N - lm.pos.y, z1 = cfg::RIVER_S - lm.pos.y;   // relative span
    if (lm.variant == 0) buildKeane(k, z0, z1);
    else                 buildTollBridge(k, lm, z0, z1);
}

// --- Ali Amjad's Clock Tower -------------------------------------------------------------------------------
static void buildClock(Kit& k, const Landmark&) {
    const vec3 MAROON(0.55f, 0.13f, 0.12f), MAROON2(0.68f, 0.20f, 0.17f);
    k.box(0, 0.16f, 0, 22, 0.14f, 22, PATHC);                    // plaza
    k.box(0, 0.30f, 0, 9, 1.2f, 9, STONE);                       // plinth
    k.box(0, 1.5f, 0, 6.2f, 13.f, 6.2f, MAROON);                 // shaft
    for (int s = -1; s <= 1; s += 2) {
        k.box(s * 3.2f, 1.5f, 3.2f, 0.7f, 13.f, 0.7f, MAROON2);  // corner pilasters
        k.box(s * 3.2f, 1.5f, -3.2f, 0.7f, 13.f, 0.7f, MAROON2);
    }
    k.box(0, 6.0f, 0, 6.8f, 0.5f, 6.8f, STONE);                  // string course
    k.box(0, 14.5f, 0, 7.2f, 0.7f, 7.2f, STONE);
    k.box(0, 15.2f, 0, 6.0f, 5.0f, 6.0f, MAROON2);               // clock chamber
    // clock faces on all four sides (white dial, black hands)
    for (int i = 0; i < 4; ++i) {
        float yaw = i * PI / 2;
        mat4 F = k.P * place(vec3(0, 17.7f, 0), yaw);            // local +Z = outward
        k.b.cylZ(vec3(0, 0, 3.05f), 2.0f, 0.2f, vec3(0.96f), F, 24);
        k.b.box(vec3(-0.12f, 0.f, 3.2f), vec3(0.24f, 1.5f, 0.1f), vec3(0.05f), F);
        mat4 Hm = F * translate(mat4(1.f), vec3(0, 0, 3.2f)) * rotate(mat4(1.f), -2.1f, vec3(0, 0, 1));
        k.b.box(vec3(-0.08f, 0.f, 0.f), vec3(0.16f, 1.7f, 0.1f), vec3(0.05f), Hm);
        k.sign(Lines{ "ALI AMJADS" }, std::sin(yaw) * 3.45f, 11.0f, std::cos(yaw) * 3.45f, yaw, 4.6f, vec3(0.30f, 0.05f, 0.05f));
    }
    k.box(0, 20.2f, 0, 7.4f, 0.6f, 7.4f, STONE);                 // cornice
    k.cone(0, 20.8f, 0, 4.6f, 4.5f, vec3(0.25f, 0.30f, 0.32f), 4, PI / 4);
    k.cyl(0, 25.2f, 0, 0.12f, 2.2f, GOLD, 6);
    // riverside railing, trees and benches
    for (int s = -1; s <= 1; s += 2) {
        k.box(s * 10.f, 0.3f, 0, 0.25f, 1.0f, 22, STONE);
        k.tree(s * 8.f, 8.f);
        k.box(s * 6.f, 0.3f, -8.f, 2.f, 0.5f, 0.6f, BRICK);
    }
    k.tree(-8, -8); k.tree(8, -8);
}

// --- Sylhet Circuit House ----------------------------------------------------------------------------------------
static void buildCircuitHouse(Kit& k, const Landmark&) {
    k.lawn(60, 22);
    k.box(0, 0.3f, -2, 44, 0.8f, 12, STONE);                                    // plinth
    k.box(0, 1.1f, -2, 42, 7.5f, 10, vec3(0.96f, 0.93f, 0.84f), MAT_WINDOWS);   // main block
    k.b.gable(vec3(0, 8.6f, -2) + k.C, vec3(44, 3.4f, 12), vec3(0.62f, 0.28f, 0.22f));  // red tile roof
    k.box(0, 1.1f, 5, 30, 0.2f, 3, STONE);                                      // veranda floor
    for (int i = 0; i < 11; ++i) k.cyl(-14.f + 2.8f * i, 1.3f, 6.2f, 0.28f, 5.4f, WHITE, 10);
    k.box(0, 6.7f, 5.2f, 31, 0.4f, 3.4f, CREAM);
    for (int s = -1; s <= 1; s += 2) {                                          // end wings
        k.box(s * 24.f, 0.3f, -2, 8, 6.2f, 10, vec3(0.96f, 0.93f, 0.84f), MAT_WINDOWS);
        k.b.gable(vec3(s * 24.f, 6.5f, -2) + k.C, vec3(8, 3.0f, 11), vec3(0.62f, 0.28f, 0.22f));
    }
    for (int s = -1; s <= 1; s += 2) { k.tree(s * 28.f, 7.f); k.tree(s * 14.f, 8.f, 0.9f); }
    k.slab(0, 8.5f, 4, 6, ASPHALT, cfg::LAWN_TOP, 0.05f);                      // drive
    for (int s = -1; s <= 1; s += 2) k.box(s * 3.5f, 0.3f, 10.f, 1.2f, 3.2f, 1.2f, BRICK);
    k.vehicle(VType::Car, 0.f, 8.5f, 0, vec3(0.08f), vec3(0));
    k.flag(-21, 8, 11);
    k.post(Lines{ "SYLHET", "CIRCUIT HOUSE" }, 0, 10.2f, 0, 14);
    k.post(Lines{ "SYLHET", "CIRCUIT HOUSE" }, 0, -10.2f, PI, 14);
}

// --- shopping malls: Karimullah Market (variant 0) and Hasan Market (variant 1) --------------------------------
static void buildMall(Kit& k, const Landmark& lm) {
    bool kar = lm.variant == 0;
    float w = kar ? 28.f : 36.f, d = kar ? 20.f : 14.f;
    int floors = kar ? 8 : 5;
    float h = floors * 3.6f;
    vec3 wall = kar ? vec3(0.78f, 0.82f, 0.88f) : vec3(0.82f, 0.62f, 0.46f);
    k.box(0, 0.2f, 0, w + 3, 0.15f, d + 3, PATHC);                              // forecourt
    k.box(0, 0.3f, 0, w, h, d, wall, MAT_WINDOWS);
    k.box(0, 0.3f + h, 0, w + 0.6f, 0.5f, d + 0.6f, STONE);
    // shop-front band + awnings on the street side
    const vec3 aw[] = { vec3(0.85f, 0.15f, 0.15f), vec3(0.15f, 0.45f, 0.80f), vec3(0.95f, 0.70f, 0.10f), vec3(0.20f, 0.65f, 0.35f) };
    int n = (int)(w / 4.f);
    for (int s = -1; s <= 1; s += 2)
        for (int i = 0; i < n; ++i)
            k.box(-w / 2 + (i + 0.5f) * w / n, 3.2f, s * (d / 2 + 0.9f), w / n - 0.2f, 0.15f, 1.8f, aw[(i + (s > 0)) % 4]);
    // rooftop tanks
    for (int i = 0; i < 3; ++i) k.cyl(-w / 3 + i * w / 3, 0.8f + h, 0, 1.1f, 1.8f, vec3(0.25f, 0.4f, 0.6f), 10);
    // big signs, all four sides
    Lines txt = kar ? Lines{ "KARIMULLAH", "MARKET" } : Lines{ "HASAN", "MARKET" };
    float sw = kar ? 20.f : 14.f, sy = h - 4.5f;
    k.sign(txt, 0, sy, d / 2 + 0.2f, 0, sw);
    k.sign(txt, 0, sy, -d / 2 - 0.2f, PI, sw);
    k.sign(txt, w / 2 + 0.2f, sy, 0, PI / 2, kar ? 14.f : 9.f);
    k.sign(txt, -w / 2 - 0.2f, sy, 0, -PI / 2, kar ? 14.f : 9.f);
    if (kar) {      // basement parking ramp on the west side
        k.box(-w / 2 - 4.5f, 0.2f, 0, 6, 0.1f, 10, ASPHALT);
        k.vehicle(VType::Car, -w / 2 - 4.5f, -2, PI / 2, vec3(0.9f), vec3(0));
        k.vehicle(VType::CNG, -w / 2 - 4.5f, 3, PI / 2, vec3(0.1f, 0.55f, 0.25f), vec3(0.08f));
    }
}

// --- Bandar Bazar market hall ----------------------------------------------------------------------
static void buildMarket(Kit& k, const Landmark&) {
    k.box(0, 0.2f, 0, 48, 0.15f, 36, vec3(0.55f, 0.54f, 0.50f));
    for (int i = 0; i < 7; ++i)
        for (int s = -1; s <= 1; s += 2) k.cyl(-21.f + 7.f * i, 0.35f, s * 13.f, 0.35f, 5.2f, STONE, 8);
    k.b.gable(vec3(0, 5.55f, 0) + k.C, vec3(48, 4, 34), vec3(0.30f, 0.45f, 0.62f));
    const vec3 vivid[] = { vec3(0.9f, 0.3f, 0.2f), vec3(0.2f, 0.5f, 0.85f), vec3(0.95f, 0.75f, 0.2f),
                           vec3(0.3f, 0.7f, 0.35f), vec3(0.85f, 0.35f, 0.65f) };
    for (int i = 0; i < 8; ++i)
        for (int s = -1; s <= 1; s += 2) {
            float x = -21.f + 6.f * i, z = s * 7.f;
            vec3 c = vivid[(i + (s > 0 ? 2 : 0)) % 5];
            k.box(x, 0.35f, z, 4.2f, 2.0f, 3.f, c);
            k.box(x, 2.35f, z + s * 0.4f, 4.6f, 0.12f, 3.6f, c * 0.8f);
            for (int j = 0; j < 3; ++j)
                k.sph(x - 1.2f + 1.2f * j, 2.0f, z, 0.45f, 0.4f, 0.45f, vivid[(i + j) % 5], 6, 4);
        }
    k.post(Lines{ "BANDAR BAZAR" }, 0, 21.f, 0, 20);
    k.post(Lines{ "BANDAR BAZAR" }, 0, -21.f, PI, 20);
}

// --- Sylhet Court ---------------------------------------------------------------------------------
static void buildCourt(Kit& k, const Landmark&) {
    k.lawn(56, 36);
    k.box(0, 0.3f, -1, 46, 0.9f, 30, STONE);
    k.box(0, 1.2f, -4, 40, 10, 20, CREAM, MAT_WINDOWS);
    k.box(0, 11.2f, -4, 41, 0.6f, 21, STONE);
    for (int i = 0; i < 6; ++i) k.cyl(-10.f + 4.f * i, 1.2f, 10.4f, 0.7f, 9.4f, WHITE, 14);
    k.box(0, 10.6f, 9.4f, 26, 1.2f, 6.8f, CREAM);
    mat4 Pg = k.P * translate(mat4(1.f), vec3(0, 11.8f, 9.4f)) * rotate(mat4(1.f), PI / 2, vec3(0, 1, 0));
    k.b.gable(vec3(0), vec3(6.8f, 3.6f, 26.f), CREAM, Pg);
    k.cyl(0, 11.8f, -4, 4.f, 3.f, CREAM, 16);
    k.sph(0, 14.8f, -4, 4.2f, 3.4f, 4.2f, PATINA, 16, 8);
    k.cyl(0, 18.0f, -4, 0.15f, 2.5f, GOLD, 6);
    for (int s = -1; s <= 1; s += 2) { k.tree(s * 25.f, -14); k.tree(s * 25.f, 10); }
    k.flag(22, 15);
    k.post(Lines{ "SYLHET COURT" }, 0, 20.5f, 0, 16);
    k.post(Lines{ "SYLHET COURT" }, 0, -19.5f, PI, 16);
}

// --- Deputy Commissioner office -----------------------------------------------------------------
static void buildDC(Kit& k, const Landmark&) {
    k.lawn(58, 30);
    k.box(0, 0.3f, -2, 48, 11.4f, 14, vec3(0.88f, 0.76f, 0.56f), MAT_WINDOWS);
    k.box(0, 11.7f, -2, 49, 0.5f, 15, STONE);
    for (int i = 0; i < 11; ++i) k.cyl(-20.f + 4.f * i, 0.3f, 6.4f, 0.45f, 4.2f, WHITE, 10);
    k.box(0, 4.5f, 6.2f, 46, 0.5f, 2.6f, CREAM);
    k.box(0, 12.2f, -2, 9, 9, 9, CREAM, MAT_WINDOWS);
    k.box(-2, 15.f, 2.6f, 4, 4, 0.3f, vec3(1.f, 0.98f, 0.85f), MAT_EMISSIVE);
    k.cone(0, 21.2f, -2, 6.4f, 6.f, vec3(0.55f, 0.25f, 0.2f), 4, PI / 4);
    k.flag(22, 13);
    for (int s = -1; s <= 1; s += 2) { k.tree(s * 26.f, -11); k.tree(s * 26.f, 12); }
    k.post(Lines{ "DC OFFICE", "SYLHET" }, 0, 17.f, 0, 11);
    k.post(Lines{ "DC OFFICE", "SYLHET" }, 0, -17.f, PI, 11);
}

// --- Sylhet City Corporation --------------------------------------------------------------------
static void buildCityCorp(Kit& k, const Landmark&) {
    k.lawn(56, 34);
    k.box(0, 0.3f, 0, 44, 7.6f, 26, vec3(0.75f, 0.76f, 0.80f), MAT_WINDOWS);
    k.box(0, 7.9f, -4, 30, 22, 18, GLASSB, MAT_WINDOWS);
    k.box(0, 4.2f, 15, 14, 0.4f, 6, STONE);
    for (int s = -1; s <= 1; s += 2) k.cyl(s * 6.5f, 0.3f, 17.f, 0.35f, 3.9f, WHITE, 8);
    k.sign(Lines{ "SYLHET CITY", "CORPORATION" }, 0, 20.f, 5.15f, 0, 22);
    k.sign(Lines{ "SYLHET CITY", "CORPORATION" }, 0, 20.f, -13.15f, PI, 22);
    k.flag(-24, 16); k.flag(24, 16);
    k.tree(-26, -14); k.tree(26, -14);
}

// --- BRTA office ------------------------------------------------------------------------------------
static void buildBRTA(Kit& k, const Landmark&) {
    k.lawn(46, 30);
    k.box(0, 0.3f, -9, 30, 7.6f, 10, vec3(0.93f, 0.93f, 0.86f), MAT_WINDOWS);
    k.box(0, 3.4f, -3.95f, 30.1f, 0.7f, 0.1f, vec3(0.1f, 0.3f, 0.7f));
    k.slab(0, 5, 42, 20, ASPHALT, cfg::LAWN_TOP, 0.05f);
    for (int i = 0; i < 6; ++i) k.box(-12.f + 5.f * i, 0.35f, 8, 0.15f, 0.02f, 5, WHITE);
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2) k.cyl(14.f + sx * 4.5f, 0.35f, 6.f + sz * 5.5f, 0.2f, 4.2f, STONE, 6);
    k.box(14, 4.5f, 6, 10.5f, 0.4f, 13, vec3(0.2f, 0.4f, 0.7f));
    k.vehicle(VType::Car, -16, 6, 0, vec3(0.9f), vec3(0)); k.vehicle(VType::Car, -11, 6, 0, vec3(0.7f, 0.1f, 0.1f), vec3(0));
    k.vehicle(VType::Car, -6, 6, 0, vec3(0.12f, 0.25f, 0.6f), vec3(0));
    k.vehicle(VType::Bus, 2, 8, 0, vec3(0.9f, 0.4f, 0.1f), vec3(0));
    k.vehicle(VType::CNG, 14, 6, 0, vec3(0.1f, 0.55f, 0.25f), vec3(0.08f));
    k.vehicle(VType::Bike, -17, 12, PI, vec3(0.8f, 0.2f, 0.2f), vec3(0.2f, 0.4f, 0.8f));
    k.tree(-22, 13); k.tree(22, 13); k.flag(-22, -2, 10);
    k.post(Lines{ "BRTA", "SYLHET" }, 0, 17.f, 0, 9);
}

// --- school (two variants) ----------------------------------------------------------------------
static void buildSchool(Kit& k, const Landmark& lm) {
    vec3 wall = lm.variant == 0 ? vec3(0.93f, 0.85f, 0.50f) : vec3(0.93f, 0.78f, 0.78f);
    k.lawn(52, 34);
    k.box(-3, 0.3f, -10, 38, 11.2f, 9, wall, MAT_WINDOWS);
    k.box(-14, 0.3f, 2, 10, 11.2f, 20, wall, MAT_WINDOWS);
    k.box(-3, 11.5f, -10, 39, 0.4f, 10, vec3(0.6f, 0.3f, 0.25f));
    k.box(-14, 11.5f, 2, 11, 0.4f, 21, vec3(0.6f, 0.3f, 0.25f));
    k.box(-14, 0.3f, 12.4f, 6, 3.4f, 1.5f, CREAM);
    k.pitch(12, 7, 24, 16);
    k.flag(4, -1);
    k.wall(-26, -17, 26, -17); k.wall(-26, -17, -26, 17); k.wall(26, -17, 26, 17);
    k.wall(-26, 17, -5, 17); k.wall(5, 17, 26, 17);
    for (int s = -1; s <= 1; s += 2) k.box(s * 5.5f, 0.3f, 17, 1.4f, 3.5f, 1.4f, BRICK);
    for (int i = 0; i < 4; ++i) k.tree(-22.f + 3.f, 8.f + i * 2.f, 0.8f);
    Lines txt = lm.variant == 0 ? Lines{ "PILOT HIGH", "SCHOOL" } : Lines{ "GIRLS' HIGH", "SCHOOL" };
    k.post(txt, 0, 19.5f, 0, 12);
}

// --- MAG Osmani Medical College Hospital --------------------------------------------------------
static void buildHospital(Kit& k, const Landmark&) {
    k.lawn(66, 34);
    k.box(0, 0.3f, -6, 34, 25.6f, 14, WHITE, MAT_WINDOWS);
    for (int s = -1; s <= 1; s += 2) k.box(s * 24.f, 0.3f, -2, 14, 14.8f, 22, vec3(0.9f, 0.92f, 0.95f), MAT_WINDOWS);
    k.box(-3, 20.0f, 1.15f, 6, 1.8f, 0.3f, RED, MAT_EMISSIVE);
    k.box(-0.9f, 17.9f, 1.15f, 1.8f, 6, 0.3f, RED, MAT_EMISSIVE);
    k.box(0, 4.2f, 7, 16, 0.4f, 10, STONE);
    for (int s = -1; s <= 1; s += 2) k.cyl(s * 7.f, 0.3f, 10.5f, 0.3f, 3.9f, WHITE, 8);
    k.box(12, 0.35f, 11, 2.2f, 2.2f, 5, WHITE);
    k.box(12, 1.3f, 11, 2.25f, 0.35f, 5.05f, RED);
    k.cyl(0, 25.9f, -6, 6, 0.2f, vec3(0.28f), 24);
    k.b.text("H", k.P * translate(mat4(1.f), vec3(-1.25f, 26.15f, -4.25f)) * rotate(mat4(1.f), -PI / 2, vec3(1, 0, 0)),
             0.5f, vec3(1.f, 0.9f, 0.1f));
    k.tree(-30, 14); k.tree(30, 14); k.tree(-30, -14); k.tree(30, -14);
    k.post(Lines{ "MEDICAL COLLEGE", "HOSPITAL" }, 0, 17.5f, 0, 22);
}

void buildLandmark(Batch& b, const Landmark& lm, std::mt19937& rng) {
    Kit k(b, lm, rng);
    switch (lm.type) {
    case LType::Point:    buildPoint(k, lm); break;
    case LType::Bridge:   buildBridge(k, lm); break;
    case LType::Market:   buildMarket(k, lm); break;
    case LType::Clock:    buildClock(k, lm); break;
    case LType::Circuit:  buildCircuitHouse(k, lm); break;
    case LType::Mall:     buildMall(k, lm); break;
    case LType::Court:    buildCourt(k, lm); break;
    case LType::DCOffice: buildDC(k, lm); break;
    case LType::CityCorp: buildCityCorp(k, lm); break;
    case LType::BRTA:     buildBRTA(k, lm); break;
    case LType::School:   buildSchool(k, lm); break;
    case LType::Hospital: buildHospital(k, lm); break;
    default:              buildCampus(b, lm, rng); break;
    }
}

// Flat text plates above the city for the top-down map.
void buildTopLabels(Batch& L, const std::vector<Landmark>& lms, float px) {
    for (const auto& lm : lms) {
        vec3 bg;
        switch (lm.cat) {
        case LCat::Junction:   bg = vec3(0.75f, 0.12f, 0.12f); break;
        case LCat::Landmark:   bg = vec3(0.05f, 0.45f, 0.45f); break;
        case LCat::Admin:      bg = vec3(0.42f, 0.2f, 0.6f); break;
        case LCat::School:     bg = vec3(0.85f, 0.5f, 0.1f); break;
        case LCat::Health:     bg = vec3(0.8f, 0.15f, 0.35f); break;
        default:               bg = vec3(0.1f, 0.3f, 0.75f); break;
        }
        float tw = (float)lm.label.size() * 6.f * px - px;
        L.box(vec3(lm.pos.x, 70.f, lm.pos.y), vec3(tw + 2.f * px, 0.4f, 9.f * px), bg, mat4(1.f), MAT_EMISSIVE);
        mat4 P = translate(mat4(1.f), vec3(lm.pos.x - tw / 2, 70.6f, lm.pos.y + 3.5f * px)) *
                 rotate(mat4(1.f), -PI / 2, vec3(1, 0, 0));
        L.text(lm.label, P, px, vec3(1.f), MAT_EMISSIVE);
    }
}
