import os
import argparse
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# -----------------------
# Args
# -----------------------
parser = argparse.ArgumentParser()
parser.add_argument("--dataset", type=str, required=True,
                    help="Dataset name: kosarak | retail | lastfm1k")
args = parser.parse_args()
DATASET = args.dataset

# -----------------------
# Setup
# -----------------------
os.makedirs("results/figs", exist_ok=True)
sns.set_theme(style="whitegrid", font_scale=1.1)

# -----------------------
# M experiment
# -----------------------
dm = pd.read_csv(f"results/m_experiment_{DATASET}.csv")
sns.lineplot(data=dm, x="M", y="avg_ms", hue="method", marker="o")
plt.title(f"Impact of number of buckets M on query time ({DATASET}, δ=0.7)")
plt.ylabel("Avg query time (ms)")
plt.xlabel("M")
plt.savefig(f"results/figs/m_experiment_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Filter experiment
# -----------------------
df = pd.read_csv(f"results/filter_experiment_{DATASET}.csv")
sns.lineplot(data=df, x="delta", y="avg_ms", hue="method", style="filter", marker="o")
plt.title(f"Average Run Time by filtering mode ({DATASET}, M=8)")
plt.savefig(f"results/figs/filter_experiment_ms_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Filter experiment
# -----------------------
sns.lineplot(data=df, x="delta", y="avg_candidates", hue="method", style="filter", marker="o")
plt.title(f"Number of Candidates by filtering mode ({DATASET}, M=8)")
plt.savefig(f"results/figs/filter_experiment_cand_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Scaling
# -----------------------
dsc = pd.read_csv(f"results/scale_experiment_{DATASET}.csv")
sns.lineplot(data=dsc, x="prop", y="avg_ms", hue="method", marker="o")
plt.title(f"Run time vs dataset proportion ({DATASET}, δ=0.7, M=8, CBDF)")
plt.savefig(f"results/figs/scale_experiment_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Baseline comparisons
# -----------------------
comp = pd.read_csv(f"results/comparison_{DATASET}.csv")
sns.lineplot(data=comp, x="delta", y="avg_ms", hue="method", marker="o")
plt.title(f"Avg Run Time ({DATASET}, M=8, CBDF)")
plt.savefig(f"results/figs/comparison_runtime_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Construction comparisons
# -----------------------
fig, axes = plt.subplots(2, 1, figsize=(9, 5))

all_comp = []
for dataset in ["kosarak", "retail", "lastfm1k"]:
    comp = pd.read_csv(f"results/index_{dataset}.csv")
    comp["dataset"] = dataset
    all_comp.append(comp)

comp = pd.concat(all_comp, ignore_index=True)
comp_plot = (
    comp.groupby(["dataset", "method"], as_index=False)[["construction_time_min", "index_size_mb"]]
    .mean()
)

sns.barplot(data=comp_plot, x="dataset", y="index_size_mb", hue="method", ax=axes[0])
axes[0].set_title("Index Size")
axes[0].set_xlabel("Dataset")
axes[0].set_ylabel("MB")

sns.barplot(data=comp_plot, x="dataset", y="construction_time_min", hue="method", ax=axes[1])
axes[1].set_title("Construction Time")
axes[1].set_xlabel("Dataset")
axes[1].set_ylabel("Minute")
axes[1].set_yscale("log")

plt.tight_layout()
plt.savefig(f"results/figs/comparison_index.png", bbox_inches="tight")
plt.clf()