#!/bin/bash

# Define the source directory
source_dir="/home/student/Desktop/ELD/14/build/tmp/deploy/images/pynq"

# Define the source files
source_files=("uImage" "u-boot.img" "boot.bin" "core-image-minimal-pynq.cpio.gz.u-boot" "uEnv.txt" "devicetree.dtb")

# Define the destination directory
destination_dir="/home/student/Desktop/ELD/14/build/tmp/deploy/images/pynq/tmp"

# Loop through the source files and copy them to the destination directory
for file in "${source_files[@]}"; do
    if [ -f "$source_dir/$file" ]; then
        cp -L "$source_dir/$file" "$destination_dir/"
        echo "Copied $file to $destination_dir"
    else
        echo "File $file not found in the source directory: $source_dir"
    fi
done

