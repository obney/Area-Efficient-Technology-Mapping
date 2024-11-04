#!/bin/sh

output_dir="./output"
program="./ori"

for input_file in "spla.blif" "alu4.blif" "apex4.blif" "cordic.blif"; do
    for i in 3 4 5 6 7 8; do
        output_file="${output_dir}/$(basename "$input_file" .blif)-${i}_ori.txt"
        $program "./input/$input_file" "$output_file" $i
    done
done

