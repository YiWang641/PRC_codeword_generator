#include "PRCWatermarker.h"
#include "Config.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    Config cfg;
    PRCWatermarker wm(cfg.l, cfg.n, cfg.k, cfg.t);

    constexpr int TRIALS = 1000;
    std::ofstream ofs("test1_codewords.txt");
    if (!ofs) {
        std::cerr << "Error: could not open test1_codewords.txt\n";
        return 1;
    }

    using Clock = std::chrono::high_resolution_clock;

    // Encode & save
    std::vector<std::vector<uint8_t>> all_cw;
    all_cw.reserve(TRIALS);

    auto t0 = Clock::now();
    for (int i = 0; i < TRIALS; ++i) {
        auto cw = wm.encode();
        all_cw.push_back(cw);
        for (auto bit : cw) {
            ofs << char('0' + bit);
        }
        ofs << '\n';
    }
    auto t1 = Clock::now();

    double avg_enc = std::chrono::duration<double>(t1 - t0).count() / double(TRIALS);

    // Decode & verify
    int fail_count = 0;
    auto t2 = Clock::now();
    for (int i = 0; i < TRIALS; ++i) {
        if (!wm.detect(all_cw[i])) {
            ++fail_count;
            std::cerr << "Detection failed on trial " << i << "\n";
        }
    }
    auto t3 = Clock::now();

    double avg_dec = std::chrono::duration<double>(t3 - t2).count() / double(TRIALS);

    std::cout
        << "Test 1 (timed)\n"
        << "Encoded & saved " << TRIALS << " codewords in "
        << std::chrono::duration<double>(t1 - t0).count() << " s  (avg "
        << avg_enc << " s)\n"
        << "Ran detect() on " << TRIALS << " codewords in "
        << std::chrono::duration<double>(t3 - t2).count() << " s  (avg "
        << avg_dec << " s)\n"
        << "Detection failures: " << fail_count << "\n";

    return (fail_count == 0 ? 0 : 2);
}
