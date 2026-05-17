
#pragma once
#include"Grid.hpp"
#include"Shader.hpp"
#include"glm/glm/gtc/type_ptr.hpp";


Grid::Grid(int halfLines, float step, int majorEvery) {
    float size = halfLines * step;

    verticies.reserve((halfLines * 2 + 1) * 4); // kabaca

    for (int i = -halfLines; i <= halfLines; ++i) {
        float x = i * step;
        bool major = (i % majorEvery == 0);
        verticies.push_back({ {x, -size, 0.0f}, major ? 1.0f : 0.0f });
        verticies.push_back({ {x,  size, 0.0f}, major ? 1.0f : 0.0f });
    }
    // Y sabit, X boyunca (yatay çizgiler)
    for (int j = -halfLines; j <= halfLines; ++j) {
        float y = j * step;
        bool major = (j % majorEvery == 0);
        verticies.push_back({ {-size, y, 0.0f}, major ? 1.0f : 0.0f });
        verticies.push_back({ { size, y, 0.0f}, major ? 1.0f : 0.0f });
    }

   
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), verticies.data(), GL_STATIC_DRAW);

    // layout(location=0) vec3 aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    // layout(location=1) float aMajor
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, isMajor));

    glBindVertexArray(0);
}

Grid::~Grid() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void Grid::draw(Shader& shader, const glm::mat4& mvp) {
    shader.use();
    shader.setFloat("uLineWidth", lineWidth);
    // shader.set* fonksiyonlarýn vektör desteklemiyorsa glUniform kullan:
    GLint loc;
    loc = glGetUniformLocation(shader.ID, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));
    loc = glGetUniformLocation(shader.ID, "uThinColor");
    glUniform3fv(loc, 1, glm::value_ptr(thinColor));
    loc = glGetUniformLocation(shader.ID, "uMajorColor");
    glUniform3fv(loc, 1, glm::value_ptr(majorColor));

    glBindVertexArray(vao);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(verticies.size()));
    glBindVertexArray(0);
}