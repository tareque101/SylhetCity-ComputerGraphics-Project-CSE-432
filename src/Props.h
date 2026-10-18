#pragma once
#include "Batch.h"
#include <random>
#include <string>
#include <vector>

namespace props {
inline float rf(std::mt19937& r, float a, float b) { return std::uniform_real_distribution<float>(a, b)(r); }
inline int ri(std::mt19937& r, int a, int b) { return std::uniform_int_distribution<int>(a, b)(r); }

void tree(Batch& b, const glm::vec3& pos, float scale, std::mt19937& r);
void lamp(Batch& b, const glm::vec3& pos, float yaw);          // arm points to local +Z
float signHeight(const std::vector<std::string>& lines, float width);
// board centred at `center`, facing local +Z rotated by yaw
void sign(Batch& b, const std::vector<std::string>& lines, const glm::vec3& center, float yaw,
          float width, const glm::vec3& bg, const glm::vec3& fg);
// board on two posts standing at groundPos
void signPost(Batch& b, const std::vector<std::string>& lines, const glm::vec3& groundPos,
              float yaw, float width);
void wall(Batch& b, const glm::vec2& a, const glm::vec2& c, float h, float thick, const glm::vec3& col);
void flag(Batch& b, const glm::vec3& pos, float h, float yaw);
}
