#!/bin/bash

echo Testimg Images

cd ../build3
#rm -rf *

cmake ..
make

echo finished building
echo

./multiDimen "testing" ../testImage.jpg

echo
echo Finished
