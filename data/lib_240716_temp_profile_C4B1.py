import datetime
import os
import pathlib
import sys
from collections import namedtuple

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import pywt
from scipy import interpolate, signal

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "pysrc")
print(f"Appending '{src_path}' to path")
sys.path.append(str(src_path))
from thermal_model import models

# matplotlib configurations
SAVE_FMT = "pdf"
SAVEFIG_PARAMS = {"dpi": 1000, "bbox_inches": "tight"}
plt.style.use("tableau-colorblind10")


### Relevant directories ###
DATA_DIR = os.path.join("data", "240716_temp_profile_C4B1")
DATA_PROC_DIR = os.path.join(DATA_DIR, "processed")
os.makedirs(DATA_PROC_DIR, exist_ok=True)
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
    ["Unnamed: 8"], axis=1, inplace=False
)
sensor_df = pd.read_csv(
    DATA_SENSOR_FILENAME,
    header=0,
)
cap_df = pd.read_csv(
    DATA_CAPACITY_FILENAME,
    names=[
        "Current",
        "Voltage",
        "Capacity",
        "Cumulative_capacity",
        "Seconds",
        "Test_State",
        "SOC",
    ],
)


### Temperature data ###
# temp_time_full = (sensor_df["unix_time_utc"] - sensor_df["unix_time_utc"][0]).to_numpy()
_temp_seconds = np.array(
    [datetime.datetime.fromisoformat(d) for d in sensor_df["timestamp_utc"]]
)
_temp_seconds = _temp_seconds - _temp_seconds[0]
temp_time_full = np.array([delta.total_seconds() for delta in _temp_seconds])
temp_s1_full = sensor_df["temp_s1"].to_numpy()
temp_s2_full = sensor_df["temp_s2"].to_numpy()
temp_a1_full = sensor_df["temp_a1"].to_numpy()
temp_a2_full = sensor_df["temp_a2"].to_numpy()
temp_sur_full = 0.5 * (temp_s1_full + temp_s2_full)
temp_air_full = 0.5 * (temp_a1_full + temp_a2_full)

_TEMP_START = None
_TEMP_CUTOFF = None
temp_time_raw = temp_time_full[_TEMP_START:_TEMP_CUTOFF]
temp_time_raw = temp_time_raw - temp_time_raw[0]
temp_sur_raw = temp_sur_full[_TEMP_START:_TEMP_CUTOFF]
temp_air_raw = temp_air_full[_TEMP_START:_TEMP_CUTOFF]


### Chamber data ###
_chamber_seconds = np.array(
    [datetime.datetime.strptime(d, "%m/%d/%Y %H:%M:%S") for d in chamber_df["DateTime"]]
)
_chamber_seconds = _chamber_seconds - _chamber_seconds[0]
chamber_time_full = np.array([delta.total_seconds() for delta in _chamber_seconds])
chamber_sp_full = chamber_df["TEMPERATURE SP"].to_numpy()
chamber_pv_full = chamber_df["TEMPERATURE PV"].to_numpy()

_CHAMBER_START = None
_CHAMBER_CUTOFF = None
chamber_time_raw = chamber_time_full[_CHAMBER_START:_CHAMBER_CUTOFF]
chamber_time_raw = chamber_time_raw - chamber_time_raw[0]
chamber_sp_raw = chamber_sp_full[_CHAMBER_START:_CHAMBER_CUTOFF]
chamber_pv_raw = chamber_pv_full[_CHAMBER_START:_CHAMBER_CUTOFF]


### Capacity data ###
_cap_seconds = np.array(
    [
        datetime.datetime.strptime(d[:-3], "%m/%d/%Y %H:%M:%S.%f")
        for d in cap_df["Seconds"]
    ]
)
_cap_seconds = _cap_seconds - _cap_seconds[0]
cap_time_full = np.array([delta.total_seconds() for delta in _cap_seconds])
cap_current_full = -cap_df["Current"].to_numpy()
cap_voltage_full = cap_df["Voltage"].to_numpy()
cap_soc_full = cap_df["SOC"].to_numpy()

