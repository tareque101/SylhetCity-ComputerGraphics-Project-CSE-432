#pragma once
// Small helper used by the landmark builders: coordinates are relative to the landmark centre.
#include "Batch.h"
#include "Config.h"
#include "Landmarks.h"
#include "Props.h"
#include "Vehicles.h"

namespace col {
static const glm::vec3 CREAM(0.90f, 0.86f, 0.74f), WHITE(0.94f, 0.94f, 0.92f), STONE(0.66f, 0.64f, 0.60f),
    LAWN(0.36f, 0.56f, 0.30f), PATINA(0.45f, 0.68f, 0.60f), GOLD(0.85f, 0.70f, 0.25f),
    BRICK(0.62f, 0.30f, 0.24f), NAVY(0.07f, 0.18f, 0.34f), GLASSB(0.36f, 0.55f, 0.70f),
    ASPHALT(0.25f, 0.25f, 0.27f), DARKG(0.2f, 0.2f, 0.22f), PATHC(0.72f, 0.70f, 0.66f),
    WATER(0.25f, 0.50f, 0.70f), RED(0.86f, 0.12f, 0.14f), STEEL(0.45f, 0.52f, 0.58f);
}

struct Kit {
    Batch& b;
    glm::vec3 C;
    glm::mat4 P;
    std::mt19937& r;

    Kit(Batch& bb, const Landmark& lm, std::mt19937& rr)
        : b(bb), C(lm.pos.x, 0.f, lm.pos.y), P(place(C, 0.f)), r(rr) {}

    void box(float x, float y, float z, float w, float h, float d, const glm::vec3& c, float m = MAT_PLAIN) {
        b.box(glm::vec3(x, y, z), glm::vec3(w, h, d), c, P, m);
    }
    void cyl(float x, float y, float z, float rad, float h, const glm::vec3& c, int seg = 12) {
        b.cyl(glm::vec3(x, y, z), rad, h, c, P, seg);
    }
    void cone(float x, float y, float z, float rad, float h, const glm::vec3& c, int seg = 10, float rotY = 0.f) {
        b.cone(glm::vec3(0), rad, h, c,
               P * glm::translate(glm::mat4(1.f), glm::vec3(x, y, z)) * glm::rotate(glm::mat4(1.f), rotY, glm::vec3(0, 1, 0)), seg);
    }
    void sph(float x, float y, float z, float rx, float ry, float rz, const glm::vec3& c, int sl = 12, int st = 7) {
        b.sphere(glm::vec3(x, y, z), glm::vec3(rx, ry, rz), c, P, sl, st);
    }
    void tree(float x, float z, float s = 1.f) { props::tree(b, C + glm::vec3(x, cfg::LAWN_TOP, z), s, r); }
    void sign(const std::vector<std::string>& L, float x, float y, float z, float yaw, float w,
              const glm::vec3& bg = col::NAVY) {
        props::sign(b, L, C + glm::vec3(x, y, z), yaw, w, bg, glm::vec3(1.f, 1.f, 0.95f));
    }
    void post(const std::vector<std::string>& L, float x, float z, float yaw, float w) {
        props::signPost(b, L, C + glm::vec3(x, cfg::LAWN_TOP, z), yaw, w);
    }
    void lawn(float w, float d, const glm::vec3& c = col::LAWN) { box(0, 0.2f, 0, w, 0.1f, d, c); }
    void slab(float x, float z, float w, float d, const glm::vec3& c, float y = cfg::LAWN_TOP, float h = 0.04f) {
        box(x, y, z, w, h, d, c);
    }
    void flag(float x, float z, float h = 12.f, float yaw = 0.f) {
        props::flag(b, C + glm::vec3(x, cfg::LAWN_TOP, z), h, yaw);
    }
    void wall(float x0, float z0, float x1, float z1, float h = 2.4f,
              const glm::vec3& c = glm::vec3(0.85f, 0.80f, 0.70f)) {
        props::wall(b, glm::vec2(C.x + x0, C.z + z0), glm::vec2(C.x + x1, C.z + z1), h, 0.5f, c);
    }
    void vehicle(VType t, float x, float z, float yaw, const glm::vec3& c1, const glm::vec3& c2, float y = 0.35f) {
        drawVehicleModel(b, t, place(C + glm::vec3(x, y, z), yaw), c1, c2);
    }
    void goal(float x, float z, float yaw) {
        glm::mat4 G = place(C + glm::vec3(x, cfg::LAWN_TOP + 0.06f, z), yaw);
        b.box(glm::vec3(-3.6f, 0, 0), glm::vec3(0.12f, 2.4f, 0.12f), col::WHITE, G);
        b.box(glm::vec3(3.6f, 0, 0), glm::vec3(0.12f, 2.4f, 0.12f), col::WHITE, G);
        b.box(glm::vec3(0, 2.4f, 0), glm::vec3(7.4f, 0.12f, 0.12f), col::WHITE, G);
    }
    // football ground with mown stripes, white lines and goals at both ends
    void pitch(float x, float z, float w, float d) {
        int n = std::max(2, (int)(w / 5.f));
        for (int i = 0; i < n; ++i)
            box(x - w / 2 + (i + 0.5f) * w / n, cfg::LAWN_TOP, z, w / n, 0.06f, d,
                (i % 2) ? glm::vec3(0.30f, 0.55f, 0.25f) : glm::vec3(0.34f, 0.60f, 0.28f));
        float y = cfg::LAWN_TOP + 0.06f;
        glm::vec3 wh(0.95f);
        box(x, y, z - d / 2, w, 0.02f, 0.2f, wh);
        box(x, y, z + d / 2, w, 0.02f, 0.2f, wh);
        box(x - w / 2, y, z, 0.2f, 0.02f, d, wh);
        box(x + w / 2, y, z, 0.2f, 0.02f, d, wh);
        box(x, y, z, 0.2f, 0.02f, d, wh);
        cyl(x, y, z, 3.0f, 0.02f, wh, 20);
        cyl(x, y + 0.01f, z, 2.8f, 0.02f, glm::vec3(0.32f, 0.57f, 0.27f), 20);
        goal(x - w / 2, z, 1.5708f);
        goal(x + w / 2, z, 1.5708f);
    }
    // Shaheed Minar style monument, facing +Z
    void minar(float x, float z) {
        box(x, cfg::LAWN_TOP, z, 12, 0.5f, 8, col::STONE);
        float hs[5] = { 4.f, 5.f, 6.f, 5.f, 4.f };
        for (int i = 0; i < 5; ++i) box(x - 3.6f + 1.8f * i, 0.8f, z, 0.9f, hs[i], 0.5f, col::WHITE);
        b.cylZ(C + glm::vec3(x, 6.5f, z - 0.5f), 2.0f, 0.2f, col::RED, glm::mat4(1.f), 20);
    }
};
