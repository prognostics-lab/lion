#!/usr/bin/env python

import sys

import matplotlib.pyplot as plt
import pandas as pd

FILENAME = "simdata/lab_240716/curropt.csv"


def main(idx=None):
    df = pd.read_csv(FILENAME)
    print(df)

    currs = df.columns.to_numpy(dtype=float)

    if idx is None:
        fig, ax = plt.subplots()
        ax.plot(currs, df.iloc[0], alpha=1, label="First")
        for _, row in df.iterrows():
            ax.plot(currs, row, alpha=0.01)
        ax.plot(currs, df.iloc[-1], alpha=1, label="Last")
        ax.legend()
        ax.set_xlabel("Current (A)")
        ax.set_ylabel("Target function (-)")


if __name__ == "__main__":
    idx = None
    if len(sys.argv) != 1:
        idx = int(sys.argv[1])
    main(idx)

    plt.show()
