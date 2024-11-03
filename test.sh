#!/bin/sh

input_files="spla.blif alu4.blif apex4.blif cordic.blif"
output_dir="./output1101"
program="./JUSTIFY"

for input_file in $input_files; do
    base_name=$(basename "$input_file" .blif)
    for i in 3 4 5 6 7 8; do
        output_file="${output_dir}/${base_name}-${i}_ori.txt"
        
        # Print which file is currently being processed
        echo "Processing $input_file with JUSTIFY for value $i..."
        
        $program "./input/$input_file" "$output_file" $i
    done
done
