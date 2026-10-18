#include "Boats.h"
#include "Props.h"
#include <algorithm>
#include <cmath>

using namespace glm;

namespace {
const float X_MIN = -1000.f, X_MAX = 1200.f;
const float WATER_Y = -7.0f;          // matches the river channel built in World.cpp

struct Lane { float z, dir, speed; BType type; int count; };
// Lanes avoid the bridge piers (absolute z = -178 and -157) and the ghat (z > -145).
const Lane LANES[] = {
    { -195.f, +1.f, 1.6f, BType::Nouka,   10 },    // traditional country boats, east-bound
    { -189.f, -1.f, 5.5f, BType::Engine,   8 },    // engine boats, west-bound
    { -183.f, +1.f, 2.2f, BType::Nouka,    9 },
    { -168.f, +1.f, 3.2f, BType::Steamer,  6 },    // passenger steamers, east-bound
    { -150.f, -1.f, 2.8f, BType::Launch,   6 },    // double-deck launches, west-bound
};
}

void BoatSystem::init(unsigned seed) {
    std::mt19937 r(seed);
    boats.clear();
    static const vec3 pal[] = { vec3(0.55f, 0.32f, 0.18f), vec3(0.20f, 0.35f, 0.60f), vec3(0.75f, 0.25f, 0.20f),
                                vec3(0.25f, 0.50f, 0.35f), vec3(0.85f, 0.70f, 0.20f), vec3(0.45f, 0.30f, 0.50f) };
    for (const Lane& L : LANES) {
        float span = X_MAX - X_MIN, gap = span / L.count;
        for (int i = 0; i < L.count; ++i) {
            Boat b;
            b.type = L.type;
            b.z = L.z;
            b.dir = L.dir;
            b.speed = L.speed;
            b.x = X_MIN + gap * i + props::rf(r, 0.f, gap * 0.4f);   // same lane + same speed: boats never overtake
            b.c1 = pal[props::ri(r, 0, 5)];
            b.c2 = pal[props::ri(r, 0, 5)];
            b.phase = props::rf(r, 0.f, 6.28f);
            boats.push_back(b);
        }
    }
}

void BoatSystem::update(float dtRaw) {
    float dt = std::min(dtRaw, 0.05f);
    time += dt;
    for (auto& b : boats) {
        b.x += b.dir * b.speed * dt;
        if (b.x > X_MAX) b.x -= (X_MAX - X_MIN);
        if (b.x < X_MIN) b.x += (X_MAX - X_MIN);
    }
}

void BoatSystem::build(Batch& dyn) const {
    for (const auto& b : boats) {
        float bob = 0.06f * std::sin(time * 1.3f + b.phase), roll = 0.025f * std::sin(time * 0.9f + b.phase * 1.7f);
        mat4 P = place(vec3(b.x, WATER_Y + bob, b.z), b.dir > 0 ? 1.5707963f : -1.5707963f) *
                 rotate(mat4(1.f), roll, vec3(0, 0, 1));
        drawBoatModel(dyn, b.type, P, b.c1, b.c2, time);
    }
}

