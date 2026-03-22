#include "leb_index.hpp"
#include "util.hpp"
#include <fstream>
#include <sstream>
#include <unordered_set>

using namespace leb;

// Read FIMI-style datasets
static std::vector<std::vector<Item>> read_fimi(const std::string& path) {
    std::ifstream fin(path);
    if (!fin) throw std::runtime_error("Cannot open " + path);
    std::vector<std::vector<Item>> data;
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::vector<Item> items;
        Item x;
        while (iss >> x) items.push_back(x);
        std::sort(items.begin(), items.end());
        items.erase(std::unique(items.begin(), items.end()), items.end());
        data.push_back(std::move(items));
    }
    return data;
}

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: main_build <dataset_path> <M> <bpt_order> <out_index_stub>\n";
        return 1;
    }
    std::string path = argv[1];
    int M = std::stoi(argv[2]);
    int order = std::stoi(argv[3]);
    std::string out = argv[4];

    auto data = read_fimi(path);
    LeBIndex idx(M, order);
    Timer t; t.tick();
    idx.build(std::move(data));
    double ms = t.to_ms();

    // Report stats
    std::cout << "INDEX_BUILD_MS " << ms << "\n";
    std::cout << "SETS " << idx.sets.size() << " M " << M << " b " << idx.packer.bits_per_field() << "\n";
    return 0;
}