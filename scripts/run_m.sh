#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ALGOS=("lebq" "lebq+")
MS=(4 6 8 10 12)
DELTA=0.7
Q=1000
RUNS=1
SEEDS=(42 24 4 2 0)
ORDER=256

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/m_experiment_${DATASET}.csv"
echo "method,dataset,seed,M,delta,avg_ms,avg_candidates" > "$OUTFILE"

for S in "${SEEDS[@]}"; do
  for A in "${ALGOS[@]}"; do
    for M in "${MS[@]}"; do
      ./build/main_query "$DATA" $M $ORDER $A $DELTA $Q $RUNS $S --filter cbdf --log-cand 1 \
        | awk -v m=$M -v alg=$A -v d=$DATASET -v de=$DELTA -v seed=$S '
            /QUERY_AVG_MS/{ms=$2}
            /CANDIDATES/{cand=$2}
            END{print alg","d","seed","m","de","ms","cand}
          ' \
        >> "$OUTFILE"
    done
  done
done