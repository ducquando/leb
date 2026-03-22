#include "util.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace leb;

int main(int argc, char** argv) {
    if (argc < 2) { std::cerr << "Usage: main_stats <dataset_path>\n"; return 1; }
    std::ifstream fin(argv[1]);
    std::string line;
    size_t n=0, sum=0, minlen=SIZE_MAX, maxlen=0;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        size_t len = 0; uint32_t x; while (iss >> x) ++len;
        ++n; sum += len; minlen = std::min(minlen, len); maxlen = std::max(maxlen, len);
    }
    std::cout << "N " << n << " MIN " << minlen << " MAX " << maxlen << " AVG " << (double)sum/n << "\n";
}