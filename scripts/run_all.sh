#!/usr/bin/env bash
set -euo pipefail

DATA=./data
BUILD=./build
M=8
ORDER=128
Q=1000
RUNS=10
SEED=42

echo "== Build indices =="
$BUILD/main_build $DATA/kosarak.dat        $M $ORDER kos.idx
$BUILD/main_build $DATA/retail.dat         $M $ORDER ret.idx
$BUILD/main_build $DATA/lastfm360k.fimi    $M $ORDER lfm.idx

for DELTA in 0.5 0.6 0.7 0.8; do
  for ALG in lebq lebq+; do
    echo "== $ALG delta=$DELTA KOSARAK =="
    $BUILD/main_query $DATA/kosarak.dat $M $ORDER $ALG $DELTA $Q $RUNS $SEED
    echo "== $ALG delta=$DELTA RETAIL =="
    $BUILD/main_query $DATA/retail.dat $M $ORDER $ALG $DELTA $Q $RUNS $SEED
    echo "== $ALG delta=$DELTA LASTFM360K =="
    $BUILD/main_query $DATA/lastfm360k.fimi $M $ORDER $ALG $DELTA $Q $RUNS $SEED
  done
done