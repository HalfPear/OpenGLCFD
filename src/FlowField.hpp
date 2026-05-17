#pragma once
#include <vector>
#include <glm/glm/glm.hpp>
#include"Grid3D.h"


struct FlowField {


	Grid3D g;
	std::vector<glm::vec3>vel;
	std::vector<unsigned char>solid;//araba

	FlowField(const Grid3D& grid) :g(grid) {


		int N = g.nx * g.ny * g.nz;
		vel.assign(N, glm::vec3(0));
		solid.assign(N, 0);

	}
	glm::vec3 sampleVelNearest(const glm::vec3& p) const;       
	glm::vec3 sampleVelTrilinear(const glm::vec3& p) const;
	glm::vec3& V(int i, int j, int k) { return vel[g.idx(i, j, k)]; }
	unsigned char& S(int i, int j, int k) { return solid[g.idx(i, j, k)]; }
};