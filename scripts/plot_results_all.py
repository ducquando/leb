import os
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

DATASETS = ["retail", "lastfm1k", "kosarak"]
FIG_SIZE = (16, 3.5)

os.makedirs("results/figs", exist_ok=True)


def _dataset_label(name: str) -> str:
    return name.upper() if name != "lastfm1k" else "LASTFM1K"


def save_pointplot_across_datasets(
    file_pattern,
    x,
    y,
    hue,
    xlabel,
    ylabel,
    output_path,
    order_col=None,
    yscale="linear",
):
    fig, axes = plt.subplots(1, len(DATASETS), figsize=FIG_SIZE)

    for i, dataset in enumerate(DATASETS):
        ax = axes[i]
        data = pd.read_csv(file_pattern.format(dataset=dataset))
        order = sorted(data[order_col].dropna().unique()) if order_col else None
        sns.pointplot(
            data=data,
            x=x,
            y=y,
            hue=hue,
            order=order,
            estimator="mean",
            errorbar=("ci", 95),
            markers="o",
            linestyles="-",
            ax=ax,
        )

        ax.set_title(_dataset_label(dataset))
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel if i == 0 else "")
        ax.set_yscale(yscale)

        legend = ax.get_legend()
        if legend is not None:
            if i == 0:
                legend.set_title(hue)
            else:
                legend.remove()

    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)


def save_lineplot_across_datasets(
    file_pattern,
    x,
    y,
    hue,
    style,
    xlabel,
    ylabel,
    output_path,
    yscale="linear",
):
    fig, axes = plt.subplots(1, len(DATASETS), figsize=FIG_SIZE)

    for i, dataset in enumerate(DATASETS):
        ax = axes[i]
        data = pd.read_csv(file_pattern.format(dataset=dataset))
        sns.lineplot(
            data=data,
            x=x,
            y=y,
            hue=hue,
            style=style,
            estimator="mean",
            errorbar=("ci", 95),
            marker="o",
            ax=ax,
        )

        ax.set_title(_dataset_label(dataset))
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel if i == 0 else "")
        ax.set_yscale(yscale)

        legend = ax.get_legend()
        if legend is not None:
            if i == 0:
                legend.set_title(f"{hue} / {style}")
            else:
                legend.remove()

    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)


def save_barplot_across_datasets(
    file_pattern,
    x,
    y,
    hue,
    xlabel,
    ylabel,
    output_path,
    yscale="linear",
    hue_order=None,
):
    fig, axes = plt.subplots(1, len(DATASETS), figsize=FIG_SIZE)

    for i, dataset in enumerate(DATASETS):
        ax = axes[i]
        data = pd.read_csv(file_pattern.format(dataset=dataset))
        plot_data = data.groupby([x, hue], as_index=False)[y].mean()
        sns.barplot(data=plot_data, x=x, y=y, hue=hue, hue_order=hue_order, ax=ax)

        ax.set_title(_dataset_label(dataset))
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel if i == 0 else "")
        ax.set_yscale(yscale)

        legend = ax.get_legend()
        if legend is not None:
            if i == 0:
                legend.set_title(hue)
            else:
                legend.remove()

    fig.tight_layout()
    fig.savefig(output_path, bbox_inches="tight")
    plt.close(fig)


# M experiment
save_pointplot_across_datasets(
    file_pattern="results/m_experiment_{dataset}.csv",
    x="M",
    y="avg_ms",
    hue="method",
    xlabel="M",
    ylabel="Run time (ms)",
    output_path="results/figs/m_experiment_all.png",
    order_col="M",
)

# Filter experiment
save_lineplot_across_datasets(
    file_pattern="results/filter_experiment_{dataset}.csv",
    x="delta",
    y="avg_ms",
    hue="method",
    style="filter",
    xlabel="δ",
    ylabel="Run time (ms)",
    output_path="results/figs/filter_experiment_ms_all.png",
)

save_lineplot_across_datasets(
    file_pattern="results/filter_experiment_{dataset}.csv",
    x="delta",
    y="avg_candidates",
    hue="method",
    style="filter",
    xlabel="δ",
    ylabel="Number of candidates",
    output_path="results/figs/filter_experiment_cand_all.png",
    yscale="log",
)

# Scaling
save_pointplot_across_datasets(
    file_pattern="results/scale_experiment_{dataset}.csv",
    x="prop",
    y="avg_ms",
    hue="method",
    xlabel="Dataset proportion",
    ylabel="Run time (ms)",
    output_path="results/figs/scale_experiment_all.png",
    order_col="prop",
)
save_pointplot_across_datasets(
    file_pattern="results/scale_experiment_{dataset}.csv",
    x="prop",
    y="avg_candidates",
    hue="method",
    xlabel="Dataset proportion",
    ylabel="Number of candidates",
    output_path="results/figs/scale_experiment_cand_all.png",
    order_col="prop",
    yscale="log",
)

# Baseline comparisons
save_pointplot_across_datasets(
    file_pattern="results/comparison_{dataset}.csv",
    x="delta",
    y="avg_ms",
    hue="method",
    xlabel="δ",
    ylabel="Run time (ms)",
    output_path="results/figs/comparison_runtime_all.png",
    order_col="delta",
)
save_pointplot_across_datasets(
    file_pattern="results/comparison_{dataset}.csv",
    x="delta",
    y="avg_candidates",
    hue="method",
    xlabel="δ",
    ylabel="Number of candidates",
    output_path="results/figs/comparison_cand_all.png",
    order_col="delta",
    yscale="log",
)

# Construction comparisons
methods = ["LES3", "LeBQ", "DualTrans"]
save_barplot_across_datasets(
    file_pattern="results/index_{dataset}.csv",
    x="method",
    y="construction_time_min",
    hue="method",
    xlabel="Method",
    ylabel="Construction Time (min)",
    output_path="results/figs/comparison_construction_time_all.png",
    yscale="log",
    hue_order=methods,
)

save_barplot_across_datasets(
    file_pattern="results/index_{dataset}.csv",
    x="method",
    y="index_size_mb",
    hue="method",
    xlabel="Method",
    ylabel="Index Size (MB)",
    output_path="results/figs/comparison_index_size_all.png",
    yscale="log",
    hue_order=methods,
)
