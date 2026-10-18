#include "World.h"
#include "Config.h"
#include "Props.h"
#include <cmath>

using namespace glm;
using props::rf;
using props::ri;

namespace {
const vec3 ROAD_C(0.21f, 0.21f, 0.23f), WALK_C(0.66f, 0.65f, 0.62f), MARK_C(0.95f, 0.93f, 0.80f);
const vec3 PALETTE[] = { vec3(0.86f, 0.80f, 0.68f), vec3(0.78f, 0.62f, 0.55f), vec3(0.70f, 0.72f, 0.75f),
                         vec3(0.90f, 0.87f, 0.80f), vec3(0.65f, 0.70f, 0.60f), vec3(0.82f, 0.66f, 0.45f),
                         vec3(0.72f, 0.55f, 0.50f), vec3(0.80f, 0.80f, 0.78f) };
const vec3 AWNING[] = { vec3(0.85f, 0.15f, 0.15f), vec3(0.15f, 0.45f, 0.80f), vec3(0.95f, 0.70f, 0.10f),
                        vec3(0.20f, 0.65f, 0.35f), vec3(0.85f, 0.35f, 0.65f), vec3(0.95f, 0.50f, 0.10f) };
const float PI = 3.14159265f;

vec2 leftOf(const vec2& d) { return vec2(d.y, -d.x); }

bool overlapsLandmark(const std::vector<Landmark>& L, float cx, float cz, float w, float d) {
    for (const auto& l : L) {
        if (l.size.x <= 0) continue;
        if (std::fabs(cx - l.pos.x) < (w + l.size.x) * 0.5f + 2 && std::fabs(cz - l.pos.y) < (d + l.size.y) * 0.5f + 2)
            return true;
    }
    return false;
}

// one shop-house, front (local +Z) facing `face`
void addShop(Batch& b, std::mt19937& r, float cx, float cz, float w, float d, vec2 face) {
    float distC = std::sqrt(cx * cx + cz * cz);                  // distance to Zindabazar
    int floors = (distC < 140.f) ? ri(r, 4, 9) : ri(r, 2, 5);
    float h = floors * 3.6f + 0.4f;
    vec3 col = PALETTE[ri(r, 0, 7)];
    mat4 P = place(vec3(cx, 0.2f, cz), std::atan2(face.x, face.y));
    b.box(vec3(0), vec3(w, h, d), col, P, MAT_WINDOWS);
    b.box(vec3(0, h, 0), vec3(w + 0.3f, 0.5f, d + 0.3f), col * 0.85f, P);
    b.box(vec3(0, 3.0f, d * 0.5f + 0.8f), vec3(w - 0.8f, 0.15f, 1.7f), AWNING[ri(r, 0, 5)], P);
    if (rf(r, 0, 1) < 0.6f)
        b.box(vec3(0, 3.3f, d * 0.5f + 0.06f), vec3(w * 0.7f, 0.9f, 0.1f), AWNING[ri(r, 0, 5)] * 0.9f, P, MAT_EMISSIVE);
    if (rf(r, 0, 1) < 0.5f)
        b.cyl(vec3(rf(r, -w / 3, w / 3), h + 0.5f, rf(r, -d / 3, d / 3)), 1.0f, 1.8f, vec3(0.3f, 0.45f, 0.6f), P, 10);
    if (rf(r, 0, 1) < 0.3f) {
        vec3 bc = AWNING[ri(r, 0, 5)];
        b.box(vec3(0, h + 0.5f, d * 0.3f), vec3(0.2f, 1.2f, 0.2f), vec3(0.3f), P);
        b.box(vec3(0, h + 1.7f, d * 0.3f), vec3(w * 0.8f, 2.6f, 0.3f), bc, P, MAT_EMISSIVE);
    }
}

void ringBuildings(Batch& b, std::mt19937& r, float x0, float x1, float z0, float z1,
                   const std::vector<Landmark>& L) {
    const float setback = 10.f, maxD = 13.f;
    auto row = [&](bool alongX, float fixed, float start, float end, vec2 face, bool nearSideMin) {
        float pos = start;
        while (pos + 6.f < end) {
            float w = rf(r, 7.f, 13.f), d = rf(r, 10.f, maxD);
            if (pos + w > end) w = end - pos;
            if (w < 5.f) break;
            float along = pos + w / 2;
            float across = nearSideMin ? fixed + d / 2 : fixed - d / 2;
            float cx = alongX ? along : across, cz = alongX ? across : along;
            float bw = w, bd = d;
            if (!overlapsLandmark(L, cx, cz, alongX ? bw : bd, alongX ? bd : bw)) addShop(b, r, cx, cz, bw, bd, face);
            pos += w + rf(r, 0.4f, 1.2f);
        }
    };
    float ix0 = x0 + setback, ix1 = x1 - setback, iz0 = z0 + setback, iz1 = z1 - setback;
    row(true, iz0, ix0, ix1, vec2(0, -1), true);                       // north side
    row(true, iz1, ix0, ix1, vec2(0, 1), false);                       // south side
    row(false, ix0, iz0 + maxD + 1, iz1 - maxD - 1, vec2(-1, 0), true);   // west side
    row(false, ix1, iz0 + maxD + 1, iz1 - maxD - 1, vec2(1, 0), false);   // east side
}
}

