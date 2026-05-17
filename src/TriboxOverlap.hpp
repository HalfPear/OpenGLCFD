#pragma once
#include <glm/glm/glm.hpp>
#include <cmath>
#include<algorithm>
class TriboxOverlap {


public:

	
    bool planeBoxOverlap(const glm::vec3 normal, float d, const glm::vec3& half);
 
    bool triBoxOverlap(const glm::vec3& boxCenter,
        const glm::vec3& boxHalf,
        const glm::vec3& a,
        const glm::vec3& b,
        const glm::vec3& c);




};
