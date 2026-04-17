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
FIG_SIZE = (9, 5)

# -----------------------
# Setup
# -----------------------
os.makedirs("results/figs", exist_ok=True)
# sns.set_theme(style="whitegrid", font_scale=1.1)

def save_pointplot(data, x, y, hue, title, xlabel, ylabel, output_path, order=None, yscale="linear"):
    fig, ax = plt.subplots(figsize=FIG_SIZE)
    sns.pointplot(data=data, x=x, y=y, hue=hue, order=order, estimator="mean", errorbar=("ci", 95), markers="o", linestyles="-", ax=ax)
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_yscale(yscale)
    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)

def save_lineplot(data, x, y, hue, style, title, xlabel, ylabel, output_path, yscale="linear"):
    fig, ax = plt.subplots(figsize=FIG_SIZE)
    sns.lineplot(data=data, x=x, y=y, hue=hue, style=style, marker="o", ax=ax)
    ax.set_title(title)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_yscale(yscale)
    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)

# -----------------------
# M experiment
# -----------------------
dm = pd.read_csv(f"results/m_experiment_{DATASET}.csv")
save_pointplot(
    dm,
    x="M",
    y="avg_ms",
    hue="method",
    title=f"Impact of number of buckets M on query time ({DATASET}, δ=0.7)",
    xlabel="M",
    ylabel="Query time (ms)",
    output_path=f"results/figs/m_experiment_{DATASET}.png",
    order=sorted(dm["M"].dropna().unique()),
)

# -----------------------
# Filter experiment -- Run Time
# -----------------------
df = pd.read_csv(f"results/filter_experiment_{DATASET}.csv")
save_lineplot(
    df,
    x="delta", 
    y="avg_ms", 
    hue="method", 
    style="filter",
    title=f"Average Run Time by filtering mode ({DATASET}, M=8)",
    xlabel="δ",
    ylabel="Run time (ms)",
    output_path=f"results/figs/filter_experiment_ms_{DATASET}.png",
)
save_lineplot(
    df,
    x="delta", 
    y="avg_candidates", 
    hue="method", 
    style="filter",
    title=f"Number of Candidates by filtering mode ({DATASET}, M=8)",
    xlabel="δ",
    ylabel="Number of candidates",
    output_path=f"results/figs/filter_experiment_cand_{DATASET}.png",
    yscale="log",
)

# -----------------------
# Scaling
# -----------------------
dsc = pd.read_csv(f"results/scale_experiment_{DATASET}.csv")
save_pointplot(
    dsc,
    x="prop",
    y="avg_ms",
    hue="method",
    title=f"Run time vs dataset proportion ({DATASET}, δ=0.7, M=8, CBDF)",
    xlabel="Dataset proportion",
    ylabel="Run time (ms)",
    output_path=f"results/figs/scale_experiment_{DATASET}.png",
    order=sorted(dsc["prop"].dropna().unique()),
)

# -----------------------
# Baseline comparisons
# -----------------------
comp = pd.read_csv(f"results/comparison_{DATASET}.csv")
save_pointplot(
    comp,
    x="delta",
    y="avg_ms",
    hue="method",
    title=f"Avg Run Time ({DATASET}, M=8, CBDF)",
    xlabel="δ",
    ylabel="Run time (ms)",
    output_path=f"results/figs/comparison_runtime_{DATASET}.png",
    order=sorted(comp["delta"].dropna().unique()),
)

# -----------------------
# Construction comparisons
# -----------------------
fig, axes = plt.subplots(2, 1, figsize=FIG_SIZE)
all_comp, datasets, methods = [], ["retail", "lastfm1k", "kosarak"], ["LES3", "LeBQ", "DualTrans"]
for dataset in datasets:
    comp = pd.read_csv(f"results/index_{dataset}.csv")
    comp["dataset"] = dataset
    all_comp.append(comp)
comp = pd.concat(all_comp)
comp_plot = (comp.groupby(["dataset", "method"], as_index=False)[["construction_time_min", "index_size_mb"]].mean())

sns.barplot(data=comp_plot, x="dataset", y="construction_time_min", hue="method", hue_order=methods, order=datasets, ax=axes[0], legend=False)
axes[0].set_xlabel(None)
axes[0].set_ylabel("Construction Time (min)")
axes[0].set_yscale("log")

sns.barplot(data=comp_plot, x="dataset", y="index_size_mb", hue="method", hue_order=methods, order=datasets, ax=axes[1])
axes[1].set_xlabel(None)
axes[1].set_ylabel("Index Size (MB)")
axes[1].set_yscale("log")

fig.tight_layout()
fig.savefig(f"results/figs/comparison_index.png", bbox_inches="tight")
plt.close(fig)