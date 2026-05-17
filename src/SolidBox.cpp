#include "SolidBox.hpp"
#include <glm/glm/gtc/matrix_transform.hpp>

SolidBox::SolidBox() { buildUnitCube(); }
SolidBox::~SolidBox() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void SolidBox::buildUnitCube() {

    static const float verts[] = {
        -1,-1,-1,  +1,-1,-1,  +1,+1,-1,  -1,+1,-1,  // back
        -1,-1,+1,  +1,-1,+1,  +1,+1,+1,  -1,+1,+1   // front
    };
    static const unsigned short idx[] = {
        // back
        0,1,2,  0,2,3,
        // front
        4,6,5,  4,7,6,
        // left
        0,3,7,  0,7,4,
        // right
        1,5,6,  1,6,2,
        // bottom
        0,4,5,  0,5,1,
        // top
        3,2,6,  3,6,7
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void SolidBox::setAABB(const glm::vec3& bmin, const glm::vec3& bmax) {
   
    glm::vec3 center = 0.5f * (bmin + bmax);
    glm::vec3 half = 0.5f * (bmax - bmin);
    model = glm::translate(glm::mat4(1.0f), center);
    model = glm::scale(model, half);      
}

void SolidBox::draw(GLuint programID, const glm::mat4& viewProj) const {
    glm::mat4 mvp = viewProj * model;
    glUseProgram(programID);
    GLint loc;
    loc = glGetUniformLocation(programID, "uMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);
    loc = glGetUniformLocation(programID, "uColor");
    glUniform3f(loc, 1.0f, 1.0f, 1.0f);   // MAT BEYAZ
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
