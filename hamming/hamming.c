/* hamming code generator/tester
    burin (c) 7-26-2020

    Written in anticipation that this question will be on Exam #2

    Algorithm and info take from:
        https://en.wikipedia.org/wiki/Hamming_code
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {

if (argc !=2) {
    printf("usage: hamming <m bits>\n");
    exit(0);
}

int m = atoi(argv[1]);

if (m < 1) {
    printf("There must be at least 1 parity bit\n");
    exit(-1);
}

int word_length = pow(2, m) - 1;
int data_length = pow(2, m) - m - 1;

int integer_max =  sizeof(size_t) * 8;
if (word_length > integer_max) {
    printf("This system only supports %d bit words.\nTotal hamming code for %d bits would need %d bits\n", integer_max, m, word_length);
    exit(-1);
}

int distance = word_length - data_length + 1;
float rate = (float)data_length / word_length;
printf("\n[Hamming code (%d,%d) rate:%f]\n\n", word_length, data_length, rate);
printf("  [distance %d]\n\n", distance);

printf("  If this was a repition code:\n");
printf("   can correct up to %d bit errors\n", distance - 2);
printf("   can detect  up to %d bit errors\n", distance - 1);
printf("\n");

printf("  Hamming code\n"); 
printf("   can correct up to 1 bit errors\n");
printf("   can detect  up to 2 bit errors\n");
printf("\n");

printf("syndrome bits: %.3d\n", m); 
printf("data bits    : %.3d\n", data_length); 
printf("------------------\n");
printf("word bits    : %.3d\n", word_length);



}
