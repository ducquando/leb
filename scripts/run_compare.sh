#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm360k
ORDER=256
Q=1000
RUNS=10
SEED=42
M=8
DELTAS=(0.5 0.6 0.7 0.8 0.9)

# Select data file
if [[ "$DATASET" == "lastfm360k" ]]; then
  DATA="$BASE/${DATASET}.fimi"
elif [[ "$DATASET" == "kosarak" || "$DATASET" == "retail" ]]; then
  DATA="$BASE/${DATASET}.dat"
else
  echo "Unknown dataset: $DATASET"
  exit 1
fi

OUTFILE="results/comparison_${DATASET}.csv"
echo "method,dataset,delta,avg_ms,avg_candidates" > "$OUTFILE"

for D in "${DELTAS[@]}"; do
  # LeBQ (CBDF)
  OUT=$(./build/main_query "$DATA" $M $ORDER lebq $D $Q $RUNS $SEED --filter cbdf --log-cand 1)
  echo "LeBQ,$DATASET,$D,$(echo "$OUT"|awk '/QUERY_AVG_MS/{print $2}'),$(echo "$OUT"|awk '/CANDIDATES/{print $2}')" >> "$OUTFILE"

  # LeBQ+ (CBDF)
  OUT=$(./build/main_query "$DATA" $M $ORDER lebq+ $D $Q $RUNS $SEED --filter cbdf --log-cand 1)
  echo "LeBQ+,$DATASET,$D,$(echo "$OUT"|awk '/QUERY_AVG_MS/{print $2}'),$(echo "$OUT"|awk '/CANDIDATES/{print $2}')" >> "$OUTFILE"
done

# Append LES3 & DualTrans rows from run_baseline.sh outputs
awk -F, 'NR>1{print $1",""'$DATASET'"","$3","$4",NA"}' results/les3_dualtrans_${DATASET}.csv >> "$OUTFILE"