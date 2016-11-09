#!/bin/bash


for letter in {G..S}; do
    rm samples/*
    bash resize.sh "$letter"
    bash gen_vec.sh "$letter"
done
