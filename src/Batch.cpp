#include "Batch.h"
#include <cctype>
#include <cmath>
#include <cstddef>

using namespace glm;
static const float PI = 3.14159265358979f;

static mat3 normalMat(const mat4& M) { return transpose(inverse(mat3(M))); }

Batch::~Batch() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void Batch::push(const mat4& M, const mat3& N, const vec3& lp, const vec3& ln,
                 const vec3& c, float m) {
    Vertex v;
    v.p = vec3(M * vec4(lp, 1.f));
    v.n = normalize(N * ln);
    v.c = c;
    v.m = m;
    verts.push_back(v);
}

void Batch::box(const vec3& base, const vec3& size, const vec3& col, const mat4& P, float m) {
    mat4 M = P * translate(mat4(1.f), base + vec3(0, size.y * 0.5f, 0)) * scale(mat4(1.f), size);
    mat3 N = normalMat(M);
    static const vec3 nrm[6] = { vec3(1, 0, 0), vec3(-1, 0, 0), vec3(0, 1, 0),
                                 vec3(0, -1, 0), vec3(0, 0, 1), vec3(0, 0, -1) };
    for (int f = 0; f < 6; ++f) {
        vec3 n = nrm[f];
        vec3 ref = (std::fabs(n.y) > 0.5f) ? vec3(0, 0, 1) : vec3(0, 1, 0);
        vec3 u = normalize(cross(n, ref));
        vec3 t = cross(n, u);
        vec3 c0 = n * 0.5f - u * 0.5f - t * 0.5f;
        vec3 c1 = n * 0.5f + u * 0.5f - t * 0.5f;
        vec3 c2 = n * 0.5f + u * 0.5f + t * 0.5f;
        vec3 c3 = n * 0.5f - u * 0.5f + t * 0.5f;
        push(M, N, c0, n, col, m); push(M, N, c1, n, col, m); push(M, N, c2, n, col, m);
        push(M, N, c0, n, col, m); push(M, N, c2, n, col, m); push(M, N, c3, n, col, m);
    }
}

void Batch::cyl(const vec3& base, float r, float h, const vec3& col, const mat4& P, int seg, float m) {
    mat4 M = P * translate(mat4(1.f), base) * scale(mat4(1.f), vec3(r, h, r));
    mat3 N = normalMat(M);
    const vec3 up(0, 1, 0), dn(0, -1, 0);
    for (int i = 0; i < seg; ++i) {
        float a0 = 2.f * PI * i / seg, a1 = 2.f * PI * (i + 1) / seg;
        float c0 = std::cos(a0), s0 = std::sin(a0), c1 = std::cos(a1), s1 = std::sin(a1);
        vec3 p0(c0, 0, s0), p1(c1, 0, s1), p2(c1, 1, s1), p3(c0, 1, s0);
        vec3 n0(c0, 0, s0), n1(c1, 0, s1);
        push(M, N, p0, n0, col, m); push(M, N, p1, n1, col, m); push(M, N, p2, n1, col, m);
        push(M, N, p0, n0, col, m); push(M, N, p2, n1, col, m); push(M, N, p3, n0, col, m);
        push(M, N, vec3(0, 1, 0), up, col, m); push(M, N, p3, up, col, m); push(M, N, p2, up, col, m);
        push(M, N, vec3(0, 0, 0), dn, col, m); push(M, N, p1, dn, col, m); push(M, N, p0, dn, col, m);
    }
}

void Batch::cylX(const vec3& center, float r, float len, const vec3& col, const mat4& P, int seg, float m) {
    mat4 P2 = P * translate(mat4(1.f), center) * rotate(mat4(1.f), PI * 0.5f, vec3(0, 0, 1));
    cyl(vec3(0, -len * 0.5f, 0), r, len, col, P2, seg, m);
}

void Batch::cylZ(const vec3& center, float r, float len, const vec3& col, const mat4& P, int seg, float m) {
    mat4 P2 = P * translate(mat4(1.f), center) * rotate(mat4(1.f), PI * 0.5f, vec3(1, 0, 0));
    cyl(vec3(0, -len * 0.5f, 0), r, len, col, P2, seg, m);
}

