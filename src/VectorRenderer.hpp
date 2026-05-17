#pragma once

#include<vector>
#include<glad/glad.h>
#include<glm/glm/glm.hpp>
#include"FlowField.hpp"


struct LineVert {
	glm::vec3 pos;   // 12 byte
	float s01;       // 4  byte  (normalize hýz)
};

class VectorRenderer {

	public:

		VectorRenderer() { glGenVertexArrays(1, &vao); glGenBuffers(1, &vbo); }

		~VectorRenderer(){ if (vbo) glDeleteBuffers(1, &vbo); if (vao) glDeleteVertexArrays(1, &vao); }

		void update(const FlowField& field, int step, float scale);
		void draw(GLuint programID, const glm::mat4& mvp);

private:

	GLuint vbo=0;
	GLuint vao=0;
	GLsizei count = 0;

		

};