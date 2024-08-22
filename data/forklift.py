import os
import sys
import pathlib
import argparse

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm
from scipy import optimize

# fmt: off
src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
print(f"Appending '{src_path}' to path")
sys.path.append(str(src_path))
from thermal_model.estimation import lti_from_data
from thermal_model.estimation.models import generate_evaluation
from thermal_model.models import generated_heat_from_current
from thermal_model.logger import LOGGER, setup_logger
# fmt: on


# Define directories and auxiliary functions
DATA_DIR = os.path.join("data_raw")
FORK_DIR = os.path.join(DATA_DIR, "forklift")
IMG_DIR = os.path.join("img_raw")
FORK_IMG_DIR = os.path.join(IMG_DIR, "forklift")
os.makedirs(FORK_IMG_DIR, exist_ok=True)


parser = argparse.ArgumentParser(
    prog="forklift",
)
parser.add_argument(
    "cell",
    type=int,
)
parser.add_argument(
    "-b", "--base",
    choices=["r", "a", "rpt", "age"],
    default=None,
    metavar="base",
    help="base dataframe to use",
)
parser.add_argument(
    "-t", "--rtrain",
    type=int,
    help="round to use for training",
)
parser.add_argument(
    "-e", "--reval",
    type=int,
    help="round to use for evaluation",
)
parser.add_argument(
    "-o", "--showfig",
    action="store_true",
    help="show figures",
)
parser.add_argument(
    "-s", "--savefig",
    action="store_true",
    help="save figures",
)
parser.add_argument(
    "-p", "--pdf",
    action="store_true",
    help="save figures in pdf format",
)
parser.add_argument(
    "-d", "--debug",
    action="store_true",
    help="run in debug mode",
)
parser.add_argument(
    "-v", "--verbose",
    action="store_true",
    help="run in verbose mode",
)


def convert_size(size_bytes):
    # Convert bytes to KB, MB, GB, TB
    power = 1024
    size_types = ["B", "KB", "MB", "GB", "TB"]
    size = size_bytes / power
    i = 0
    while size >= power:
        size /= power
        i += 1
    return "{:.2f} {}".format(size, size_types[i])


def get_data(df, amb_temp):
    # Data extractable from dataset
    LOGGER.debug("Extracting known values")
    surf = df["Temperature"].to_numpy()
    t = np.arange(len(surf)) + 1
    curr = df["Current"].to_numpy()
    amb = amb_temp * np.ones(len(surf))

    # Secondary data
    LOGGER.debug("Calculating secondary data")
    resistance = (df["Voltage"] / df["Current"])
    internal_resistance = resistance[resistance != np.inf].max()
    soc = curr.cumsum() / (3600 * 180)
    soc = soc - soc.min()
    qgen = generated_heat_from_current(curr, surf, soc, internal_resistance)

    # Data assembly
    LOGGER.debug("Assembling outputs")
    y = surf
    u = np.array([
        amb,
        qgen,
    ]).T
    x0 = np.array([surf[0], surf[0]])
    return y, u, t, x0


def optimizer_callback(intermediate_result):
    print(f"\rTarget function: {intermediate_result.fun}             ", end="")


