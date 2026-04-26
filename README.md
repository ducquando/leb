# Length-Enhanced B+-tree (LeB) and Query Algorithms

This repository focuses on implementing and evaluating:

1. LeB tree index construction
2. LeBQ query processing
3. LeBQ+ query processing
4. Benchmarks against LES3 and DualTrans baselines

## What is implemented

- LeB index and data structures:
  - `include/leb/leb_index.hpp`
  - `include/leb/bptree.hpp`
  - `include/leb/buckets.hpp`
  - `include/leb/keypacking.hpp`
- Query algorithms:
  - `include/leb/lebq.hpp`
  - `include/leb/lebq_plus.hpp`
- CLI programs:
  - `src/main_build.cpp`: build index and optionally persist serialized leaf payloads
  - `src/main_query.cpp`: run LeBQ/LeBQ+ queries and report runtime/candidates
  - `src/main_stats.cpp`: print basic dataset statistics

## Repository layout

```text
baselines/                LES3 + DualTrans baseline code and datasets
data/                     Input datasets (FIMI-like format; one set per line)
include/leb/              Core LeB/LeBQ/LeBQ+ implementation
results/                  CSV outputs, index binaries, and generated plots
scripts/                  Reproducible experiment scripts
src/                      CLI entry points (build/query/stats)
CMakeLists.txt            CMake build configuration
README.md
```

## Requirements

- C++17 compiler (`clang++` or `g++`)
- CMake >= 3.14
- Bash
- Python3 and Python packages (i.e., pandas, seaborn, matplotlib) for plotting

```bash
python3 -m pip install pandas seaborn matplotlib
```

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
cd ..
```

This produces:

- `build/main_build`
- `build/main_query`
- `build/main_stats`

## Workflow

Run everything end-to-end:

```bash
bash scripts/run_all.sh
python3 scripts/plot_results_all.py
```

Or run individual experiment on a specific dataset (`kosarak`, `retail`, or `lastfm1k`):

```bash
bash scripts/run_index.sh kosarak
bash scripts/run_m.sh kosarak
bash scripts/run_filter.sh kosarak
bash scripts/run_scale.sh kosarak
bash scripts/run_compare.sh kosarak
python3 scripts/plot_results.py --dataset kosarak
```

Figures are saved to `results/figs/`.

## Reproducibility notes

- Script defaults use the fixed seed set `(2, 4, 24, 42, 0)` for repeated query experiments.
- Main query comparisons generally run with CBDF filtering enabled.
