import os
from multiprocessing import Pool
import argparse

import pandas as pd
from tqdm import tqdm


RDATA_DIR = os.path.join("data_raw")
RFORK_DIR = os.path.join(RDATA_DIR, "forklift",
                         "Lithium-ion battery degradation dataset based \
on a realistic forklift operation profile")
FORK_DIR = os.path.join(RDATA_DIR, "forklift")
os.makedirs(FORK_DIR, exist_ok=True)


parser = argparse.ArgumentParser(
    prog="forklift_tocsv",
)
parser.add_argument(
    "-p", "--parallel",
    action="store_true",
    help="run in parallel",
)
parser.add_argument(
    "-j", "--workers",
    type=int,
    help="number of workers for parallel execution",
)
parser.add_argument(
    "-s", "--slice-size",
    type=int,
    help="size of each slice",
)


def process_cell(cell, slice_size=None):
    cell_dir = os.path.join(RFORK_DIR, cell)
    print(f"Slice {slice_size}")

    rounds = os.listdir(cell_dir)
    rounds.sort()
    # rpt = pd.DataFrame()
    # age = pd.DataFrame()
    first_rpt = True
    first_age = True
    for r in tqdm(rounds, f"Reading files from {cell}"):
        csv_files = os.listdir(os.path.join(cell_dir, r))
        if (n := "RPT.csv") in csv_files:
            # rpt = pd.concat([rpt, pd.read_csv(os.path.join(cell_dir, r, n))])
            if first_rpt:
                pd.read_csv(os.path.join(cell_dir, r, n)).to_csv(
                    os.path.join(
                        FORK_DIR,
                        f"{cell.lower()}_rpt.csv",
                    ),
                    index=False,
                )
                first_rpt = False
            else:
                pd.read_csv(os.path.join(cell_dir, r, n)).to_csv(
                    os.path.join(
                        FORK_DIR,
                        f"{cell.lower()}_rpt.csv",
                    ),
                    index=False,
                    header=False,
                    mode="a",
                )
        if (n := "Ageing.csv") in csv_files:
            # age = pd.concat([age, pd.read_csv(os.path.join(cell_dir, r, n))])
            if first_age:
                pd.read_csv(os.path.join(cell_dir, r, n)).to_csv(
                    os.path.join(
                        FORK_DIR,
                        f"{cell.lower()}_age.csv",
                    ),
                    index=False,
                )
                first_age = False
            else:
                pd.read_csv(os.path.join(cell_dir, r, n)).to_csv(
                    os.path.join(
                        FORK_DIR,
                        f"{cell.lower()}_age.csv",
                    ),
                    index=False,
                    header=False,
                    mode="a",
                )
    # print(f"Saving RPT for {cell}")
    # rpt.to_csv(os.path.join(FORK_DIR, f"{cell.lower()}_rpt.csv"), index=False)
    # print(f"Saving AGE for {cell}")
    # age.to_csv(os.path.join(FORK_DIR, f"{cell.lower()}_age.csv"), index=False)


def main(parallel=True, workers=10, slice_size=None):
    print(f"Parsing cells: {os.listdir(RFORK_DIR)}")
    if parallel:
        print("Running in parallel mode")
        with Pool(workers) as p:
            p.starmap(
                process_cell,
                [(c, slice_size) for c in os.listdir(RFORK_DIR)],
            )
    else:
        print("Running in sequential mode")
        for cell in os.listdir(RFORK_DIR):
            process_cell(cell, slice_size)


if __name__ == "__main__":
    args = parser.parse_args()
    main(args.parallel, args.workers, args.slice_size)
