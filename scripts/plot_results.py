import argparse
import json
from pathlib import Path
from typing import Dict

import pandas as pd
import matplotlib.pyplot as plt


def parse_params_from_name(name: str) -> Dict[str, str]:
    """Parse directory name like 'generations_100_population_50'."""
    parts = name.split("_")
    params = {}
    i = 0
    while i < len(parts) - 1:
        key = parts[i]
        value = parts[i + 1]
        params[key] = value
        i += 2
    return params


def load_experiments(base_dir: Path) -> pd.DataFrame:
    records = []
    for csv_path in base_dir.rglob("results.csv"):
        dir_path = csv_path.parent
        params = {}
        # Prefer JSON config if available
        for cfg_name in ["config.json", "params.json"]:
            cfg_file = dir_path / cfg_name
            if cfg_file.is_file():
                with open(cfg_file) as f:
                    try:
                        params.update(json.load(f))
                    except json.JSONDecodeError:
                        pass
                break
        # Fallback to directory name parsing
        if not params:
            params = parse_params_from_name(dir_path.name)

        df = pd.read_csv(csv_path)
        # Filter numeric runs only
        df_numeric = df[pd.to_numeric(df["run"], errors="coerce").notnull()]
        for _, row in df_numeric.iterrows():
            record = {"run": int(row["run"]), "cost": float(row["cost"])}
            record.update(params)
            records.append(record)
    return pd.DataFrame(records)


def save_summary(df: pd.DataFrame, param: str, out_dir: Path) -> str:
    summary = df.groupby(param)["cost"].agg(["mean", "std"]).reset_index()
    summary.to_csv(out_dir / f"{param}_summary.csv", index=False)

    plt.figure()
    plt.errorbar(summary[param], summary["mean"], yerr=summary["std"], fmt="-o")
    plt.xlabel(param)
    plt.ylabel("Cost")
    plt.title(f"Effect of {param}")
    plt.tight_layout()
    plt.savefig(out_dir / f"{param}_plot.png")
    plt.close()

    # Produce a simple textual interpretation
    best_idx = summary["mean"].idxmin()
    best_val = summary.loc[best_idx, param]
    best_mean = summary.loc[best_idx, "mean"]
    comment = (
        f"Najlepszy średni koszt {best_mean:.2f} uzyskano dla {param} = {best_val}."
    )
    with open(out_dir / f"{param}_analysis.txt", "w") as f:
        f.write(comment + "\n")
    return comment


def main():
    parser = argparse.ArgumentParser(description="Plot GA results")
    parser.add_argument("results_dir", type=Path, help="Directory with experiment results")
    parser.add_argument("--output", type=Path, default=Path("plots"), help="Directory to save plots")
    args = parser.parse_args()

    df = load_experiments(args.results_dir)
    if df.empty:
        print("No results found.")
        return

    args.output.mkdir(parents=True, exist_ok=True)
    param_cols = [c for c in df.columns if c not in {"run", "cost"}]
    for param in param_cols:
        comment = save_summary(df, param, args.output)
        print(comment)


if __name__ == "__main__":
    main()
