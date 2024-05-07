import os
from multiprocessing import Pool
import argparse

import pandas as pd
from tqdm import tqdm


RDATA_DIR = os.path.join("data_raw")
RFORK_DIR = os.path.join(RDATA_DIR, "forklift", "Lithium-ion battery degradation dataset based on a realistic forklift operation profile")
FORK_DIR = os.path.join(RDATA_DIR, "forklift")
os.makedirs(FORK_DIR, exist_ok=True)


parser = argparse.ArgumentParser(
    prog="forklift_tocsv",
)
parser.add_argument(
    "-j", "--workers",
    type=int,
)


def process_cell(cell):
    cell_dir = os.path.join(RFORK_DIR, cell)
    AMB_TEMPS = {
        1: 45,
        2: 40,
        3: 35,
    }

    rounds = os.listdir(cell_dir)
    rounds.sort()
    rpt = pd.DataFrame()
    age = pd.DataFrame()
    for r in tqdm(rounds, f"Reading files from {cell}"):
        csv_files = os.listdir(os.path.join(cell_dir, r))
        if (n := "RPT.csv") in csv_files:
            rpt = pd.concat([rpt, pd.read_csv(os.path.join(cell_dir, r, n))])
        if (n := "Ageing.csv") in csv_files:
            age = pd.concat([age, pd.read_csv(os.path.join(cell_dir, r, n))])
    print(f"Saving RPT for {cell}")
    rpt.to_csv(os.path.join(FORK_DIR, f"{cell.lower()}_rpt.csv"), index=False)
    print(f"Saving AGE for {cell}")
    age.to_csv(os.path.join(FORK_DIR, f"{cell.lower()}_age.csv"), index=False)


def main(workers=10):
    print(f"Parsing cells: {os.listdir(RFORK_DIR)}")
    with Pool(workers) as p:
        p.map(process_cell, os.listdir(RFORK_DIR))


if __name__ == "__main__":
    args = parser.parse_args()
    main(args.workers)
