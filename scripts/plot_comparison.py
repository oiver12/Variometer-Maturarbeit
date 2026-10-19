#!/usr/bin/env python3
"""Compile and rerun the original C++ simulation, export CSV, and plot both filters."""

import argparse
import csv
import hashlib
import io
import json
import math
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile
import urllib.request

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parents[1]
SIMULATION = ROOT / "SimulationArduino"
HOST = ROOT / "scripts/host_simulation"
LIBRARIES = [
    {
        "name": "Kalman",
        "repository": "rfetick/Kalman",
        "revision": "df339d72f9f90f5cf5459b2f82e9234a12ab4162",
        "files": {"Kalman.h": "09eaafc6e14b016e06f685215193efa15696491436e23be5e52fad1db73a1fa3"},
    },
    {
        "name": "BasicLinearAlgebra-2.3",
        "repository": "tomstewart89/BasicLinearAlgebra",
        "revision": "fb37152795998668d1a1392026a8e69b5e18bef7",
        "files": {
            "BasicLinearAlgebra.h": "13a58669ebd4807759e52cc35d3ff001b98690642d02f4fe3c793340486aec73",
            "MemoryDelegate.hpp": "3b4ea71cd1c7ffbf669fdeffcde38cf0a33ad0fc52ad94d1ebbcb2947292944a",
        },
    },
]


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def dependencies(directory):
    for library in LIBRARIES:
        for filename, digest in library["files"].items():
            target = directory / library["name"] / filename
            if not target.exists() or sha256(target.read_bytes()) != digest:
                url = (f"https://raw.githubusercontent.com/{library['repository']}/"
                       f"{library['revision']}/{filename}")
                with urllib.request.urlopen(url, timeout=30) as response:
                    data = response.read()
                if sha256(data) != digest:
                    raise ValueError(f"Dependency checksum mismatch: {filename}")
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(data)
            print(f"Verified {library['name']}/{filename}")


def run_filters(compiler, directory):
    with tempfile.TemporaryDirectory(prefix="variometer-simulation-") as temporary:
        binary = Path(temporary) / "simulation"
        command = compiler + ["-std=c++11", "-O0", "-ffp-contract=off"]
        for include in [HOST, SIMULATION] + [directory / lib["name"] for lib in LIBRARIES]:
            command += ["-I", str(include)]
        command += [str(HOST / "main.cpp"), str(SIMULATION / "Variometer.cpp"),
                    str(SIMULATION / "LinearRegression.cpp"), "-o", str(binary)]
        subprocess.run(command, check=True)
        result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    print(result.stderr.strip())
    match = re.search(r"double\[\] KalmanVel = new double\[\] \{([^}]+)\}", result.stdout)
    if not match:
        raise ValueError("Sketch did not produce the expected Kalman array")
    kalman = [float(value) for value in match[1].split(",") if value.strip()]
    regression = list(csv.reader(io.StringIO(result.stdout.split("REGRESSION_BEGIN\n", 1)[1])))
    if len(kalman) != len(regression) or not kalman:
        raise ValueError("Filters did not produce one result per input sample")
    rows = []
    for velocity, fields in zip(kalman, regression):
        if len(fields) != 4:
            raise ValueError("Unexpected host-runner output")
        time, height, acceleration, slope = map(float, fields)
        row = [time, height, acceleration, velocity, slope]
        if not all(math.isfinite(value) for value in row):
            raise ValueError("Non-finite filter output")
        if rows and time <= rows[-1][0]:
            raise ValueError("Input times are not strictly increasing")
        rows.append(row)
    return rows


def check_archive(rows):
    source = ROOT / "TheorieExperimentC#/TheorieExperiment/Program.cs"
    text = source.read_text(encoding="utf-8-sig")
    match = re.search(r"double\[\] KalmanVel = new double\[\] \{([^}]+)\}", text)
    if not match:
        raise ValueError("Archived Kalman reference array not found")
    archived = [float(value) for value in match[1].split(",") if value.strip()]
    if len(archived) != len(rows):
        raise ValueError("Archived reference has a different sample count")
    error = max(abs(row[3] - value) for row, value in zip(rows, archived))
    print(f"Maximum difference from archived Kalman array: {error:.3g} m/s")
    if error > 2e-5:
        raise ValueError("Archived Kalman check failed; check inputs, parameters and library versions")
    return error


