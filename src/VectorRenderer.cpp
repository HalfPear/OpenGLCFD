#include "VectorRenderer.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

void VectorRenderer::update(const FlowField& f, int step, float /*scaleIgnored*/) {
    if (step <= 0) step = 1;

  
    float maxMag = 0.0f;
    for (int k = 0; k < f.g.nz; k += step)
        for (int j = 0; j < f.g.ny; j += step)
            for (int i = 0; i < f.g.nx; i += step) {
                int id = f.g.idx(i, j, k);
                if (!f.solid[id]) maxMag = std::max(maxMag, glm::length(f.vel[id]));
            }
    if (maxMag < 1e-8f) maxMag = 1e-8f;
    static float ema = 0.0f;
    float alpha = 0.2f;
    ema = (ema == 0.0f ? maxMag : (1 - alpha) * ema + alpha * maxMag);
    float vmax = ema;

    
    const float lineLen = f.g.h*3;        

    std::vector<LineVert> lines;
    lines.reserve((size_t)((f.g.nx + step - 1) / step) *
        ((f.g.ny + step - 1) / step) *
        ((f.g.nz + step - 1) / step) * 2);

    for (int k = 0; k < f.g.nz; k += step)
        for (int j = 0; j < f.g.ny; j += step)
            for (int i = 0; i < f.g.nx; i += step) {
                int id = f.g.idx(i, j, k);
                if (f.solid[id]) continue;

                glm::vec3 p = f.g.center(i, j, k);
                glm::vec3 v = f.vel[id];
                float m = glm::length(v);
                if (m < 1e-10f) continue;      

                // renk (0..1)
                float s01 = glm::clamp(m / vmax, 0.0f, 1.0f);

                
                glm::vec3 dir = v / (m*75.0f);
                glm::vec3 a = p - 0.5f * lineLen * dir;
                glm::vec3 b = p + 0.5f * lineLen * dir*100.0f;

                lines.push_back({ a, s01 });
                lines.push_back({ b, s01 });
            }

    count = (GLsizei)lines.size();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(LineVert), lines.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, s01));
    glBindVertexArray(0);
}



void VectorRenderer::draw(GLuint programID, const glm::mat4& mvp) {
    glUseProgram(programID);
    glUniformMatrix4fv(glGetUniformLocation(programID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, count);
    glBindVertexArray(0);
}
