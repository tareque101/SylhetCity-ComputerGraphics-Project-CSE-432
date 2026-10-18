#include "Vehicles.h"
#include "Props.h"
#include "Config.h"
#include <algorithm>
#include <cmath>

using namespace glm;

static const float PI = 3.14159265f;
static const vec3 GLASS(0.16f, 0.22f, 0.28f);
static const vec3 HEADL(1.f, 0.95f, 0.7f), TAILL(0.9f, 0.05f, 0.05f);

static void wheel(Batch& b, const mat4& P, float x, float y, float z, float r, float w) {
    b.cylX(vec3(x, y, z), r, w, vec3(0.05f), P, 10);
}

void drawVehicleModel(Batch& b, VType t, const mat4& P, const vec3& c1, const vec3& c2) {
    const vec3 dark(0.12f, 0.12f, 0.13f);
    switch (t) {
    case VType::Car:
        for (int sx = -1; sx <= 1; sx += 2)
            for (int sz = -1; sz <= 1; sz += 2) wheel(b, P, sx * 0.85f, 0.34f, sz * 1.3f, 0.34f, 0.24f);
        b.box(vec3(0, 0.28f, 0), vec3(1.8f, 0.55f, 4.2f), c1, P);
        b.box(vec3(0, 0.83f, -0.25f), vec3(1.55f, 0.5f, 2.2f), GLASS, P);
        b.box(vec3(0, 1.33f, -0.25f), vec3(1.5f, 0.07f, 1.9f), c1, P);
        for (int s = -1; s <= 1; s += 2) {
            b.box(vec3(s * 0.6f, 0.5f, 2.1f), vec3(0.32f, 0.14f, 0.06f), HEADL, P, MAT_EMISSIVE);
            b.box(vec3(s * 0.65f, 0.55f, -2.1f), vec3(0.3f, 0.12f, 0.06f), TAILL, P, MAT_EMISSIVE);
        }
        break;
    case VType::CNG:
    case VType::BabyTaxi:   // c1 = body colour, c2 = roof colour
        wheel(b, P, -0.65f, 0.28f, -0.7f, 0.28f, 0.16f);
        wheel(b, P, 0.65f, 0.28f, -0.7f, 0.28f, 0.16f);
        wheel(b, P, 0.f, 0.26f, 0.95f, 0.26f, 0.14f);
        b.box(vec3(0, 0.25f, -0.1f), vec3(1.3f, 0.45f, 2.2f), c1, P);
        b.box(vec3(0, 0.3f, 0.95f), vec3(0.6f, 0.75f, 0.6f), c1, P);
        b.box(vec3(0, 0.7f, 0.55f), vec3(1.2f, 0.7f, 0.05f), GLASS, P);
        b.box(vec3(0, 0.7f, -0.65f), vec3(1.3f, 0.55f, 0.9f), c1, P);
        for (int s = -1; s <= 1; s += 2) {
            b.box(vec3(s * 0.62f, 0.7f, 0.55f), vec3(0.06f, 0.85f, 0.06f), dark, P);
            b.box(vec3(s * 0.62f, 0.7f, -1.05f), vec3(0.06f, 0.85f, 0.06f), dark, P);
        }
        b.box(vec3(0, 1.55f, -0.25f), vec3(1.4f, 0.07f, 1.9f), c2, P);
        b.box(vec3(0, 0.7f, 0.35f), vec3(0.3f, 0.5f, 0.25f), vec3(0.8f, 0.8f, 0.75f), P);
        b.sphere(vec3(0, 1.4f, 0.35f), vec3(0.12f), vec3(0.75f, 0.55f, 0.42f), P, 6, 4);
        b.box(vec3(0, 0.55f, 1.27f), vec3(0.2f, 0.15f, 0.05f), HEADL, P, MAT_EMISSIVE);
        break;
    case VType::Bike:       // c1 = bike colour, c2 = rider shirt
        wheel(b, P, 0, 0.32f, 0.62f, 0.32f, 0.1f);
        wheel(b, P, 0, 0.32f, -0.62f, 0.32f, 0.1f);
        b.box(vec3(0, 0.35f, 0), vec3(0.2f, 0.25f, 1.1f), c1, P);
        b.box(vec3(0, 0.62f, 0.2f), vec3(0.28f, 0.22f, 0.5f), c1, P);
        b.box(vec3(0, 0.62f, -0.3f), vec3(0.25f, 0.1f, 0.5f), dark, P);
        b.box(vec3(0, 1.0f, 0.5f), vec3(0.6f, 0.05f, 0.06f), dark, P);
        b.box(vec3(0, 0.4f, 0.55f), vec3(0.06f, 0.65f, 0.06f), dark, P);
        b.box(vec3(0, 0.72f, -0.15f), vec3(0.35f, 0.6f, 0.25f), c2, P);
        b.sphere(vec3(0, 1.47f, -0.05f), vec3(0.14f), c1 * 0.8f, P, 8, 5);
        b.box(vec3(0, 0.5f, 0.95f), vec3(0.15f, 0.12f, 0.05f), HEADL, P, MAT_EMISSIVE);
        break;
    case VType::Rickshaw:   // c1 = hood colour, c2 = back panel colour
        wheel(b, P, -0.5f, 0.36f, -0.3f, 0.36f, 0.06f);
        wheel(b, P, 0.5f, 0.36f, -0.3f, 0.36f, 0.06f);
        wheel(b, P, 0.f, 0.34f, 1.15f, 0.34f, 0.06f);
        b.box(vec3(0, 0.35f, 0.5f), vec3(0.08f, 0.08f, 1.4f), dark, P);
        b.box(vec3(0, 0.5f, -0.3f), vec3(0.85f, 0.12f, 0.6f), vec3(0.15f, 0.15f, 0.2f), P);
        b.box(vec3(0, 0.62f, -0.62f), vec3(0.9f, 0.75f, 0.08f), c2, P);
        b.box(vec3(0, 1.35f, -0.3f), vec3(1.0f, 0.06f, 0.95f), c1, P);
        for (int s = -1; s <= 1; s += 2) b.box(vec3(s * 0.45f, 0.65f, -0.05f), vec3(0.05f, 0.7f, 0.05f), dark, P);
        b.box(vec3(0, 0.6f, 0.55f), vec3(0.3f, 0.55f, 0.2f), vec3(0.25f, 0.4f, 0.65f), P);
        b.sphere(vec3(0, 1.3f, 0.58f), vec3(0.12f), vec3(0.75f, 0.55f, 0.42f), P, 8, 5);
        b.box(vec3(0, 0.95f, 0.95f), vec3(0.55f, 0.04f, 0.04f), dark, P);
        break;
    case VType::Bus:
        for (int sx = -1; sx <= 1; sx += 2)
            for (int sz = -1; sz <= 1; sz += 2) wheel(b, P, sx * 1.1f, 0.5f, sz * 3.0f, 0.5f, 0.3f);
        b.box(vec3(0, 0.5f, 0), vec3(2.5f, 2.7f, 9.f), c1, P);
        b.box(vec3(0, 1.55f, 0), vec3(2.54f, 0.95f, 8.6f), GLASS, P);
        for (int s = -1; s <= 1; s += 2) {
            b.box(vec3(s * 0.8f, 0.8f, 4.5f), vec3(0.4f, 0.2f, 0.06f), HEADL, P, MAT_EMISSIVE);
            b.box(vec3(s * 0.9f, 0.8f, -4.5f), vec3(0.4f, 0.2f, 0.06f), TAILL, P, MAT_EMISSIVE);
        }
        break;
    }
}

