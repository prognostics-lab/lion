#!/bin/bash

# Relevant constants
GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"
OUT_DIR="data"
ROUT_DIR="data_raw"

# Make directories
mkdir -p $ROUT_DIR
mkdir -p $ROUT_DIR/forklift
mkdir -p $OUT_DIR

URL="https://prod-dcd-datasets-cache-zipfiles.s3.eu-west-1.amazonaws.com/yz4pttm73n-2.zip"

echo -e "Downloading '${GREEN}forklift.zip${RESET}'"
wget2 -O $ROUT_DIR/forklift.zip $URL && {
    echo -e "Unpacking '${GREEN}forklift.zip${RESET}'"
    unzip $ROUT_DIR/forklift.zip -d $ROUT_DIR/forklift
    rm $ROUT_DIR/forklift.zip
}
