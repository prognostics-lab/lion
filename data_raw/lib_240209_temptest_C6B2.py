import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from IPython.display import display
from scipy import signal, interpolate
import winsound

import os
import time
import sys
import datetime
from collections import namedtuple
import pathlib

# fmt: off
src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
print(f"Appending '{src_path}' to path")
sys.path.append(str(src_path))
from thermal_model import models
# fmt: on


def alert():
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 1000)
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 100)
    winsound.Beep(1440, 100)


TargetParams = namedtuple("TargetParams", "cp cair rair rin rout")
TargetNLParams = namedtuple("TargetNLParams", "cp cair rair rin rout k")
Data = namedtuple("Data", "t y u x0")


DATA_DIR = os.path.join("data_raw", "240209_temptest_C6B2")
TEMP_IMG_DIR = os.path.join("img_raw")
IMG_DIR = os.path.join("img_raw")
SAVE_FMT = "pdf"
SAVEFIG_PARAMS = {"dpi": 1000, "bbox_inches": "tight"}

plt.style.use("tableau-colorblind10")
# plt.rc("text", usetex=True)
# plt.rc("font", family="serif")

INTERNAL_RESISTANCE = 4


DATA_CHAMBER_FILENAME = os.path.join(DATA_DIR, "chamber.csv")
DATA_SENSOR_FILENAME = os.path.join(DATA_DIR, "sensor_temp.csv")
DATA_CAPACITY_FILENAME = os.path.join(DATA_DIR, "TestData.csv")
DATA_PROCEDURE_FILENAME = os.path.join(DATA_DIR, "TestProcedure.csv")

chamber_df = pd.read_csv(DATA_CHAMBER_FILENAME).drop(
    ["Unnamed: 8"], axis=1, inplace=False)
sensor_df = pd.read_csv(
    DATA_SENSOR_FILENAME,
    header=0,
    skiprows=[1, 2, 3],
)
cap_df = pd.read_csv(
    DATA_CAPACITY_FILENAME,
    header=0,
    names=["Current", "Voltage", "Capacity",
           "Cumulative_capacity", "Seconds", "Test_State", "SOC"],
)
# proc_df = pd.read_csv(DATA_PROCEDURE_FILENAME)

# Temperature sensor data
TEMP_TIME_FULL = (sensor_df["unix_time_utc"] -
                  sensor_df["unix_time_utc"][0]).to_numpy()
TEMP_S1_FULL = sensor_df["temp_s1"].to_numpy()
TEMP_S2_FULL = sensor_df["temp_s2"].to_numpy()
TEMP_A1_FULL = sensor_df["temp_a1"].to_numpy()
TEMP_A2_FULL = sensor_df["temp_a2"].to_numpy()
TEMP_SUR_FULL = 0.5 * (TEMP_S1_FULL + TEMP_S2_FULL)
TEMP_AIR_FULL = 0.5 * (TEMP_A1_FULL + TEMP_A2_FULL)

_TEMP_CUTOFF = -29789
TEMP_TIME = TEMP_TIME_FULL[:_TEMP_CUTOFF]
TEMP_S1 = TEMP_S1_FULL[:_TEMP_CUTOFF]
TEMP_S2 = TEMP_S2_FULL[:_TEMP_CUTOFF]
TEMP_A1 = TEMP_A1_FULL[:_TEMP_CUTOFF]
TEMP_A2 = TEMP_A2_FULL[:_TEMP_CUTOFF]
TEMP_SUR = TEMP_SUR_FULL[:_TEMP_CUTOFF]
TEMP_AIR = TEMP_AIR_FULL[:_TEMP_CUTOFF]


# Chamber data
_chamber_seconds = np.array([datetime.datetime.strptime(
    d, "%m/%d/%Y %H:%M:%S") for d in chamber_df["DateTime"]])
_chamber_seconds = _chamber_seconds - _chamber_seconds[0]
CHAMBER_TIME_FULL = np.array([delta.total_seconds()
                             for delta in _chamber_seconds])
CHAMBER_SP_FULL = chamber_df["TEMPERATURE SP"].to_numpy()
CHAMBER_PV_FULL = chamber_df["TEMPERATURE PV"].to_numpy()
_CHAMBER_CUTOFF = -4069
CHAMBER_TIME = CHAMBER_TIME_FULL[:_CHAMBER_CUTOFF]
CHAMBER_SP = CHAMBER_SP_FULL[:_CHAMBER_CUTOFF]
CHAMBER_PV = CHAMBER_PV_FULL[:_CHAMBER_CUTOFF]


# Capacity data
_cap_seconds = np.array([datetime.datetime.strptime(
    d[:-3], "%m/%d/%Y %H:%M:%S.%f") for d in cap_df["Seconds"]])
