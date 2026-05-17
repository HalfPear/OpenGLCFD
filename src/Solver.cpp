#include"Solver.hpp"
#include<algorithm>
#include<iostream>
#include<cmath>	


// D3Q19
static constexpr int Q = 19;

const int Solver::ex[Q] = {
    0,  1,-1, 0, 0, 0, 0,
    1,-1, 1,-1,   1,-1,   1,-1,   0, 0,  0,  0
};
const int Solver::ey[Q] = {
    0,  0, 0, 1,-1, 0, 0,
    1, 1,-1,-1,   0, 0,   0, 0,   1,-1,  1,-1
};
const int Solver::ez[Q] = {
    0,  0, 0, 0, 0, 1,-1,
    0, 0, 0, 0,   1, 1,  -1,-1,   1, 1, -1,-1
};
const int Solver::opp[Q] = {
    0,  2,1, 4,3, 6,5,
   10, 9,8, 7, 14,13, 12,11, 18,17, 16,15
};

const float Solver::w[Q] = {
    1.f / 3.f,
    1.f / 18.f,1.f / 18.f,1.f / 18.f,1.f / 18.f,1.f / 18.f,1.f / 18.f,
    1.f / 36.f,1.f / 36.f,1.f / 36.f,1.f / 36.f,1.f / 36.f,1.f / 36.f,
    1.f / 36.f,1.f / 36.f,1.f / 36.f,1.f / 36.f,1.f / 36.f,1.f / 36.f
};



Solver::Solver(FlowField& field, float tau, float uIn)
    : f(field), nx(field.g.nx), ny(field.g.ny), nz(field.g.nz),
    N(size_t(field.g.nx)* field.g.ny* field.g.nz),
    tau(tau), omega(1 / tau), uIn(uIn)
{


    F.assign(N * Q, 0.f);
    Fstmp.assign(N * Q, 0.f);
    rho.assign(N, 1.0f);
    reset();


}

void Solver::reset() {

    for(int k=0;k<nz;++k)
        for(int j=0;j<ny;++j)
            for (int i = 0; i < nx; ++i) {

                glm::vec3 u(0.0f);
                if (i == 0 && !f.S(i, j, k)) u.x = uIn;// havanın giriş kısmında katı kontrolü

                float r = 1.0f;
                rho[f.g.idx(i, j, k)] = r;

                for (int q = 0; q < Q; q++) {

                    glm::vec3 e(ex[q], ey[q], ez[q]);
                    float eu = 3.0f * glm::dot(e, u);
                    float uu = 1.5f * glm::dot(u, u);
                    F[fi(i, j, k, q)] = w[q] * r * (1.f + eu + 0.5f * eu * eu - uu);


                }


            }

   
    std::fill(f.vel.begin(), f.vel.end(), glm::vec3(0));
}

void Solver::step(float) {
    colide();                 
    applyInletToFtmp();       

  //inlet
    if (true) {
        int j = ny / 2, k = nz / 2;
        auto [r_in, u_in] = macroFromBufAt(Fstmp, 0, j, k);
        static int tick = 0;
        if ((++tick % 100) == 0) {
            std::cout << "[inlet Ftmp] rho=" << r_in
                << " |u|=" << glm::length(u_in) << "\n";
        }
    }
  
    applyInletToFtmp();

    stream();                
    applyOutletToF();        
    computeMacro();           
}



void Solver::colide() {
    // yerel BGK relaksasyonu
    for (int k = 0; k < nz; ++k)
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i) {
                int base = f.g.idx(i, j, k);
                if (f.solid[base]) { for (int q = 0; q < Q; ++q) Fstmp[fi(i, j, k, q)] = F[fi(i, j, k, q)];
                    continue;
                    
                }
                // makro
                float r = 0.f; glm::vec3 u(0);
                for (int q = 0; q < Q; ++q) {
                    float fiq = F[fi(i, j, k, q)];
                    r += fiq;
                    u += fiq * glm::vec3(ex[q], ey[q], ez[q]);
                }
                u /= (r > 0.f ? r : 1.f);
                rho[base] = r;

                // feq ve relaks
                for (int q = 0; q < Q; ++q) {
                    glm::vec3 e(ex[q], ey[q], ez[q]);
                    float eu = 3.0f * glm::dot(e, u);
                    float uu = 1.5f * glm::dot(u, u);
                    float feq = w[q] * r * (1.f + eu + 0.5f * eu * eu - uu);
                    float fiq = F[fi(i, j, k, q)];
                    Fstmp[fi(i, j, k, q)] = fiq - omega * (fiq - feq);
                }
            }
}