def main(savefig, showfig, img_fmt, cell, r_train, r_eval, base_str=None):
    if img_fmt:
        img_fmt = "pdf"
    else:
        img_fmt = "png"

    ### Data loading ###
    AMB_TEMPS = {
        1: 45,
        2: 40,
        3: 35,
    }
    constant_ambient_temperature = AMB_TEMPS[cell]

    if base_str is None:
        LOGGER.warning("Base was not specified, using 'age'")
        base_str = "age"

    match base_str.lower():
        case "rpt":
            LOGGER.info("Reading RPT")
            base = pd.read_csv(os.path.join(FORK_DIR, f"cell{cell}_rpt.csv"))
            LOGGER.info(f"RPT: {convert_size(base.memory_usage(deep=True).sum())}")
        case "age":
            LOGGER.info("Reading age")
            base = pd.read_csv(os.path.join(FORK_DIR, f"cell{cell}_age.csv"))
            LOGGER.info(f"AGE: {convert_size(base.memory_usage(deep=True).sum())}")

    LOGGER.debug("Choosing desired rounds")
    df_train = base[base["Round"] == r_train]
    df_eval = base[base["Round"] == r_eval]

    LOGGER.info("Generating data")
    y_train, u_train, t_train, x0_train = get_data(
        df_train, amb_temp=constant_ambient_temperature)
    y_eval, u_eval, t_eval, x0_eval = get_data(
        df_eval, amb_temp=constant_ambient_temperature)

    ### Minimization ###
    LOGGER.info("Starting minimization\n")
    (A, B, C, _), params = lti_from_data(
        y_train,
        u_train,
        t_train,
        x0_train,
        1e-1,
        0,
        system_kwargs={
            "outputs": "sf",
        }, optimizer_kwargs={
            "method": "Nelder-Mead",
            "callback": optimizer_callback,
            "fn": optimize.minimize,
            #"method": "trf",
            #"verbose": 2,
            #"fn": optimize.least_squares,
        },
    )
    LOGGER.info("\n")
    LOGGER.info(f"Final parameters: {params}")
    LOGGER.info(f"A = \n{A}")
    LOGGER.info(f"B = \n{B}")
    LOGGER.info(f"C = \n{C}")

    LOGGER.info(f"Continous poles: {np.linalg.eig(A)[0]}")

    if np.linalg.matrix_rank(np.vstack([C, C @ A])) != 2:
        LOGGER.warning("!!! SYSTEM IS NOT OBSERVABLE !!!")

    ### Evaluation ###
    LOGGER.info(f"Evaluating estimation")
    evaluate = generate_evaluation(
        y_eval, u_eval, t_eval, x0_eval, outputs="sf")
    expected, obtained, error, x = evaluate(params)
    error_square = error ** 2
    try:
        mse = np.diag(error.conjugate().T @ error) / len(error)
    except ValueError:
        mse = error.conjugate().T @ error / len(error)
    LOGGER.info(f"MSE: {mse}")

    if savefig or showfig:
        LOGGER.info(f"Generating plots")
        fig, ((ax1, ax2, ax3), (ax4, ax5, ax6)) = plt.subplots(
            2, 3, figsize=(15, 10), sharex=True)

        ax1.plot(t_eval / 3600, expected, label="Expected", linestyle="--")
        ax1.plot(t_eval / 3600, obtained, label="Obtained")
        # ax2.plot(t_eval / 3600, expected, label="Expected", linestyle="--")
        # ax2.plot(t_eval / 3600, obtained[:, 1], label="Obtained")
        ax3.plot(t_eval / 3600, x[:, 0], label="Internal temperature")
        ax3.plot(t_eval / 3600, x[:, 1], label="Air temperature")

        ax4.plot(t_eval / 3600, u_eval[:, 1], label="Generated heat")
        ax5.plot(t_eval / 3600, u_eval[:, 0], label="Ambient temperature")
        ax6.plot(t_eval / 3600, error_square, label="Error, surface")

        ax1.set_xlabel("Time (h)")
        ax1.set_ylabel("Surface temperature (°C)")
        # ax2.set_xlabel("Time (h)")
        # ax2.set_ylabel("Air temperature (°C)")
        ax3.set_xlabel("Time (h)")
        ax3.set_ylabel("States (°C)")
        ax4.set_xlabel("Time (h)")
        ax4.set_ylabel("Heat (W)")
        ax5.set_xlabel("Time (h)")
        ax5.set_ylabel("Ambient temperature (°C)")
        ax6.set_xlabel("Time (h)")
        ax6.set_ylabel("$\\left(y - \\hat y\\right)^2$")

        ax1.set_title("Surface")
        ax2.set_title("Air")
        ax3.set_title("States")
        ax6.set_title("Error")
        ax4.set_title("Generated heat")
        ax5.set_title("Ambient temperature")

        ax1.legend()
        # ax2.legend()
        ax3.legend()
        ax6.legend()
        ax1.grid(alpha=0.25)
        ax2.grid(alpha=0.25)
        ax3.grid(alpha=0.25)
        ax4.grid(alpha=0.25)
        ax5.grid(alpha=0.25)
        ax6.grid(alpha=0.25)

        ax6.set_yscale("log")

        fig.tight_layout()
        if savefig:
            fig.savefig(os.path.join(
                FORK_IMG_DIR,
                f"eval_curves.{img_fmt}"), bbox_inches="tight")
        if showfig:
            plt.show()


if __name__ == "__main__":
    args = parser.parse_args()
    setup_logger(debug=args.debug, verbose=args.verbose)
    main(
        args.savefig,
        args.showfig,
        args.pdf,
        args.cell,
        args.rtrain,
        args.reval,
        args.base,
    )
