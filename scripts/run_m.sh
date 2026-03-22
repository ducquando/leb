#!/usr/bin/env bash
set -euo pipefail
BASE=./data
DATASET=$1          # kosarak|retail|lastfm360k
ALGOS=("lebq" "lebq+")
MS=(4 6 8 10 12)
DELTA=0.7
Q=1000
RUNS=10
SEED=42
ORDER=256

# Select data file
if [[ "$DATASET" == "lastfm360k" ]]; then
  DATA="$BASE/${DATASET}.fimi"
elif [[ "$DATASET" == "kosarak" || "$DATASET" == "retail" ]]; then
  DATA="$BASE/${DATASET}.dat"
else
  echo "Unknown dataset: $DATASET"
  exit 1
fi

OUTFILE="results/m_experiment_${DATASET}.csv"
echo "method,dataset,M,delta,avg_ms,avg_candidates" > "$OUTFILE"

for A in "${ALGOS[@]}"; do
  for M in "${MS[@]}"; do
    ./build/main_query "$DATA" $M $ORDER $A $DELTA $Q $RUNS $SEED --filter cbdf \
      | awk -v m=$M -v alg=$A -v d=$DATA -v de=$DELTA '/QUERY_AVG_MS/{print alg","d","m","de","$2}' \
      >> "$OUTFILE"
  done
done