// --- simulation -------------------------------------------------------------------------------

static vec2 leftOf(const vec2& d) { return vec2(d.y, -d.x); }   // drive on the left

int VehicleSystem::pickNext(int a, int b) {
    std::vector<int> opts;
    for (int n : net->nodes[b].nbr) if (n != a) opts.push_back(n);
    if (opts.empty()) return a;
    return opts[rng() % opts.size()];
}

// Lane-offset path point at node n when travelling p -> n -> q (turns cut the corner properly).
vec2 VehicleSystem::corner(int p, int n, int q, float off) const {
    const auto& N = net->nodes;
    vec2 P = N[n].p;
    vec2 d2 = normalize(N[q].p - P);
    vec2 l2 = leftOf(d2);
    if (p < 0) return P + l2 * off;
    vec2 d1 = normalize(P - N[p].p);
    vec2 l1 = leftOf(d1);
    float dt = dot(d1, d2);
    if (dt > 0.99f || dt < -0.99f) return P + l1 * off;
    return P + (l1 + l2) * off;
}

void VehicleSystem::setSegment(Vehicle& a) {
    a.A = corner(a.prev, a.a, a.b, a.off);
    a.B = corner(a.a, a.b, a.c, a.off);
    float L = length(a.B - a.A);
    a.len = std::max(1.f, L);
    a.dir = (L > 0.01f) ? normalize(a.B - a.A) : net->dir(a.a, a.b);
}

