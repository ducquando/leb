#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm1k
ORDER=256
Q=1000
RUNS=1
SEED=42
M=8
DELTA=0.7
PROPS=(0.25 0.5 0.75 1.0)

DATA="$BASE/${DATASET}.dat"
OUTFILE="results/scale_experiment_${DATASET}.csv"
echo "method,dataset,prop,delta,avg_ms" > "$OUTFILE"

for P in "${PROPS[@]}"; do
  head -n $(python3 - <<EOF
import math,sys
N=sum(1 for _ in open("$DATA"))
print(max(1,int(N*$P)))
EOF
) "$DATA" > "data/${DATASET}_tmp_${P}.dat"
  for A in lebq lebq+; do
    ms=$(./build/main_query "data/${DATASET}_tmp_${P}.$FORMAT" $M $ORDER $A $DELTA $Q $RUNS $SEED --filter cbdf \
          | awk '/QUERY_AVG_MS/{print $2}')
    echo "$A,$DATA,$P,$DELTA,$ms" >> "$OUTFILE"
  done
done