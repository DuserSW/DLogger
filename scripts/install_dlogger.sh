#!/bin/bash

#Author: Kamil Kielbasa
#Email: kamilkielbasa64@gmail.com
#License: GPL3


# Get an absolute path to this script.
script=$(readlink -f $0)
# Get an absolute path this script is in.
script_path=`dirname $script`

# Path to installation of library.
lib_path=

# Check if user specify path for library.
if [ ! -z "$1" ]; then
    lib_path=$1
else
    # default directory for library.
    lib_path=~/dlogger
fi


# Go to script path, clean and compile library.
cd $script_path
cd ..
make clean
make lib

# Installation of library.
echo "Installing of DLogger to $lib_path"
mkdir -p $lib_path
cp ./libdlogger.a $lib_path/
cp -R ./inc/ $lib_path

echo "Installation is successfully completed. Exiting!"