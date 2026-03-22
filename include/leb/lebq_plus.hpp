#pragma once
#include "lebq.hpp"

namespace leb {
    struct LeBQPlus {
        const LeBIndex& index;
        explicit LeBQPlus(const LeBIndex& idx) : index(idx) {}

        static inline uint32_t Lmin(size_t qlen, double delta) {
            return (uint32_t)((qlen * delta) + 0.999999999);
        }
        static inline uint32_t Lmax(size_t qlen, double delta) {
            return (uint32_t)(qlen / delta);
        }
        static inline uint32_t lambdaLQ(size_t qlen, uint32_t L, double delta) {
            // lambda_{L,Q} = (1-d)/(1+d) * (L + |Q|)
            double val = (1.0 - delta) / (1.0 + delta) * (double)(L + qlen);
            if (val < 0) val = 0;
            return (uint32_t)val;
        }

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

        std::vector<SetID> query(const std::vector<Item>& q, double delta) const {
            std::vector<SetID> results;
            auto QV = index.mapV(q);
            const size_t qlen = q.size();
            const uint32_t L_minus = Lmin(qlen, delta);
            const uint32_t L_plus  = Lmax(qlen, delta);

            for (uint32_t L = L_minus; L <= L_plus; ++L) {
                const uint32_t lam = lambdaLQ(qlen, L, delta);
                auto [KminV, KmaxV] = FBFA::compute(QV, lam, L, L, index.Mb);
                Key64 Kmin = index.packV(KminV);
                Key64 Kmax = index.packV(KmaxV);

                auto it = index.tree.lower_bound(Kmin);
                while (it.leaf) {
                    Key64 K = index.tree.curr_key(it);
                    if (K > Kmax) break;
                    if (pass_cbdf(QV, K, lam)) {
                        for (auto id : index.tree.curr_list(it)) {
                            if (index.sets[id].size() != L) continue;
                            if (leb::jaccard(index.sets[id], q) >= delta) results.push_back(id);
                        }
                    }
                    if (!index.tree.next(it)) break;
                }
            }
            return results;
        }
    };
}