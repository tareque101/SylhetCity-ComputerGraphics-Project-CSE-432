#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

constexpr float MAT_PLAIN    = 0.f;
constexpr float MAT_WINDOWS  = 1.f;   // procedural windows on vertical faces
constexpr float MAT_EMISSIVE = 2.f;   // unlit / glowing (signs, lights)

struct Vertex { glm::vec3 p, n, c; float m; };

inline glm::mat4 place(const glm::vec3& pos, float yaw) {
    return glm::rotate(glm::translate(glm::mat4(1.f), pos), yaw, glm::vec3(0.f, 1.f, 0.f));
}

// Collects triangles in world space and draws them with one draw call.
class Batch {
public:
    Batch() = default;
    Batch(const Batch&) = delete;
    Batch& operator=(const Batch&) = delete;
    ~Batch();

    void clear() { verts.clear(); }
    size_t vertexCount() const { return verts.size(); }

    // base = centre of the bottom face
    void box(const glm::vec3& base, const glm::vec3& size, const glm::vec3& col,
             const glm::mat4& P = glm::mat4(1.f), float mat = MAT_PLAIN);
    void cyl(const glm::vec3& base, float r, float h, const glm::vec3& col,
             const glm::mat4& P = glm::mat4(1.f), int seg = 12, float mat = MAT_PLAIN);
    void cylX(const glm::vec3& center, float r, float len, const glm::vec3& col,
              const glm::mat4& P = glm::mat4(1.f), int seg = 12, float mat = MAT_PLAIN);
    void cylZ(const glm::vec3& center, float r, float len, const glm::vec3& col,
              const glm::mat4& P = glm::mat4(1.f), int seg = 12, float mat = MAT_PLAIN);
    void cone(const glm::vec3& base, float r, float h, const glm::vec3& col,
              const glm::mat4& P = glm::mat4(1.f), int seg = 10, float mat = MAT_PLAIN);
    void sphere(const glm::vec3& center, const glm::vec3& radii, const glm::vec3& col,
                const glm::mat4& P = glm::mat4(1.f), int slices = 10, int stacks = 6,
                float mat = MAT_PLAIN);
    // triangular prism, ridge along local X, triangle spans local Z
    void gable(const glm::vec3& base, const glm::vec3& size, const glm::vec3& col,
               const glm::mat4& P = glm::mat4(1.f), float mat = MAT_PLAIN);
    // 5x7 pixel font drawn in the local XY plane, reading toward +X, facing +Z
    void text(const std::string& s, const glm::mat4& P, float px, const glm::vec3& col,
              float mat = MAT_EMISSIVE);

    void upload(bool dynamic = false);
    void draw() const;

private:
    std::vector<Vertex> verts;
    GLuint vao = 0, vbo = 0;
    GLsizei drawCount = 0;
    void push(const glm::mat4& M, const glm::mat3& N, const glm::vec3& lp,
              const glm::vec3& ln, const glm::vec3& c, float m);
};
