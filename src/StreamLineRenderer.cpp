#include "StreamlineRenderer.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

StreamLineRenderer::StreamLineRenderer() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)offsetof(Vtx, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vtx), (void*)offsetof(Vtx, s01));
    glBindVertexArray(0);
}

StreamLineRenderer::~StreamLineRenderer() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void StreamLineRenderer::setSeeds(const std::vector<glm::vec3>& s) { seeds = s; }

void StreamLineRenderer::seedFromInlet(const FlowField& f, int sy, int sz) {
    seeds.clear();
    for (int j = 0; j < sy; ++j)
        for (int k = 0; k < sz; ++k) {
            float y = ((j + 0.5f) / sy) * (f.g.ny * f.g.h);
            float z = ((k + 0.5f) / sz) * (f.g.nz * f.g.h);
            float x = 0.5f * f.g.h; 
            seeds.emplace_back(x, y, z);
        }
}

// birim yön alaný
static inline glm::vec3 unitDir(const FlowField& f, const glm::vec3& p) {
    glm::vec3 v = f.sampleVelTrilinear(p);
    float m = glm::length(v);
    return (m > 1e-12f) ? (v / m) : glm::vec3(0);
}

//rk4 
glm::vec3 StreamLineRenderer::rk4(const FlowField& f, const glm::vec3& p, float stepWorld) const {
    glm::vec3 k1 = unitDir(f, p);
    glm::vec3 k2 = unitDir(f, p + 0.5f * stepWorld * k1);
    glm::vec3 k3 = unitDir(f, p + 0.5f * stepWorld * k2);
    glm::vec3 k4 = unitDir(f, p + stepWorld * k3);
    return p + (stepWorld / 6.0f) * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
}

void StreamLineRenderer::build(const FlowField& f, float dt, int maxSteps,
    float vmaxColor, float) {
   
    StreamLineRenderer st;
    float stepWorld = dt * f.g.h;

    packed.clear(); starts.clear(); counts.clear();
    if (seeds.empty()) return;

    for (const auto& s : seeds) {
        std::vector<Vtx> line;
        line.reserve(maxSteps);

        glm::vec3 p = s;
        for (int n = 0; n < maxSteps; ++n) {
            // domain dýþýna çýktýysa ya da duvar ise bitir
            if (p.x < 0 || p.x >= f.g.nx * f.g.h ||
                p.y < 0 || p.y >= f.g.ny * f.g.h ||
                p.z < 0 || p.z >= f.g.nz * f.g.h) break;

            int i = int(p.x / f.g.h), j = int(p.y / f.g.h), k = int(p.z / f.g.h);
            if (f.solid[f.g.idx(i, j, k)]) break;

            glm::vec3 v = f.sampleVelTrilinear(p);
            float m = glm::length(v);
            float s01 = glm::clamp(m / vmaxColor, 0.0f, 1.0f); 

            line.push_back({ p, s01 });

            glm::vec3 pNext = rk4(f, p, stepWorld);

            
            if (pNext.x < 0 || pNext.x >= f.g.nx * f.g.h ||
                pNext.y < 0 || pNext.y >= f.g.ny * f.g.h ||
                pNext.z < 0 || pNext.z >= f.g.nz * f.g.h) {
                p = pNext; break;
            }

            int i2 = int(pNext.x / f.g.h), j2 = int(pNext.y / f.g.h), k2 = int(pNext.z / f.g.h);
            if (f.solid[f.g.idx(i2, j2, k2)]) { p = pNext; break; }

            p = pNext;
        }

        if (line.size() >= 2) {
            starts.push_back((GLint)packed.size());
            counts.push_back((GLsizei)line.size());
            packed.insert(packed.end(), line.begin(), line.end());
        }
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, packed.size() * sizeof(Vtx), packed.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void StreamLineRenderer::draw(GLuint programID, const glm::mat4& mvp) {
    if (packed.empty()) return;
    glUseProgram(programID);
    glUniformMatrix4fv(glGetUniformLocation(programID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vao);
    for (size_t s = 0; s < starts.size(); ++s)
        glDrawArrays(GL_LINE_STRIP, starts[s], counts[s]);
    glBindVertexArray(0);
}