void Batch::cone(const vec3& base, float r, float h, const vec3& col, const mat4& P, int seg, float m) {
    mat4 M = P * translate(mat4(1.f), base) * scale(mat4(1.f), vec3(r, h, r));
    mat3 N = normalMat(M);
    const vec3 dn(0, -1, 0);
    for (int i = 0; i < seg; ++i) {
        float a0 = 2.f * PI * i / seg, a1 = 2.f * PI * (i + 1) / seg, am = 0.5f * (a0 + a1);
        vec3 p0(std::cos(a0), 0, std::sin(a0)), p1(std::cos(a1), 0, std::sin(a1));
        vec3 n0(std::cos(a0), 1, std::sin(a0)), n1(std::cos(a1), 1, std::sin(a1));
        vec3 nm(std::cos(am), 1, std::sin(am));
        push(M, N, p0, n0, col, m); push(M, N, p1, n1, col, m); push(M, N, vec3(0, 1, 0), nm, col, m);
        push(M, N, vec3(0, 0, 0), dn, col, m); push(M, N, p1, dn, col, m); push(M, N, p0, dn, col, m);
    }
}

void Batch::sphere(const vec3& center, const vec3& radii, const vec3& col, const mat4& P,
                   int slices, int stacks, float m) {
    mat4 M = P * translate(mat4(1.f), center) * scale(mat4(1.f), radii);
    mat3 N = normalMat(M);
    auto pt = [&](int i, int j) {
        float phi = PI * (float)i / stacks - PI * 0.5f;
        float th = 2.f * PI * (float)j / slices;
        return vec3(std::cos(phi) * std::cos(th), std::sin(phi), std::cos(phi) * std::sin(th));
    };
    for (int i = 0; i < stacks; ++i)
        for (int j = 0; j < slices; ++j) {
            vec3 a = pt(i, j), b = pt(i, j + 1), c = pt(i + 1, j + 1), d = pt(i + 1, j);
            push(M, N, a, a, col, m); push(M, N, b, b, col, m); push(M, N, c, c, col, m);
            push(M, N, a, a, col, m); push(M, N, c, c, col, m); push(M, N, d, d, col, m);
        }
}

void Batch::gable(const vec3& base, const vec3& size, const vec3& col, const mat4& P, float m) {
    mat4 M = P * translate(mat4(1.f), base);
    mat3 N = normalMat(M);
    float hw = size.x * 0.5f, hd = size.z * 0.5f, h = size.y;
    vec3 A0(-hw, 0, -hd), B0(-hw, 0, hd), C0(-hw, h, 0);
    vec3 A1(hw, 0, -hd), B1(hw, 0, hd), C1(hw, h, 0);
    vec3 ctr(0, h / 3.f, 0);
    auto tri = [&](vec3 a, vec3 b, vec3 c) {
        vec3 n = normalize(cross(b - a, c - a));
        if (dot(n, (a + b + c) / 3.f - ctr) < 0) n = -n;
        push(M, N, a, n, col, m); push(M, N, b, n, col, m); push(M, N, c, n, col, m);
    };
    tri(A0, B0, C0); tri(A1, B1, C1);
    tri(A0, A1, C1); tri(A0, C1, C0);
    tri(B0, B1, C1); tri(B0, C1, C0);
    tri(A0, B0, B1); tri(A0, B1, A1);
}