def plot(rows, output):
    time, height, _, kalman, regression = zip(*rows)
    with plt.rc_context({"svg.fonttype": "none", "svg.hashsalt": "variometer-cpp-reproduction"}):
        fig, (ax_height, ax_velocity) = plt.subplots(
            2, 1, figsize=(10, 7), sharex=True, gridspec_kw={"height_ratios": [1, 2]}
        )
        fig.suptitle("Kalman filter vs. linear regression", fontsize=17)
        ax_height.plot(time, height, color="#667a89", lw=1, label="Archived simulated height input")
        ax_height.set_ylabel("Height (m)")
        ax_height.legend(loc="upper left", frameon=False, fontsize=9)
        ax_velocity.plot(time, kalman, color="#136e8a", lw=1.8, label="Kalman filter")
        ax_velocity.plot(time, regression, color="#b95423", lw=1.8, label="Linear regression")
        ax_velocity.set(xlabel="Time (s)", ylabel="Vertical speed (m/s)", xlim=(time[0], time[-1]))
        ax_velocity.legend(loc="lower right", frameon=False)
        for axis in (ax_height, ax_velocity):
            axis.grid(alpha=0.22)
        fig.text(0.1, 0.025,
                 f"Original C++ filters rerun on {len(rows)} archived simulated measurements · Linux host run\n"
                 "Original inputs and parameters; no new sensor noise or flight measurements.",
                 fontsize=9, color="#555555")
        fig.tight_layout(rect=(0, 0.095, 1, 0.96))
        output.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(output, dpi=160, metadata={"Date": None})
        plt.close(fig)
    if output.suffix.lower() == ".svg":
        svg = output.read_text(encoding="utf-8")
        output.write_text("\n".join(line.rstrip() for line in svg.splitlines()) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "docs/images/filter-comparison.svg")
    parser.add_argument("--csv", type=Path, default=ROOT / "docs/data/filter-comparison.csv")
    parser.add_argument("--metadata", type=Path, default=ROOT / "docs/data/filter-comparison.json")
    parser.add_argument("--deps-dir", type=Path, default=Path(tempfile.gettempdir()) / "variometer-reproduction-deps")
    parser.add_argument("--compiler", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--verify-archive", action="store_true", help="Check rerun against saved C# Kalman outputs")
    args = parser.parse_args()
    dependencies(args.deps_dir)
    compiler = shlex.split(args.compiler)
    rows = run_filters(compiler, args.deps_dir)
    error = check_archive(rows) if args.verify_archive else None
    args.csv.parent.mkdir(parents=True, exist_ok=True)
    with args.csv.open("w", newline="") as handle:
        writer = csv.writer(handle, lineterminator="\n")
        writer.writerow(["time_s", "height_m", "acceleration_m_s2", "kalman_m_s", "linear_regression_m_s"])
        writer.writerows(rows)
    sources = sorted(SIMULATION.glob("*")) + sorted(HOST.glob("*")) + [
        Path(__file__).resolve(), ROOT / "TheorieExperimentC#/TheorieExperiment/Program.cs"
    ]
    metadata = {
        "samples": len(rows), "time_range_s": [rows[0][0], rows[-1][0]],
        "execution": "Unmodified SimulationArduino sketch and C++ classes, compiled for the Linux host",
        "libraries": LIBRARIES,
        "source_sha256": {str(path.relative_to(ROOT)): sha256(path.read_bytes())
                          for path in sources if path.is_file()},
        "alignment": "One regression estimate per input; extra serial warm-up zero tokens are ignored",
        "precision": "Host double is 64-bit; AVR double is 32-bit. Matrix and regression state remain float.",
        "archived_kalman_max_absolute_difference_m_s": error,
    }
    args.metadata.parent.mkdir(parents=True, exist_ok=True)
    args.metadata.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    plot(rows, args.output)
    print(f"Executed both filters for {len(rows)} samples ({rows[0][0]:g}–{rows[-1][0]:g} s)")
    print(f"Plot: {args.output}\nData: {args.csv}\nProvenance: {args.metadata}")


if __name__ == "__main__":
    main()
