#include "Walkers.h"
#include "Config.h"
#include "Props.h"
#include "RoadNet.h"
#include <algorithm>
#include <cmath>

using namespace glm;

void Walkers::init(unsigned seed) {
    std::mt19937 r(seed);
    using props::rf; using props::ri;
    w.clear();
    static const vec3 shirts[] = { vec3(0.9f, 0.9f, 0.88f), vec3(0.15f, 0.35f, 0.7f), vec3(0.8f, 0.15f, 0.2f), vec3(0.2f, 0.55f, 0.3f),
                                   vec3(0.95f, 0.75f, 0.15f), vec3(0.55f, 0.2f, 0.55f), vec3(0.1f, 0.1f, 0.12f), vec3(0.85f, 0.5f, 0.2f) };
    static const vec3 skins[] = { vec3(0.76f, 0.58f, 0.44f), vec3(0.62f, 0.45f, 0.33f), vec3(0.50f, 0.36f, 0.26f) };
    float kx = grid::X[0];
    auto make = [&](vec2 a, vec2 b, float ground) {
        Walker k;
        k.a = a; k.b = b;
        k.t = rf(r, 0.f, 1.f);
        k.dir = (ri(r, 0, 1) ? 1.f : -1.f);
        k.speed = rf(r, 1.0f, 1.7f);
        k.ground = ground;
        k.phase = rf(r, 0.f, 6.28f);
        k.shirt = shirts[ri(r, 0, 7)];
        k.trousers = ri(r, 0, 1) ? vec3(0.15f, 0.15f, 0.22f) : vec3(0.35f, 0.3f, 0.22f);
        k.skin = skins[ri(r, 0, 2)];
        k.style = ri(r, 0, 3);
        k.height = rf(r, 1.55f, 1.8f);
        w.push_back(k);
    };
    // Keane Bridge: from the Bandar Bazar plaza (south) across the river to the far-bank plaza
    for (int i = 0; i < 46; ++i) {
        float lane = rf(r, -2.8f, 2.8f);
        make(vec2(kx + lane, -109.f), vec2(kx + lane, -224.f), 0.16f);
    }
    // riverside promenade along the city bank (stone strip z = -135.6 ... -131.6)
    for (int i = 0; i < 18; ++i) {
        float z = rf(r, -134.8f, -133.4f);
        make(vec2(-215.f, z), vec2(95.f, z), 0.02f);
    }
    // Ali Amjad's Clock plaza
    for (int i = 0; i < 8; ++i) {
        float x = -142.f + (ri(r, 0, 1) ? 1.f : -1.f) * rf(r, 5.5f, 9.f);
        make(vec2(x, -127.f), vec2(x, -115.f), 0.30f);
    }
}

void Walkers::update(float dtRaw) {
    float dt = std::min(dtRaw, 0.05f);
    time += dt;
    for (auto& k : w) {
        float len = std::max(1.f, length(k.b - k.a));
        k.t += k.dir * k.speed * dt / len;
        if (k.t > 1.f) { k.t = 1.f; k.dir = -1.f; }
        if (k.t < 0.f) { k.t = 0.f; k.dir = 1.f; }
    }
}

void Walkers::build(Batch& b) const {
    for (const auto& k : w) {
        vec2 p = mix(k.a, k.b, k.t);
        vec2 d = normalize(k.b - k.a) * k.dir;
        float yaw = std::atan2(d.x, d.y);
        float swing = std::sin(time * k.speed * 5.5f + k.phase) * 0.55f;
        float gy = k.ground;
        float H = k.height, legH = H * 0.47f, torsoH = H * 0.33f;
        mat4 P = place(vec3(p.x, gy, p.y), yaw);
        // legs
        for (int s = -1; s <= 1; s += 2) {
            mat4 L = P * translate(mat4(1.f), vec3(s * 0.11f, legH, 0.f)) * rotate(mat4(1.f), s * swing, vec3(1, 0, 0));
            b.box(vec3(0, -legH, 0), vec3(0.17f, legH, 0.2f), k.trousers, L);
        }
        // torso, arms, head
        b.box(vec3(0, legH, 0), vec3(0.44f, torsoH, 0.24f), k.shirt, P);
        for (int s = -1; s <= 1; s += 2) {
            mat4 A = P * translate(mat4(1.f), vec3(s * 0.28f, legH + torsoH - 0.05f, 0.f)) * rotate(mat4(1.f), -s * swing, vec3(1, 0, 0));
            b.box(vec3(0, -torsoH * 0.9f, 0), vec3(0.11f, torsoH * 0.9f, 0.12f), k.shirt * 0.9f, A);
        }
        b.sphere(vec3(0, legH + torsoH + 0.15f, 0), vec3(0.13f), k.skin, P, 8, 5);
        if (k.style == 1) {            // umbrella
            b.cyl(vec3(0.3f, legH, 0.f), 0.015f, torsoH + 0.7f, vec3(0.2f), P, 5);
            b.cone(vec3(0.3f, legH + torsoH + 0.65f, 0.f), 0.65f, 0.25f, vec3(0.8f - 0.5f * k.shirt.x, 0.2f, 0.3f + 0.4f * k.shirt.z), P, 10);
        } else if (k.style == 2) {     // shoulder bag
            b.box(vec3(0.3f, legH + 0.1f, 0.f), vec3(0.14f, 0.28f, 0.22f), vec3(0.35f, 0.22f, 0.12f), P);
        } else if (k.style == 3) {     // cap
            b.box(vec3(0, legH + torsoH + 0.22f, 0.f), vec3(0.27f, 0.07f, 0.29f), vec3(0.95f, 0.95f, 0.9f), P);
        }
    }
}
