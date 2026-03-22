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
                    help="Dataset name: kosarak | retail | lastfm360k")
args = parser.parse_args()
DATASET = args.dataset

# -----------------------
# Setup
# -----------------------
os.makedirs("results/figs", exist_ok=True)
sns.set_theme(style="whitegrid", font_scale=1.1)

# -----------------------
# Index construction time & size
# -----------------------
df_les = pd.read_csv(f"results/les3_dualtrans_{DATASET}.csv")

# -----------------------
# M experiment
# -----------------------
dm = pd.read_csv(f"results/m_experiment_{DATASET}.csv")
sns.lineplot(data=dm, x="M", y="avg_ms", hue="method")
plt.title("Impact of number of buckets M on query time (δ=0.7)")
plt.ylabel("Avg query time (ms)")
plt.xlabel("M")
plt.savefig(f"results/figs/m_experiment_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Filter experiment
# -----------------------
df = pd.read_csv(f"results/filter_experiment_{DATASET}.csv")
sns.lineplot(data=df, x="delta", y="avg_ms", hue="method", style="filter")
plt.title("Average Run Time by filtering mode (M=8)")
plt.savefig(f"results/figs/filter_experiment_ms_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Filter experiment
# -----------------------
sns.lineplot(data=df, x="delta", y="avg_candidates", hue="method", style="filter")
plt.title("Number of Candidates by filtering mode (M=8)")
plt.savefig(f"results/figs/filter_experiment_cand_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Scaling
# -----------------------
dsc = pd.read_csv(f"results/scale_experiment_{DATASET}.csv")
sns.lineplot(data=dsc, x="prop", y="avg_ms", hue="method", marker="o")
plt.title("Run time vs dataset proportion (δ=0.7, M=8, CBDF)")
plt.savefig(f"results/figs/scale_experiment_{DATASET}.png", bbox_inches="tight")
plt.clf()

# -----------------------
# Baseline comparisons
# -----------------------
# comp = pd.read_csv(f"results/comparison_{DATASET}.csv")
# sns.lineplot(data=comp, x="delta", y="avg_ms", hue="method", marker="o")
# plt.title("Avg Run Time: LES3 vs LeBQ vs LeBQ+ vs DualTrans (M=8, CBDF)")
# plt.savefig(f"results/figs/comparison_runtime_{DATASET}.png", bbox_inches="tight")
# plt.clf()
