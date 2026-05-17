#pragma once
#include <glad/glad.h>
#include <glm/glm/glm.hpp>

class SolidBox {
public:
    SolidBox();
    ~SolidBox();
    //  AABB
    void setAABB(const glm::vec3& bmin, const glm::vec3& bmax);
   
    void draw(GLuint programID, const glm::mat4& viewProj) const;

private:
    GLuint vao = 0, vbo = 0, ebo = 0;
    glm::mat4 model{ 1.0f };
    void buildUnitCube();
};