void VehicleSystem::advance(Vehicle& a) {
    vec2 d1 = net->dir(a.a, a.b), d2 = net->dir(a.b, a.c);
    a.turnedIn = dot(d1, d2) < 0.9f;
    a.paid = false;
    a.waitT = 0.f;
    a.prev = a.a;
    a.a = a.b;
    a.b = a.c;
    a.c = pickNext(a.a, a.b);
    setSegment(a);
}

Vehicle VehicleSystem::spawn(VType t) {
    using props::rf;
    static const vec3 carCols[] = { vec3(0.92f), vec3(0.7f, 0.72f, 0.75f), vec3(0.08f), vec3(0.7f, 0.1f, 0.1f),
                                    vec3(0.12f, 0.25f, 0.6f), vec3(0.75f, 0.6f, 0.25f), vec3(0.4f) };
    static const vec3 vivid[] = { vec3(0.9f, 0.1f, 0.15f), vec3(0.1f, 0.35f, 0.85f), vec3(0.95f, 0.75f, 0.1f),
                                  vec3(0.85f, 0.15f, 0.6f), vec3(0.1f, 0.7f, 0.75f), vec3(0.95f, 0.45f, 0.1f),
                                  vec3(0.4f, 0.75f, 0.2f) };
    Vehicle a;
    a.type = t;
    int node = (int)(rng() % net->nodes.size());
    const auto& nb = net->nodes[node].nbr;
    a.a = node;
    a.b = nb[rng() % nb.size()];
    a.prev = -1;
    a.c = pickNext(a.a, a.b);
    switch (t) {
    case VType::CNG:
        a.maxSpeed = rf(rng, 8.f, 11.f); a.off = 3.9f; a.length = 2.6f;
        a.c1 = vec3(0.10f, 0.55f, 0.25f); a.c2 = vec3(0.08f);
        break;
    case VType::BabyTaxi:
        a.maxSpeed = rf(rng, 7.f, 10.f); a.off = 3.9f; a.length = 2.6f;
        a.c1 = vec3(0.95f, 0.78f, 0.1f); a.c2 = vec3(0.08f);
        break;
    case VType::Car:
        a.maxSpeed = rf(rng, 10.f, 14.f); a.off = 2.4f; a.length = 4.2f;
        a.c1 = carCols[rng() % 7];
        break;
    case VType::Bike:
        a.maxSpeed = rf(rng, 11.f, 15.f); a.off = 3.9f; a.length = 1.8f;
        a.c1 = vivid[rng() % 7] * 0.8f; a.c2 = vivid[rng() % 7];
        break;
    case VType::Rickshaw:
        a.maxSpeed = rf(rng, 3.f, 4.5f); a.off = 5.1f; a.length = 2.4f;
        a.c1 = vivid[rng() % 7]; a.c2 = vivid[rng() % 7];
        break;
    case VType::Bus:
        a.maxSpeed = rf(rng, 7.f, 9.f); a.off = 2.4f; a.length = 9.f;
        a.c1 = vivid[rng() % 3];
        break;
    }
    setSegment(a);
    a.s = rf(rng, 0.f, a.len - 0.1f);
    a.speed = a.maxSpeed * 0.5f;
    a.pos = mix(a.A, a.B, a.s / a.len);
    a.heading = std::atan2(a.dir.x, a.dir.y);
    return a;
}

void VehicleSystem::init(const RoadNet* n, unsigned seed) {
    net = n;
    rng.seed(seed);
    v.clear();
    gates.assign(net->tolls.size(), GateState());
    tollSt = TollStats();
    struct Spec { VType t; int count; };
    const Spec specs[] = { { VType::CNG, 60 }, { VType::BabyTaxi, 25 }, { VType::Car, 40 },
                           { VType::Bike, 45 }, { VType::Rickshaw, 40 }, { VType::Bus, 8 } };
    for (const auto& sp : specs)
        for (int i = 0; i < sp.count; ++i) v.push_back(spawn(sp.t));
}

int tollFare(VType t) {
    switch (t) {
    case VType::Bike:     return 10;
    case VType::Rickshaw: return 5;
    case VType::CNG:
    case VType::BabyTaxi: return 20;
    case VType::Car:      return 50;
    case VType::Bus:      return 100;
    }
    return 20;
}

