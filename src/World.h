#pragma once
#include "Batch.h"
#include "Landmarks.h"
#include "RoadNet.h"

struct World {
    RoadNet roads;
    std::vector<Landmark> landmarks;
    Batch stat;      // all static geometry, one draw call
};

void buildWorld(World& w);