void drawBoatModel(Batch& b, BType t, const mat4& P, const vec3& c1, const vec3& c2, float time) {
    const vec3 dark(0.12f, 0.12f, 0.13f), white(0.94f), glass(0.20f, 0.30f, 0.38f);
    switch (t) {
    case BType::Nouka: {                    // wooden country boat with a curved prow and a bamboo-mat roof
        b.box(vec3(0, -0.2f, 0), vec3(1.5f, 0.7f, 5.4f), c1, P);
        b.box(vec3(0, -0.1f, 3.0f), vec3(1.0f, 0.6f, 1.2f), c1 * 0.9f, P);       // raised prow
        b.box(vec3(0, 0.2f, 3.7f), vec3(0.7f, 0.5f, 0.6f), c1 * 0.8f, P);
        b.box(vec3(0, -0.1f, -3.0f), vec3(1.0f, 0.6f, 1.0f), c1 * 0.9f, P);      // raised stern
        b.box(vec3(0, 0.2f, -3.5f), vec3(0.7f, 0.5f, 0.6f), c1 * 0.8f, P);
        b.box(vec3(0, 0.5f, -0.2f), vec3(1.5f, 0.08f, 2.4f), vec3(0.65f, 0.55f, 0.30f), P);   // hood
        b.box(vec3(0.f, 0.58f, -0.2f), vec3(1.3f, 0.5f, 2.2f), vec3(0.62f, 0.52f, 0.28f), P);
        b.box(vec3(0, 0.5f, -2.9f), vec3(0.1f, 1.5f, 0.1f), dark, P);            // steering oar
        break;
    }
    case BType::Engine: {                   // long-tail engine boat
        b.box(vec3(0, -0.15f, 0), vec3(1.7f, 0.6f, 6.2f), c1, P);
        b.box(vec3(0, -0.15f, 3.3f), vec3(1.1f, 0.5f, 1.0f), c1, P);
        b.box(vec3(0, 0.45f, -0.5f), vec3(1.5f, 0.06f, 3.0f), white, P);         // canopy
        for (int s = -1; s <= 1; s += 2)
            for (int z = 0; z < 2; ++z) b.box(vec3(s * 0.7f, 0.45f, -1.5f + z * 2.f), vec3(0.07f, 1.0f, 0.07f), dark, P);
        b.box(vec3(0, 0.45f, -3.3f), vec3(0.5f, 0.5f, 0.5f), dark, P);           // engine
        b.box(vec3(0, 0.2f, -5.0f), vec3(0.08f, 0.08f, 3.0f), dark, P);          // long-tail shaft
        break;
    }
    case BType::Steamer: {                  // paddle-steamer style: dark hull, two white decks, funnel (top <= 5.5 m: fits under the bridges)
        b.box(vec3(0, -0.3f, 0), vec3(4.4f, 1.8f, 17.f), vec3(0.15f, 0.20f, 0.33f), P);
        b.box(vec3(0, -0.3f, 8.6f), vec3(3.0f, 1.8f, 2.4f), vec3(0.15f, 0.20f, 0.33f), P);   // bow
        b.box(vec3(0, 1.5f, -0.5f), vec3(4.0f, 1.2f, 14.f), white, P, MAT_WINDOWS);          // main deck
        b.box(vec3(0, 2.7f, -1.5f), vec3(3.5f, 1.0f, 9.f), white, P, MAT_WINDOWS);           // upper deck
        b.box(vec3(0, 3.7f, -1.5f), vec3(3.7f, 0.12f, 9.5f), c1, P);
        b.box(vec3(0, 3.82f, 3.5f), vec3(2.2f, 0.8f, 2.0f), white, P);                       // wheelhouse
        b.box(vec3(0, 4.62f, 3.5f), vec3(2.4f, 0.1f, 2.2f), c1, P);
        b.cyl(vec3(0, 3.82f, -3.5f), 0.7f, 1.5f, vec3(0.7f, 0.1f, 0.1f), P, 10);             // funnel
        b.cyl(vec3(0, 5.32f, -3.5f), 0.72f, 0.2f, dark, P, 10);
        for (int s = -1; s <= 1; s += 2)                                                     // side paddle boxes
            b.cylX(vec3(s * 2.45f, 1.0f, -5.5f), 1.4f, 0.5f, vec3(0.4f, 0.1f, 0.1f), P, 12);
        break;
    }
    case BType::Launch: {                   // smaller passenger launch
        b.box(vec3(0, -0.25f, 0), vec3(3.2f, 1.3f, 12.f), c1, P);
        b.box(vec3(0, -0.25f, 6.3f), vec3(2.2f, 1.3f, 1.8f), c1, P);
        b.box(vec3(0, 1.0f, -0.3f), vec3(3.0f, 1.2f, 10.f), white, P, MAT_WINDOWS);
        b.box(vec3(0, 2.2f, -0.3f), vec3(3.2f, 0.12f, 10.4f), c2, P);
        b.box(vec3(0, 2.3f, 3.0f), vec3(1.8f, 0.9f, 1.6f), white, P);
        b.cyl(vec3(0, 2.3f, -3.0f), 0.45f, 1.5f, dark, P, 8);
        break;
    }
    }
    (void)glass; (void)time;
}
