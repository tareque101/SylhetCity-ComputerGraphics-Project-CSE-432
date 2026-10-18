#include "Props.h"
#include "Config.h"
#include <algorithm>
#include <cmath>

using namespace glm;

namespace props {

void tree(Batch& b, const vec3& pos, float s, std::mt19937& r) {
    b.cyl(pos, 0.22f * s, 3.0f * s, vec3(0.38f, 0.27f, 0.17f), mat4(1.f), 6);
    vec3 g(rf(r, 0.16f, 0.28f), rf(r, 0.40f, 0.55f), rf(r, 0.16f, 0.26f));
    b.sphere(pos + vec3(0, 4.2f * s, 0), vec3(1.9f * s, 1.7f * s, 1.9f * s), g, mat4(1.f), 8, 5);
    b.sphere(pos + vec3(0.7f * s, 3.6f * s, 0.4f * s), vec3(1.2f * s, 1.1f * s, 1.2f * s), g * 0.9f, mat4(1.f), 8, 5);
}

void lamp(Batch& b, const vec3& pos, float yaw) {
    mat4 P = place(pos, yaw);
    b.cyl(vec3(0), 0.09f, 7.0f, vec3(0.3f, 0.3f, 0.32f), P, 6);
    b.box(vec3(0, 7.0f, 0.6f), vec3(0.14f, 0.12f, 1.4f), vec3(0.3f), P);
    b.box(vec3(0, 6.85f, 1.2f), vec3(0.35f, 0.14f, 0.5f), vec3(1.f, 0.92f, 0.65f), P, MAT_EMISSIVE);
}

static float pxFor(const std::vector<std::string>& L, float width) {
    size_t m = 1;
    for (auto& s : L) m = std::max(m, s.size());
    return width / ((float)m * 6.f - 1.f);
}

float signHeight(const std::vector<std::string>& L, float width) {
    float px = pxFor(L, width), n = (float)L.size();
    return n * 7.f * px + (n - 1.f) * 2.f * px + 4.f * px;
}

void sign(Batch& b, const std::vector<std::string>& L, const vec3& center, float yaw, float width,
          const vec3& bg, const vec3& fg) {
    float px = pxFor(L, width), n = (float)L.size();
    float H = signHeight(L, width), W = width + 4.f * px;
    mat4 P = place(center, yaw);
    b.box(vec3(0, -H / 2, 0), vec3(W, H, 0.25f), bg, P);
    float textH = n * 7.f * px + (n - 1.f) * 2.f * px;
    float yTop = textH / 2.f;
    for (size_t i = 0; i < L.size(); ++i) {
        float w = (float)L[i].size() * 6.f * px - px;
        float y0 = yTop - 7.f * px * (float)(i + 1) - 2.f * px * (float)i;
        b.text(L[i], P * translate(mat4(1.f), vec3(-w / 2, y0, 0.14f)), px, fg);
    }
}

void signPost(Batch& b, const std::vector<std::string>& L, const vec3& g, float yaw, float width) {
    float H = signHeight(L, width), px = pxFor(L, width), W = width + 4.f * px;
    mat4 P = place(g, yaw);
    vec3 gray(0.35f);
    b.cyl(vec3(-W / 2 + 0.5f, 0, 0), 0.1f, 2.4f, gray, P, 6);
    b.cyl(vec3(W / 2 - 0.5f, 0, 0), 0.1f, 2.4f, gray, P, 6);
    sign(b, L, g + vec3(0, 2.4f + H / 2, 0), yaw, width, vec3(0.08f, 0.2f, 0.35f), vec3(1.f, 1.f, 0.95f));
}

void wall(Batch& b, const vec2& a, const vec2& c, float h, float t, const vec3& col) {
    vec2 d = c - a;
    float L = length(d);
    if (L < 0.01f) return;
    vec2 m = (a + c) * 0.5f;
    mat4 P = place(vec3(m.x, 0.1f, m.y), std::atan2(d.x, d.y));
    b.box(vec3(0), vec3(t, h, L), col, P);
    b.box(vec3(0, h, 0), vec3(t + 0.25f, 0.25f, L), col * 0.8f, P);
}

void flag(Batch& b, const vec3& pos, float h, float yaw) {
    mat4 P = place(pos, yaw);
    b.cyl(vec3(0), 0.09f, h, vec3(0.75f), P, 6);
    b.box(vec3(0, h - 1.9f, 1.35f), vec3(0.06f, 1.6f, 2.5f), vec3(0.0f, 0.42f, 0.30f), P);
    b.cylX(vec3(0, h - 1.1f, 1.1f), 0.5f, 0.1f, vec3(0.86f, 0.1f, 0.16f), P, 14);
}

}
