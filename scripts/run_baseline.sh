#!/usr/bin/env bash
set -euo pipefail
BASE=baselines/les3-dualtrans
BIN=baselines/les3_dualtrans
DATASET=$1             # kosarak|retail|lastfm360k
OUT=results/les3_dualtrans_${DATASET}.csv

# Select data file
if [[ "$DATASET" == "lastfm360k" ]]; then
  DATA="$BASE/datasets/${DATASET}/all.fimi"
elif [[ "$DATASET" == "kosarak" || "$DATASET" == "retail" ]]; then
  DATA="$BASE/datasets/${DATASET}/all.dat"
else
  echo "Unknown dataset: $DATASET"
  exit 1
fi

echo "method,dataset,delta,avg_ms,index_size_bytes" > "$OUT"

for D in 0.5 0.6 0.7 0.8 0.9; do
  # Run the baseline search executable 
  RES=$("./$BIN" "$DATA" "$D")

  les3_build=$(echo "$RES" | awk '/LES3.*INDEX_BUILD_MS/{print $NF}')
  les3_isz=$(echo "$RES"   | awk '/LES3.*INDEX_SIZE/{print $NF}')
  les3_qms=$(echo "$RES"   | awk '/LES3.*QUERY_AVG_MS/{print $NF}')
  echo "LES3,$DATASET,$D,$les3_qms,$les3_isz" >> "$OUT"

  dt_build=$(echo "$RES" | awk '/DUALTRANS.*INDEX_BUILD_MS/{print $NF}')
  dt_isz=$(echo "$RES"   | awk '/DUALTRANS.*INDEX_SIZE/{print $NF}')
  dt_qms=$(echo "$RES"   | awk '/DUALTRANS.*QUERY_AVG_MS/{print $NF}')
  echo "DUALTRANS,$DATASET,$D,$dt_qms,$dt_isz" >> "$OUT"
done