import matplotlib.pyplot as plt
import pandas as pd


def _plot(df):
    fig, ax = plt.subplots(3, 1)

    ax_t = ax[0].twinx()
    ax[0].plot(df["time"], df["voltage"], label="Voltage")
    ax_t.plot(df["time"], df["current"], color="y", label="Current")
    ax[0].legend()
    ax[0].set_ylabel("Voltage")
    ax_t.set_ylabel("Current")

    ax[1].plot(df["time"], df["voltage"] * df["current"], label="Power")
    ax[1].legend()

    ax[2].plot(df["time"], df["amb_temp"], label="Ambient")
    ax[2].plot(df["time"], df["sf_temp"], label="Surface")
    ax[2].legend()

    fig.tight_layout()
    return fig, ax


def plot_results():
    df_air = pd.read_csv("examples/air_effect/sim2_air.csv")
    df_noair = pd.read_csv("examples/air_effect/sim2_noair.csv")

    _plot(df_noair)
    plt.show()