_cap_seconds = _cap_seconds - _cap_seconds[0]
CAP_TIME = np.array([delta.total_seconds() for delta in _cap_seconds])
CAP_CURRENT = cap_df["Current"].to_numpy()
CAP_VOLTAGE = cap_df["Voltage"].to_numpy()
CAP_POWER = CAP_CURRENT * CAP_VOLTAGE
CAP_SOC = cap_df["SOC"].to_numpy()


_TIME_LIMIT = 50
starts_idx = np.concatenate(
    ([0], np.where((CAP_TIME[1:] - CAP_TIME[:-1]) >= _TIME_LIMIT)[0] + 1))
ends_idx = np.concatenate(
    (np.where((CAP_TIME[1:] - CAP_TIME[:-1]) >= _TIME_LIMIT)[0], [-1]))
starts = CAP_TIME[starts_idx]
ends = CAP_TIME[ends_idx]


segments_total = sum(1 for _ in zip(starts, ends))
temp_masks = [(s <= TEMP_TIME) & (TEMP_TIME <= e)
              for s, e in zip(starts, ends)]
chamber_masks = [(s <= CHAMBER_TIME) & (CHAMBER_TIME <= e)
                 for s, e in zip(starts, ends)]
cap_masks = [(s <= CAP_TIME) & (CAP_TIME <= e) for s, e in zip(starts, ends)]

# All segments will always have `segments_total` elements
# The variables `temp_*_rs_segs` correspond to segmented and resampled temperature sensor data,
# while `temp_*_segs` also is low-pass filtred to remove sensor noise

cap_time_segs = [CAP_TIME[m] for m in cap_masks]
cap_current_segs = [CAP_CURRENT[m] for m in cap_masks]
cap_voltage_segs = [CAP_VOLTAGE[m] for m in cap_masks]
cap_power_segs = [CAP_POWER[m] for m in cap_masks]
cap_soc_segs = [CAP_SOC[m] for m in cap_masks]

temp_time_segs = [TEMP_TIME[m] for m in temp_masks]
temp_sur_raw_segs = [TEMP_SUR[m] for m in temp_masks]
temp_air_raw_segs = [TEMP_AIR[m] for m in temp_masks]
temp_sur_rs_segs = [signal.resample(temp_sur_raw_segs[i], len(
    cap_time_segs[i])) for i in range(segments_total)]
temp_air_rs_segs = [signal.resample(temp_air_raw_segs[i], len(
    cap_time_segs[i])) for i in range(segments_total)]
b, a = signal.butter(5, 0.005)
_zi = signal.lfilter_zi(b, a)
temp_sur_segs = [signal.lfilter(b, a, temp, zi=_zi * temp[0])[0]
                 for temp in temp_sur_rs_segs]
temp_air_segs = [signal.lfilter(b, a, temp, zi=_zi * temp[0])[0]
                 for temp in temp_air_rs_segs]
gen_heat_segs = [models.generated_heat_from_current(
    current, temp, soc, INTERNAL_RESISTANCE) for current, temp, soc in zip(cap_current_segs, temp_sur_segs, cap_soc_segs)]

chamber_time_segs = [CHAMBER_TIME[m] for m in chamber_masks]
chamber_sp_raw_segs = [CHAMBER_SP[m] for m in chamber_masks]
chamber_pv_raw_segs = [CHAMBER_PV[m] for m in chamber_masks]
_sp_lerp = [interpolate.interp1d(t, y, fill_value="extrapolate") for t, y in zip(
    chamber_time_segs, chamber_sp_raw_segs)]
_pv_lerp = [interpolate.interp1d(t, y, fill_value="extrapolate") for t, y in zip(
    chamber_time_segs, chamber_pv_raw_segs)]
chamber_sp_segs = [f(t) for t, f in zip(cap_time_segs, _sp_lerp)]
chamber_pv_rs_segs = [f(t) for t, f in zip(cap_time_segs, _pv_lerp)]
chamber_pv_segs = [signal.lfilter(
    b, a, temp, zi=_zi * temp[0])[0] for temp in chamber_pv_rs_segs]

temp_sensor_std = temp_air_segs[2][1000:30000].std()
chamber_pv_std = chamber_pv_segs[2][1000:30000].std()


def get_data(training_segment=3, eval_segment=2, *, training_start=None, training_cutoff=None, eval_start=None, eval_cutoff=None):
    # We assume the observations match the states at the beginning
    t_train = np.linspace(0, cap_time_segs[training_segment][-1] -
                          cap_time_segs[training_segment][0], len(cap_time_segs[training_segment]))[training_start:training_cutoff]
    y_train = np.array([temp_sur_segs[training_segment],
                        temp_air_segs[training_segment]]).T[training_start:training_cutoff, :]
    u_train = np.array([chamber_pv_segs[training_segment],
                        gen_heat_segs[training_segment]]).T[training_start:training_cutoff, :]
    x0_train = y_train[0]
    train_data = Data(t_train, y_train, u_train, x0_train)

    t_eval = np.linspace(0, cap_time_segs[eval_segment][-1] -
                         cap_time_segs[eval_segment][0], len(cap_time_segs[eval_segment]))[eval_start:eval_cutoff]
    y_eval = np.array([temp_sur_segs[eval_segment],
                       temp_air_segs[eval_segment]]).T[eval_start:eval_cutoff]
    u_eval = np.array([chamber_pv_segs[eval_segment],
                       gen_heat_segs[eval_segment]]).T[eval_start:eval_cutoff]
    x0_eval = y_eval[0]
    eval_data = Data(t_eval, y_eval, u_eval, x0_eval)

    return train_data, eval_data


