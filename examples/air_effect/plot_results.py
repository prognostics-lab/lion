import matplotlib.pyplot as plt
import pandas as pd


def _plot(df):
    fig, ax = plt.subplots(2, 1)

    ax_t = ax[0].twinx()
    ax[0].plot(df["time"], df["voltage"], label="Current")
    ax_t.plot(df["time"], df["current"], label="Voltage")
    ax[0].legend()

    ax[1].plot(df["time"], df["amb_temp"], label="Ambient")
    ax[1].plot(df_air["time"], df_air["sf_temp"], label="Surface")

    fig.tight_layout()


def plot_results():
    df_air = pd.read_csv("examples/air_effect/sim2_air.csv")
    df_noair = pd.read_csv("examples/air_effect/sim2_noair.csv")



