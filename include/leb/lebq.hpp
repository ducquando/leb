#pragma once
#include "leb_index.hpp"

namespace leb {
    struct FBFA {
        // Compute K- and K+
        static std::pair<std::vector<uint32_t>, std::vector<uint32_t>>
        compute(const std::vector<uint32_t>& QV, uint32_t lambda, uint32_t Lmin, uint32_t Lmax, uint64_t Mb) {
            const int M = (int)QV.size() - 1;
            std::vector<uint32_t> Kminus = QV, Kplus = QV;
            Kminus[0] = Lmin; Kplus[0] = Lmax;

            // Downward allocate to earliest buckets
            uint32_t rem = lambda;
            for (int i = 1; i <= M && rem > 0; ++i) {
                uint32_t take = std::min<uint32_t>(Kminus[i], rem);
                Kminus[i] -= take;
                rem -= take;
            }

            // Upward allocate to earliest buckets
            rem = lambda;
            for (int i = 1; i <= M && rem > 0; ++i) {
                uint64_t space = (Mb > Kplus[i]) ? (Mb - Kplus[i]) : 0ULL;
                uint32_t add = (uint32_t) std::min<uint64_t>(rem, space);
                Kplus[i] += add;
                rem -= add;
            }
            return {Kminus, Kplus};
        }
    };

    struct LeBQ {
        const LeBIndex& index;
        explicit LeBQ(const LeBIndex& idx) : index(idx) {}

        // Length bounds for Jaccard
        static inline uint32_t Lmin(size_t qlen, double delta) {
            return (uint32_t)((qlen * delta) + 0.999999999); // ceil
        }
        static inline uint32_t Lmax(size_t qlen, double delta) {
            return (uint32_t)(qlen / delta); // floor
        }
        static inline uint32_t lambdaQ(size_t qlen, double delta) {
            return Lmax(qlen, delta) - (uint32_t)qlen;
        }

        // Early stop on total difference > lambda
        bool pass_cbdf(const std::vector<uint32_t>& QV, Key64 K, uint32_t lambda) const {
            uint32_t sum = 0;
            for (int i = 1; i <= index.M; ++i) {
                uint32_t kv = index.field(K, i);
                uint32_t qv = QV[i];
                sum += (kv>qv) ? (kv-qv) : (qv-kv);
                if (sum > lambda) return false;
            }
            return true;
        }

        // Return vector of matching setIDs
        std::vector<SetID> query(const std::vector<Item>& q, double delta) const {
            std::vector<SetID> results;
            auto QV = index.mapV(q);
            const size_t qlen = q.size();
            const uint32_t L_minus = Lmin(qlen, delta);
            const uint32_t L_plus  = Lmax(qlen, delta);
            const uint32_t lambda  = lambdaQ(qlen, delta);
            auto [KminusV, KplusV] = FBFA::compute(QV, lambda, L_minus, L_plus, index.Mb);
            Key64 Kmin = index.packV(KminusV);
            Key64 Kmax = index.packV(KplusV);
            auto it = index.tree.lower_bound(Kmin);

            // Scan leaves while key <= Kmax
            while (it.leaf) {
                Key64 K = index.tree.curr_key(it);
                if (K > Kmax) break;

                if (pass_cbdf(QV, K, lambda)) {
                    for (auto id : index.tree.curr_list(it)) {
                        // Verify true Jaccard
                        if (leb::jaccard(index.sets[id], q) >= delta) results.push_back(id);
                    }
                }
                if (!index.tree.next(it)) break;
            }
            return results;
        }
    };
}