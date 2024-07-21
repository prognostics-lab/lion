import os
import datetime
import sys
import pathlib
from collections import namedtuple

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy import signal, interpolate

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
print(f"Appending '{src_path}' to path")
sys.path.append(str(src_path))
from thermal_model import models


# matplotlib configurations
SAVE_FMT = "pdf"
SAVEFIG_PARAMS = {"dpi": 1000, "bbox_inches": "tight"}
plt.style.use("tableau-colorblind10")


### Relevant directories ###
DATA_DIR = os.path.join("data", "240716_temp_profile_C4B1")
TEMP_IMG_DIR = os.path.join("img_raw")
IMG_DIR = os.path.join("img_raw")

DATA_CHAMBER_FILENAME = os.path.join(DATA_DIR, "chamber.csv")
DATA_SENSOR_FILENAME = os.path.join(DATA_DIR, "sensor_temp.csv")
DATA_CAPACITY_FILENAME = os.path.join(DATA_DIR, "TestData.csv")
DATA_PROCEDURE_FILENAME = os.path.join(DATA_DIR, "TestProcedure.csv")


### Data structures ###
Data = namedtuple("Data", "t y u x0")


### Load data ###
chamber_df = pd.read_csv(DATA_CHAMBER_FILENAME).drop(
    ["Unnamed: 8"], axis=1, inplace=False)
sensor_df = pd.read_csv(
    DATA_SENSOR_FILENAME,
    header=0,
)
cap_df = pd.read_csv(
    DATA_CAPACITY_FILENAME,
    names=["Current", "Voltage", "Capacity",
           "Cumulative_capacity", "Seconds", "Test_State", "SOC"],
)

### Temperature data ###
temp_time_full = (sensor_df["unix_time_utc"] -
                  sensor_df["unix_time_utc"][0]).to_numpy()
temp_s1_full = sensor_df["temp_s1"].to_numpy()
temp_s2_full = sensor_df["temp_s2"].to_numpy()
temp_a1_full = sensor_df["temp_a1"].to_numpy()
temp_a2_full = sensor_df["temp_a2"].to_numpy()
temp_sur_full = 0.5 * (temp_s1_full + temp_s2_full)
temp_air_full = 0.5 * (temp_a1_full + temp_a2_full)

_TEMP_START = None
_TEMP_CUTOFF = None
temp_time = temp_time_full[_TEMP_START:_TEMP_CUTOFF]
temp_time = temp_time - temp_time[0]
temp_sur = temp_sur_full[_TEMP_START:_TEMP_CUTOFF]
temp_air = temp_air_full[_TEMP_START:_TEMP_CUTOFF]


### Chamber data ###
_chamber_seconds = np.array([datetime.datetime.strptime(d, "%m/%d/%Y %H:%M:%S") for d in chamber_df["DateTime"]])
_chamber_seconds = _chamber_seconds - _chamber_seconds[0]
chamber_time_full = np.array([delta.total_seconds() for delta in _chamber_seconds])
chamber_sp_full = chamber_df["TEMPERATURE SP"].to_numpy()
chamber_pv_full = chamber_df["TEMPERATURE PV"].to_numpy()

_CHAMBER_START = None
_CHAMBER_CUTOFF = None
chamber_time = chamber_time_full[_CHAMBER_START:_CHAMBER_CUTOFF]
chamber_time = chamber_time - chamber_time[0]
chamber_sp = chamber_sp_full[_CHAMBER_START:_CHAMBER_CUTOFF]
chamber_pv = chamber_pv_full[_CHAMBER_START:_CHAMBER_CUTOFF]


### Capacity data ###
_cap_seconds = np.array([datetime.datetime.strptime(
    d[:-3], "%m/%d/%Y %H:%M:%S.%f") for d in cap_df["Seconds"]])
_cap_seconds = _cap_seconds - _cap_seconds[0]
cap_time_full = np.array([delta.total_seconds() for delta in _cap_seconds])
cap_current_full = cap_df["Current"].to_numpy()
cap_voltage_full = cap_df["Voltage"].to_numpy()
cap_soc_full = cap_df["SOC"].to_numpy()

