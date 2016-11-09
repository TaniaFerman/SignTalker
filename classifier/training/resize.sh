#!/bin/bash

letter="$1"

fs=$(identify train_dataset/$letter/${letter}1.jpg | awk '{print $3}')
w=$(echo $fs | cut -d'x' -f1)
h=$(echo $fs | cut -d'x' -f2)

for i in train_dataset/$letter/*; do 
    convert $i -resize ${w}x${h}! $i
done
