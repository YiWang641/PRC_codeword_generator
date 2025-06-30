#include "PRCWatermarker.h"
#include "Config.h"
#include <iostream>

int main() {
    Config cfg;
    PRCWatermarker wm(cfg.l, cfg.n, cfg.k, cfg.t);

    auto cw = wm.encode();
    bool detected = wm.detect(cw);

    std::cout << "Watermark "
              << (detected ? "detected\n" : "not detected\n");
    return 0;
}
