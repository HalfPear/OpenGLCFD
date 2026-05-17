#pragma once

#include<glad/glad.h>
#include<glm/glm/glm.hpp>

#include<vector>
#include<math.h>
#include"FlowField.hpp"



class  StreamLineRenderer
{
public:


	struct Vtx { glm::vec3 pos; float s01; };


	 StreamLineRenderer();
	~ StreamLineRenderer();

	void setSeeds(const std::vector<glm::vec3>& seeds);


	void build(const FlowField& f, float dt = 0.02f, int maxSteps = 400,
		float vmaxColor = 0.02f, float stopSpeed =1e-5);

	void seedFromInlet(const FlowField& f, int sy = 8, int sz = 8);
	void draw(GLuint programID, const glm::mat4& mvp);

private:


	GLuint vao = 0, vbo = 0;
	std::vector<Vtx>packed;
	std::vector<GLint>starts;
	std::vector<GLsizei>counts;
	std::vector<glm::vec3>seeds;



	glm::vec3 rk4(const FlowField& f, const glm::vec3& p, float dt)const;


};

