import os
import sys
import pathlib
import argparse

import pandas as pd
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
sys.path.append(str(src_path))
# pylint: disable=import-error
from thermal_model.estimation import lti_from_data
from thermal_model.estimation.models import generate_evaluation
from thermal_model.models import (
    generated_heat_from_current,
    EhcParams,
    OcvParams,
    ThermalParams,
    calculate_soc_coeff,
    calculate_soc_with_temperature,
    calculate_ocv_temperature,
    calculate_ocv,
    calculate_ocv,
)
# pylint: enable=import-error


# Argument parser
parser = argparse.ArgumentParser(
    prog="phm_plots", description="generator for PHM plots"
)
parser.add_argument(
    "-s",
    "--showfig",
    help="show plots",
    action="store_true",
)
parser.add_argument(
    "-f",
    "--savefig",
    help="save plots",
    action="store_true",
)


# Configure default parameters for plots
SMALL_SIZE = 8
MEDIUM_SIZE = 10
BIGGER_SIZE = 12
DEFAULT_FIGSIZE = (3.38765625, 3)

mpl.rcParams["text.usetex"] = True
mpl.rcParams["font.family"] = "Times"
mpl.rcParams["font.size"] = MEDIUM_SIZE
mpl.rcParams["axes.titlesize"] = MEDIUM_SIZE
mpl.rcParams["axes.labelsize"] = MEDIUM_SIZE
mpl.rcParams["ytick.labelsize"] = SMALL_SIZE
mpl.rcParams["ytick.labelsize"] = SMALL_SIZE
mpl.rcParams["legend.fontsize"] = SMALL_SIZE
mpl.rcParams["figure.titlesize"] = MEDIUM_SIZE
mpl.rcParams["figure.figsize"] = DEFAULT_FIGSIZE
mpl.rcParams["lines.linewidth"] = 1

plt.style.use("tableau-colorblind10")


# Paths and constants
TEMP_IMG_DIR = os.path.join("img_raw")
IMG_DIR = os.path.join("img")
THEORY_IMG_DIR = os.path.join(IMG_DIR, "theory")
EXPERIMENTAL_IMG_DIR = os.path.join(IMG_DIR, "experiments")
SAVE_FMT = "pdf"
SAVEFIG_PARAMS = {"dpi": 1000, "bbox_inches": "tight"}
os.makedirs(TEMP_IMG_DIR, exist_ok=True)
os.makedirs(THEORY_IMG_DIR, exist_ok=True)
os.makedirs(EXPERIMENTAL_IMG_DIR, exist_ok=True)


# Model constants and parameters
TH_TREF = 2.980e2
TH_ALPHA = -5.738
TH_BETA = 2.099e2
TH_PARAMS = ThermalParams(TH_ALPHA, TH_BETA, TH_TREF)

OCV_V0 = 4.14
OCV_VL = 3.977
OCV_ALPHA = 0.15
OCV_BETA = 17
OCV_GAMMA = 10.5
OCV_PARAMS = OcvParams(OCV_V0, OCV_VL, OCV_ALPHA, OCV_BETA, OCV_GAMMA)

EHC_AMPLITUDE = 0.04e-3
EHC_KAPPA = 3
EHC_MU = 0.4
EHC_SIGMA = 0.05
EHC_LAMBDA = 7
EHC_PARAMS = EhcParams(EHC_AMPLITUDE, EHC_KAPPA, EHC_MU, EHC_SIGMA, EHC_LAMBDA)


# Helper functions
def celsius_to_kelvin(t):
    return t + 273


def kelvin_to_celsius(t):
    return t - 273


# Functions to generate the different plots
def generate_soc_shift_plt(savefig):
    fig, ax = plt.subplots()
    soc = np.linspace(0, 1, 100)
    temperatures = np.linspace(celsius_to_kelvin(-40), celsius_to_kelvin(100), 100)

    for t in temperatures:
        soc_t = calculate_soc_with_temperature(soc, t, TH_PARAMS)
        if t == temperatures[0] or t == temperatures[-1]:
            ax.plot(
                100 * soc,
                100 * soc_t,
                label=f"T° = {t:.0f}\\,K ({kelvin_to_celsius(t):.0f}\\,°C)",
            )
        else:
            ax.plot(100 * soc, 100 * soc_t, alpha=0.1, color="#aaa")
    ax.plot(100 * soc, 100 * soc, linestyle="--", color="#000", label="Reference T°")
    fig.suptitle("SoC variation with temperature")
    ax.grid(alpha=0.25)
    ax.set_xlabel(r"$\mathrm{SoC}_0$ (\%)")
    ax.set_ylabel(r"$\mathrm{SoC}$ (\%)")
    ax.legend()

    fig.tight_layout()

    if savefig:
        fig.savefig(
            os.path.join(THEORY_IMG_DIR, f"soc_shift.{SAVE_FMT}"), **SAVEFIG_PARAMS
        )