const char* vtypeName(VType t) {
    switch (t) {
    case VType::Bike:     return "Bike";
    case VType::Rickshaw: return "Rickshaw";
    case VType::CNG:      return "CNG";
    case VType::BabyTaxi: return "Baby taxi";
    case VType::Car:      return "Car";
    case VType::Bus:      return "Bus";
    }
    return "Vehicle";
}

void VehicleSystem::payVehicle(Vehicle& a, int gi, bool byPlayer) {
    int fare = tollFare(a.type);
    a.paid = true;
    a.waitT = 0.f;
    gates[gi].total += fare;
    if (byPlayer) { tollSt.mine += fare; ++tollSt.vehiclesMine; }
    else          { tollSt.staff += fare; ++tollSt.vehiclesStaff; }
    lastMsg = std::string(byPlayer ? "+Tk " : "staff +Tk ") + std::to_string(fare) + " (" + vtypeName(a.type) + ")";
    msgTimer = 3.f;
}

void VehicleSystem::update(float dtRaw, const vec3& player, bool freeRoam, bool collect) {
    float dt = std::min(dtRaw, 0.05f);
    collectCD = std::max(0.f, collectCD - dt);
    msgTimer = std::max(0.f, msgTimer - dt);

    // which booth is the player standing at? (nearest one within 45 m, free-roam mode only)
    int manned = -1;
    float bestD = 45.f;
    if (freeRoam)
        for (size_t gi = 0; gi < net->tolls.size(); ++gi) {
            float d = length(vec2(player.x, player.z) - net->tolls[gi].p);
            if (d < bestD) { bestD = d; manned = (int)gi; }
        }
    for (size_t gi = 0; gi < gates.size(); ++gi) { gates[gi].manned = ((int)gi == manned); gates[gi].queue = 0; }

    for (auto& a : v) {
        a.gate = -1;
        float remaining = a.len - a.s;
        float target = a.maxSpeed;

        // slow down for turns
        vec2 dNext = normalize(net->nodes[a.c].p - net->nodes[a.b].p);
        if (dot(a.dir, dNext) < 0.9f && remaining < 14.f) target = std::min(target, 4.5f);
        if (a.turnedIn && a.s < 6.f) target = std::min(target, 5.f);

        // toll plaza: stop in front of the barrier until the toll has been collected
        if (!a.paid)
            for (size_t gi = 0; gi < net->tolls.size(); ++gi) {
                const TollGateDef& g = net->tolls[gi];
                if (a.a != g.from || a.b != g.to) continue;
                float dist = dot(g.p - a.pos, g.dir) - 0.5f * a.length - 0.8f;
                if (dist < -4.f || dist > 70.f) continue;   // already past the barrier, or still far away
                a.gate = (int)gi;
                a.gateDist = dist;
                float lim = (dist < 0.15f) ? 0.f : std::min(a.maxSpeed, dist * 0.9f);
                target = std::min(target, lim);
                if (dist < 1.5f && a.speed < 0.6f) a.waitT += dt;
                break;
            }

        // keep distance to the vehicle ahead in the same lane
        for (const auto& w : v) {
            if (&w == &a || std::fabs(w.off - a.off) > 1.0f) continue;
            float gap = 1e9f;
            if (w.a == a.a && w.b == a.b && w.s > a.s) gap = w.s - a.s;
            else if (remaining < 40.f && w.a == a.b && w.b == a.c && w.prev == a.a) gap = remaining + w.s;
            if (gap > 1e8f) continue;
            gap -= 0.5f * (a.length + w.length);
            if (gap < 12.f) {
                float lim = (gap < 1.5f) ? 0.f : std::min(a.maxSpeed, w.speed + (gap - 1.5f) * 0.7f);
                target = std::min(target, lim);
            }
        }

        float dv = target - a.speed;
        a.speed = std::max(0.f, a.speed + std::clamp(dv, -10.f * dt, 3.f * dt));
        a.s += a.speed * dt;
        while (a.s >= a.len) {
            a.s -= a.len;
            advance(a);
        }

        a.pos = mix(a.A, a.B, a.s / a.len);
        float tgt = std::atan2(a.dir.x, a.dir.y);
        float diff = tgt - a.heading;
        while (diff > PI) diff -= 2.f * PI;
        while (diff < -PI) diff += 2.f * PI;
        a.heading += diff * std::min(1.f, dt * 7.f);
    }

    // collect tolls: first waiting vehicle of every plaza
    for (size_t gi = 0; gi < net->tolls.size(); ++gi) {
        Vehicle* front = nullptr;
        for (auto& a : v)
            if (a.gate == (int)gi && !a.paid) {
                ++gates[gi].queue;
                if (a.waitT > 0.f && (!front || a.gateDist < front->gateDist)) front = &a;
            }
        if (!front) continue;
        if (gates[gi].manned) {
            if (collect && collectCD <= 0.f) { payVehicle(*front, (int)gi, true); collectCD = 0.35f; }
        } else if (front->waitT > 3.5f) {
            payVehicle(*front, (int)gi, false);      // the booth attendant handles it
        }
    }

    // barrier arms: up while a paid vehicle is crossing the line
    for (size_t gi = 0; gi < net->tolls.size(); ++gi) {
        const TollGateDef& g = net->tolls[gi];
        bool open = false;
        for (const auto& a : v) {
            if (!a.paid || a.a != g.from || a.b != g.to) continue;
            float d = dot(g.p - a.pos, g.dir);
            if (d > -(0.5f * a.length + 3.f) && d < 0.5f * a.length + 7.f) { open = true; break; }
        }
        float tgt = open ? 1.f : 0.f;
        gates[gi].arm += std::clamp(tgt - gates[gi].arm, -dt * 1.6f, dt * 1.6f);
    }
}

