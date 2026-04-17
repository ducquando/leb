#!/usr/bin/env python3
import argparse
import csv
from pathlib import Path


TARGET_METHODS = ("LES3", "DualTrans")


def to_result_dataset(dataset: str) -> str:
    """Map dataset input names to the canonical result dataset name."""
    return "lastfm1k" if dataset == "lastfm" else dataset


def normalize_num(value: str) -> str:
    """Normalize numeric strings so 0.90 and 0.9 match."""
    try:
        return str(float(value))
    except (TypeError, ValueError):
        return value


def resolve_baseline_file(repo_root: Path, dataset: str) -> Path:
    base_dir = repo_root / "baselines" / "results"
    candidates = [base_dir / f"{dataset}_results.csv"]

    # Keep compatibility with repos that use lastfm_results.csv for lastfm1k.
    if dataset == "lastfm1k":
        candidates.append(base_dir / "lastfm_results.csv")
    if dataset == "lastfm":
        candidates.append(base_dir / "lastfm1k_results.csv")

    for file_path in candidates:
        if file_path.exists():
            return file_path

    raise FileNotFoundError(
        f"No baseline CSV found for dataset '{dataset}'. Tried: "
        + ", ".join(str(p) for p in candidates)
    )


def read_baseline_updates(baseline_csv: Path, dataset: str):
    comparison_updates = {}
    index_updates = {}

    with baseline_csv.open("r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            method = row.get("method", "").strip()
            if method not in TARGET_METHODS:
                continue

            delta = row.get("delta", "").strip()
            comparison_updates[(method, normalize_num(delta))] = [
                method,
                dataset,
                "",
                delta,
                row.get("avg_time_ms", "").strip(),
                row.get("avg_candidates", "").strip(),
            ]

            # One index row per method.
            if method not in index_updates:
                index_updates[method] = [
                    method,
                    dataset,
                    row.get("construction_time_min", "").strip(),
                    row.get("index_size_mb", "").strip(),
                ]

    return comparison_updates, index_updates


def update_comparison_csv(path: Path, updates):
    with path.open("r", newline="") as f:
        rows = list(csv.reader(f))

    if not rows:
        raise ValueError(f"Empty CSV: {path}")

    header = rows[0]
    data_rows = rows[1:]

    remaining = dict(updates)
    replaced = 0
    new_rows = []

    for row in data_rows:
        if len(row) >= 4:
            method = row[0].strip()
            seed = row[2].strip() if len(row) > 2 else ""
            delta = normalize_num(row[3].strip())
            key = (method, delta)
            if method in TARGET_METHODS and seed == "" and key in remaining:
                new_rows.append(remaining.pop(key))
                replaced += 1
                continue
        new_rows.append(row)

    # Append any method+delta pairs that were missing in the destination file.
    for key in updates:
        if key in remaining:
            new_rows.append(remaining[key])

    with path.open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)
        writer.writerows(new_rows)

    return replaced, len(remaining)


def update_index_csv(path: Path, updates):
    with path.open("r", newline="") as f:
        rows = list(csv.reader(f))

    if not rows:
        raise ValueError(f"Empty CSV: {path}")

    header = rows[0]
    data_rows = rows[1:]

    remaining = dict(updates)
    replaced = 0
    new_rows = []

    for row in data_rows:
        if row:
            method = row[0].strip()
            if method in remaining:
                new_rows.append(remaining.pop(method))
                replaced += 1
                continue
        new_rows.append(row)

    # Append any method rows that were missing in the destination file.
    for method in updates:
        if method in remaining:
            new_rows.append(remaining[method])

    with path.open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)
        writer.writerows(new_rows)

    return replaced, len(remaining)


def discover_datasets(repo_root: Path):
    result_dir = repo_root / "results"
    datasets = []
    for file_path in sorted(result_dir.glob("comparison_*.csv")):
        suffix = file_path.stem.replace("comparison_", "", 1)
        if suffix:
            datasets.append(suffix)
    return datasets


def sync_dataset(repo_root: Path, dataset: str):
    baseline_csv = resolve_baseline_file(repo_root, dataset)
    result_dataset = to_result_dataset(dataset)
    comparison_csv = repo_root / "results" / f"comparison_{result_dataset}.csv"
    index_csv = repo_root / "results" / f"index_{result_dataset}.csv"

    if not comparison_csv.exists():
        raise FileNotFoundError(f"Missing file: {comparison_csv}")
    if not index_csv.exists():
        raise FileNotFoundError(f"Missing file: {index_csv}")

    comparison_updates, index_updates = read_baseline_updates(baseline_csv, result_dataset)

    if not comparison_updates and not index_updates:
        print(f"[{dataset}] no LES3/DualTrans rows found in {baseline_csv}")
        return

    comp_replaced, comp_missing = update_comparison_csv(comparison_csv, comparison_updates)
    idx_replaced, idx_missing = update_index_csv(index_csv, index_updates)

    print(
        f"[{dataset}] comparison updated (replaced={comp_replaced}, appended={comp_missing}), "
        f"index updated (replaced={idx_replaced}, appended={idx_missing})"
    )


def main():
    parser = argparse.ArgumentParser(
        description="Sync LES3/DualTrans rows from baselines/results into results CSVs."
    )
    parser.add_argument(
        "--dataset",
        action="append",
        help="Dataset name (e.g., kosarak, retail, lastfm or lastfm1k). Can be used multiple times.",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Update all datasets discovered from results/comparison_*.csv.",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parents[1]

    datasets = args.dataset or []
    if args.all:
        datasets = discover_datasets(repo_root)

    if not datasets:
        parser.error("Provide --dataset <name> or use --all")

    for ds in datasets:
        sync_dataset(repo_root, ds)


if __name__ == "__main__":
    main()