#!/usr/bin/env bash
set -euo pipefail

DATA=./data
BUILD=./build
SCRIPT=./scripts
RESULT=./results
M=8
ORDER=256

for DATASET in kosarak retail lastfm1k; do
  echo "== Run index construction for $DATASET =="
  bash $SCRIPT/run_index.sh $DATASET
done

for DATASET in kosarak retail lastfm1k; do
  echo "== Run M experiment for $DATASET =="
  bash $SCRIPT/run_m.sh $DATASET
done

for DATASET in kosarak retail lastfm1k; do
  echo "== Run filtering experiment for $DATASET =="
  bash scripts/run_filter.sh $DATASET
done

for DATASET in kosarak retail lastfm1k; do
  echo "== Run scaling experiment for $DATASET =="
  bash scripts/run_scale.sh $DATASET
done

for DATASET in kosarak retail lastfm1k; do
  echo "== Run Cross-method comparisons for $DATASET =="
  bash scripts/run_compare.sh $DATASET
done