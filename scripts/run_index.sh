#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ORDER=256
M=8

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/index_${DATASET}"
BASELINES="./baselines/results/${DATASET}_results.csv"
echo "method,dataset,construction_time_min,index_size_mb" > "$OUTFILE.csv"

OUT=$(./build/main_build "$DATA" $M $ORDER $OUTFILE --save-index $OUTFILE.idxbin)
echo "LeBQ,$DATASET,$(echo "$OUT"|awk '/INDEX_BUILD_MIN/{print $2}'),$(echo "$OUT"|awk '/INDEX_SIZE_MB/{print $2}')" >> "$OUTFILE.csv"

# Append LES3 & DualTrans rows from run_baseline.sh outputs
awk -F, 'NR>1 && !seen[$1]++{print $1",""'$DATASET'"","$6","$7}' "$BASELINES" >> "$OUTFILE.csv"