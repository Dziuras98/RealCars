#!/usr/bin/env python3
"""Create dependency-free SVG parity plots for model-to-measurement output."""

from __future__ import annotations

import argparse
import csv
import html
from collections import defaultdict
from pathlib import Path

COMPONENTS = {
    "longitudinal": ("fx",),
    "lateral": ("fy", "mz"),
    "combined": ("fx", "fy", "mz"),
}
PLOT_SIZE = 720
MARGIN_LEFT = 92
MARGIN_RIGHT = 28
MARGIN_TOP = 72
MARGIN_BOTTOM = 82
PLOT_WIDTH = PLOT_SIZE - MARGIN_LEFT - MARGIN_RIGHT
PLOT_HEIGHT = PLOT_SIZE - MARGIN_TOP - MARGIN_BOTTOM
POINT_SYMBOLS = ("circle", "square", "diamond", "triangle")


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
        return "measured_fx_n", "predicted_fx_n", "N"
    if component == "fy":
        return "measured_fy_n", "predicted_fy_n", "N"
    if component == "mz":
        return "measured_mz_nm", "predicted_mz_nm", "N m"
    raise ValueError(f"Unknown component: {component}")


def scale(value: float, lower: float, upper: float, start: float, extent: float) -> float:
    if upper == lower:
        return start + extent / 2.0
    return start + (value - lower) / (upper - lower) * extent


def format_tick(value: float) -> str:
    magnitude = abs(value)
    if magnitude >= 1000.0:
        return f"{value:.0f}"
    if magnitude >= 10.0:
        return f"{value:.1f}"
    return f"{value:.2f}"


def point_svg(symbol: str, x: float, y: float) -> str:
    if symbol == "square":
        return f'<rect x="{x - 2.4:.2f}" y="{y - 2.4:.2f}" width="4.8" height="4.8" />'
    if symbol == "diamond":
        return (
            f'<polygon points="{x:.2f},{y - 3.2:.2f} {x + 3.2:.2f},{y:.2f} '
            f'{x:.2f},{y + 3.2:.2f} {x - 3.2:.2f},{y:.2f}" />'
        )
    if symbol == "triangle":
        return (
            f'<polygon points="{x:.2f},{y - 3.5:.2f} {x + 3.3:.2f},{y + 2.8:.2f} '
            f'{x - 3.3:.2f},{y + 2.8:.2f}" />'
        )
    return f'<circle cx="{x:.2f}" cy="{y:.2f}" r="2.6" />'


