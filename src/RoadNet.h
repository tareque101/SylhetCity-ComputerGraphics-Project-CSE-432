#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <vector>

// Road grid.  Columns (X) and rows (Z) in metres.  Edit these to reshape the city.
namespace grid {
constexpr float X[5] = { -120.f, 0.f, 110.f, 230.f, 350.f };
constexpr float Z[4] = { -100.f, 0.f, 100.f, 180.f };
inline int id(int row, int col) { return row * 5 + col; }
}

// A toll plaza guarding the entrance of a vehicle bridge, for one driving direction.
struct TollGateDef {
    int from = 0, to = 0;      // road segment this gate controls (vehicles travel from -> to)
    glm::vec2 p{ 0.f };        // stop-line point on the road centre line
    glm::vec2 dir{ 0.f, 1.f }; // travel direction at the gate
    const char* name = "";
};

struct RoadNode {
    glm::vec2 p;               // (x, z)
    std::vector<int> nbr;
};

class RoadNet {
public:
    std::vector<RoadNode> nodes;
    std::vector<std::pair<int, int>> segs;   // undirected, for drawing
    std::vector<TollGateDef> tolls;          // toll plazas on the vehicle bridges

    int add(float x, float z);
    void link(int a, int b);
    glm::vec2 dir(int a, int b) const;
    bool isBridge(int a, int b) const;
};

RoadNet buildSylhetRoads();