_CAP_START = None
_CAP_CUTOFF = None
cap_time_raw = cap_time_full[_CAP_START:_CAP_CUTOFF]
cap_time_raw = cap_time_raw - cap_time_raw[0]
cap_voltage_raw = cap_voltage_full[_CAP_START:_CAP_CUTOFF]
cap_current_raw = cap_current_full[_CAP_START:_CAP_CUTOFF]
cap_soc_raw = cap_soc_full[_CAP_START:_CAP_CUTOFF]
cap_power_raw = cap_current_raw * cap_voltage_raw

# cell_internal_resistance = np.abs(cap_voltage_raw / cap_current_raw).min()
cell_internal_resistance = 0.1

_TO_UTC = "-04:00"
sensor_idx = 0 if _TEMP_START is None else _TEMP_START
chamber_idx = 0 if _CHAMBER_START is None else _CHAMBER_START
cap_idx = 0 if _CAP_START is None else _CAP_START
sensor_start_time = datetime.datetime.strptime(
    sensor_df["timestamp_utc"].iloc[sensor_idx], "%Y-%m-%d %H:%M:%S.%f%z"
)
chamber_start_time = datetime.datetime.strptime(
    chamber_df["DateTime"].iloc[chamber_idx] + _TO_UTC, "%m/%d/%Y %H:%M:%S%z"
)
cap_start_time = datetime.datetime.strptime(
    cap_df["Seconds"].iloc[cap_idx][:-3] + _TO_UTC, "%m/%d/%Y %H:%M:%S.%f%z"
)
sensor_start_time = sensor_start_time.astimezone()
chamber_start_time = chamber_start_time.astimezone()
cap_start_time = cap_start_time.astimezone()


### Interpolation and resampling ###
# Temperature - Low pass filtering
temp_time = np.linspace(temp_time_raw[0], temp_time_raw[-1], len(temp_time_raw))
# b, a = signal.butter(5, 0.1)
# _zi = signal.lfilter_zi(b, a)
# temp_sur_filtered = signal.lfilter(b, a, temp_sur_raw, zi=_zi * temp_sur_raw[0])[0]
# temp_air_filtered = signal.lfilter(b, a, temp_air_raw, zi=_zi * temp_air_raw[0])[0]
_WAVELET_NAME = "haar"
_WAVELET_MODE = "periodic"
w = pywt.Wavelet(_WAVELET_NAME)
coeff_sur = pywt.wavedec(temp_sur_raw, wavelet=w, mode=_WAVELET_MODE)
coeff_air = pywt.wavedec(temp_air_raw, wavelet=w, mode=_WAVELET_MODE)
WAVELET_CUTOFF = -5
for i, c in enumerate(coeff_sur[WAVELET_CUTOFF:]):
    c[:] = 0
for i, c in enumerate(coeff_air[WAVELET_CUTOFF:]):
    c[:] = 0
temp_sur_filtered = pywt.waverec(coeff_sur, _WAVELET_NAME, _WAVELET_MODE)
temp_air_filtered = pywt.waverec(coeff_air, _WAVELET_NAME, _WAVELET_MODE)
_sur_lerp = interpolate.interp1d(
    temp_time_raw, temp_sur_filtered, fill_value="extrapolate"
)
_air_lerp = interpolate.interp1d(
    temp_time_raw, temp_air_filtered, fill_value="extrapolate"
)
temp_sur = _sur_lerp(temp_time) + 273  # K
temp_air = _air_lerp(temp_time) + 273  # K

# Chamber - Interpolation
_pv_lerp = interpolate.interp1d(
    chamber_time_raw, chamber_pv_raw, fill_value="extrapolate"
)
_sp_lerp = interpolate.interp1d(
    chamber_time_raw, chamber_sp_raw, fill_value="extrapolate"
)
chamber_pv = _pv_lerp(temp_time) + 273  # K
chamber_sp = _sp_lerp(temp_time) + 273  # K
chamber_time = temp_time

