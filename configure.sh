#!/bin/bash

current_dir=`pwd`

# test if ubuntu had install freetype 
if [ ! -d "/usr/include/freetype2" ]; then
	echo "camera" | sudo add-apt-repository ppa:no1wantdthisname/ppa
	echo "camera" | sudo apt update
	echo "camera" | sudo apt install libfreetype6
fi 

# test if ubuntu had install libexif 
if [ ! -d "/usr/include/libexif" ]; then
	echo "camera" | sudo apt-get install libexif-dev
fi 

# test if ubuntu had install libjpeg-turbo 
if [ ! -d "/opt/libjpeg-turbo/include" ]; then
	echo "camera" | sudo apt-get install nasm
	cd $current_dir/tools
	uz $current_dir/tools/libjpeg-turbo-1.5.2.tar.gz 
	cd $current_dir/tools/libjpeg-turbo-1.5.2
	autoreconf -fiv
	./configure
	make
	echo "camera" | sudo make install
	cd $current_dir
fi 