def main():
    # Displaying the data tables
    display(chamber_df.iloc[:-4069])
    display(sensor_df.iloc[:-29789])
    # display(eis_df)
    display(cap_df)
    # display(proc_df)

    # Show results of the segmentation
    fig, ax = plt.subplots()
    ax.scatter(CAP_TIME, np.zeros(CAP_TIME.shape), s=1)
    ax.vlines(starts, -1, 1, alpha=0.2)
    ax.vlines(ends, -0.5, 0.5, alpha=0.2)

    # Raw and filtered temperatures
    fig, axs = plt.subplots(3, segments_total, figsize=(15, 10))
    for i, ax in enumerate(axs[0]):
        ax.plot(cap_time_segs[i] / 3600, temp_sur_rs_segs[i],
                label="$T^{{MMSEE}}_{{sur}}$, resampled", alpha=0.5)
        ax.plot(cap_time_segs[i] / 3600, temp_sur_segs[i],
                label="$T^{{MMSEE}}_{{sur}}$, resampled and filtered", alpha=0.5)
    for i, ax in enumerate(axs[1]):
        ax.plot(cap_time_segs[i] / 3600, temp_air_rs_segs[i],
                label="$T^{{MMSEE}}_{{air}}$, resampled", alpha=0.5)
        ax.plot(cap_time_segs[i] / 3600, temp_air_segs[i],
                label="$T^{{MMSEE}}_{{air}}$, resampled and filtered", alpha=0.5)
    for i, ax in enumerate(axs[2]):
        ax.plot(cap_time_segs[i] / 3600, chamber_pv_rs_segs[i],
                label="$T^{{MMSEE}}_{{pv}}$, resampled", alpha=0.5)
        ax.plot(cap_time_segs[i] / 3600, chamber_pv_segs[i],
                label="$T^{{MMSEE}}_{{pv}}$, resampled and filtered", alpha=0.5)
    axs[0][0].set_ylabel("Surface temperature (°C)")
    axs[1][0].set_ylabel("Air temperature (°C)")
    fig.tight_layout()

    # Temperatures and different system features
    fig, axs = plt.subplots(4, segments_total, figsize=(15, 10))
    for i, ax in enumerate(axs[0]):
        ax.plot(cap_time_segs[i] / 3600, temp_sur_segs[i],
                label="$T_{{sur}}$", alpha=0.5)
        ax.plot(cap_time_segs[i] / 3600, temp_air_segs[i],
                label="$T_{{air}}$", alpha=0.5)
        ax.plot(cap_time_segs[i] / 3600, chamber_pv_segs[i],
                label="$T_{{amb}}^{{PV}}$", alpha=0.5, linestyle="--")
        ax.plot(cap_time_segs[i] / 3600, chamber_sp_segs[i],
                label="$T_{{amb}}^{{SP}}$", alpha=0.5, linestyle="--", color="k")
        ax.set_xlim(*(cap_time_segs[i][[0, -1]] / 3600))
        ax.grid(alpha=0.25)

    for i, ax in enumerate(axs[1]):
        ax.plot(cap_time_segs[i] / 3600,
                cap_current_segs[i], label="$I$ (A)", alpha=0.75)
        ax.set_xlim(*(cap_time_segs[i][[0, -1]] / 3600))
        ax.grid(alpha=0.25)

    for i, ax in enumerate(axs[2]):
        ax.plot(cap_time_segs[i] / 3600, cap_soc_segs[i],
                label="SoC (%)", alpha=0.75)
        ax.set_xlim(*(cap_time_segs[i][[0, -1]] / 3600))
        ax.grid(alpha=0.25)

    for i, ax in enumerate(axs[3]):
        ax.plot(cap_time_segs[i] / 3600,
                cap_power_segs[i], label="$P$ (W)", alpha=0.75)
        ax.set_xlim(*(cap_time_segs[i][[0, -1]] / 3600))
        ax.grid(alpha=0.25)

    axs[0][0].set_ylabel("Temperature (°C)")
    axs[1][0].set_ylabel("Current (A)")
    axs[2][0].set_ylabel("SoC (%)")
    axs[3][0].set_ylabel("Power (W)")
    for ax in axs[2]:
        ax.set_xlabel("Time (h)")
    axs[0][-1].legend()
    fig.tight_layout()

    plt.show()


if __name__ == "__main__":
    main()
