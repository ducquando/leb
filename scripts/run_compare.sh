#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ORDER=256
Q=1000
RUNS=1
SEEDS=(42 24 4 2 0)
M=8
DELTAS=(0.5 0.6 0.7 0.8 0.9)

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/comparison_${DATASET}.csv"
BASELINES="./baselines/results/${DATASET}_results.csv"
echo "method,dataset,seed,delta,avg_ms,avg_candidates" > "$OUTFILE"

for S in "${SEEDS[@]}"; do
  for D in "${DELTAS[@]}"; do
    # LeBQ (CBDF)
    OUT=$(./build/main_query "$DATA" $M $ORDER lebq $D $Q $RUNS $S --filter cbdf --log-cand 1)
    echo "LeBQ,$DATASET,$S,$D,$(echo "$OUT"|awk '/QUERY_AVG_MS/{print $2}'),$(echo "$OUT"|awk '/CANDIDATES/{print $2}')" >> "$OUTFILE"

    # LeBQ+ (CBDF)
    OUT=$(./build/main_query "$DATA" $M $ORDER lebq+ $D $Q $RUNS $S --filter cbdf --log-cand 1)
    echo "LeBQ+,$DATASET,$S,$D,$(echo "$OUT"|awk '/QUERY_AVG_MS/{print $2}'),$(echo "$OUT"|awk '/CANDIDATES/{print $2}')" >> "$OUTFILE"
  done
done

# Append LES3 & DualTrans rows from run_baseline.sh outputs
awk -F, 'NR>1{print $1",""'$DATASET'"",,"$3","$4","$5}' "$BASELINES" >> "$OUTFILE"