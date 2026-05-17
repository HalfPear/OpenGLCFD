#include "BoxWire.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

BoxWire::BoxWire() { initGL(); }
BoxWire::~BoxWire() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void BoxWire::initGL() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0); // vec3 pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

   
    static const GLuint edges[] = {
        // alt yüzey
        0,1, 1,3, 3,2, 2,0,
        // üst yüzey
        4,5, 5,7, 7,6, 6,4,
        // düþeyler
        0,4, 1,5, 2,6, 3,7
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);
    idxCount = (GLsizei)(sizeof(edges) / sizeof(GLuint));

    glBindVertexArray(0);
}

void BoxWire::setAABB(const glm::vec3& mn, const glm::vec3& mx) {
    std::array<glm::vec3, 8> c = {
        // alt (y=min)
        glm::vec3(mn.x, mn.y, mn.z), //0
        glm::vec3(mx.x, mn.y, mn.z), //1
        glm::vec3(mn.x, mn.y, mx.z), //2
        glm::vec3(mx.x, mn.y, mx.z), //3
        // üst (y=max)
        glm::vec3(mn.x, mx.y, mn.z), //4
        glm::vec3(mx.x, mx.y, mn.z), //5
        glm::vec3(mn.x, mx.y, mx.z), //6
        glm::vec3(mx.x, mx.y, mx.z)  //7
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(c), c.data(), GL_DYNAMIC_DRAW);
}

void BoxWire::draw(GLuint programID, const glm::mat4& mvp, const glm::vec3& color) {
    glUseProgram(programID);
    glUniformMatrix4fv(glGetUniformLocation(programID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(glGetUniformLocation(programID, "uColor"), 1, &color[0]);

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, idxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
