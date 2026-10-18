#include "RoadNet.h"
#include "Config.h"

int RoadNet::add(float x, float z) {
    RoadNode n;
    n.p = glm::vec2(x, z);
    nodes.push_back(n);
    return (int)nodes.size() - 1;
}

void RoadNet::link(int a, int b) {
    nodes[a].nbr.push_back(b);
    nodes[b].nbr.push_back(a);
    segs.push_back(std::make_pair(a, b));
}

glm::vec2 RoadNet::dir(int a, int b) const { return glm::normalize(nodes[b].p - nodes[a].p); }

bool RoadNet::isBridge(int a, int b) const { return (a < 20) != (b < 20); }

RoadNet buildSylhetRoads() {
    using namespace grid;
    RoadNet n;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 5; ++c) n.add(X[c], Z[r]);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) n.link(id(r, c), id(r, c + 1));
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 5; ++c) n.link(id(r, c), id(r + 1, c));
    // Keane Bridge (x = X[0]) is PEDESTRIAN ONLY, so it has no road link.
    // Two toll bridges carry the vehicles over the Surma: Shah Jalal Bridge (X[2]) and East Surma Bridge (X[4]).
    int b2 = n.add(X[2], cfg::NORTH_BANK_Z), b3 = n.add(X[4], cfg::NORTH_BANK_Z);
    n.link(id(0, 2), b2);
    n.link(id(0, 4), b3);
    n.link(b2, b3);   // road along the north bank keeps traffic looping

    // toll plazas: one per driving direction at each bridge end (stop line on the road centre line)
    auto gate = [&](int from, int to, float x, float z, float dz, const char* nm) {
        TollGateDef g;
        g.from = from; g.to = to; g.p = glm::vec2(x, z); g.dir = glm::vec2(0.f, dz); g.name = nm;
        n.tolls.push_back(g);
    };
    gate(id(0, 2), b2, X[2], -122.f, -1.f, "SHAH JALAL BRIDGE (CITY END)");
    gate(b2, id(0, 2), X[2], -215.f, 1.f, "SHAH JALAL BRIDGE (NORTH END)");
    gate(id(0, 4), b3, X[4], -122.f, -1.f, "EAST SURMA BRIDGE (CITY END)");
    gate(b3, id(0, 4), X[4], -215.f, 1.f, "EAST SURMA BRIDGE (NORTH END)");
    return n;
}
