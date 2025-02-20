#!/bin/bash
set -e


#USEAGE INSTRUCTIONS:
# PARAMETER 1
#  1= GENERATE NEW RANDOMISED TRAINING AND TEST IMAGES

## if a flag is added(1) then generate new set of random images
if [ $1 == "1" ]
then
  echo "Generating new images."
  useImgs="Y"
else 
  useImgs="N"
fi

## cd to build dir and make
cd ../build/
cmake ..
make
echo made and built

## Set number of repeats (potentially with new images)
Repeats=2
imgCounter=0

# Only run tests once if static images used
if [ "$useImgs" == "N" ]
then
Repeats=1
fi

echo this is the number of repeats $Repeats

while [ $imgCounter -lt $Repeats ]
do

  # Generate new images if flag input
  if [ "$useImgs" == "Y" ]
  then
	~/Desktop/TEST_IMAGES/CapturedImgs/./ranImgs.sh
	echo images randomised and allocated
  fi
		
	## VARIABLE TYPES
	inputType="scale"
	## SCALE VALUES
	Scale=(9 8 7 6 5 4 3 2 1 0)

	firstGo=1
	counter=0
	for i in ${Scale[@]}; do

		# If not first iteration then set flag as 0
		if [ $counter -gt 0 ]
		then
		  echo not first iteration
		  firstGo=0
		fi

		echo starting program
	
		#Note 1.inputValue  2.Input Type 3.firstLineFlag
		./multiDimen ${i} $inputType $firstGo
		echo Test Type is: $inputType
		echo iteration $counter
		echo
		let counter=counter+1
	done
	let imgCounter=imgCounter+1

done
echo Done