def create_parity_plot(
    rows: list[dict[str, str]],
    model: str,
    sweep: str,
    component: str,
    output_directory: Path,
) -> None:
    measured_field, predicted_field, unit = field_names(component)
    selected = [row for row in rows if row["model"] == model and row["sweep"] == sweep]
    if not selected:
        raise ValueError(f"No rows for {model}/{sweep}/{component}")

    by_load: dict[float, list[tuple[float, float]]] = defaultdict(list)
    all_values: list[float] = []
    for row in selected:
        measured = float(row[measured_field])
        predicted = float(row[predicted_field])
        by_load[float(row["normal_load_n"])].append((measured, predicted))
        all_values.extend((measured, predicted))

    data_lower = min(all_values)
    data_upper = max(all_values)
    span = data_upper - data_lower
    padding = max(span * 0.06, 1.0e-9)
    lower = data_lower - padding
    upper = data_upper + padding

    def x_position(value: float) -> float:
        return scale(value, lower, upper, MARGIN_LEFT, PLOT_WIDTH)

    def y_position(value: float) -> float:
        return MARGIN_TOP + PLOT_HEIGHT - scale(value, lower, upper, 0.0, PLOT_HEIGHT)

    lines = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        (
            f'<svg xmlns="http://www.w3.org/2000/svg" width="{PLOT_SIZE}" '
            f'height="{PLOT_SIZE}" viewBox="0 0 {PLOT_SIZE} {PLOT_SIZE}">'
        ),
        '<rect width="100%" height="100%" fill="white" />',
        '<g font-family="sans-serif" font-size="12" fill="black" stroke="none">',
        (
            f'<text x="{PLOT_SIZE / 2:.1f}" y="30" text-anchor="middle" '
            f'font-size="18">{html.escape(model)}: {html.escape(sweep)} '
            f'{component.upper()} parity</text>'
        ),
    ]

    tick_count = 6
    for index in range(tick_count):
        fraction = index / (tick_count - 1)
        value = lower + fraction * (upper - lower)
        x = MARGIN_LEFT + fraction * PLOT_WIDTH
        y = MARGIN_TOP + PLOT_HEIGHT - fraction * PLOT_HEIGHT
        lines.extend(
            [
                (
                    f'<line x1="{x:.2f}" y1="{MARGIN_TOP}" x2="{x:.2f}" '
                    f'y2="{MARGIN_TOP + PLOT_HEIGHT}" stroke="#dddddd" stroke-width="1" />'
                ),
                (
                    f'<line x1="{MARGIN_LEFT}" y1="{y:.2f}" '
                    f'x2="{MARGIN_LEFT + PLOT_WIDTH}" y2="{y:.2f}" '
                    f'stroke="#dddddd" stroke-width="1" />'
                ),
                (
                    f'<text x="{x:.2f}" y="{MARGIN_TOP + PLOT_HEIGHT + 23}" '
                    f'text-anchor="middle">{html.escape(format_tick(value))}</text>'
                ),
                (
                    f'<text x="{MARGIN_LEFT - 12}" y="{y + 4:.2f}" '
                    f'text-anchor="end">{html.escape(format_tick(value))}</text>'
                ),
            ]
        )

    lines.extend(
        [
            (
                f'<rect x="{MARGIN_LEFT}" y="{MARGIN_TOP}" width="{PLOT_WIDTH}" '
                f'height="{PLOT_HEIGHT}" fill="none" stroke="black" stroke-width="1.2" />'
            ),
            (
                f'<line x1="{x_position(lower):.2f}" y1="{y_position(lower):.2f}" '
                f'x2="{x_position(upper):.2f}" y2="{y_position(upper):.2f}" '
                f'stroke="black" stroke-width="1.2" stroke-dasharray="7,5" />'
            ),
        ]
    )

    legend_x = MARGIN_LEFT + 12
    legend_y = MARGIN_TOP + 20
    for load_index, (load, load_rows) in enumerate(sorted(by_load.items())):
        symbol = POINT_SYMBOLS[load_index % len(POINT_SYMBOLS)]
        lines.append('<g fill="black" fill-opacity="0.52" stroke="black" stroke-width="0.4">')
        for measured, predicted in load_rows:
            lines.append(point_svg(symbol, x_position(measured), y_position(predicted)))
        lines.append('</g>')
        lines.append('<g fill="black" stroke="black" stroke-width="0.5">')
        lines.append(point_svg(symbol, legend_x, legend_y + load_index * 20))
        lines.append('</g>')
        lines.append(
            f'<text x="{legend_x + 10}" y="{legend_y + load_index * 20 + 4}" '
            f'text-anchor="start">{load:.0f} N</text>'
        )

    lines.extend(
        [
            (
                f'<text x="{MARGIN_LEFT + PLOT_WIDTH / 2:.2f}" y="{PLOT_SIZE - 24}" '
                f'text-anchor="middle" font-size="14">Measured {component.upper()} '
                f'[{html.escape(unit)}]</text>'
            ),
            (
                f'<text x="24" y="{MARGIN_TOP + PLOT_HEIGHT / 2:.2f}" '
                f'text-anchor="middle" font-size="14" '
                f'transform="rotate(-90 24 {MARGIN_TOP + PLOT_HEIGHT / 2:.2f})">'
                f'Predicted {component.upper()} [{html.escape(unit)}]</text>'
            ),
            '</g>',
            '</svg>',
        ]
    )

    output_path = output_directory / f"{model}_{sweep}_{component}_parity.svg"
    output_path.write_text("\n".join(lines), encoding="utf-8")


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
