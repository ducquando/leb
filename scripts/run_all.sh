#!/usr/bin/env bash
set -euo pipefail

DATA=./data
BUILD=./build
SCRIPT=./scripts
RESULT=./results
M=8
ORDER=128

echo "== Build indices =="
$BUILD/main_build $DATA/kosarak.dat $M $ORDER $RESULT/leb_kosarak --save-index $RESULT/leb_kosarak.idxbin
$BUILD/main_build $DATA/retail.dat $M $ORDER $RESULT/leb_retail --save-index $RESULT/leb_retail.idxbin
$BUILD/main_build $DATA/lastfm360k.fimi $M $ORDER $RESULT/leb_lastfm360k --save-index $RESULT/leb_lastfm360k.idxbin

for DATASET in kosarak retail lastfm360k; do
  echo "== Run baseline models for $DATASET =="
  bash $SCRIPT/run_baseline.sh $DATASET
done

for DATASET in kosarak retail lastfm360k; do
  echo "== Run M experiment for $DATASET =="
  bash $SCRIPT/run_m.sh $DATASET
done

for DATASET in kosarak retail lastfm360k; do
  echo "== Run filtering experiment for $DATASET =="
  bash scripts/run_filter.sh $DATASET
done

for DATASET in kosarak retail lastfm360k; do
  echo "== Run scaling experiment for $DATASET =="
  bash scripts/run_scale.sh $DATASET
done

for DATASET in kosarak retail lastfm360k; do
  echo "== Run Cross-method comparisons for $DATASET =="
  bash scripts/run_compare.sh $DATASET
done