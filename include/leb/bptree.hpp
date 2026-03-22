#pragma once
#include "util.hpp"
#include <map>

namespace leb {
    // Minimal B+ Tree implementation using map
    struct BPTree {
        struct LeafIter {
            std::map<Key64, std::vector<SetID>>::const_iterator it;
            std::map<Key64, std::vector<SetID>>::const_iterator end;
            bool leaf = false;
            size_t pos = 0;
        };

        const size_t ORDER;
        std::map<Key64, std::vector<SetID>> data;

        explicit BPTree(size_t order = 128) : ORDER(order) {}

        void insert(Key64 k, SetID id) {
            data[k].push_back(id);
        }

        LeafIter lower_bound(Key64 k) const {
            auto it = data.lower_bound(k);
            return LeafIter{it, data.end(), it != data.end(), 0};
        }

        bool next(LeafIter& it) const {
            if (!it.leaf) return false;
            ++it.it;
            it.leaf = (it.it != it.end);
            it.pos = 0;
            return it.leaf;
        }

        Key64 curr_key(const LeafIter& it) const { return it.it->first; }
        const std::vector<SetID>& curr_list(const LeafIter& it) const {
            return it.it->second;
        }
    };
}