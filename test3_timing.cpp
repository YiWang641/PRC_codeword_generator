#include "PRCWatermarker.h"
#include "Config.h"
#include <chrono>
#include <iostream>
#include <vector>

int main() {
    Config cfg;
    PRCWatermarker wm(cfg.l, cfg.n, cfg.k, cfg.t);

    constexpr int DET_TRIALS = 1000;

    // Pre-generate unique codewords, one per trial
    std::vector<std::vector<uint8_t>> cws;
    cws.reserve(DET_TRIALS);
    for (int i = 0; i < DET_TRIALS; ++i) {
        cws.push_back(wm.encode());
    }

    // Time on-the-fly detect()
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < DET_TRIALS; ++i) {
        wm.detect(cws[i]);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double avg_onfly = std::chrono::duration<double>(t1 - t0).count() / DET_TRIALS;

    // Time precomputed-XOR detect()
    auto t2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < DET_TRIALS; ++i) {
        wm.detect_precomputed(cws[i]);
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    double avg_pre   = std::chrono::duration<double>(t3 - t2).count() / DET_TRIALS;

    std::cout
        << "Average detect()           : " << avg_onfly << " s per call\n"
        << "Average detect_precomputed(): " << avg_pre   << " s per call\n";

    return 0;
}