# Capacity - Interpolation
_KIND = "previous"
_curr_lerp = interpolate.interp1d(
    cap_time_raw, cap_current_raw, kind=_KIND, fill_value="extrapolate"
)
_volt_lerp = interpolate.interp1d(
    cap_time_raw, cap_voltage_raw, kind=_KIND, fill_value="extrapolate"
)
_power_lerp = interpolate.interp1d(
    cap_time_raw, cap_power_raw, kind=_KIND, fill_value="extrapolate"
)
_soc_lerp = interpolate.interp1d(
    cap_time_raw, cap_soc_raw, kind=_KIND, fill_value="extrapolate"
)
cap_current = _curr_lerp(temp_time)
cap_voltage = _volt_lerp(temp_time)
cap_power = _power_lerp(temp_time)
cap_soc = _soc_lerp(temp_time)
cap_time = cap_time_raw
indices = []
# These are outliers that we will replace with other values
for i, (c, v, s, p) in enumerate(zip(cap_current, cap_voltage, cap_soc, cap_power)):
    indices.append(i)
    last_c = c
    last_v = v
    last_p = p
    last_s = s
    if c <= 0.2:
        break
cap_current[indices] = last_c
cap_voltage[indices] = last_v
cap_power[indices] = last_p
cap_soc[indices] = last_s
cap_cumsum = np.zeros(cap_current.shape)
for i in range(1, len(cap_current)):
    cap_cumsum[i] = (
        cap_cumsum[i - 1] - (temp_time[i] - temp_time[i - 1]) * cap_current[i]
    )
cell_capacity = np.abs(cap_cumsum).max()
# cell_initial_soc = cap_soc[0]
cell_initial_soc = 0.1


# Metrics of the data
temp_sensor_std = temp_sur[30000:].std()
temp_sensor_mean = temp_sur[30000:].mean()
temp_sensor_snr = (temp_sensor_mean / temp_sensor_std) ** 2
temp_sensor_snr = 10 * np.log10(temp_sensor_snr)
chamber_pv_std = chamber_pv[4000:].std()
chamber_pv_mean = chamber_pv[4000:].mean()
chamber_pv_snr = (chamber_pv_mean / chamber_pv_std) ** 2
chamber_pv_snr = 10 * np.log10(chamber_pv_snr)


def get_data(start=None, cutoff=None):
    t = temp_time[start:cutoff].astype(float)
    y = np.array([temp_sur, temp_air]).T[start:cutoff, :]
    u = np.array([chamber_pv, cap_power]).T[start:cutoff, :]
    # For initial temperature, we assume the experiment starts after cells have rested
    if start is None:
        start = 0
    x0 = np.array([chamber_pv[start], chamber_pv[start]])
    data = Data(t, y, u, x0)
    return data


