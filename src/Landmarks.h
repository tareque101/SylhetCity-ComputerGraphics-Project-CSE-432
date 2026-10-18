#pragma once
#include "Batch.h"
#include <random>
#include <string>
#include <vector>

enum class LType { Point, Bridge, Market, Court, DCOffice, CityCorp, BRTA, School, Hospital,
                   SIU, Leading, Metropolitan, Northeast, Clock, Circuit, Mall };
enum class LCat { Junction, Landmark, Admin, School, Health, University };

struct Landmark {
    std::string name;    // full name (window title)
    std::string label;   // short label (top-down map)
    LType type;
    glm::vec2 pos;       // world (x, z) centre
    glm::vec2 size;      // footprint, keeps filler buildings away
    int variant;
    LCat cat;
};

// === Edit this list to move / rename places ===
std::vector<Landmark> makeLandmarks();
bool isCampus(LType t);
void buildLandmark(Batch& b, const Landmark& lm, std::mt19937& rng);
void buildCampus(Batch& b, const Landmark& lm, std::mt19937& rng);   // Campus.cpp
void buildTopLabels(Batch& labels, const std::vector<Landmark>& lms, float px);
