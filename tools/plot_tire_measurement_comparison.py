#!/usr/bin/env python3
"""Create parity plots from RealCars model-to-measurement comparison output."""

from __future__ import annotations

import argparse
import csv
from collections import defaultdict
from pathlib import Path

COMPONENTS = {
    "longitudinal": ("fx",),
    "lateral": ("fy", "mz"),
    "combined": ("fx", "fy", "mz"),
}


def load_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as input_file:
        reader = csv.DictReader(input_file)
        required = {
            "sweep",
            "model",
            "normal_load_n",
            "measured_fx_n",
            "predicted_fx_n",
            "measured_fy_n",
            "predicted_fy_n",
            "measured_mz_nm",
            "predicted_mz_nm",
        }
        missing = required.difference(reader.fieldnames or [])
        if missing:
            raise ValueError(f"Missing CSV columns: {', '.join(sorted(missing))}")
        return list(reader)


def field_names(component: str) -> tuple[str, str, str]:
    if component == "fx":
        return "measured_fx_n", "predicted_fx_n", "Force [N]"
    if component == "fy":
        return "measured_fy_n", "predicted_fy_n", "Force [N]"
    if component == "mz":
        return "measured_mz_nm", "predicted_mz_nm", "Moment [N m]"
    raise ValueError(f"Unknown component: {component}")


def create_parity_plot(
    rows: list[dict[str, str]],
    model: str,
    sweep: str,
    component: str,
    output_directory: Path,
) -> None:
    import matplotlib.pyplot as plt

    measured_field, predicted_field, unit = field_names(component)
    selected = [row for row in rows if row["model"] == model and row["sweep"] == sweep]
    by_load: dict[float, list[dict[str, str]]] = defaultdict(list)
    for row in selected:
        by_load[float(row["normal_load_n"])].append(row)

    figure = plt.figure(figsize=(7, 7))
    axes = figure.add_subplot(1, 1, 1)
    all_values: list[float] = []
    for load, load_rows in sorted(by_load.items()):
        measured = [float(row[measured_field]) for row in load_rows]
        predicted = [float(row[predicted_field]) for row in load_rows]
        all_values.extend(measured)
        all_values.extend(predicted)
        axes.scatter(measured, predicted, s=16, label=f"{load:.0f} N")

    lower = min(all_values)
    upper = max(all_values)
    axes.plot([lower, upper], [lower, upper], linestyle="--", label="ideal")
    axes.set_xlabel(f"Measured {component.upper()} [{unit}]")
    axes.set_ylabel(f"Predicted {component.upper()} [{unit}]")
    axes.set_title(f"{model}: {sweep} {component.upper()} parity")
    axes.grid(True)
    axes.legend()
    axes.set_aspect("equal", adjustable="box")
    figure.tight_layout()
    figure.savefig(output_directory / f"{model}_{sweep}_{component}_parity.png", dpi=160)
    plt.close(figure)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("samples_csv", type=Path)
    parser.add_argument("output_directory", type=Path)
    args = parser.parse_args()

    args.output_directory.mkdir(parents=True, exist_ok=True)
    rows = load_rows(args.samples_csv)
    models = sorted({row["model"] for row in rows})
    for model in models:
        for sweep, components in COMPONENTS.items():
            for component in components:
                create_parity_plot(rows, model, sweep, component, args.output_directory)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