def main():
    print(f"Calculated internal resistance is {cell_internal_resistance} Ohm")
    print(f"Calculated capacity is {cell_capacity} C ({cell_capacity / 3600} Ah)")
    print(f"Calculated initial SOC is {cell_initial_soc} ({100 * cell_initial_soc} %)")

    print("===============================")
    print("Start times report (Local time)")
    print("-------------------------------")
    print(f"Sensors  : {sensor_start_time}")
    print(f"Chamber  : {chamber_start_time}")
    print(f"Capacity : {cap_start_time}")
    print("===============================")

    print(f"Temperature sensor mean: {temp_sensor_mean}")
    print(f"Temperature sensor std: {temp_sensor_std}")
    print(f"Temperature sensor SNR: {temp_sensor_snr} dB")
    print(f"Chamber mean: {chamber_pv_mean}")
    print(f"Chamber std: {chamber_pv_std}")
    print(f"Chamber SNR: {chamber_pv_snr} dB")

    ### Temperature filtering plots ###
    fig, ax = plt.subplots(2, 1, sharex=True)
    for i, c in enumerate(coeff_sur):
        x = np.linspace(0, 1, len(c))
        ax[0].plot(x, c, label=i)
    for i, c in enumerate(coeff_air):
        x = np.linspace(0, 1, len(c))
        ax[1].plot(x, c, label=i)
    ax[0].legend()
    ax[1].legend()
    fig.suptitle(f"Cutoff: {WAVELET_CUTOFF}")

    ### Raw data plots ###
    fig, ax = plt.subplots(4, 1, sharex=True)

    ax[0].plot(temp_time_raw, temp_sur_raw, alpha=0.5, label="Surface")
    ax[0].plot(temp_time_raw, temp_air_raw, alpha=0.5, label="Air")
    ax[0].scatter(chamber_time_raw, chamber_pv_raw, 1, alpha=0.5, label="Ambient (PV)")
    ax[0].scatter(chamber_time_raw, chamber_sp_raw, 1, alpha=0.5, label="Ambient (SP)")
    ax[0].legend()

    ax[1].scatter(cap_time_raw, cap_voltage_raw, 1, alpha=0.5, label="Voltage")
    ax_t = ax[1].twinx()
    ax_t.scatter(
        cap_time_raw, cap_current_raw, 1, alpha=0.5, color="y", label="Current"
    )
    ax[1].legend()
    ax[1].set_ylabel("Voltage")
    ax_t.set_ylabel("Current")

    ax[2].scatter(cap_time_raw, cap_power_raw, 1, label="Power")
    ax[2].set_ylabel("Power")
    ax[2].legend()

    _resistance = cap_voltage_raw / cap_current_raw
    idx = np.where(_resistance >= -1000)
    ax[3].scatter(cap_time_raw[idx], _resistance[idx], 1, label="Resistance")
    ax[3].set_ylabel("Resistance")
    ax[3].legend()

    ### Plot interpolated and resampled data ###
    fig, ax = plt.subplots(5, 1, sharex=True)
    ax[0].plot(temp_time, temp_sur, alpha=0.5, label="Surface")
    ax[0].plot(temp_time, temp_air, alpha=0.5, label="Air")
    ax[0].plot(temp_time, chamber_pv, alpha=0.5, label="Ambient")
    ax[0].grid(alpha=0.25)
    ax[0].legend()

    ax[1].plot(temp_time, cap_current, alpha=1, label="Current")
    ax[1].grid(alpha=0.25)
    ax[1].legend()

    ax[2].plot(temp_time, cap_voltage, alpha=1, label="Voltage")
    ax[2].grid(alpha=0.25)
    ax[2].legend()

    ax[3].plot(temp_time, cap_soc, alpha=1, label="SoC")
    ax[3].grid(alpha=0.25)
    ax[3].legend()

    ax[4].plot(temp_time, cap_cumsum, alpha=1, label="Coulomb count")
    ax[4].grid(alpha=0.25)
    ax[4].legend()

    ### Plot filtered temperature data ###
    # fig, axs = plt.subplots(2, segments_total)
    # for ax, time, sur, amb in zip(axs[0, :].flatten(), cap_time_segs, temp_sur_segs, chamber_pv_segs):
    #     ax.plot(time, sur, label="Surface")
    #     # ax.plot(time, amb, label="Ambient")
    #     ax.grid(alpha=0.25)
    #     ax.legend()

    # for ax, time, current, power in zip(axs[1, :].flatten(), cap_time_segs, cap_current_segs, cap_power_segs):
    #     ax.plot(time, current, label="Current")
    #     ax.plot(time, power, label="Power")
    #     ax.grid(alpha=0.25)
    #     ax.legend()
    # fig.tight_layout()


def save_processed_data():
    print("Creating and storing large dataframe")
    unified_df = pd.DataFrame(
        {
            "time": temp_time,
            # Temperatures
            "sf_temp": temp_sur,
            "air_temp": temp_air,
            "amb_pv_temp": chamber_pv,
            "amb_sp_temp": chamber_sp,
            # Electrical
            "voltage": cap_voltage,
            "current": cap_current,
            "power": cap_power,
            "soc": cap_soc,
            "cumsum": cap_cumsum,
        }
    )
    unified_df.to_csv(os.path.join(DATA_PROC_DIR, "data.csv"), index=False)
    for c in unified_df.columns:
        unified_df[c].to_csv(os.path.join(DATA_PROC_DIR, f"data_{c}.csv"), index=False)


if __name__ == "__main__":
    save_processed_data()
    main()
    plt.show()
