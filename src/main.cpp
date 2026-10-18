// Sylhet City Simulator - modern OpenGL 3.3, GLFW, GLAD, GLM
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Batch.h"
#include "Boats.h"
#include "Camera.h"
#include "Shader.h"
#include "Vehicles.h"
#include "Walkers.h"
#include "World.h"

struct App {
    Camera cam;
    bool captured = true, firstMouse = true;
    double lx = 0, ly = 0;
    float night = 0.f, nightTarget = 0.f;
} g;

static void applyCursor(GLFWwindow* w) {
    glfwSetInputMode(w, GLFW_CURSOR, (g.captured && !g.cam.topDown) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    g.firstMouse = true;
}
static void onKey(GLFWwindow* w, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, 1);
    if (key == GLFW_KEY_M) {
        g.cam.topDown = !g.cam.topDown;
        if (g.cam.topDown) g.cam.mapCenter = glm::vec2(g.cam.pos.x, g.cam.pos.z);
        applyCursor(w);
    }
    if (key == GLFW_KEY_N) g.nightTarget = g.nightTarget > 0.5f ? 0.f : 1.f;
    if (key == GLFW_KEY_TAB) { g.captured = !g.captured; applyCursor(w); }
}
static void onMouse(GLFWwindow*, double x, double y) {
    if (g.firstMouse) { g.lx = x; g.ly = y; g.firstMouse = false; }
    double dx = x - g.lx, dy = g.ly - y;
    g.lx = x; g.ly = y;
    if (g.captured && !g.cam.topDown) g.cam.look((float)dx, (float)dy);
}
static void onScroll(GLFWwindow*, double, double dy) { g.cam.scroll((float)dy); }
static void onResize(GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); }