// --- tiny 5x7 font: A-Z, 0-9, '.', '-', '\'' ---------------------------------------------------
static const char* GLYPHS[] = {
    "01110" "10001" "10001" "11111" "10001" "10001" "10001", // A
    "11110" "10001" "10001" "11110" "10001" "10001" "11110", // B
    "01110" "10001" "10000" "10000" "10000" "10001" "01110", // C
    "11110" "10001" "10001" "10001" "10001" "10001" "11110", // D
    "11111" "10000" "10000" "11110" "10000" "10000" "11111", // E
    "11111" "10000" "10000" "11110" "10000" "10000" "10000", // F
    "01110" "10001" "10000" "10111" "10001" "10001" "01110", // G
    "10001" "10001" "10001" "11111" "10001" "10001" "10001", // H
    "01110" "00100" "00100" "00100" "00100" "00100" "01110", // I
    "00111" "00010" "00010" "00010" "00010" "10010" "01100", // J
    "10001" "10010" "10100" "11000" "10100" "10010" "10001", // K
    "10000" "10000" "10000" "10000" "10000" "10000" "11111", // L
    "10001" "11011" "10101" "10101" "10001" "10001" "10001", // M
    "10001" "11001" "10101" "10011" "10001" "10001" "10001", // N
    "01110" "10001" "10001" "10001" "10001" "10001" "01110", // O
    "11110" "10001" "10001" "11110" "10000" "10000" "10000", // P
    "01110" "10001" "10001" "10001" "10101" "10010" "01101", // Q
    "11110" "10001" "10001" "11110" "10100" "10010" "10001", // R
    "01110" "10001" "10000" "01110" "00001" "10001" "01110", // S
    "11111" "00100" "00100" "00100" "00100" "00100" "00100", // T
    "10001" "10001" "10001" "10001" "10001" "10001" "01110", // U
    "10001" "10001" "10001" "10001" "10001" "01010" "00100", // V
    "10001" "10001" "10001" "10101" "10101" "10101" "01010", // W
    "10001" "10001" "01010" "00100" "01010" "10001" "10001", // X
    "10001" "10001" "01010" "00100" "00100" "00100" "00100", // Y
    "11111" "00001" "00010" "00100" "01000" "10000" "11111", // Z
    "01110" "10001" "10011" "10101" "11001" "10001" "01110", // 0
    "00100" "01100" "00100" "00100" "00100" "00100" "01110", // 1
    "01110" "10001" "00001" "00010" "00100" "01000" "11111", // 2
    "11110" "00001" "00001" "01110" "00001" "00001" "11110", // 3
    "00010" "00110" "01010" "10010" "11111" "00010" "00010", // 4
    "11111" "10000" "11110" "00001" "00001" "10001" "01110", // 5
    "00110" "01000" "10000" "11110" "10001" "10001" "01110", // 6
    "11111" "00001" "00010" "00100" "01000" "01000" "01000", // 7
    "01110" "10001" "10001" "01110" "10001" "10001" "01110", // 8
    "01110" "10001" "10001" "01111" "00001" "00010" "01100", // 9
    "00000" "00000" "00000" "00000" "00000" "01100" "01100", // .
    "00000" "00000" "00000" "11111" "00000" "00000" "00000", // -
    "00100" "00100" "01000" "00000" "00000" "00000" "00000", // '
};

static int glyphIndex(char ch) {
    ch = (char)std::toupper((unsigned char)ch);
    if (ch >= 'A' && ch <= 'Z') return ch - 'A';
    if (ch >= '0' && ch <= '9') return 26 + (ch - '0');
    if (ch == '.') return 36;
    if (ch == '-') return 37;
    if (ch == '\'') return 38;
    return -1;
}

void Batch::text(const std::string& s, const mat4& P, float px, const vec3& col, float m) {
    mat3 N = normalMat(P);
    const vec3 n(0, 0, 1);
    float pen = 0.f;
    for (char ch : s) {
        int gi = glyphIndex(ch);
        if (gi >= 0) {
            const char* g = GLYPHS[gi];
            for (int row = 0; row < 7; ++row)
                for (int cl = 0; cl < 5; ++cl)
                    if (g[row * 5 + cl] == '1') {
                        float x0 = pen + cl * px, y0 = (6 - row) * px;
                        vec3 a(x0, y0, 0), b(x0 + px, y0, 0), c(x0 + px, y0 + px, 0), d(x0, y0 + px, 0);
                        push(P, N, a, n, col, m); push(P, N, b, n, col, m); push(P, N, c, n, col, m);
                        push(P, N, a, n, col, m); push(P, N, c, n, col, m); push(P, N, d, n, col, m);
                    }
        }
        pen += 6.f * px;
    }
}

void Batch::upload(bool dynamic) {
    if (!vao) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, c));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m));
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(),
                 dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    drawCount = (GLsizei)verts.size();
}

void Batch::draw() const {
    if (!drawCount) return;
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, drawCount);
}
