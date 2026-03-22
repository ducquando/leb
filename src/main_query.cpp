#include "leb_index.hpp"
#include "lebq.hpp"
#include "lebq_plus.hpp"
#include "util.hpp"
#include <random>
#include <fstream>
#include <sstream>

using namespace leb;

static std::vector<std::vector<Item>> read_fimi(const std::string& path);

int main(int argc, char** argv) {
    if (argc < 9) {
        std::cerr << "Usage: main_query <dataset_path> <M> <bpt_order> <algo:lebq|lebq+> <delta> <num_queries> <runs> <seed> [--filter none|sbdf|cbdf] [--log-cand 1]\n";
        return 1;
    }

    // Arguments
    std::string path = argv[1];
    int M = std::stoi(argv[2]);
    int order = std::stoi(argv[3]);
    std::string algo = argv[4];
    double delta = std::stod(argv[5]);
    int numQ = std::stoi(argv[6]);
    int runs = std::stoi(argv[7]);
    uint64_t seed = std::stoull(argv[8]);

    // Optional arguments
    std::string filter = "cbdf";
    bool log_cand = false;
    for (int i = 9; i < argc; ++i) {
        std::string a = argv[i];
        if (a=="--filter") { filter = argv[++i]; }
        else if (a=="--log-cand") { log_cand = std::stoi(argv[++i])!=0; }
    }

    auto data = read_fimi(path);
    LeBIndex idx(M, order);
    idx.build(std::move(data));
    if (idx.sets.empty()) {
        std::cout << "QUERY_AVG_MS 0\n";
        return 0;
    }

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<size_t> unif(0, idx.sets.size()-1);

    // Pick queries
    std::vector<std::vector<Item>> queries;
    queries.reserve(numQ);
    for (int i=0;i<numQ;++i) queries.push_back(idx.sets[unif(rng)]);

    double total_ms = 0.0;
    uint64_t total_candidates = 0;
    for (int r=0;r<runs;++r) {
        Timer t; t.tick();
        size_t total_found = 0;
        if (algo == "lebq") {
            LeBQ q(idx);
            q.mode = (filter=="none") ? FilterMode::NONE : (filter=="sbdf") ? FilterMode::SBDF : FilterMode::CBDF;
            for (auto& Q : queries) {
                uint64_t cand = 0;
                auto out = q.query(Q, delta, &cand);
                total_candidates += cand;
                total_found += out.size();
            }
        } else {
            LeBQPlus qp(idx);
            qp.mode = (filter=="none") ? FilterMode::NONE : (filter=="sbdf") ? FilterMode::SBDF : FilterMode::CBDF;
            for (auto& Q : queries) {
                uint64_t cand = 0;
                auto out = qp.query(Q, delta, &cand);
                total_candidates += cand;
                total_found += out.size();
            }
        }
        total_ms += t.to_ms();
    }

    std::cout << "QUERY_AVG_MS " << (total_ms / runs) << "\n";
    if (log_cand) std::cout << "CANDIDATES " << total_candidates / (double)(numQ * runs) << "\n";
    return 0;
}

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