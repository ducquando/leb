#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace leb {
    using Item = uint32_t;      // item IDs in datasets
    using SetID = uint32_t;     // set ID
    using Key64 = uint64_t;     // packed key

    struct Timer {
        using clock = std::chrono::high_resolution_clock;
        clock::time_point t0;
        void tick() { t0 = clock::now(); }
        double to_ms() const {
            auto dt = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t0).count();
            return dt / 1000.0;
        }
    };

    // Merge-style Jaccard over sorted vectors
    inline double jaccard(const std::vector<Item>& a, const std::vector<Item>& b) {
        size_t i=0, j=0, inter=0;
        while (i < a.size() && j < b.size()) {
            if (a[i] == b[j]) { ++inter; ++i; ++j; }
            else if (a[i] < b[i]) { ++i; }
            else { ++j; }
        }
        const size_t uni = a.size() + b.size() - inter;
        return uni ? (double)inter / (double)uni : 1.0;
    }
}