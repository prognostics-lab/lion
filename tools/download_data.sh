#!/bin/bash

# Relevant constants
GREEN="\033[0;32m"
RESET="\033[0m"
OUT_DIR="data"
ROUT_DIR="data_raw"

# Make directories
mkdir $ROUT_DIR
mkdir $ROUT_DIR/icl
mkdir $OUT_DIR

# Define URLs
EXP1_URL="https://zenodo.org/records/10637534/files/Expt%201%20-%20Si-based%20Degradation.zip?download=1"
EXP2_URL="https://zenodo.org/records/10637534/files/Expt%202,2%20-%20C-based%20Degradation%202.zip?download=1"
EXP3_URL="https://zenodo.org/records/10637534/files/Expt%203%20-%20Cathode%20Degradation%20and%20Li-Plating.zip?download=1"
EXP4_URL="https://zenodo.org/records/10637534/files/Expt%204%20-%20Drive%20Cycle%20Aging%20(Control).zip?download=1"
EXP5_URL="https://zenodo.org/records/10637534/files/Expt%205%20-%20Standard%20Cycle%20Aging%20(Control).zip?download=1"

# Downloads
download1() {
    echo -e "Downloading '${GREEN}exp1.zip${RESET}'"
    wget -O $ROUT_DIR/exp1.zip $EXP1_URL
    unzip $ROUT_DIR/exp1.zip -d $ROUT_DIR/icl/exp1
    rm $ROUT_DIR/exp1.zip
}

download2() {
    echo -e "Downloading '${GREEN}exp2.zip${RESET}'"
    wget -O $ROUT_DIR/exp2.zip $EXP2_URL
    unzip $ROUT_DIR/exp2.zip -d $ROUT_DIR/icl/exp2
    rm $ROUT_DIR/exp2.zip
}

download3() {
    echo -e "Downloading '${GREEN}exp3.zip${RESET}'"
    wget -O $ROUT_DIR/exp3.zip $EXP3_URL
    unzip $ROUT_DIR/exp3.zip -d $ROUT_DIR/icl/exp3
    rm $ROUT_DIR/exp3.zip
}

download4() {
    echo -e "Downloading '${GREEN}exp4.zip${RESET}'"
    wget -O $ROUT_DIR/exp4.zip $EXP4_URL
    unzip $ROUT_DIR/exp4.zip -d $ROUT_DIR/icl/exp4
    rm $ROUT_DIR/exp4.zip
}

download5() {
    echo -e "Downloading '${GREEN}exp5.zip${RESET}'"
    wget -O $ROUT_DIR/exp5.zip $EXP5_URL
    unzip $ROUT_DIR/exp5.zip -d $ROUT_DIR/icl/exp5
    rm $ROUT_DIR/exp5.zip
}
