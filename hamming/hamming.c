/* hamming code generator/tester
    burin (c) 7-26-2020

    Written in anticipation that this question will be on Exam #2

    Algorithm and info take from:
        https://en.wikipedia.org/wiki/Hamming_code
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <assert.h>

//https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html (lame!)
#define xstr(s) str(s)
#define str(s) #s

#define PRINT_LABEL_HELPER(s, l)    printf("%-" xstr(l) "s", s)
#define PRINT_LABEL(s, l)  PRINT_LABEL_HELPER(s, l)

int m=0;
int word_length = 0;
int data_length = 0;
uint64_t memory = 0;

void write_data(uint64_t);
void print_binary(uint64_t data);
int get_check_bits();

int main(int argc, char* argv[]) {

struct timespec t;
clock_gettime(CLOCK_MONOTONIC, &t);
srand(t.tv_nsec);

if (argc !=2) {
    printf("usage: hamming <m bits>\n");
    exit(0);
}

m = atoi(argv[1]);

if (m < 1) {
    printf("There must be at least 1 parity bit\n");
    exit(-1);
}

word_length = pow(2, m) - 1;
data_length = pow(2, m) - m - 1;

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

//One based, because the documentation is 1 based

// Header
printf("\n");
PRINT_LABEL("Bit", 15); 
for (int b=1; b <= word_length; b++) {
    printf(" %2.d  ", b);
}
printf("\n");

PRINT_LABEL("", 15);
int d_num = 1;
for (int p=0; p< m; p++) {
    printf("[p%-2d]", (int)pow(2,p)); 
    for (int d = 1; d < pow(2, p); d++) {
        printf("[d%-2d]", d_num++); 
    }
}
printf("\n");


for (int p=0; p < m; p++) {
    PRINT_LABEL("", 12);
    printf("p%-2d", (int)pow(2,p));
    for (int b=1; b <= word_length; b++) {
        if (b & (1 << p)) {
            printf("[ X ]");
        } else {
            printf("[   ]");
        }
    }
    printf("\n");
}

printf("\n");
uint64_t test = 0xffffffff;
printf("----testing----%lu------\n", test);
memory = 0;
write_data(test);
printf("memory: data\n");
print_binary(memory);

for (int p=0; p < m; p++) {
    int xor = 0;
    int parity_count = 0;
    for (int b=0; b < word_length; b++) {

        if ((b+1) & (1 << p)) { //this bit is used in XOR
            xor ^= (memory & (1L << b)) ? 1 : 0;
            parity_count +=  (memory & (1L << b)) ? 1 : 0;
            //printf("i:%d (%lu)\n", b, memory & (1L << b));
        }
    }
    int p_pos = (int)pow(2,p);
    //printf("XOR of p%d is %d\n", p_pos, xor);
    //printf("parity count is %d\n", parity_count);
assert ( parity_count % 2 == xor);
    if (xor) {
        //printf("setting parity at pos %d to 1\n", p_pos);
        memory |= (1L << (p_pos -1)); //zero indexed
    }
}

printf("memory: data + parity\n");
print_binary(memory);

get_check_bits();

int r = rand() % word_length;
printf("flip %dth bit!\n", r +1);
memory ^= (1L << r);
print_binary(memory);
get_check_bits();





}

int get_check_bits() {
int total_c = 0;
for (int p=0; p < m; p++) {
    int xor = 0;
    int parity_count = 0;
    for (int b=0; b < word_length; b++) {

        if ((b+1) & (1 << p)) { //this bit is used in XOR
            xor ^= (memory & (1L << b)) ? 1 : 0;
            parity_count +=  (memory & (1L << b)) ? 1 : 0;
            //printf("i:%d (%lu)\n", b, memory & (1L << b));
        }
    }
    int p_pos = (int)pow(2,p);
    //printf("XOR of p%d is %d\n", p_pos, xor);
    //printf("parity count is %d\n", parity_count);
assert ( parity_count % 2 == xor);
    printf("c  %d is %d\n", p_pos, xor);
    if (xor) {
        total_c += p_pos;
    }
}
printf("total c  %d\n", total_c);
assert(total_c == 0);
return total_c;
}


void write_data(uint64_t data)  {
    
int word_pos = 1;
int bit_pos = 0;

for (int p=0; p< m; p++, word_pos++) {
    for (int d = 1; d < pow(2, p); d++, word_pos++) {

        if (data & (1<<bit_pos)) { //bit is 1
            memory |=  (uint64_t)(1L << word_pos);
        } //else bit is 0 

        //printf("writing word pos %d ,bit %d = %lu\n", word_pos, bit_pos,  memory & (uint64_t)(1L << word_pos));
        //print_binary( (uint64_t)(1 << word_pos));
        //print_binary(memory);

        bit_pos++;

        //printf("%d\n", word_pos);
    }
}

}

void print_binary(uint64_t memory) {
    PRINT_LABEL("", 15);
    for (int i=0; i < word_length; i++) {
        if (memory & (uint64_t)(1L << i)) {
            printf("[ 1 ]");
        } else {
            printf("[ 0 ]");
        }
    }

printf("\n");
}
