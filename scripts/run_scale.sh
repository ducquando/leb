#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm360k
ORDER=256
Q=1000
RUNS=10
SEED=42
M=8
DELTA=0.7
PROPS=(0.25 0.5 0.75 1.0)

# Select data file
if [[ "$DATASET" == "lastfm360k" ]]; then
  FORMAT="fimi"
elif [[ "$DATASET" == "kosarak" || "$DATASET" == "retail" ]]; then
  FORMAT="dat"
else
  echo "Unknown dataset: $DATASET"
  exit 1
fi

DATA="$BASE/${DATASET}.$FORMAT"
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