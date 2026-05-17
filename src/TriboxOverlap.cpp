#include"TriboxOverlap.hpp"





  bool TriboxOverlap:: planeBoxOverlap(const glm::vec3 normal, float d, const glm::vec3& half) {

	glm::vec3 vmin, vmax;

	for (int q = 0; q < 3; q++) {

		if (normal[q] > 0.0f) { vmin[q] = -half[q]; vmax[q] = half[q]; }
		else { vmin[q] = half[q]; vmax[q] = -half[q]; }
	}

	if (glm::dot(normal, vmin) + d > 0.0f) return false;
	if (glm::dot(normal, vmax) + d >= 0.0f) return true;
	return false;

}
bool TriboxOverlap::triBoxOverlap(const glm::vec3& boxCenter,
	const glm::vec3& boxHalf,
	const glm::vec3& a,
	const glm::vec3& b,
	const glm::vec3& c)
{

	//merkeze kaydýrma
	glm::vec3 v0 = a - boxCenter;
	glm::vec3 v1 = b - boxCenter;
	glm::vec3 v2 = c - boxCenter;


	//kenerlara kaydýrma

	glm::vec3 e0 = v1 - v0;
	glm::vec3 e1 = v2 - v1;
	glm::vec3 e2 = v0 - v2;


	


	auto AXISTEST = [&](float pmin, float pmax, float rad) -> bool {
		return !(pmin > rad || pmax < -rad);
		};

	float minv, maxv, p0, p1, p2, rad;


	auto fabs3 = [](const glm::vec3& v) {return glm::vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z)); };


	// – Akenine-Möller’in sýrasý

	
		{
			glm::vec3 fe = fabs3(e0);
			// X01
			p0 = e0.z * v0.y - e0.y * v0.z;
			p2 = e0.z * v2.y - e0.y * v2.z;
			minv = std::min(p0, p2); maxv = std::max(p0, p2);
			rad = fe.z * boxHalf.y + fe.y * boxHalf.z;
			if (!AXISTEST(minv, maxv, rad)) return false;

			// Y02
			p0 = -e0.z * v0.x + e0.x * v0.z;
			p2 = -e0.z * v2.x + e0.x * v2.z;
			minv = std::min(p0, p2); maxv = std::max(p0, p2);
			rad = fe.z * boxHalf.x + fe.x * boxHalf.z;
			if (!AXISTEST(minv, maxv, rad)) return false;

			// Z12
			p1 = e0.y * v1.x - e0.x * v1.y;
			p2 = e0.y * v2.x - e0.x * v2.y;
			minv = std::min(p1, p2); maxv = std::max(p1, p2);
			rad = fe.y * boxHalf.x + fe.x * boxHalf.y;
			if (!AXISTEST(minv, maxv, rad)) return false;
		}

	{

			glm::vec3 fe = fabs3(e1);

			//X01

			p0 = e1.z * v0.y - e1.y * v0.z;
			p2 = e1.z * v2.y - e1.y * v2.z;
			minv = std::min(p0, p2); maxv = std::max(p0, p2);
			rad = fe.z * boxHalf.y + fe.y * boxHalf.z;
			if (!AXISTEST(minv, maxv, rad)) return false;

		


			// Y02
			p0 = -e1.z * v0.x + e1.x * v0.z;
			p2 = -e1.z * v2.x + e1.x * v2.z;
			minv = std::min(p0, p2); maxv = std::max(p0, p2);
			rad = fe.z * boxHalf.x + fe.x * boxHalf.z;
			if (!AXISTEST(minv, maxv, rad)) return false;

	
			// Z0
			p0 = e1.y * v0.x - e1.x * v0.y;
			p1 = e1.y * v1.x - e1.x * v1.y;
			minv = std::min(p0, p1); maxv = std::max(p0, p1);
			rad = fe.y * boxHalf.x + fe.x * boxHalf.y;
			if (!AXISTEST(minv, maxv, rad)) return false;
		
		
		}

	{
		glm::vec3 fe = fabs3(e2);
		// X2
		p0 = e2.z * v0.y - e2.y * v0.z;
		p1 = e2.z * v1.y - e2.y * v1.z;
		minv = std::min(p0, p1); maxv = std::max(p0, p1);
		rad = fe.z * boxHalf.y + fe.y * boxHalf.z;
		if (!AXISTEST(minv, maxv, rad)) return false;

		// Y1
		p0 = -e2.z * v0.x + e2.x * v0.z;
		p1 = -e2.z * v1.x + e2.x * v1.z;
		minv = std::min(p0, p1); maxv = std::max(p0, p1);
		rad = fe.z * boxHalf.x + fe.x * boxHalf.z;
		if (!AXISTEST(minv, maxv, rad)) return false;

		// Z12
		p1 = e2.y * v1.x - e2.x * v1.y;
		p2 = e2.y * v2.x - e2.x * v2.y;
		minv = std::min(p1, p2); maxv = std::max(p1, p2);
		rad = fe.y * boxHalf.x + fe.x * boxHalf.y;
		if (!AXISTEST(minv, maxv, rad)) return false;
	}

	// Üçgen AABB eksenleri

	auto findMinMax = [](float a, float b, float c, float& mn, float& mx) {


		mn = std::min(a, std::min(b, c));
		mx = std::max(a, std::max(b, c));

		};

	findMinMax(v0.x, v1.x, v2.x, minv, maxv);
	if (minv > boxHalf.x || maxv < -boxHalf.x) return false;
	findMinMax(v0.y, v1.y, v2.y, minv, maxv);
	if (minv > boxHalf.y || maxv < -boxHalf.y) return false;
	findMinMax(v0.z, v1.z, v2.z, minv, maxv);
	if (minv > boxHalf.z || maxv < -boxHalf.z) return false;

	// Kutu–düzlem kesiþimi
	glm::vec3 n = glm::cross(e0, e1);
	float d = -glm::dot(n, v0);
	if (!planeBoxOverlap(n, d, boxHalf)) return false;

	return true; 

}

