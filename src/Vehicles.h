#pragma once
#include "Batch.h"
#include "RoadNet.h"
#include <random>
#include <string>
#include <vector>

enum class VType { CNG, BabyTaxi, Car, Bike, Rickshaw, Bus };

// Draws one vehicle in local space (front = +Z, wheels on y=0).
void drawVehicleModel(Batch& b, VType t, const glm::mat4& P, const glm::vec3& c1, const glm::vec3& c2);

struct Vehicle {
    VType type = VType::Car;
    int prev = -1, a = 0, b = 0, c = 0;   // path: prev -> a -> b -> c
    float s = 0.f;                        // distance travelled on the current segment
    float len = 1.f;                      // length of the current segment
    float speed = 0.f, maxSpeed = 10.f;
    float off = 3.f;                      // lane offset from road centre (drives on the left)
    float length = 4.f;                   // vehicle length (for following gaps)
    glm::vec3 c1{ 1.f }, c2{ 1.f };
    glm::vec2 A{ 0.f }, B{ 0.f }, dir{ 0.f, 1.f }, pos{ 0.f };
    float heading = 0.f;
    bool turnedIn = false;
    // toll plaza state
    bool paid = false;        // toll paid for the current road segment
    float waitT = 0.f;        // seconds waiting at the stop line
    int gate = -1;            // toll gate this vehicle is approaching (-1 = none)
    float gateDist = 0.f;     // distance from the front bumper to the stop line
};

// Fare table (simulation values, not the real tariffs)
int tollFare(VType t);
const char* vtypeName(VType t);

// Run-time state of one toll plaza
struct GateState {
    float arm = 0.f;          // 0 = barrier down, 1 = barrier up
    int queue = 0;            // vehicles waiting in front of the barrier
    bool manned = false;      // the player is standing at this booth
    long total = 0;           // everything collected at this plaza
};

struct TollStats {
    long mine = 0, staff = 0;
    int vehiclesMine = 0, vehiclesStaff = 0;
};

class VehicleSystem {
public:
    void init(const RoadNet* net, unsigned seed);
    // player = camera position; freeRoam = false in top-down map mode; collect = T key held
    void update(float dt, const glm::vec3& player, bool freeRoam, bool collect);
    void build(Batch& dyn) const;
    void buildGates(Batch& dyn) const;    // barrier arms, signal lights, income boards
    const TollStats& stats() const { return tollSt; }
    std::string tollStatus() const;       // one-line text for the window title
    size_t count() const { return v.size(); }

private:
    const RoadNet* net = nullptr;
    std::vector<Vehicle> v;
    std::mt19937 rng;
    std::vector<GateState> gates;
    TollStats tollSt;
    float collectCD = 0.f, msgTimer = 0.f;
    std::string lastMsg;
    void payVehicle(Vehicle& a, int gi, bool byPlayer);

    Vehicle spawn(VType t);
    int pickNext(int a, int b);
    glm::vec2 corner(int p, int n, int q, float off) const;
    void setSegment(Vehicle& a);
    void advance(Vehicle& a);
};
