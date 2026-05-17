#pragma once
#include<vector>
#include<glm/glm/glm.hpp>
#include"FlowField.hpp"	


class Solver {

public:

	explicit Solver(FlowField& field, float tau = 0.8f, float uIn = 0.06f);

	void reset();
	void step(float/*dt*/);
private:
	FlowField& f;

	int nx, ny, nz; size_t N;
	float tau, omega;
	float uIn;
	

	//Daðýlým
	std::vector<float> F;
	std::vector<float>Fstmp;
	
	std::vector<float>rho;

	// D3Q19

	static constexpr int Q = 19;
	static const int ex[Q], ey[Q], ez[Q], opp[Q];
	static const float w[Q];

	inline int fi(int i, int j, int k, int q)const {  return (((size_t)k * (size_t)ny + (size_t)j) * (size_t)nx + (size_t)i) * (size_t)Q
		+ (size_t)q;
	}
	inline bool inside(int i, int j, int k) const {
		return (i >= 0 && i < nx) && (j >= 0 && j < ny) && (k >= 0 && k < nz);
	}
	void colide();
	void stream();
	void applyOutletToF();
	void computeMacro();
	void setEquilibrium(int i, int j, int k, float rhoVal, const glm::vec3& u);

	void setEquilibriumToBuf(std::vector<float>& buf,
		int i, int j, int k, float r, const glm::vec3& u);
	void applyInletToFtmp();
	void pumpInsideBox(int i0, int i1, int j0, int j1, int k0, int k1,
		float rhoTarget, float alpha);

	std::pair<float, glm::vec3>macroFromBufAt(const std::vector<float>& buf, int i, int j, int k) const;

};