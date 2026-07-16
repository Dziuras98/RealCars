#!/usr/bin/env python3
"""Download and extract the public KIT tire-force dataset used for validation."""

from __future__ import annotations

import argparse
import hashlib
import shutil
import tarfile
import tempfile
import urllib.request
import zipfile
from pathlib import Path

DATASET_DOI = "10.35097/p0rr2jc5wmf0drf8"
DATASET_URL = (
    "https://radar.kit.edu/radar/api/datasets/"
    "p0rr2jc5wmf0drf8/download"
)
EXPECTED_ARCHIVE_MD5 = "8fe6695d591f944d0abcd97af3039c8b"
EXPECTED_PAYLOAD_ZIP_SHA256 = (
    "5ffe1c57663f94a85c42498dcd83d875a1263858a8bd6e67970039b06ffb1353"
)
PAYLOAD_ZIP_NAME = "Determination of tire properties - Data.zip"


def digest(path: Path, algorithm: str) -> str:
    hasher = hashlib.new(algorithm)
    with path.open("rb") as input_file:
        for block in iter(lambda: input_file.read(1024 * 1024), b""):
            hasher.update(block)
    return hasher.hexdigest()


def ensure_within(root: Path, destination: Path) -> None:
    root_resolved = root.resolve()
    destination_resolved = destination.resolve()
    if root_resolved != destination_resolved and root_resolved not in destination_resolved.parents:
        raise RuntimeError(f"Archive entry escapes destination: {destination}")


def extract_tar_safely(archive_path: Path, destination: Path) -> None:
    with tarfile.open(archive_path, "r:*") as archive:
        for member in archive.getmembers():
            ensure_within(destination, destination / member.name)
        archive.extractall(destination)


def extract_zip_safely(archive_path: Path, destination: Path) -> None:
    with zipfile.ZipFile(archive_path) as archive:
        for member in archive.infolist():
            ensure_within(destination, destination / member.filename)
        archive.extractall(destination)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("output_directory", type=Path)
    args = parser.parse_args()

    output_directory = args.output_directory.resolve()
    if output_directory.exists():
        shutil.rmtree(output_directory)
    output_directory.mkdir(parents=True)

    request = urllib.request.Request(
        DATASET_URL,
        headers={"User-Agent": "RealCars-tire-validation/1.0"},
    )
    with tempfile.TemporaryDirectory() as temporary_directory_text:
        temporary_directory = Path(temporary_directory_text)
        archive_path = temporary_directory / "kit-tire-dataset.tar"
        with urllib.request.urlopen(request, timeout=120) as response:
            with archive_path.open("wb") as output_file:
                shutil.copyfileobj(response, output_file)

        archive_md5 = digest(archive_path, "md5")
        if archive_md5 != EXPECTED_ARCHIVE_MD5:
            raise RuntimeError(
                f"Unexpected dataset archive MD5: {archive_md5}; "
                f"expected {EXPECTED_ARCHIVE_MD5}"
            )

        extracted_archive = temporary_directory / "archive"
        extracted_archive.mkdir()
        extract_tar_safely(archive_path, extracted_archive)
        payload_candidates = list(extracted_archive.rglob(PAYLOAD_ZIP_NAME))
        if len(payload_candidates) != 1:
            raise RuntimeError(
                f"Expected exactly one {PAYLOAD_ZIP_NAME!r}, found {len(payload_candidates)}"
            )
        payload_path = payload_candidates[0]
        payload_sha256 = digest(payload_path, "sha256")
        if payload_sha256 != EXPECTED_PAYLOAD_ZIP_SHA256:
            raise RuntimeError(
                f"Unexpected payload SHA-256: {payload_sha256}; "
                f"expected {EXPECTED_PAYLOAD_ZIP_SHA256}"
            )
        extract_zip_safely(payload_path, output_directory)

    manifest = output_directory / "realcars_source_manifest.txt"
    manifest.write_text(
        "\n".join(
            [
                f"dataset_doi={DATASET_DOI}",
                f"download_url={DATASET_URL}",
                "license=CC BY-NC-SA 4.0",
                f"archive_md5={EXPECTED_ARCHIVE_MD5}",
                f"payload_sha256={EXPECTED_PAYLOAD_ZIP_SHA256}",
                "data_vendored_in_realcars=false",
                "purpose=non-commercial model validation",
                "",
            ]
        ),
        encoding="utf-8",
    )
    print(f"Extracted KIT tire measurements to {output_directory}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
