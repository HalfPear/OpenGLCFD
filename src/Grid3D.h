#pragma once
#include <cassert>
#include <glm/glm/glm.hpp>

struct Grid3D {
    int nx, ny, nz;
    float h;
    Grid3D(int nx, int ny, int nz, float h) : nx(nx), ny(ny), nz(nz), h(h) {}

    inline int idx(int i, int j, int k) const {
        if (!(i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz)) {
            fprintf(stderr, "IDX OOB: i=%d j=%d k=%d | nx=%d ny=%d nz=%d\n", i, j, k, nx, ny, nz);
            __debugbreak(); 
        }
        return (k * ny + j) * nx + i;
    }

    inline glm::vec3 center(int i, int j, int k) const {
        assert(i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz);
        return glm::vec3((i + 0.5f) * h, (j + 0.5f) * h, (k + 0.5f) * h);
    }
};
