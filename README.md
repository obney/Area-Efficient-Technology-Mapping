## Project Description

This is a programming assignment from National Tsinghua University, course CS 516000: FPGA Architecture & CAD. The task is to implement technology mapping for a two-bounded circuit with the objective of minimizing the number of LUTs used. Detailed problem specifications can be found in CS516000_2024_Fall_Programming_Assignment.pdf. The implementation is provided in optmap.cpp.

## Experiment result

Comparison of number of K-LUTs for different circuits.

| Circuit |origin| K = 3 | K = 4 | K = 5 | K = 6 | K = 7 | K = 8 |
|---------|------|-------|-------|-------|-------|-------|-------|
| **spla**  |7454| 4425  | 3152  | 2539  | 2084  | 1743  | 1398  |
| **alu4**  |2746| 1644  | 1186  | 969   | 810   | 687   | 575   |
| **apex4** |2204| 1405  | 1028  | 863   | 750   | 667   | 597   |
| **cordic**|877 | 492   | 364   | 274   | 235   | 213   | 189   |

## Project Instructions

### 1. Build the Project
To compile the code, use the `Makefile` provided:

```bash
make
```
This will generate the executable necessary to run the program.

### 2. Run the Program

To execute the program with the provided test cases, use the following command:
```bash
sh run.sh
```

This script runs the code on all included test cases.

### 3. Verify Results

After running the program, use the following command to verify the correctness of the output:

```bash
sh test.sh
```

This will check the accuracy of output files.

### Summary
1. **Build**: `make`
2. **Run**: `sh run.sh`
3. **Test**: `sh test.sh`

