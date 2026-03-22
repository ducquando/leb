#pragma once
#include "util.hpp"
#include <queue>
#include <unordered_map>

namespace leb {
    // Sort items by freq desc then greedily assign to bucket with smallest cumulative freq
    struct BucketMapping {
        int M;
        std::vector<uint32_t> item2bucket;  // index by raw item id
        std::vector<uint64_t> bucket_freq;  // cumulative freqs

        explicit BucketMapping(int M_) : M(M_) {}

        void build(const std::vector<std::vector<Item>>& sets) {
            // Count global frequencies
            std::unordered_map<Item, uint64_t> freq;
            freq.reserve(1 << 20);
            Item max_item = 0;
            for (const auto& s : sets) {
                for (auto x : s) { ++freq[x]; if (x > max_item) max_item = x; }
            }

            // Sort by freq desc
            std::vector<std::pair<Item, uint64_t>> items(freq.begin(), freq.end());
            std::sort(items.begin(), items.end(), [](auto& a, auto& b) {
                if (a.second != b.second) return a.second > b.second;
                return a.first < b.first;
            });

            item2bucket.assign((size_t)max_item + 1, 0);
            bucket_freq.assign(M + 1, 0);

            // Min-heap by (cummulative_freq, bucket_id)
            using Node = std::pair<uint64_t,int>;
            auto cmp = [](const Node& a, const Node& b) {
                if (a.first != b.first) return a.first > b.first;
                return a.second > b.second;
            };
            std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);
            for (int i = 1; i <= M; ++i) pq.emplace(0ULL, i);

            for (auto &p : items) {
                auto [f, bid] = pq.top(); pq.pop();
                item2bucket[p.first] = (uint32_t)bid;
                uint64_t nf = f + p.second;
                bucket_freq[bid] = nf;
                pq.emplace(nf, bid);
            }
        }

        inline uint32_t bucket_of(Item x) const {
            if (x >= item2bucket.size()) return 0;
            return item2bucket[x];
        }

        // Map the set to [v0,...,vM]
        std::vector<uint32_t> map_set(const std::vector<Item>& s, int M) const {
            std::vector<uint32_t> V(M + 1, 0);
            V[0] = (uint32_t)s.size();
            for (auto x : s) {
                uint32_t b = bucket_of(x);
                if (b >= 1 && b <= (uint32_t)M) ++V[b];
            }
            return V;
        }
    };
}