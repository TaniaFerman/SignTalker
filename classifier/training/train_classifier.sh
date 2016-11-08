#!/bin/bash

letter="$1"

#600~700
find ./train_dataset/negatives/ -iname "*.jpg" | head -100 > negatives.txt #717
#find ./train_dataset/[^$letter]/ -iname "*.jpg" > negatives.txt #106
#50 ~ 60
find ./train_dataset/$letter/ -iname "*.jpg" > positives.txt

echo "Positives=" $(cat positives.txt | wc -l)
echo "Negatives=" $(cat negatives.txt | wc -l)

img=$(cat positives.txt | sort | head -1)
fs=$(identify $img | awk '{print $3}')
w=$(echo $fs | cut -d'x' -f1)
h=$(echo $fs | cut -d'x' -f2)

echo "WxH=${w}x${h}"

#Sample 1500~1700
perl createsamples.pl positives.txt negatives.txt samples 275 "opencv_createsamples -bgcolor 0 -bgthresh 0 -maxxangle 1.1 -maxyangle 1.1 maxzangle 0.5 -maxidev 40 -w ${w:0:2} -h ${h:0:2}"

echo "Samples=" $(ls samples/ | wc -l)

python mergevec.py -v samples/ -o samples.vec

numNeg=$(cat negatives.txt | wc -l)

#85% of positive samples size
opencv_traincascade -data data/${letter} -vec samples.vec -bg negatives.txt -numStages 20 -minHitRate 0.999 -maxFalseAlarmRate 0.5 -numPos 210 -numNeg $numNeg -w ${w:0:2} -h ${h:0:2}

#beep
