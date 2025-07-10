#pragma once

#include <cstddef>

// Configuration for PRCWatermarker.
// Edit these defaults and rebuild to change behavior.
struct Config {
    int    l = 500;        // codeword length
    std::size_t n = 1'000'000;  // number of parity-check sets
    int    k = 1367;         // checks per set
    int    t = 1010;         // detection threshold
    double p = 0.1;        // flip probability for Test2
};
