#pragma once

#include <bitset>
#include <cstddef>
#include <random>
#include <utility>
#include <vector>

class PRCWatermarker {
public:
    using Pair = std::pair<int,int>;

    // maximum supported codeword length
    static constexpr int MAXL = 512;

    // ctor: l = codeword length, n = #parity‐sets,
    //       k = #pairs per set,  t = detection threshold
    PRCWatermarker(int l = 500,
                   std::size_t n = 1'000'000,
                   int k = 64,
                   int t = 53);

    // produce one watermarked codeword (0/1 vector of length l_)
    std::vector<uint8_t> encode();

    // test a codeword; returns true if watermark is detected
    // (on the fly XOR)
    bool detect(const std::vector<uint8_t>& cw) const;

    // optimized detect: precompute all a<b XORs once, then threshold
    bool detect_precomputed(const std::vector<uint8_t>& cw) const;

private:
    int                l_, k_, t_;
    std::size_t        n_;
    std::vector<std::vector<Pair>>            paritySets_;
    std::vector<std::vector<std::bitset<MAXL+1>>> lhsMasks_;
    mutable std::mt19937 rng_;  // mutable so detect() can use RNG if needed

    void init_parity_sets_and_masks();

    // flatten (a,b) with 0 ≤ a < b < l_ into [0..l(l–1)/2)
    inline std::size_t pairIndex(int a, int b) const {
        // offset = sum_{i=0}^{a-1}(l_-1 - i)
        //        = a*l_ - a*(a+1)/2
        return (std::size_t)a * l_ - (std::size_t)a * (a + 1) / 2 + (b - a - 1);
    }
};
