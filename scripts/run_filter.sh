#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ORDER=256
Q=1000
RUNS=1
SEED=42
M=8
ALGOS=("lebq" "lebq+")
FILTERS=("none" "sbdf" "cbdf")
DELTAS=(0.5 0.6 0.7 0.8 0.9)

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/filter_experiment_${DATASET}.csv"
echo "method,filter,dataset,delta,avg_ms,avg_candidates" > "$OUTFILE"

for A in "${ALGOS[@]}"; do
  for F in "${FILTERS[@]}"; do
    for D in "${DELTAS[@]}"; do
      OUT=$(./build/main_query "$DATA" $M $ORDER $A $D $Q $RUNS $SEED --filter $F --log-cand 1)
      ms=$(echo "$OUT" | awk '/QUERY_AVG_MS/{print $2}')
      cand=$(echo "$OUT" | awk '/CANDIDATES/{print $2}')
      echo "$A,$F,$DATASET,$D,$ms,$cand" >> "$OUTFILE"
    done
  done
done