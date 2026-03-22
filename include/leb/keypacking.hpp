#pragma once
#include "util.hpp"
#include <array>
#include <cassert>

namespace leb {
    // Pack [v0,...,vM] into 64 bits, each field has floor(64/(M+1)) width size
    struct KeyPacker {
        int M;                  // # buckets
        int b;                  // bits/field
        uint64_t field_mask;    // (1<<b)-1

        explicit KeyPacker(int M_) : M(M_) {
            b = 64 / (M + 1);
            if (b <= 0) throw std::runtime_error("M too large for 64-bit key");
            field_mask = (b == 64 ? ~0ULL : ((1ULL << b) - 1ULL));
        }
        inline int bits_per_field() const { return b; }
        inline uint64_t max_field() const { return field_mask; }

        // Pack [v0,...,vM] to unit64
        uint64_t pack(const std::vector<uint32_t>& V) const {
            if ((int)V.size() != (M + 1)) throw std::runtime_error("V size mismatch");
            uint64_t K = 0;
            for (int i = 0; i <= M; ++i) {
                uint64_t vi = std::min<uint64_t>(V[i], field_mask);
                K <<= b;
                K |= vi;
            }
            return K;
        }

        // Unpack into vector<uint32_t>
        std::vector<uint32_t> unpack(uint64_t K) const {
            std::vector<uint32_t> V(M + 1);
            for (int i = M; i >= 0; --i) { V[i] = (uint32_t)(K & field_mask); K >>= b; }
            return V;
        }

        // Extract field i from key
        uint32_t field(uint64_t K, int i) const {
            const int shift = b * (M - i);
            return (uint32_t)((K >> shift) & field_mask);
        }
    };
}