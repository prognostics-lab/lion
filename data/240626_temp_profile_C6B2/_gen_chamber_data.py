#!/bin/env python

import os
import sys
import shutil


OUT_DIR = os.path.join("data", "240626_temp_profile_C6B2")
DATA_DIR = os.path.join(OUT_DIR, "Data Files_06-27-2024-15-18-12")
TEMP_DIR = os.path.join(".temp")


print("Creating temporary directory")
os.makedirs(TEMP_DIR, exist_ok=True)
files = os.listdir(DATA_DIR)
print(f"Files to append: {files}")
header_exists = False
with open(os.path.join(TEMP_DIR, "chamber.csv"), "w") as dst:
    for f in files:
        print(f"Parsing file {f}")
        with open(os.path.join(DATA_DIR, f), "r") as src:
            l = src.readline()
            if not header_exists:
                # Create file header
                l_split = l.split(",")
                header_exists = True
                header = []
                header.append("".join(l_split[:2]))
                header.extend(l_split[4:])
                header = ",".join(header)
                dst.writelines([header])

            lines = src.readlines()
            for l in lines:
                l_split = l.split(",")
                new_l = []
                new_l.append(" ".join(l_split[:2]))
                new_l.extend(l_split[4:])
                new_l = ",".join(new_l)
                dst.write(new_l)

shutil.copyfile(os.path.join(TEMP_DIR, "chamber.csv"), os.path.join(OUT_DIR, "chamber.csv"))
print("Removing temporary directory")
shutil.rmtree(TEMP_DIR)

