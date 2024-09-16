#!/usr/bin/env python

import matplotlib.pyplot as plt
import pandas as pd

FILENAME = "simdata/lab_240716/data.csv"


def main():
    df = pd.read_csv(FILENAME)
    print(df)
    time = df["time"]

    fig, ax = plt.subplots(3, 1)
    fig.suptitle("Electrical behavior")
    ax[0].plot(time, df["voltage"])
    ax[1].plot(time, df["open_circuit_voltage"])
    ax[2].plot(time, df["current"])
    ax[0].set_title("Terminal voltage")
    ax[1].set_title("OCV")
    ax[2].set_title("Current")

    fig, ax = plt.subplots(3, 1)
    fig.suptitle("Charge and stuff")
    ax[0].plot(time, df["internal_resistance"])
    ax[1].plot(time, df["soc_use"])
    ax[2].plot(time, df["soc_nominal"])
    ax[0].set_title("Internal resistance")
    ax[1].set_title("Usable SoC")
    ax[2].set_title("Nominal SoC")

    fig, ax = plt.subplots(3, 1)
    fig.suptitle("Temperature")
    ax[0].plot(time, df["ambient_temperature"])
    ax[1].plot(time, df["surface_temperature"])
    ax[2].plot(time, df["internal_temperature"])
    ax[0].set_title("Ambient")
    ax[1].set_title("Surface")
    ax[2].set_title("Internal")


if __name__ == "__main__":
    main()
    plt.show()
