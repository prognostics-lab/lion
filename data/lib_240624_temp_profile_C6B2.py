import os
import datetime

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# matplotlib configurations
SAVE_FMT = "pdf"
SAVEFIG_PARAMS = {"dpi": 1000, "bbox_inches": "tight"}
plt.style.use("tableau-colorblind10")


# Relevant directories
DATA_DIR = os.path.join("data", "240624_temp_profile_C6B2")
TEMP_IMG_DIR = os.path.join("img_raw")
IMG_DIR = os.path.join("img_raw")

DATA_CHAMBER_FILENAME = os.path.join(DATA_DIR, "chamber.csv")
DATA_SENSOR_FILENAME = os.path.join(DATA_DIR, "sensor_temp.csv")
DATA_CAPACITY_FILENAME = os.path.join(DATA_DIR, "TestData.csv")
DATA_PROCEDURE_FILENAME = os.path.join(DATA_DIR, "TestProcedure.csv")


# Load data
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

# Temperature sensor data
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


# Chamber data
_chamber_seconds = np.array([datetime.datetime.strptime(d, "%m/%d/%Y %H:%M:%S") for d in chamber_df["DateTime"]])
_chamber_seconds = _chamber_seconds - _chamber_seconds[0]
chamber_time_full = np.array([delta.total_seconds() for delta in _chamber_seconds])
chamber_sp_full = chamber_df["TEMPERATURE SP"].to_numpy()
chamber_pv_full = chamber_df["TEMPERATURE PV"].to_numpy()

_CHAMBER_START = 350
_CHAMBER_CUTOFF = None
chamber_time = chamber_time_full[_CHAMBER_START:_CHAMBER_CUTOFF]
chamber_time = chamber_time - chamber_time[0]
chamber_sp = chamber_sp_full[_CHAMBER_START:_CHAMBER_CUTOFF]
chamber_pv = chamber_pv_full[_CHAMBER_START:_CHAMBER_CUTOFF]


# Capacity data
_cap_seconds = np.array([datetime.datetime.strptime(
    d[:-3], "%m/%d/%Y %H:%M:%S.%f") for d in cap_df["Seconds"]])
_cap_seconds = _cap_seconds - _cap_seconds[0]
cap_time_full = np.array([delta.total_seconds() for delta in _cap_seconds])
cap_current_full = cap_df["Current"].to_numpy()
cap_voltage_full = cap_df["Voltage"].to_numpy()
cap_soc_full = cap_df["SOC"].to_numpy()

_CAP_START = None
_CAP_CUTOFF = None
cap_time = cap_time_full[_CAP_START:_CAP_CUTOFF]
cap_time = cap_time - cap_time[0]
cap_voltage = cap_voltage_full[_CAP_START:_CAP_CUTOFF]
cap_current = cap_current_full[_CAP_START:_CAP_CUTOFF]
cap_soc = cap_soc_full[_CAP_START:_CAP_CUTOFF]
cap_power = cap_current * cap_voltage


# Segmentation of experiments
_TIME_LIMIT = 50
starts_idx = np.concatenate(
    ([0], np.where((cap_time[1:] - cap_time[:-1]) >= _TIME_LIMIT)[0] + 1))
ends_idx = np.concatenate(
    (np.where((cap_time[1:] - cap_time[:-1]) >= _TIME_LIMIT)[0], [-1]))
starts = cap_time[starts_idx]
ends = cap_time[ends_idx]

temp_masks = [(s <= temp_time) & (temp_time <= e)
              for s, e in zip(starts, ends)]
chamber_masks = [(s <= chamber_time) & (chamber_time <= e)
                 for s, e in zip(starts, ends)]
cap_masks = [(s <= cap_time) & (cap_time <= e) for s, e in zip(starts, ends)]

cap_time_segs = [cap_time[m] for m in cap_masks]
cap_current_segs = [cap_current[m] for m in cap_masks]
cap_voltage_segs = [cap_voltage[m] for m in cap_masks]
cap_power_segs = [cap_power[m] for m in cap_masks]
cap_soc_segs = [cap_soc[m] for m in cap_masks]


def main():
    fig, ax = plt.subplots(2, 1)

    ax[0].scatter(temp_time, temp_sur, 1, alpha=0.5, label="Surface")
    ax[0].scatter(temp_time, temp_air, 1, alpha=0.5, label="Air")
    ax[0].scatter(chamber_time, chamber_pv, 1, alpha=0.5, label="Ambient")
    ax[0].legend()

    # idx = 5
    # t = cap_time_segs[idx]
    # t = t - t[0]
    # ax[1].scatter(t, np.ones(t.shape), 1)
    ax[1].scatter(cap_time, cap_voltage, 1, alpha=0.5, label="Voltage")
    ax[1].legend()


if __name__ == "__main__":
    main()
    plt.show()
