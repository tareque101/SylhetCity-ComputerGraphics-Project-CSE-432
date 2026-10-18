#pragma once
// Pedestrians: they walk across Keane Bridge (pedestrian only) and along the riverside promenade.
#include "Batch.h"
#include <random>
#include <vector>

struct Walker {
    glm::vec2 a{ 0.f }, b{ 0.f };   // the two ends of the walk
    float t = 0.f;                  // 0..1 along a -> b
    float dir = 1.f;                // +1 walking toward b, -1 toward a
    float speed = 1.3f;             // m/s
    float lateral = 0.f;            // offset to the side of the path
    float phase = 0.f;              // step animation
    glm::vec3 shirt{ 1.f }, trousers{ 0.2f }, skin{ 0.7f, 0.5f, 0.4f };
    int style = 0;                  // 0 plain, 1 umbrella, 2 bag, 3 cap
    float height = 1.7f;
    float ground = 0.16f;           // walking surface height
};

class Walkers {
public:
    void init(unsigned seed);
    void update(float dt);
    void build(Batch& dyn) const;
    size_t count() const { return w.size(); }

private:
    std::vector<Walker> w;
    float time = 0.f;
};
