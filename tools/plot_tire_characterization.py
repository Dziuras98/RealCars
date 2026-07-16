#!/usr/bin/env python3
"""Plot RealCars tire-characterization CSV output."""

from __future__ import annotations

import argparse
import csv
from collections import defaultdict
from pathlib import Path
from typing import Iterable


def load_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as input_file:
        reader = csv.DictReader(input_file)
        required = {
            "model",
            "sweep",
            "normal_load_n",
            "slip_ratio",
            "slip_angle_deg",
            "camber_angle_deg",
            "fx_n",
            "fy_n",
            "mz_nm",
            "combined_utilization",
        }
        missing = required.difference(reader.fieldnames or [])
        if missing:
            raise ValueError(f"Missing CSV columns: {', '.join(sorted(missing))}")
        return list(reader)


def as_float(row: dict[str, str], key: str) -> float:
    return float(row[key])


def grouped_lines(
    rows: Iterable[dict[str, str]],
    sweep: str,
    x_key: str,
    y_key: str,
) -> dict[tuple[str, float], tuple[list[float], list[float]]]:
    grouped: dict[tuple[str, float], list[tuple[float, float]]] = defaultdict(list)
    for row in rows:
        if row["sweep"] != sweep:
            continue
        key = (row["model"], as_float(row, "normal_load_n"))
        grouped[key].append((as_float(row, x_key), as_float(row, y_key)))

    result: dict[tuple[str, float], tuple[list[float], list[float]]] = {}
    for key, values in grouped.items():
        values.sort(key=lambda item: item[0])
        result[key] = ([item[0] for item in values], [item[1] for item in values])
    return result


def plot_lines(
    rows: list[dict[str, str]],
    output_path: Path,
    sweep: str,
    x_key: str,
    y_key: str,
    x_label: str,
    y_label: str,
    title: str,
) -> None:
    import matplotlib.pyplot as plt

    figure = plt.figure(figsize=(10, 6))
    axes = figure.add_subplot(1, 1, 1)
    for (model, load), (x_values, y_values) in grouped_lines(
        rows, sweep, x_key, y_key
    ).items():
        axes.plot(x_values, y_values, label=f"{model}, {load:.0f} N")
    axes.set_xlabel(x_label)
    axes.set_ylabel(y_label)
    axes.set_title(title)
    axes.grid(True)
    axes.legend()
    figure.tight_layout()
    figure.savefig(output_path, dpi=160)
    plt.close(figure)


def plot_combined_utilization(
    rows: list[dict[str, str]], output_directory: Path
) -> None:
    import matplotlib.pyplot as plt

    models = sorted({row["model"] for row in rows if row["sweep"] == "combined"})
    for model in models:
        selected = [
            row
            for row in rows
            if row["sweep"] == "combined" and row["model"] == model
        ]
        figure = plt.figure(figsize=(9, 7))
        axes = figure.add_subplot(1, 1, 1)
        scatter = axes.scatter(
            [as_float(row, "slip_ratio") for row in selected],
            [as_float(row, "slip_angle_deg") for row in selected],
            c=[as_float(row, "combined_utilization") for row in selected],
            s=18,
        )
        axes.set_xlabel("Slip ratio")
        axes.set_ylabel("Slip angle [deg]")
        axes.set_title(f"Combined-force utilization — {model}")
        figure.colorbar(scatter, ax=axes, label="Utilization")
        figure.tight_layout()
        figure.savefig(output_directory / f"combined_utilization_{model}.png", dpi=160)
        plt.close(figure)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("samples_csv", type=Path)
    parser.add_argument("output_directory", type=Path, nargs="?")
    args = parser.parse_args()

    output_directory = args.output_directory or args.samples_csv.parent / "plots"
    output_directory.mkdir(parents=True, exist_ok=True)
    rows = load_rows(args.samples_csv)

    plot_lines(
        rows,
        output_directory / "longitudinal_force.png",
        "longitudinal",
        "slip_ratio",
        "fx_n",
        "Slip ratio",
        "Longitudinal force [N]",
        "Pure longitudinal tire response",
    )
    plot_lines(
        rows,
        output_directory / "lateral_force.png",
        "lateral",
        "slip_angle_deg",
        "fy_n",
        "Slip angle [deg]",
        "Lateral force [N]",
        "Pure lateral tire response",
    )
    plot_lines(
        rows,
        output_directory / "aligning_moment.png",
        "lateral",
        "slip_angle_deg",
        "mz_nm",
        "Slip angle [deg]",
        "Aligning moment [N m]",
        "Self-aligning moment response",
    )
    plot_lines(
        rows,
        output_directory / "camber_thrust.png",
        "camber",
        "camber_angle_deg",
        "fy_n",
        "Camber angle [deg]",
        "Lateral force [N]",
        "Camber-thrust response",
    )
    plot_combined_utilization(rows, output_directory)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
