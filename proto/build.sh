#!/bin/bash

SRC_DIR=.
DST_DIR=.

#build protobuf

protoc -I=${SRC_DIR} --cpp_out=$DST_DIR $SRC_DIR/*.proto

mv ./*.h ../include

for name in `ls $DST_DIR/*.cc`
do
	mv $name ${name/[.]cc/.cpp}
done

mv ./*.cpp ../src