_CAP_START = 3800
_CAP_CUTOFF = None
cap_time = cap_time_full[_CAP_START:_CAP_CUTOFF]
cap_time = cap_time - cap_time[0]
cap_voltage = cap_voltage_full[_CAP_START:_CAP_CUTOFF]
cap_current = cap_current_full[_CAP_START:_CAP_CUTOFF]
cap_soc = cap_soc_full[_CAP_START:_CAP_CUTOFF]
cap_power = cap_current * cap_voltage

cell_internal_resistance = np.max(cap_voltage / cap_current)
print(f"Calculated internal resistance is {cell_internal_resistance} Ohm")

_TO_UTC = "-04:00"
sensor_idx = 0 if _TEMP_START is None else _TEMP_START
chamber_idx = 0 if _CHAMBER_START is None else _CHAMBER_START
cap_idx = 0 if _CAP_START is None else _CAP_START
sensor_start_time = datetime.datetime.strptime(sensor_df["timestamp_utc"].iloc[sensor_idx], "%Y-%m-%d %H:%M:%S.%f%z")
chamber_start_time = datetime.datetime.strptime(chamber_df["DateTime"].iloc[chamber_idx] + _TO_UTC, "%m/%d/%Y %H:%M:%S%z")
cap_start_time = datetime.datetime.strptime(cap_df["Seconds"].iloc[cap_idx][:-3] + _TO_UTC, "%m/%d/%Y %H:%M:%S.%f%z")
sensor_start_time = sensor_start_time.astimezone()
chamber_start_time = chamber_start_time.astimezone()
cap_start_time = cap_start_time.astimezone()
print("===============================")
print("Start times report (Local time)")
print("-------------------------------")
print(f"Sensors  : {sensor_start_time}")
print(f"Chamber  : {chamber_start_time}")
print(f"Capacity : {cap_start_time}")
print("===============================")


### Segmentation ###
_TIME_LIMIT = 100
starts_idx = np.concatenate(
    ([0], np.where((cap_time[1:] - cap_time[:-1]) >= _TIME_LIMIT)[0] + 1))
ends_idx = np.concatenate(
    (np.where((cap_time[1:] - cap_time[:-1]) >= _TIME_LIMIT)[0], [-1]))
starts = cap_time[starts_idx]
ends = cap_time[ends_idx]

# Define masks
segments_total = sum(1 for _ in zip(starts, ends))
temp_masks = [(s <= temp_time) & (temp_time <= e)
              for s, e in zip(starts, ends)]
chamber_masks = [(s <= chamber_time) & (chamber_time <= e)
                 for s, e in zip(starts, ends)]
cap_masks = [(s <= cap_time) & (cap_time <= e) for s, e in zip(starts, ends)]

# Segmentate capacity data
cap_time_segs = [cap_time[m] for m in cap_masks]
cap_current_segs = [cap_current[m] for m in cap_masks]
cap_voltage_segs = [cap_voltage[m] for m in cap_masks]
cap_power_segs = [cap_power[m] for m in cap_masks]
cap_soc_segs = [cap_soc[m] for m in cap_masks]

# Segmentation and filtering of temperature data
temp_time_segs = [temp_time[m] for m in temp_masks]
temp_sur_raw_segs = [temp_sur[m] for m in temp_masks]
temp_air_raw_segs = [temp_air[m] for m in temp_masks]
temp_sur_rs_segs = [signal.resample(temp_sur_raw_segs[i], len(
    cap_time_segs[i])) for i in range(segments_total)]
temp_air_rs_segs = [signal.resample(temp_air_raw_segs[i], len(
    cap_time_segs[i])) for i in range(segments_total)]
# temp_sur_rs_segs = temp_sur_raw_segs
# temp_air_rs_segs = temp_air_raw_segs
b, a = signal.butter(5, 0.01)
_zi = signal.lfilter_zi(b, a)
temp_sur_segs = [signal.lfilter(b, a, temp, zi=_zi * temp[0])[0]
                 for temp in temp_sur_rs_segs]
temp_air_segs = [signal.lfilter(b, a, temp, zi=_zi * temp[0])[0]
                 for temp in temp_air_rs_segs]
# gen_heat_segs = [models.generated_heat_from_current(
#     current, temp, soc, cell_internal_resistance) for current, temp, soc in zip(cap_current_segs, temp_sur_segs, cap_soc_segs)]