def generate_ocv_plt(savefig):
    fig, ax = plt.subplots()
    soc = np.linspace(0.01, 1, 100)
    temperatures = np.linspace(celsius_to_kelvin(-40), celsius_to_kelvin(100), 100)

    for t in temperatures:
        soc_t = calculate_soc_with_temperature(soc, t, TH_PARAMS)
        ocv = calculate_ocv_temperature(soc_t, t, OCV_PARAMS)
        if t == temperatures[0] or t == temperatures[-1]:
            ax.plot(
                100 * soc,
                ocv,
                label=f"T° = {t:.0f}\\,K ({kelvin_to_celsius(t):.0f}\\,°C)",
            )
        else:
            ax.plot(100 * soc, ocv, alpha=0.1, color="#aaa")
    ax.plot(
        100 * soc,
        calculate_ocv(soc),
        linestyle="--",
        color="#000",
        label="Reference T°",
    )
    ax.set_title("Effect of temperature on OCV curve according to model")
    ax.grid(alpha=0.25)
    ax.set_xlabel(r"$\mathrm{SoC}_0$ (\%)")
    ax.set_ylabel(r"$V_{\mathrm{oc}}$ (V)")
    ax.legend()

    fig.tight_layout()

    if savefig:
        fig.savefig(
            os.path.join(THEORY_IMG_DIR, f"ocv_curve.{SAVE_FMT}"), **SAVEFIG_PARAMS
        )


def generate_air_theory_plt(savefig):
    figsize = (DEFAULT_FIGSIZE[0], 7)
    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=figsize)

    df_real = pd.read_csv(os.path.join("examples", "air_effect", "data_air_real.csv"))
    df_est = pd.read_csv(os.path.join("examples", "air_effect", "data_air_est.csv"))

    real_time = df_real["time"].to_numpy() / 3600
    est_time = df_est["time"].to_numpy() / 3600

    ax1.plot(real_time, kelvin_to_celsius(df_real["sf_temp"]), label="Expected", linestyle="--")
    ax1.plot(est_time, kelvin_to_celsius(df_est["sf_temp"]), label="Obtained")
    ax2.plot(real_time, 1e3 * df_real["q_gen"], label="Expected", linestyle="--")
    ax2.plot(est_time, 1e3 * df_est["q_gen"], label="Obtained")

    ax1.set_title("Surface temperature")
    ax2.set_title("Generated heat")
    ax1.grid(alpha=0.25)
    ax2.grid(alpha=0.25)
    ax1.legend()
    ax2.legend()

    ax2.set_xlabel("Time (h)")
    ax1.set_ylabel("Temperature (°C)")
    ax2.set_ylabel("Heat (mW)")

    fig.tight_layout()
    if savefig:
        fig.savefig(
            os.path.join(THEORY_IMG_DIR, f"air_curves.{SAVE_FMT}"), **SAVEFIG_PARAMS
        )


def generate_noair_theory_plt(savefig):
    figsize = (DEFAULT_FIGSIZE[0], 7)
    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=figsize)

    df_real = pd.read_csv(os.path.join("examples", "air_effect", "data_noair_real.csv"))
    df_est = pd.read_csv(os.path.join("examples", "air_effect", "data_noair_est.csv"))

    real_time = df_real["time"].to_numpy() / 3600
    est_time = df_est["time"].to_numpy() / 3600

    ax1.plot(real_time, kelvin_to_celsius(df_real["sf_temp"]), label="Expected", linestyle="--")
    ax1.plot(est_time, kelvin_to_celsius(df_est["sf_temp"]), label="Obtained")
    ax2.plot(real_time, 1e3 * df_real["q_gen"], label="Expected", linestyle="--")
    ax2.plot(est_time, 1e3 * df_est["q_gen"], label="Obtained")

    ax1.set_title("Surface temperature")
    ax2.set_title("Generated heat")
    ax1.grid(alpha=0.25)
    ax2.grid(alpha=0.25)
    ax1.legend()
    ax2.legend()

    ax2.set_xlabel("Time (h)")
    ax1.set_ylabel("Temperature (°C)")
    ax2.set_ylabel("Heat (mW)")

    fig.tight_layout()
    if savefig:
        fig.savefig(
            os.path.join(THEORY_IMG_DIR, f"noair_curves.{SAVE_FMT}"), **SAVEFIG_PARAMS
        )


def main(savefig=False, showfig=False):
    generate_soc_shift_plt(savefig)
    generate_ocv_plt(savefig)
    generate_air_theory_plt(savefig)
    generate_noair_theory_plt(savefig)

    if showfig:
        plt.show()


if __name__ == "__main__":
    args = parser.parse_args()
    main(savefig=args.savefig, showfig=args.showfig)
