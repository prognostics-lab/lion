#!/bin/bash


# Copy files to a temporary directory for modification
mkdir .temp
cp data/240624_temp_profile_C6B2/Data\ Files_06-25-2024-18-47-58/*.csv .temp/
echo "DateTime,CSZ System,TEMPERATURE PV,TEMPERATURE SP,TEMPERATURE %Out,PRODUCT PV,PRODUCT SP,PRODUCT %Out," > data/240624_temp_profile_C6B2/chamber.csv

