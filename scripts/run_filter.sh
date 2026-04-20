#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ORDER=256
Q=1000
RUNS=1
SEEDS=(42 24 4 2 0)
M=8
ALGOS=("lebq" "lebq+")
FILTERS=("none" "sbdf" "cbdf")
DELTAS=(0.5 0.6 0.7 0.8 0.9)

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/filter_experiment_${DATASET}.csv"
echo "method,filter,dataset,seed,delta,avg_ms,avg_candidates" > "$OUTFILE"

for S in "${SEEDS[@]}"; do
  for A in "${ALGOS[@]}"; do
    for F in "${FILTERS[@]}"; do
      for D in "${DELTAS[@]}"; do
        OUT=$(./build/main_query "$DATA" $M $ORDER $A $D $Q $RUNS $S --filter $F --log-cand 1)
        ms=$(echo "$OUT" | awk '/QUERY_AVG_MS/{print $2}')
        cand=$(echo "$OUT" | awk '/CANDIDATES/{print $2}')
        echo "$A,$F,$DATASET,$S,$D,$ms,$cand" >> "$OUTFILE"
      done
    done
  done
done