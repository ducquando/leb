#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ORDER=256
Q=1000
RUNS=1
SEEDS=(42 24 4 2 0)
M=8
DELTA=0.7
PROPS=(0.25 0.5 0.75 1.0)

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/scale_experiment_${DATASET}.csv"
echo "method,dataset,seed,prop,delta,avg_ms,avg_candidates" > "$OUTFILE"

for P in "${PROPS[@]}"; do
  head -n $(python3 - <<EOF
import math,sys
N=sum(1 for _ in open("$DATA"))
print(max(1,int(N*$P)))
EOF
) "$DATA" > "data/${DATASET}_tmp_${P}.dat"
  for S in "${SEEDS[@]}"; do
    for A in lebq lebq+; do
      OUT=$(./build/main_query "data/${DATASET}_tmp_${P}.dat" $M $ORDER $A $DELTA $Q $RUNS $S --filter cbdf --log-cand 1)
      ms=$(echo "$OUT" | awk '/QUERY_AVG_MS/{print $2}')
      cand=$(echo "$OUT" | awk '/CANDIDATES/{print $2}')
      echo "$A,$DATASET,$S,$P,$DELTA,$ms,$cand" >> "$OUTFILE"
    done
  done
done