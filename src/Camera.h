#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

// Free-roam 3D camera + top-down map camera (toggle with M).
class Camera {
public:
    // free roam
    glm::vec3 pos{ -60.f, 7.f, 4.f };
    float yaw = 0.f, pitch = -8.f;          // degrees; yaw 0 = looking east
    float speed = 30.f;
    float sensitivity = 0.12f;
    // top-down
    bool topDown = false;
    glm::vec2 mapCenter{ 110.f, 40.f };
    float mapHalf = 260.f;                  // half of the visible height in metres

    glm::vec3 front() const {
        float y = glm::radians(yaw), p = glm::radians(pitch);
        return glm::normalize(glm::vec3(std::cos(y) * std::cos(p), std::sin(p), std::sin(y) * std::cos(p)));
    }
    glm::vec3 eye() const { return topDown ? glm::vec3(mapCenter.x, 800.f, mapCenter.y) : pos; }

    glm::mat4 view() const {
        if (topDown)
            return glm::lookAt(eye(), glm::vec3(mapCenter.x, 0.f, mapCenter.y), glm::vec3(0, 0, -1));
        return glm::lookAt(pos, pos + front(), glm::vec3(0, 1, 0));
    }
    glm::mat4 proj(float aspect) const {
        if (topDown) return glm::ortho(-mapHalf * aspect, mapHalf * aspect, -mapHalf, mapHalf, 1.f, 2000.f);
        return glm::perspective(glm::radians(60.f), aspect, 0.5f, 3000.f);
    }

    void update(GLFWwindow* w, float dt) {
        auto key = [&](int k) { return glfwGetKey(w, k) == GLFW_PRESS ? 1.f : 0.f; };
        float boost = key(GLFW_KEY_LEFT_SHIFT) > 0 ? 3.f : 1.f;
        if (topDown) {
            float s = mapHalf * 1.2f * dt * boost;
            mapCenter.x += (key(GLFW_KEY_D) - key(GLFW_KEY_A)) * s;
            mapCenter.y += (key(GLFW_KEY_S) - key(GLFW_KEY_W)) * s;
            return;
        }
        glm::vec3 f = front(), r = glm::normalize(glm::cross(f, glm::vec3(0, 1, 0)));
        float s = speed * dt * boost;
        pos += f * ((key(GLFW_KEY_W) - key(GLFW_KEY_S)) * s);
        pos += r * ((key(GLFW_KEY_D) - key(GLFW_KEY_A)) * s);
        pos.y += (key(GLFW_KEY_E) - key(GLFW_KEY_Q)) * s;
        pos.y = std::max(1.5f, pos.y);
    }
    void look(float dx, float dy) {
        yaw += dx * sensitivity;
        pitch = std::clamp(pitch + dy * sensitivity, -89.f, 89.f);
    }
    void scroll(float dy) {
        if (topDown) mapHalf = std::clamp(mapHalf * std::pow(0.9f, dy), 40.f, 700.f);
        else speed = std::clamp(speed * std::pow(1.15f, dy), 5.f, 250.f);
    }
};
