#!/usr/bin/env python3
"""Inspect a public KIT RADAR dataset landing page and discover download resources."""

from __future__ import annotations

import json
import re
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

URL_PATTERN = re.compile(r"https?://[^\"'<>\\\s]+")
ATTRIBUTE_PATTERN = re.compile(r"(?:href|src)=[\"']([^\"']+)[\"']", re.IGNORECASE)
RESOURCE_TERMS = ("download", "archive", "dataset", "api", "content", "file")


def fetch(url: str, destination: Path) -> tuple[int, str]:
    request = urllib.request.Request(url, headers={"User-Agent": "RealCars-dataset-inspector/1.0"})
    with urllib.request.urlopen(request, timeout=15) as response:
        destination.write_bytes(response.read())
        return response.status, response.geturl()


def extract_links(text: str, base_url: str) -> list[str]:
    links = {urllib.parse.urljoin(base_url, match) for match in ATTRIBUTE_PATTERN.findall(text)}
    links.update(URL_PATTERN.findall(text))
    return sorted(link.rstrip("),.;") for link in links)


def main() -> int:
    if len(sys.argv) != 2:
        raise SystemExit("usage: inspect_tire_measurement_dataset.py OUTPUT_DIRECTORY")

    output_directory = Path(sys.argv[1])
    landing_path = output_directory / "landing.html"
    kitopen_path = output_directory / "kitopen.html"
    datacite_path = output_directory / "datacite.json"

    datacite = json.loads(datacite_path.read_text(encoding="utf-8"))
    landing_text = landing_path.read_text(encoding="utf-8", errors="replace")
    kitopen_text = kitopen_path.read_text(encoding="utf-8", errors="replace")
    landing_url = datacite["data"]["attributes"]["url"]

    report = [
        "# DataCite metadata",
        json.dumps(datacite, indent=2, ensure_ascii=False),
        "\n# DOI redirect headers",
        (output_directory / "doi-headers.txt").read_text(encoding="utf-8", errors="replace"),
        "\n# Landing HTML fragments containing download/API terms",
    ]
    for match in re.finditer(
        r"[^<>\n]{0,180}(?:download|archive|dataset|api|content)[^<>\n]{0,300}",
        landing_text,
        re.IGNORECASE,
    ):
        report.append(match.group(0))

    all_links = extract_links(landing_text, landing_url)
    all_links.extend(extract_links(kitopen_text, "https://publikationen.bibliothek.kit.edu/1000181287"))
    all_links = sorted(set(all_links))
    report.append("\n# Landing-page links")
    report.extend(all_links)

    script_links = [
        link for link in all_links if urllib.parse.urlparse(link).path.lower().endswith(".js")
    ][:12]
    script_directory = output_directory / "scripts"
    script_directory.mkdir(exist_ok=True)
    discovered: set[str] = set()
    for index, script_url in enumerate(script_links):
        destination = script_directory / f"script-{index:02d}.js"
        try:
            status, final_url = fetch(script_url, destination)
        except (urllib.error.URLError, TimeoutError) as error:
            report.append(f"SCRIPT ERROR {script_url}: {error}")
            continue
        script_text = destination.read_text(encoding="utf-8", errors="replace")
        report.append(f"SCRIPT {status} {script_url} -> {final_url} ({len(script_text)} chars)")
        for candidate in extract_links(script_text, final_url):
            lowered = candidate.lower()
            if any(term in lowered for term in RESOURCE_TERMS):
                discovered.add(candidate)
        for match in re.finditer(
            r"[^\"']{0,100}(?:download|archive|dataset|api)[^\"']{0,180}",
            script_text,
            re.IGNORECASE,
        ):
            fragment = match.group(0).replace("\\/", "/")
            if "p0rr2jc5wmf0drf8" in fragment or "download" in fragment.lower():
                report.append(f"SCRIPT FRAGMENT: {fragment}")

    report.append("\n# Candidate resource URLs")
    report.extend(sorted(discovered))

    report_path = output_directory / "inspection.txt"
    report_path.write_text("\n".join(report), encoding="utf-8")
    print(report_path.read_text(encoding="utf-8"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