void Solver::stream() {
    std::fill(F.begin(), F.end(), 0.f);

    for (int k = 0; k < nz; ++k)
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i) {
                for (int q = 0; q < Q; ++q) {
                    float fiq = Fstmp[fi(i, j, k, q)];
                    int in = i + ex[q], jn = j + ey[q], kn = k + ez[q];

                    bool inside = (in >= 0 && in < nx) && (jn >= 0 && jn < ny) && (kn >= 0 && kn < nz);
                    if (inside && !f.solid[f.g.idx(in, jn, kn)]) {
                        // hedef hücreye aynı yönle it
                        F[fi(in, jn, kn, q)] += fiq;
                    }
                    else {
                        // duvar veya domain dışı
                        F[fi(i, j, k, opp[q])] += fiq;
                    }
                }
            }
}



void Solver::applyInletToFtmp() {
    for (int k = 0; k < nz; ++k)
        for (int j = 0; j < ny; ++j) {
            if (!f.S(0, j, k)) {
                setEquilibriumToBuf(Fstmp, 0, j, k, 1.0f, glm::vec3(uIn, 0, 0));
            }
        }
}

void Solver::applyOutletToF() {
    for (int k = 0; k < nz; ++k)
        for (int j = 0; j < ny; ++j) {
            int i = nx - 1;
            if (f.S(i, j, k)) continue;

        
            int baseN = f.g.idx(i - 1, j, k);
            float r = rho[baseN];
            glm::vec3 u = f.vel[baseN];

            setEquilibriumToBuf(F, i, j, k, r, u);
        }
}

void Solver::computeMacro() {
    const float rho_floor = 1e-6f;
    const float umax = 0.1f; 
    float uMaxSeen = 0.0f, rhoMin = 1e9f, rhoMax = -1e9f;

    for (int k = 0; k < nz; ++k)
        for (int j = 0; j < ny; ++j)
            for (int i = 0; i < nx; ++i) {
                int base = f.g.idx(i, j, k);
                if (f.solid[base]) { f.vel[base] = glm::vec3(0); continue; }

                float r = 0.f; glm::vec3 u(0);
                for (int q = 0; q < Q; ++q) {
                    float fiq = F[fi(i, j, k, q)];
                    r += fiq;
                    u += fiq * glm::vec3(ex[q], ey[q], ez[q]);
                }

                
                if (r < rho_floor || !std::isfinite(r)) { r = 1.0f; u = glm::vec3(0); }

                // hız clamp
                if (!std::isfinite(u.x) || !std::isfinite(u.y) || !std::isfinite(u.z)) u = glm::vec3(0);
                float m = glm::length(u);
                if (m > umax) u *= (umax / m);

                rho[base] = r;
                f.vel[base] = u;

                uMaxSeen = std::max(uMaxSeen, glm::length(u));
                rhoMin = std::min(rhoMin, r);
                rhoMax = std::max(rhoMax, r);
            }

    static int tick = 0;
    if ((++tick % 200) == 0) {
        std::cout << "[macro] |u|max=" << uMaxSeen
            << " rho:[" << rhoMin << "," << rhoMax << "]\n";
    }
}

void Solver::setEquilibrium(int i, int j, int k, float r, const glm::vec3& u) {
    for (int q = 0; q < Q; ++q) {
        glm::vec3 e(ex[q], ey[q], ez[q]);
        float eu = 3.0f * glm::dot(e, u);
        float uu = 1.5f * glm::dot(u, u);
        F[fi(i, j, k, q)] = w[q] * r * (1.f + eu + 0.5f * eu * eu - uu);
    }
}

void Solver::setEquilibriumToBuf(std::vector<float>& buf,
    int i, int j, int k, float r, const glm::vec3& u) {
    for (int q = 0; q < Q; ++q) {
        glm::vec3 e(ex[q], ey[q], ez[q]);
        float eu = 3.0f * glm::dot(e, u);
        float uu = 1.5f * glm::dot(u, u);
        buf[fi(i, j, k, q)] = w[q] * r * (1.f + eu + 0.5f * eu * eu - uu);
    }
}
std::pair<float, glm::vec3>
Solver::macroFromBufAt(const std::vector<float>& buf, int i, int j, int k) const {
    float r = 0.f; glm::vec3 u(0);
    for (int q = 0; q < Q; ++q) {
        float fiq = buf[fi(i, j, k, q)];
        r += fiq;
        u += fiq * glm::vec3(ex[q], ey[q], ez[q]);
    }
    if (r > 0) u /= r;
    return { r, u };
}
void Solver::pumpInsideBox(int i0, int i1, int j0, int j1, int k0, int k1,
    float rhoTarget, float alpha)
{
    for (int k = k0; k < k1; ++k)
        for (int j = j0; j < j1; ++j)
            for (int i = i0; i < i1; ++i) {
                if (f.S(i, j, k)) continue;                 
                auto [r, u] = macroFromBufAt(Fstmp, i, j, k); 
                r = r + alpha * (rhoTarget - r);          
                setEquilibriumToBuf(Fstmp, i, j, k, r, u);   
            }
}
