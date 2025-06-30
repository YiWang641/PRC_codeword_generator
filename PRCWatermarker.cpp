#include "PRCWatermarker.h"
#include <cassert>
#include <bitset>
#include <random>
#include <utility>
#include <omp.h>        // for OpenMP pragmas

PRCWatermarker::PRCWatermarker(int l, std::size_t n, int k, int t)
  : l_(l)
  , n_(n)
  , k_(k)
  , t_(t)
  , paritySets_(n)
  , lhsMasks_(n, std::vector<std::bitset<MAXL+1>>(k))
  , rng_(std::random_device{}())
{
    assert(l_ <= MAXL);
    init_parity_sets_and_masks();
}

void PRCWatermarker::init_parity_sets_and_masks() {
    std::uniform_int_distribution<int> dist_pos(0, l_-1);

    for (std::size_t i = 0; i < n_; ++i) {
        auto& S = paritySets_[i];
        auto& M = lhsMasks_[i];
        S.reserve(k_);

        for (int j = 0; j < k_; ++j) {
            int a = dist_pos(rng_), b = dist_pos(rng_);
            while (b == a) b = dist_pos(rng_);
            S.emplace_back(a, b);

            M[j].reset();
            M[j].set(a);
            M[j].set(b);
            // RHS bit left 0
        }
    }
}

std::vector<uint8_t> PRCWatermarker::encode() {
    std::uniform_int_distribution<std::size_t> dist_set(0, n_-1);
    std::bernoulli_distribution              dist_id(0.5);

    std::size_t r  = dist_set(rng_);
    uint8_t     id = dist_id(rng_);

    // copy precomputed LHS masks
    std::vector<std::bitset<MAXL+1>> M = lhsMasks_[r];
    // set RHS
    for (int i = 0; i < k_; ++i) {
        M[i].set(l_, id);
    }

    // Gaussian elimination over GF(2)
    std::vector<int> pivot_col(k_, -1);
    int row = 0;
    for (int col = 0; col < l_ && row < k_; ++col) {
        int sel = -1;
        for (int i = row; i < k_; ++i) {
            if (M[i].test(col)) { sel = i; break; }
        }
        if (sel < 0) continue;
        std::swap(M[row], M[sel]);
        pivot_col[row] = col;
        for (int i = 0; i < k_; ++i) {
            if (i != row && M[i].test(col))
                M[i] ^= M[row];
        }
        ++row;
    }
    int rank = row;

    // sample solution
    std::vector<uint8_t> x(l_, 0);
    std::vector<bool>    is_pivot(l_, false);
    for (int i = 0; i < rank; ++i) {
        is_pivot[pivot_col[i]] = true;
    }
    std::bernoulli_distribution dist_bit(0.5);
    for (int c = 0; c < l_; ++c) {
        if (!is_pivot[c]) x[c] = dist_bit(rng_);
    }
    for (int i = rank - 1; i >= 0; --i) {
        int pc   = pivot_col[i];
        bool rhs = M[i].test(l_);
        for (int j = pc + 1; j < l_; ++j) {
            if (M[i].test(j) && x[j]) rhs = !rhs;
        }
        x[pc] = rhs;
    }

    return x;
}

bool PRCWatermarker::detect(const std::vector<uint8_t>& cw) const {
    assert((int)cw.size() == l_);
    int flagged = 0;

    #pragma omp parallel for schedule(dynamic) reduction(|:flagged)
    for (std::size_t i = 0; i < n_; ++i) {
        int c0 = 0, c1 = 0;
        for (auto [a,b] : paritySets_[i]) {
            uint8_t v = cw[a] ^ cw[b];
            (v ? ++c1 : ++c0);
        }
        if (c0 >= t_ || c1 >= t_) flagged = 1;
    }
    return flagged != 0;
}

bool PRCWatermarker::detect_precomputed(const std::vector<uint8_t>& cw) const {
    assert((int)cw.size() == l_);

    // 1) build full XOR table once
    std::size_t Msz = (std::size_t)l_ * (l_ - 1) / 2;
    std::vector<uint8_t> xorTable(Msz);
    std::size_t pos = 0;
    for (int a = 0; a < l_; ++a) {
        for (int b = a + 1; b < l_; ++b, ++pos) {
            xorTable[pos] = cw[a] ^ cw[b];
        }
    }

    // 2) parallel threshold over table-lookups
    int flagged = 0;
    #pragma omp parallel for schedule(dynamic) reduction(|:flagged)
    for (std::size_t i = 0; i < n_; ++i) {
        int c0 = 0, c1 = 0;
        for (auto [a,b] : paritySets_[i]) {
            std::size_t idx = pairIndex(a,b);
            (xorTable[idx] ? ++c1 : ++c0);
        }
        if (c0 >= t_ || c1 >= t_) flagged = 1;
    }
    return flagged != 0;
}
