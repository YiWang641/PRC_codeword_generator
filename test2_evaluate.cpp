#include "PRCWatermarker.h"
#include "Config.h"
#include <iostream>
#include <random>
#include <vector>

int main() {
    Config cfg;
    PRCWatermarker wm(cfg.l, cfg.n, cfg.k, cfg.t);

    constexpr int TRIALS = 1000;
    std::mt19937 rng(std::random_device{}());
    std::bernoulli_distribution flip(cfg.p);
    std::bernoulli_distribution randbit(0.5);

    int detected_modified = 0;
    int detected_random   = 0;

    // Modified watermarked codewords
    for (int i = 0; i < TRIALS; ++i) {
        auto cw = wm.encode();
        for (auto &b : cw) if (flip(rng)) b ^= 1;
        if (wm.detect(cw)) ++detected_modified;
    }

    // Totally random strings
    for (int i = 0; i < TRIALS; ++i) {
        std::vector<uint8_t> rnd(cfg.l);
        for (int j = 0; j < cfg.l; ++j) rnd[j] = randbit(rng);
        if (wm.detect(rnd)) ++detected_random;
    }

    std::cout
        << "Out of " << TRIALS << " modified codewords, detected: "
        << detected_modified << "\n"
        << "Out of " << TRIALS << " random strings, detected:   "
        << detected_random << "\n";

    return 0;
}