static void saveScreenshot(const char* path, int w, int h) {
    std::vector<unsigned char> px((size_t)w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    std::ofstream f(path, std::ios::binary);
    f << "P6\n" << w << " " << h << "\n255\n";
    for (int y = h - 1; y >= 0; --y) f.write((const char*)&px[(size_t)y * w * 3], (std::streamsize)w * 3);
}

static int run(int argc, char** argv) {
    const char* shotPath = nullptr;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--shot" && i + 1 < argc) shotPath = argv[++i];
        else if (a == "--topdown") g.cam.topDown = true;
        else if (a == "--night") g.night = g.nightTarget = 1.f;
        else if (a == "--cam" && i + 5 < argc) {
            g.cam.pos = glm::vec3(std::atof(argv[i + 1]), std::atof(argv[i + 2]), std::atof(argv[i + 3]));
            g.cam.yaw = (float)std::atof(argv[i + 4]);
            g.cam.pitch = (float)std::atof(argv[i + 5]);
            i += 5;
        }
    }

    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* win = glfwCreateWindow(1280, 720, "Sylhet City Simulator", nullptr, nullptr);
    if (!win) { std::cerr << "Cannot create window (needs OpenGL 3.3)\n"; glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cerr << "GLAD failed\n"; return 1; }
    glfwSetFramebufferSizeCallback(win, onResize);
    glfwSetCursorPosCallback(win, onMouse);
    glfwSetScrollCallback(win, onScroll);
    glfwSetKeyCallback(win, onKey);
    applyCursor(win);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    Shader sh;
    if (!sh.load("shaders/scene.vert", "shaders/scene.frag")) return 1;

    World world;
    buildWorld(world);
    world.stat.upload(false);
    VehicleSystem vehicles;
    vehicles.init(&world.roads, 7);
    BoatSystem boats;
    boats.init(11);
    Walkers walkers;
    walkers.init(5);
    Batch dyn, labels;
    float lastHalf = -1.f;
    std::printf("Static vertices: %zu | vehicles: %zu | boats: %zu | pedestrians: %zu\n", world.stat.vertexCount(),
                vehicles.count(), boats.count(), walkers.count());
    std::printf("WASD move | mouse look | Shift fast | Q/E down/up | scroll speed/zoom\n"
                "M top-down map | N day/night | TAB release mouse | ESC quit\n"
                "TOLL: stand within 45 m of a toll booth on Shah Jalal / East Surma Bridge and HOLD T to collect\n");

    double last = glfwGetTime(), titleTimer = 0;
    int frame = 0;
    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime();
        float dt = (float)(now - last);
        last = now;
        glfwPollEvents();
        g.cam.update(win, dt);
        vehicles.update(dt, g.cam.pos, !g.cam.topDown, glfwGetKey(win, GLFW_KEY_T) == GLFW_PRESS);
        boats.update(dt);
        walkers.update(dt);
        dyn.clear();
        vehicles.build(dyn);
        vehicles.buildGates(dyn);
        boats.build(dyn);
        walkers.build(dyn);
        dyn.upload(true);
        g.night += (g.nightTarget - g.night) * std::min(1.f, dt * 1.5f);

        if (g.cam.topDown && std::fabs(g.cam.mapHalf - lastHalf) > lastHalf * 0.02f) {
            labels.clear();
            buildTopLabels(labels, world.landmarks, g.cam.mapHalf * 0.0042f);
            labels.upload(false);
            lastHalf = g.cam.mapHalf;
        }

        int fw, fh;
        glfwGetFramebufferSize(win, &fw, &fh);
        if (fh == 0) continue;
        float n = g.night;
        glm::vec3 skyDay(0.53f, 0.72f, 0.92f), skyNight(0.03f, 0.04f, 0.09f);
        glm::vec3 sky = glm::mix(skyDay, skyNight, n);
        glClearColor(sky.r, sky.g, sky.b, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sh.use();
        sh.setMat4("uVP", g.cam.proj((float)fw / fh) * g.cam.view());
        sh.setVec3("uCamPos", g.cam.eye());
        sh.setVec3("uSunDir", glm::normalize(glm::mix(glm::vec3(0.45f, 0.75f, 0.35f), glm::vec3(-0.3f, 0.8f, -0.4f), n)));
        sh.setVec3("uSunColor", glm::mix(glm::vec3(1.f, 0.95f, 0.85f), glm::vec3(0.10f, 0.12f, 0.22f), n));
        sh.setVec3("uAmbient", glm::mix(glm::vec3(0.42f, 0.46f, 0.52f), glm::vec3(0.10f, 0.12f, 0.20f), n));
        sh.setVec3("uFogColor", sky);
        sh.setFloat("uFogK", g.cam.topDown ? 0.f : 0.0008f + 0.0007f * n);
        sh.setFloat("uNight", n);
        world.stat.draw();
        dyn.draw();
        if (g.cam.topDown) labels.draw();

        glfwSwapBuffers(win);
        ++frame;

        titleTimer += dt;
        if (titleTimer > 0.25) {
            titleTimer = 0;
            glm::vec2 ref = g.cam.topDown ? g.cam.mapCenter : glm::vec2(g.cam.pos.x, g.cam.pos.z);
            const Landmark* best = nullptr;
            float bd = g.cam.topDown ? 150.f : 70.f;
            for (const auto& l : world.landmarks) {
                float d = glm::length(l.pos - ref);
                if (d < bd) { bd = d; best = &l; }
            }
            char t[640];
            std::snprintf(t, sizeof t, "Sylhet City Simulator | %s | %s | %.0f FPS | %s", best ? best->name.c_str() : "-",
                          g.cam.topDown ? "Top-down map" : "Free roam", dt > 0 ? 1.0 / dt : 0.0,
                          vehicles.tollStatus().c_str());
            glfwSetWindowTitle(win, t);
        }
        if (shotPath && frame == 8) {
            saveScreenshot(shotPath, fw, fh);
            break;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    int r = run(argc, argv);   // GL objects are destroyed inside run(), before the context
    glfwTerminate();
    return r;
}
