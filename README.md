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

## Baseline setup (LES3 and DualTrans)

If baseline code is not already present under `baselines/les3-dualtrans`:

```bash
cd baselines
git clone https://github.com/princeampofo/learning-based-set-sim-search
```

Then follow the steps in the readme to reproduce the results for LES3 and DualTrans.

## CLI usage

### 1. Build LeB index

```text
./build/main_build <dataset_path> <M> <bpt_order> <out_index_stub> [--save-index <path>]
```

Example:

```bash
./build/main_build data/kosarak.dat 8 128 results/leb_kosarak --save-index results/leb_kosarak.idxbin
```

Output fields include:

- `INDEX_BUILD_MS`
- `SETS`

### 2. Run LeBQ or LeBQ+

```text
./build/main_query <dataset_path> <M> <bpt_order> <algo:lebq|lebq+> <delta> <num_queries> <runs> <seed> [--filter none|sbdf|cbdf] [--log-cand 1]
```

Examples:

```bash
./build/main_query data/kosarak.dat 8 128 lebq 0.7 1000 10 42 --filter cbdf --log-cand 1
./build/main_query data/kosarak.dat 8 128 lebq+ 0.7 1000 10 42 --filter cbdf --log-cand 1
```

Output fields include:

- `QUERY_AVG_MS`
- `CANDIDATES` (when `--log-cand 1`)

### 3. Dataset statistics

```text
./build/main_stats <dataset_path>
```

## Benchmark workflow

Run everything end-to-end:

```bash
bash scripts/run_all.sh
```

Or run experiments individually (replace dataset with `kosarak`, `retail`, or `lastfm360k`):

```bash
bash scripts/run_m.sh kosarak
bash scripts/run_filter.sh kosarak
bash scripts/run_scale.sh kosarak
bash scripts/run_compare.sh kosarak
```

### What each script produces
- `scripts/run_m.sh` -> `results/m_experiment_<dataset>.csv`
  - LeBQ/LeBQ+ runtime vs bucket count `M`
- `scripts/run_filter.sh` -> `results/filter_experiment_<dataset>.csv`
  - runtime and candidate counts across filter modes `none|sbdf|cbdf`
- `scripts/run_scale.sh` -> `results/scale_experiment_<dataset>.csv`
  - runtime vs dataset proportion
- `scripts/run_compare.sh` -> `results/comparison_<dataset>.csv`
  - LeBQ/LeBQ+ plus LES3/DualTrans combined comparison table

## Plot figures

After CSVs are generated:

```bash
python3 scripts/plot_results.py --dataset kosarak
python3 scripts/plot_results.py --dataset retail
python3 scripts/plot_results.py --dataset lastfm360k
```

Figures are saved to `results/figs/`.

## Datasets and input format

- Input is line-based transactional data (FIMI-like): each line is a set of integer item IDs.
- Typical file names used by scripts:
  - `data/kosarak.dat`
  - `data/retail.dat`
  - `data/lastfm360k.fimi`

## Reproducibility notes

- Script defaults use fixed seeds (`SEED=42` in several scripts).
- Delta sweeps usually use `0.5, 0.6, 0.7, 0.8, 0.9`.
- Main query comparisons generally run with CBDF filtering enabled.

---

## Results

### Index Construction Time and Size

| Method    | Kosarak Construction Time (min) | Kosarak Index Size (MB) | Retail Construction Time (min) | Retail Index Size (MB) | LastFM Construction Time (min) | LastFM Index Size (MB) |
|-----------|--------------------------------:|------------------------:|-------------------------------:|-----------------------:|-------------------------------:|-----------------------:|
| LeB       |                                 |                         |                                |                        |                                |                        |
| LES3      | 166.25                          | 4                       | 13.10                          | 2                      | 45.65                          | 1                      |
| DualTrans | 1.5467                          | 8.70                    | 0.0645                         | 5.13                   | 1.8831                         | 6.32                   |

---

### Query Performance: Avg Time (ms) and Candidates

#### Kosarak

| Method    | δ=0.9 Avg ms | δ=0.9 Candidates | δ=0.8 Avg ms | δ=0.8 Candidates | δ=0.7 Avg ms | δ=0.7 Candidates | δ=0.6 Avg ms | δ=0.6 Candidates | δ=0.5 Avg ms | δ=0.5 Candidates |
|-----------|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|
| LeBQ      |              |                  |              |                  |              |                  |              |                  |              |                  |
| LeBQ+     |              |                  |              |                  |              |                  |              |                  |              |                  |
| LES3      | 4.0879       | 101,094          | 5.3016       | 113,792          | 6.8051       | 137,313          | 9.3194       | 194,279          | 13.3493      | 319,237          |
| DualTrans | 6.0198       | 67,216           | 9.8843       | 107,012          | 16.3796      | 194,741          | 22.1994      | 273,018          | 33.5938      | 438,180          |

#### Retail

| Method    | δ=0.9 Avg ms | δ=0.9 Candidates | δ=0.8 Avg ms | δ=0.8 Candidates | δ=0.7 Avg ms | δ=0.7 Candidates | δ=0.6 Avg ms | δ=0.6 Candidates | δ=0.5 Avg ms | δ=0.5 Candidates |
|-----------|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|
| LeBQ      |              |                  |              |                  |              |                  |              |                  |              |                  |
| LeBQ+     |              |                  |              |                  |              |                  |              |                  |              |                  |
| LES3      | 2.4031       | 2,778            | 2.4442       | 3,167            | 2.5074       | 4,514            | 2.7179       | 8,964            | 3.1982       | 20,873           |
| DualTrans | 3.5701       | 33,399           | 5.4329       | 47,793           | 7.2350       | 63,428           | 8.1159       | 71,810           | 9.0463       | 80,724           |

#### LastFM

| Method    | δ=0.9 Avg ms | δ=0.9 Candidates | δ=0.8 Avg ms | δ=0.8 Candidates | δ=0.7 Avg ms | δ=0.7 Candidates | δ=0.6 Avg ms | δ=0.6 Candidates | δ=0.5 Avg ms | δ=0.5 Candidates |
|-----------|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|-------------:|-----------------:|
| LeBQ      |              |                  |              |                  |              |                  |              |                  |              |                  |
| LeBQ+     |              |                  |              |                  |              |                  |              |                  |              |                  |
| LES3      | 4.8818       | 127,871          | 5.4287       | 134,003          | 5.8596       | 140,763          | 6.5373       | 157,724          | 7.3634       | 192,600          |
| DualTrans | 7.3043       | 113,738          | 9.3592       | 135,753          | 12.5271      | 180,693          | 15.6672      | 231,792          | 22.8878      | 367,520          |