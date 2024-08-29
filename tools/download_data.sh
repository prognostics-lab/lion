#!/bin/bash

# Relevant constants
GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"
OUT_DIR="data"
ROUT_DIR="data_raw"

# Make directories
mkdir -p $ROUT_DIR
mkdir -p $ROUT_DIR/icl
mkdir -p $OUT_DIR

# Define URLs
EXP1_URL="https://zenodo.org/records/10637534/files/Expt%201%20-%20Si-based%20Degradation.zip?download=1"
EXP2_URL="https://zenodo.org/records/10637534/files/Expt%202,2%20-%20C-based%20Degradation%202.zip?download=1"
EXP3_URL="https://zenodo.org/records/10637534/files/Expt%203%20-%20Cathode%20Degradation%20and%20Li-Plating.zip?download=1"
EXP4_URL="https://zenodo.org/records/10637534/files/Expt%204%20-%20Drive%20Cycle%20Aging%20(Control).zip?download=1"
EXP5_URL="https://zenodo.org/records/10637534/files/Expt%205%20-%20Standard%20Cycle%20Aging%20(Control).zip?download=1"

# Downloads
download1() {
    echo -e "Downloading '${GREEN}exp1.zip${RESET}'"
    wget2 -O $ROUT_DIR/exp1.zip $EXP1_URL && {
        echo -e "Unpacking '${GREEN}exp1.zip${RESET}'"
        unzip $ROUT_DIR/exp1.zip -d $ROUT_DIR/icl/exp1
        rm $ROUT_DIR/exp1.zip
    }
}

download2() {
    echo -e "Downloading '${GREEN}exp2.zip${RESET}'"
    wget2 -O $ROUT_DIR/exp2.zip $EXP2_URL && {
        echo -e "Unpacking '${GREEN}exp2.zip${RESET}'"
        unzip $ROUT_DIR/exp2.zip -d $ROUT_DIR/icl/exp2
        rm $ROUT_DIR/exp2.zip
    }
}

download3() {
    echo -e "Downloading '${GREEN}exp3.zip${RESET}'"
    wget2 -O $ROUT_DIR/exp3.zip $EXP3_URL && {
        echo -e "Unpacking '${GREEN}exp3.zip${RESET}'"
        unzip $ROUT_DIR/exp3.zip -d $ROUT_DIR/icl/exp3
        rm $ROUT_DIR/exp3.zip
    }
}

download4() {
    echo -e "Downloading '${GREEN}exp4.zip${RESET}'"
    wget2 -O $ROUT_DIR/exp4.zip $EXP4_URL && {
        echo -e "Unpacking '${GREEN}exp4.zip${RESET}'"
        unzip $ROUT_DIR/exp4.zip -d $ROUT_DIR/icl/exp4
        rm $ROUT_DIR/exp4.zip
    }
}

download5() {
    echo -e "Downloading '${GREEN}exp5.zip${RESET}'"
    wget2 -O $ROUT_DIR/exp5.zip $EXP5_URL && {
        echo -e "Unpacking '${GREEN}exp5.zip${RESET}'"
        unzip $ROUT_DIR/exp5.zip -d $ROUT_DIR/icl/exp5
        rm $ROUT_DIR/exp5.zip
    }
}


# Parse the input to check the datasets to download
if [[ $1 == "" ]]; then
    echo -e "${RED}!!! DOWNLOADING ALL DATASETS !!!${RESET}"
    read -p "Continue? (y/n): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || { echo -e "${RED}Cancelling download${RESET}"; exit 1; }
    for (( i=1; i<=5; i++ )); do
        download$i
    done
else
    while IFS="," read -ra substrings; do
        for group in ${substrings[@]}; do
            # Parse groups
            if [[ $group == *"-"* ]]; then
                a=$(echo $group | cut -d'-' -f1)
                b=$(echo $group | cut -d'-' -f2)

                # Ensure a and b are numeric
                if ! [[ $a =~ ^[0-9]+$ ]] || ! [[ $b =~ ^[0-9]+$ ]]; then
                    echo -e "${RED}Error${RESET}: 'a' and 'b' must be numbers."
                    exit 1
                fi

                # Iterate from a to b
                for (( i=a; i<=b; i++ )); do
                    download$i
                done
            else
                download$group
            fi
        done
    done <<< "$1"
fi
