#!/bin/bash
gcc -Wall -Werror test.c
./a.out < input.txt > output.txt
diff correct_output.txt output.txt