void VehicleSystem::build(Batch& dyn) const {
    for (const auto& a : v)
        drawVehicleModel(dyn, a.type, place(vec3(a.pos.x, cfg::ROAD_TOP, a.pos.y), a.heading), a.c1, a.c2);
}

std::string VehicleSystem::tollStatus() const {
    std::string s = "Toll: you Tk " + std::to_string(tollSt.mine) + " (" + std::to_string(tollSt.vehiclesMine) +
                    " veh) | staff Tk " + std::to_string(tollSt.staff);
    for (size_t gi = 0; gi < gates.size(); ++gi)
        if (gates[gi].manned) {
            s += std::string(" | AT BOOTH: ") + net->tolls[gi].name + " - " + std::to_string(gates[gi].queue) +
                 " waiting, hold T to collect";
            break;
        }
    if (msgTimer > 0.f) s += " | " + lastMsg;
    return s;
}

// barrier arm + signal + income board for every toll plaza (the booth itself is static, see World.cpp)
void VehicleSystem::buildGates(Batch& b) const {
    for (size_t gi = 0; gi < net->tolls.size(); ++gi) {
        const TollGateDef& g = net->tolls[gi];
        const GateState& st = gates[gi];
        float yaw = std::atan2(g.dir.x, g.dir.y);
        mat4 P = place(vec3(g.p.x, cfg::ROAD_TOP, g.p.y), yaw);    // local +X = driver's left, +Z = travel
        // barrier arm hinged at the booth, striped red / white
        mat4 H = P * translate(mat4(1.f), vec3(1.0f, 1.15f, 0.f)) * rotate(mat4(1.f), st.arm * 1.5f, vec3(0, 0, 1));
        const float armLen = 5.6f;
        int n = 8;
        for (int i = 0; i < n; ++i)
            b.box(vec3(armLen / n * (i + 0.5f), -0.07f, 0.f), vec3(armLen / n, 0.14f, 0.14f),
                  (i % 2) ? vec3(0.95f) : vec3(0.85f, 0.10f, 0.10f), H);
        // signal light on the booth: green = go, red = stop
        vec3 lamp = (st.arm > 0.5f) ? vec3(0.1f, 0.9f, 0.2f) : vec3(0.95f, 0.1f, 0.1f);
        b.box(vec3(0.f, 3.0f, 0.f), vec3(0.5f, 0.5f, 0.5f), lamp, P, MAT_EMISSIVE);

        // income board on the canopy, readable from both sides
        std::string txt = "TK " + std::to_string(st.total);
        float px = 0.13f, tw = (float)txt.size() * 6.f * px - px;
        vec2 bc = g.p + leftOf(g.dir) * 3.2f;      // over the toll lane
        for (int side = 0; side < 2; ++side) {
            mat4 B = place(vec3(bc.x, cfg::ROAD_TOP + 5.1f, bc.y), yaw + (side ? 0.f : 3.14159265f));
            if (side == 0) b.box(vec3(0, 0, 0), vec3(tw + 1.0f, 1.5f, 0.1f), vec3(0.04f), B);
            b.text(txt, B * translate(mat4(1.f), vec3(-tw / 2, 0.3f, 0.08f)), px,
                   st.manned ? vec3(0.3f, 1.f, 0.4f) : vec3(1.f, 0.8f, 0.2f));
        }
    }
}
