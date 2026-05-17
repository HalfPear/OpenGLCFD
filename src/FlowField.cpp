#include"FlowField.hpp"

// FlowField.cpp
glm::vec3 FlowField::sampleVelNearest(const glm::vec3& p) const {
    if (p.x < 0 || p.y < 0 || p.z < 0) return glm::vec3(0);
    if (p.x >= g.nx * g.h || p.y >= g.ny * g.h || p.z >= g.nz * g.h) return glm::vec3(0);

    int i = (int)floorf(p.x / g.h);
    int j = (int)floorf(p.y / g.h);
    int k = (int)floorf(p.z / g.h);
    return solid[g.idx(i, j, k)] ? glm::vec3(0) : vel[g.idx(i, j, k)];
}


glm::vec3 FlowField::sampleVelTrilinear(const glm::vec3& p) const {
    float fx = p.x / g.h - 0.5f;
    float fy = p.y / g.h - 0.5f;
    float fz = p.z / g.h - 0.5f;
    int i0 = (int)floorf(fx), j0 = (int)floorf(fy), k0 = (int)floorf(fz);
    int i1 = i0 + 1, j1 = j0 + 1, k1 = k0 + 1;

    auto safe = [&](int i, int j, int k)->glm::vec3 {
        if (i < 0 || i >= g.nx || j < 0 || j >= g.ny || k < 0 || k >= g.nz) return glm::vec3(0);
        if (solid[g.idx(i, j, k)]) return glm::vec3(0); 
        return vel[g.idx(i, j, k)];
        };

    float tx = fx - i0, ty = fy - j0, tz = fz - k0;
    glm::vec3 c000 = safe(i0, j0, k0);
    glm::vec3 c100 = safe(i1, j0, k0);
    glm::vec3 c010 = safe(i0, j1, k0);
    glm::vec3 c110 = safe(i1, j1, k0);
    glm::vec3 c001 = safe(i0, j0, k1);
    glm::vec3 c101 = safe(i1, j0, k1);
    glm::vec3 c011 = safe(i0, j1, k1);
    glm::vec3 c111 = safe(i1, j1, k1);

    glm::vec3 c00 = mix(c000, c100, tx);
    glm::vec3 c10 = mix(c010, c110, tx);
    glm::vec3 c01 = mix(c001, c101, tx);
    glm::vec3 c11 = mix(c011, c111, tx);
    glm::vec3 c0 = mix(c00, c10, ty);
    glm::vec3 c1 = mix(c01, c11, ty);
    return mix(c0, c1, tz);
}