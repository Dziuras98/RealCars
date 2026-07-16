#!/usr/bin/env python3
"""Inspect the public Dataverse tire-measurement dataset without extra dependencies."""

from __future__ import annotations

import json
import sys
import zipfile
from pathlib import Path
from xml.etree import ElementTree

SPREADSHEET_NS = "{http://schemas.openxmlformats.org/spreadsheetml/2006/main}"
REL_NS = "{http://schemas.openxmlformats.org/officeDocument/2006/relationships}"
PACKAGE_REL_NS = "{http://schemas.openxmlformats.org/package/2006/relationships}"


def shared_strings(workbook: zipfile.ZipFile) -> list[str]:
    try:
        root = ElementTree.fromstring(workbook.read("xl/sharedStrings.xml"))
    except KeyError:
        return []
    values: list[str] = []
    for item in root.findall(f"{SPREADSHEET_NS}si"):
        values.append("".join(node.text or "" for node in item.iter(f"{SPREADSHEET_NS}t")))
    return values


def workbook_sheets(workbook: zipfile.ZipFile) -> list[tuple[str, str]]:
    root = ElementTree.fromstring(workbook.read("xl/workbook.xml"))
    relationships = ElementTree.fromstring(workbook.read("xl/_rels/workbook.xml.rels"))
    targets = {
        relation.attrib["Id"]: relation.attrib["Target"]
        for relation in relationships.findall(f"{PACKAGE_REL_NS}Relationship")
    }
    result: list[tuple[str, str]] = []
    for sheet in root.find(f"{SPREADSHEET_NS}sheets") or []:
        relation_id = sheet.attrib[f"{REL_NS}id"]
        target = targets[relation_id].lstrip("/")
        if not target.startswith("xl/"):
            target = f"xl/{target}"
        result.append((sheet.attrib["name"], target))
    return result


def cell_value(cell: ElementTree.Element, strings: list[str]) -> str:
    cell_type = cell.attrib.get("t")
    value_node = cell.find(f"{SPREADSHEET_NS}v")
    if cell_type == "inlineStr":
        return "".join(
            node.text or "" for node in cell.iter(f"{SPREADSHEET_NS}t")
        )
    if value_node is None or value_node.text is None:
        return ""
    if cell_type == "s":
        return strings[int(value_node.text)]
    return value_node.text


def preview_workbook(path: Path, report: list[str]) -> None:
    report.append(f"\n## Workbook: {path.name}")
    with zipfile.ZipFile(path) as workbook:
        strings = shared_strings(workbook)
        for sheet_name, sheet_path in workbook_sheets(workbook):
            report.append(f"\n### Sheet: {sheet_name} ({sheet_path})")
            root = ElementTree.fromstring(workbook.read(sheet_path))
            rows = root.findall(f".//{SPREADSHEET_NS}row")
            report.append(f"rows={len(rows)}")
            for row in rows[:40]:
                values = [cell_value(cell, strings) for cell in row.findall(f"{SPREADSHEET_NS}c")]
                report.append("\t".join(values))


def main() -> int:
    if len(sys.argv) != 2:
        raise SystemExit("usage: inspect_tire_measurement_dataset.py OUTPUT_DIRECTORY")

    output_directory = Path(sys.argv[1])
    metadata_path = output_directory / "dataset.json"
    archive_path = output_directory / "dataset.zip"
    extracted_directory = output_directory / "extracted"
    extracted_directory.mkdir(parents=True, exist_ok=True)

    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    report = ["# Dataset API metadata", json.dumps(metadata, indent=2, ensure_ascii=False)]

    with zipfile.ZipFile(archive_path) as archive:
        report.append("\n# Dataset archive contents")
        for item in archive.infolist():
            report.append(f"{item.filename}\t{item.file_size}")
        archive.extractall(extracted_directory)

    for workbook_path in sorted(extracted_directory.rglob("*.xlsx")):
        preview_workbook(workbook_path, report)

    report_path = output_directory / "inspection.txt"
    report_path.write_text("\n".join(report), encoding="utf-8")
    print(report_path.read_text(encoding="utf-8"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