void buildWorld(World& w) {
    std::mt19937 rng(20261018);
    w.roads = buildSylhetRoads();
    w.landmarks = makeLandmarks();
    Batch& b = w.stat;
    const auto& N = w.roads.nodes;

    // --- ground, river, distant hills ---------------------------------------------------------
    const vec3 grass(0.36f, 0.52f, 0.28f), water(0.20f, 0.42f, 0.60f);
    b.box(vec3(110, -3, 632), vec3(3600, 3, 1535), grass);                 // south of the river
    b.box(vec3(110, -3, -800), vec3(3600, 3, 1200), grass);                // north of the river
    // The Surma flows in a deep channel (water surface y = -7) so steamers can pass under the bridge decks.
    b.box(vec3(110, -9.5f, -167.5f), vec3(3600, 2.5f, 65), water);
    const vec3 wallC(0.56f, 0.56f, 0.53f), wallDark(0.32f, 0.34f, 0.30f), copingC(0.70f, 0.69f, 0.66f);
    b.box(vec3(110, -9.5f, -133.6f), vec3(3600, 9.52f, 4.f), wallC);        // city bank embankment (+ 4 m stone promenade)
    b.box(vec3(110, -9.5f, -201.9f), vec3(3600, 9.52f, 4.f), wallC);        // far bank embankment
    b.box(vec3(110, -9.5f, -135.65f), vec3(3600, 3.0f, 0.1f), wallDark);    // weathered waterline band
    b.box(vec3(110, -9.5f, -199.85f), vec3(3600, 3.0f, 0.1f), wallDark);
    b.box(vec3(110, 0.f, -135.45f), vec3(3600, 0.04f, 0.4f), copingC);      // coping stone along the edge
    b.box(vec3(110, 0.f, -200.05f), vec3(3600, 0.04f, 0.4f), copingC);
    // ghat: stone steps down to the water beside Ali Amjad's Clock
    for (int k = 0; k < 10; ++k) {
        float depth = 0.9f * (k + 1), top = -0.65f * (k + 1);
        b.box(vec3(-143.f, -9.5f, -135.6f - depth / 2), vec3(14.f, top + 9.5f, depth),
              (k % 2) ? vec3(0.66f, 0.64f, 0.60f) : vec3(0.60f, 0.59f, 0.55f));
    }
    // riverside railings (broken where bridges and the ghat meet the bank)
    {
        auto rail = [&](float x0, float x1, float z) {
            if (x1 - x0 < 1.f) return;
            b.box(vec3((x0 + x1) / 2, 1.0f, z), vec3(x1 - x0, 0.1f, 0.1f), vec3(0.35f, 0.36f, 0.38f));
            for (float x = x0; x <= x1 + 0.01f; x += 4.f) b.box(vec3(x, 0.f, z), vec3(0.12f, 1.05f, 0.12f), vec3(0.35f, 0.36f, 0.38f));
        };
        rail(-220.f, -151.f, -135.3f); rail(-135.f, -126.f, -135.3f); rail(-114.f, 99.f, -135.3f);
        rail(121.f, 339.f, -135.3f);   rail(361.f, 420.f, -135.3f);
        rail(-220.f, -126.f, -200.3f); rail(-114.f, 99.f, -200.3f); rail(121.f, 339.f, -200.3f); rail(361.f, 420.f, -200.3f);
        for (float x = -204.f; x <= 92.f; x += 24.f) {
            if (std::fabs(x + 120.f) < 14.f || (x > -152.f && x < -134.f)) continue;
            props::lamp(b, vec3(x, 0.02f, -134.6f), 3.14159265f);   // lamp arm points over the water
        }
        for (float x : { -205.f, -190.f, -175.f, 30.f, 52.f, 80.f })
            props::tree(b, vec3(x, 0.02f, -132.5f), rf(rng, 0.9f, 1.2f), rng);
    }
    for (int i = 0; i < 16; ++i) {
        float a = 2 * PI * i / 16 + rf(rng, -0.1f, 0.1f), dist = rf(rng, 650.f, 900.f);
        b.sphere(vec3(110 + std::cos(a) * dist, -6, 40 + std::sin(a) * dist),
                 vec3(rf(rng, 60, 120), rf(rng, 18, 38), rf(rng, 60, 120)), vec3(0.30f, 0.45f, 0.25f), mat4(1.f), 12, 6);
    }

    // --- roads ----------------------------------------------------------------------------------
    for (const auto& sg : w.roads.segs) {
        vec2 A = N[sg.first].p, B = N[sg.second].p, d = B - A;
        float len = length(d);
        vec2 u = d / len, m = (A + B) * 0.5f;
        mat4 P = place(vec3(m.x, 0, m.y), std::atan2(u.x, u.y));
        bool bridge = w.roads.isBridge(sg.first, sg.second);
        b.box(vec3(0), vec3(cfg::ROAD_W, cfg::ROAD_TOP, len + cfg::ROAD_W), ROAD_C, P);
        for (int s = -1; s <= 1; s += 2) b.box(vec3(s * 7.5f, 0, 0), vec3(3.f, cfg::WALK_TOP, len - cfg::ROAD_W), WALK_C, P);
        for (float t = -len / 2 + 10; t < len / 2 - 10; t += 7.f)
            b.box(vec3(0, cfg::ROAD_TOP, t), vec3(0.25f, 0.04f, 3.f), MARK_C, P);
        // street lamps (alternate sides) and trees
        vec2 l = leftOf(u);
        for (int k = 0;; ++k) {
            float t = -len / 2 + 16 + 32.f * k;
            if (t > len / 2 - 14) break;
            int side = (k % 2) ? 1 : -1;
            vec2 p = m + u * t + l * (side * 7.f), toRoad = -l * (float)side;
            props::lamp(b, vec3(p.x, cfg::WALK_TOP, p.y), std::atan2(toRoad.x, toRoad.y));
        }
        if (!bridge)
            for (float t = -len / 2 + 20; t < len / 2 - 14; t += 22.f)
                if (rf(rng, 0, 1) < 0.55f) {
                    int side = ri(rng, 0, 1) ? 1 : -1;
                    vec2 p = m + u * t + l * (side * 7.6f);
                    props::tree(b, vec3(p.x, cfg::WALK_TOP, p.y), rf(rng, 0.8f, 1.1f), rng);
                }
    }

    // zebra crossings + stop lines at every junction arm
    for (size_t i = 0; i < N.size(); ++i) {
        if (N[i].nbr.size() < 3) continue;
        for (int nb : N[i].nbr) {
            vec2 u = normalize(N[nb].p - N[i].p), c = N[i].p + u * 7.8f;
            mat4 P = place(vec3(c.x, 0, c.y), std::atan2(u.x, u.y));
            for (int s = -5; s <= 5; ++s) b.box(vec3(s * 1.0f, cfg::ROAD_TOP, 0), vec3(0.5f, 0.04f, 2.4f), MARK_C, P);
            b.box(vec3(-3.2f, cfg::ROAD_TOP, 1.8f), vec3(5.4f, 0.04f, 0.35f), MARK_C, P);
        }
    }

    // --- city blocks -----------------------------------------------------------------------------
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 4; ++c) {
            float x0 = grid::X[c], x1 = grid::X[c + 1], z0 = grid::Z[r], z1 = grid::Z[r + 1];
            bool campus = false;
            for (const auto& l : w.landmarks)
                if (isCampus(l.type) && l.pos.x > x0 && l.pos.x < x1 && l.pos.y > z0 && l.pos.y < z1) campus = true;
            vec3 plate = campus ? vec3(0.38f, 0.55f, 0.32f) : vec3(0.58f, 0.57f, 0.54f);
            b.box(vec3((x0 + x1) / 2, 0, (z0 + z1) / 2), vec3(x1 - x0 - 18, cfg::PLATE_TOP, z1 - z0 - 18), plate);
            if (!campus) ringBuildings(b, rng, x0, x1, z0, z1, w.landmarks);
        }

    // --- landmarks ---------------------------------------------------------------------------------
    for (const auto& lm : w.landmarks) buildLandmark(b, lm, rng);

    // --- toll plazas (booth, canopy, fare board); barrier arms + income boards are dynamic (VehicleSystem) ------
    for (const auto& g : w.roads.tolls) {
        float yaw = std::atan2(g.dir.x, g.dir.y);
        vec2 l = leftOf(g.dir);
        auto W2 = [&](float x, float z) { return g.p + l * x + g.dir * z; };
        mat4 P = place(vec3(g.p.x, cfg::ROAD_TOP, g.p.y), yaw);   // +X = driver's left, +Z = travel direction
        b.box(vec3(0.f, 0.f, -3.f), vec3(2.2f, 0.25f, 12.f), vec3(0.78f), P);                        // median island
        b.box(vec3(0.f, 0.25f, 0.f), vec3(1.6f, 2.6f, 2.4f), vec3(0.93f, 0.90f, 0.80f), P);          // booth
        b.box(vec3(0.f, 0.15f + 1.35f, -1.22f), vec3(1.2f, 0.9f, 0.05f), vec3(0.36f, 0.55f, 0.70f), P);
        b.box(vec3(0.82f, 0.15f + 1.35f, 0.f), vec3(0.05f, 0.9f, 1.4f), vec3(0.36f, 0.55f, 0.70f), P);
        b.box(vec3(0.f, 2.85f, 0.f), vec3(2.0f, 0.2f, 2.8f), vec3(0.2f, 0.25f, 0.5f), P);            // booth roof
        b.box(vec3(3.1f, 4.2f, 0.f), vec3(8.4f, 0.4f, 5.f), vec3(0.15f, 0.35f, 0.62f), P);           // canopy
        b.cyl(vec3(0.f, 2.95f, 0.f), 0.12f, 1.25f, vec3(0.5f), P, 8);
        b.cyl(vec3(7.0f, 0.f, 0.f), 0.14f, 4.2f, vec3(0.5f), P, 8);
        for (int i = 0; i < 6; ++i)                                                                    // hazard stripes on the island
            b.box(vec3(0.f, 0.26f, -8.5f + 1.4f * i), vec3(2.0f, 0.02f, 0.7f), (i % 2) ? vec3(0.95f) : vec3(0.9f, 0.75f, 0.1f), P);
        b.box(vec3(3.5f, 0.04f, -1.0f), vec3(5.5f, 0.04f, 0.3f), vec3(0.95f), P);                     // stop line (cosmetic)
        b.box(vec3(7.0f, 0.f, 0.f), vec3(0.5f, 1.15f, 0.5f), vec3(0.8f, 0.1f, 0.1f), P);               // barrier end post
        vec2 sc = W2(3.1f, -2.7f);
        props::sign(b, std::vector<std::string>{ "TOLL PLAZA" }, vec3(sc.x, cfg::ROAD_TOP + 3.9f, sc.y), yaw + 3.14159265f, 6.f,
                    vec3(0.05f, 0.18f, 0.40f), vec3(1.f, 1.f, 0.95f));
        vec2 fb = W2(9.0f, -12.f);
        props::signPost(b, std::vector<std::string>{ "TOLL TK", "BIKE 10", "CNG 20", "CAR 50", "BUS 100" },
                        vec3(fb.x, cfg::WALK_TOP, fb.y), yaw + 3.14159265f, 7.f);
    }

    // --- scattered trees outside the city ----------------------------------------------------------
    for (int i = 0; i < 260; ++i) {
        float x = rf(rng, -500, 700), z = rf(rng, -650, 550);
        bool inCity = x > -150 && x < 380 && z > -300 && z < 210;
        bool inRiver = z > -215 && z < -120;
        if (!inCity && !inRiver) props::tree(b, vec3(x, 0, z), rf(rng, 1.f, 1.6f), rng);
    }
}
