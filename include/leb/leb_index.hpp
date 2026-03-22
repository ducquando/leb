#pragma once
#include "bptree.hpp"
#include "buckets.hpp"
#include "keypacking.hpp"
#include <unordered_map>

namespace leb {
    struct LeBIndex {
        int M;
        KeyPacker packer;
        BucketMapping bm;
        BPTree tree;
        std::vector<std::vector<Item>> sets;
        std::vector<Key64> key_of_set;          // cached keys
        uint64_t Mb;                            // per-field max

        explicit LeBIndex(int M_, size_t bpt_order = 128) : M(M_), packer(M_), bm(M_), tree(bpt_order) {
            Mb = packer.max_field();
        }

        // Compute bucket mapping, pack keys, then insert in B+ tree
        void build(std::vector<std::vector<Item>> inputSets) {
            sets = std::move(inputSets);
            bm.build(sets);
            key_of_set.resize(sets.size());
            for (SetID i = 0; i < (SetID)sets.size(); ++i) {
                auto &s = sets[i];
                std::vector<uint32_t> V = bm.map_set(s, M);
                // if |R| > Mb, skip mapping and use direct verification
                if (V[0] > Mb) { key_of_set[i] = UINT64_MAX; continue; }
                uint64_t K = packer.pack(V);
                key_of_set[i] = K;
                tree.insert(K, i);
            }
        }

        // Map query set to its vector/key
        std::vector<uint32_t> mapV(const std::vector<Item>& q) const { return bm.map_set(q, M); }
        Key64 packV(const std::vector<uint32_t>& V) const { return packer.pack(V); }

        // Extract Vi
        inline uint32_t field(Key64 K, int i) const { return packer.field(K, i); }
    };
}