# Segementation, filtering and interpolation of chamber data
chamber_time_segs = [chamber_time[m] for m in chamber_masks]
chamber_sp_raw_segs = [chamber_sp[m] for m in chamber_masks]
chamber_pv_raw_segs = [chamber_pv[m] for m in chamber_masks]
_sp_lerp = [interpolate.interp1d(t, y, fill_value="extrapolate") for t, y in zip(
    chamber_time_segs, chamber_sp_raw_segs)]
_pv_lerp = [interpolate.interp1d(t, y, fill_value="extrapolate") for t, y in zip(
    chamber_time_segs, chamber_pv_raw_segs)]
chamber_sp_segs = [f(t) for t, f in zip(cap_time_segs, _sp_lerp)]
chamber_pv_rs_segs = [f(t) for t, f in zip(cap_time_segs, _pv_lerp)]
chamber_pv_segs = [signal.lfilter(
    b, a, temp, zi=_zi * temp[0])[0] for temp in chamber_pv_rs_segs]

temp_sensor_std = temp_sur[30000:].std()
temp_sensor_mean = temp_sur[30000:].mean()
temp_sensor_snr = (temp_sensor_mean / temp_sensor_std) ** 2
temp_sensor_snr = 10 * np.log10(temp_sensor_snr)
print(f"Temperature sensor mean: {temp_sensor_mean}")
print(f"Temperature sensor std: {temp_sensor_std}")
print(f"Temperature sensor SNR: {temp_sensor_snr} dB")
chamber_pv_std = chamber_pv[4000:].std()
chamber_pv_mean = chamber_pv[4000:].mean()
chamber_pv_snr = (chamber_pv_mean / chamber_pv_std) ** 2
chamber_pv_snr = 10 * np.log10(chamber_pv_snr)
print(f"Chamber mean: {chamber_pv_mean}")
print(f"Chamber std: {chamber_pv_std}")
print(f"Chamber SNR: {chamber_pv_snr} dB")


def get_data(training_segment=0, eval_segment=0, *, training_start=None, training_cutoff=None, eval_start=None, eval_cutoff=None):
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
    ### Raw data plots ###
    fig, ax = plt.subplots(3, 1, sharex=True)

    # ax[0].scatter(temp_time, temp_sur, 1, alpha=0.5, label="Surface")
    # ax[0].scatter(temp_time, temp_air, 1, alpha=0.5, label="Air")
    ax[0].plot(temp_time, temp_sur, alpha=0.5, label="Surface")
    ax[0].plot(temp_time, temp_air, alpha=0.5, label="Air")
    ax[0].scatter(chamber_time, chamber_pv, 1, alpha=0.5, label="Ambient (PV)")
    ax[0].scatter(chamber_time, chamber_sp, 1, alpha=0.5, label="Ambient (SP)")
    ax[0].legend()

    ax[1].scatter(cap_time, cap_voltage, 1, alpha=0.5, label="Voltage")
    ax_t = ax[1].twinx()
    ax_t.scatter(cap_time, cap_current, 1, alpha=0.5, color="y", label="Current")
    ax[1].legend()
    ax[1].set_ylabel("Voltage")
    ax_t.set_ylabel("Current")

    ax[2].scatter(cap_time, cap_power, 1, label="Power")
    ax_t = ax[2].twinx()
    ax_t.scatter(cap_time, cap_voltage / cap_current, 1, color="k", label="Resistance")
    ax_t.set_ylabel("Resistance")
    ax[2].set_ylabel("Power")
    ax[2].legend()

    ### Plot filtered temperature data ###
    fig, axs = plt.subplots(2, segments_total + 1, sharex=True)
    for ax, time, sur, amb in zip(axs[0, :].flatten(), cap_time_segs, temp_sur_segs, chamber_pv_segs):
        ax.plot(time, sur, label="Surface")
        # ax.plot(time, amb, label="Ambient")
        ax.grid(alpha=0.25)
        ax.legend()

    for ax, time, current, power in zip(axs[1, :].flatten(), cap_time_segs, cap_current_segs, cap_power_segs):
        ax.plot(time, current, label="Current")
        ax.plot(time, power, label="Power")
        ax.grid(alpha=0.25)
        ax.legend()


if __name__ == "__main__":
    main()
    plt.show()
