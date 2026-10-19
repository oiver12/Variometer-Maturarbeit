#!/usr/bin/env python3
"""Plot archived C# velocity arrays; does not execute C# or rerun the filters."""

import argparse
import math
from pathlib import Path
import re

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "TheorieExperimentC#" / "TheorieExperiment" / "Program.cs"


def scalar(source, name):
    match = re.search(rf"\b(?:const\s+)?(?:int|double)\s+{name}\s*=\s*([\d.]+)\s*;", source)
    if not match:
        raise ValueError(f"Cannot read {name} from the C# source")
    return float(match[1])


def array(source, name):
    match = re.search(rf"double\[\]\s+{name}\s*=\s*new double\[\]\s*\{{([^}}]+)\}}", source)
    if not match:
        raise ValueError(f"Cannot read {name} from the C# source")
    values = [float(value.strip()) for value in match[1].split(",") if value.strip()]
    if not all(math.isfinite(value) for value in values):
        raise ValueError(f"Non-finite value in {name}")
    return values


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "docs/images/archived-comparison.svg")
    args = parser.parse_args()
    source = SOURCE.read_text(encoding="utf-8-sig")
    n_value = scalar(source, "n")
    n, dt = int(n_value), scalar(source, "dT")
    if n < 1 or n != n_value or dt <= 0:
        raise ValueError("Expected a positive integer sample count and sampling interval")
    regression = array(source, "linearRegressionVel")
    kalman = array(source, "KalmanVel")
    if min(len(regression), len(kalman)) < n:
        raise ValueError("Saved arrays are shorter than the C# sample count")
    time = [i * dt for i in range(n)]
    with plt.rc_context({"svg.fonttype": "none", "svg.hashsalt": "variometer-2021"}):
        fig, ax = plt.subplots(figsize=(10, 5.6))
        ax.plot(time, kalman[:n], label="Kalman filter (saved array)", color="#136e8a", lw=1.8)
        ax.plot(time, regression[:n], label="Linear regression (saved array)", color="#b95423", lw=1.8)
        ax.set(title="Archived velocity estimates · 2021 Maturarbeit", xlabel="Time (s)", ylabel="Vertical speed (m/s)")
        ax.grid(alpha=0.25)
        ax.legend(loc="upper right", frameon=False)
        ax.set_xlim(time[0], time[-1] if n > 1 else dt)
        fig.text(0.1, 0.05, "Source: TheorieExperimentC#/TheorieExperiment/Program.cs\n"
                 "Saved arrays only; filters not rerun. Run provenance and thesis-figure match unverified.",
                 fontsize=9, color="#555555")
        fig.tight_layout(rect=(0, 0.13, 1, 1))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(args.output, dpi=160, metadata={"Date": None})
        if args.output.suffix.lower() == ".svg":
            svg = args.output.read_text(encoding="utf-8")
            args.output.write_text(
                "\n".join(line.rstrip() for line in svg.splitlines()) + "\n",
                encoding="utf-8",
            )
        plt.close(fig)
    print(f"Plotted {n} saved samples ({time[0]:g}–{time[-1]:g} s): {args.output}")


if __name__ == "__main__":
    main()
