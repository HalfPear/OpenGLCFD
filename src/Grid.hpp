
#pragma once

#include<vector>
#include"glad/glad.h"
#include"glm/glm/glm.hpp"

class Shader;
class  Grid
{
public:
	Grid(int halfLines = 50, float step = 1.0f, int majorEvry = 10);
	~Grid();

	void draw(Shader& shader, const glm::mat4& mvp);

	void setColors(const glm::vec3& thin, const glm::vec3& major) { thinColor = thin; majorColor = major; }
	void setLineWidth(float w) { lineWidth = w; }



private:

	struct Vertex { glm::vec3 pos; float isMajor; };

	std::vector<Vertex>verticies;
	std::vector<GLuint> indices;
	GLuint vao = 0, vbo = 0, ebo = 0;
	glm::vec3 thinColor = { 0.35f,0.40f,0.50f };
	glm::vec3 majorColor = { 0.80f,0.85f,0.90f };
	float lineWidth = 1.0f;
};

