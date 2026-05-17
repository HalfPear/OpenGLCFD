#pragma once
#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <array>

class BoxWire {
public:
    BoxWire();
    ~BoxWire();

    void setAABB(const glm::vec3& mn, const glm::vec3& mx);

    
    void draw(GLuint programID, const glm::mat4& mvp, const glm::vec3& color);

private:
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei idxCount = 0;
    void initGL();
};
