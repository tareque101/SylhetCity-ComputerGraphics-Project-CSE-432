#pragma once
// Boats and steamers moving along the Surma river.
#include "Batch.h"
#include <random>
#include <vector>

enum class BType { Nouka, Engine, Steamer, Launch };

struct Boat {
    BType type = BType::Nouka;
    float x = 0.f, z = 0.f;      // world position (river runs along X)
    float dir = 1.f;             // +1 = east, -1 = west
    float speed = 2.f;
    glm::vec3 c1{ 1.f }, c2{ 1.f };
    float phase = 0.f;           // bobbing phase
};

class BoatSystem {
public:
    void init(unsigned seed);
    void update(float dt);
    void build(Batch& dyn) const;
    size_t count() const { return boats.size(); }

private:
    std::vector<Boat> boats;
    float time = 0.f;
};

// local space: bow = +Z, waterline y = 0
void drawBoatModel(Batch& b, BType t, const glm::mat4& P, const glm::vec3& c1, const glm::vec3& c2, float time);
