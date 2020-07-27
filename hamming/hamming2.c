#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#define M   4
#define PARITY_WORD_BITS    1
#define PARITY_WORD_BIT     ((uint32_t)(1<<31))

#define WRITE_F     1
#define READ_F      0

int word_bits = 0;
int data_bits = 0;

int write_w_hamming(uint8_t value, uint32_t *memory);
int read_w_hamming(uint8_t *value, uint32_t *memory);
int hamming_data(uint8_t value, uint32_t *memory, int write);
int apply_parity(uint32_t *memory, int write);
int word_parity(uint32_t *memory, int write);
int get_parity(uint32_t *memory);


void print_binary(char* s, uint32_t *memory);

int main() {

word_bits = pow(2, M) - 1;
data_bits = pow(2, M) - M - 1;



printf("Hamming code (%d/%d) rate = %f\n", word_bits, data_bits, (float)data_bits/word_bits);
printf("with secded - rate = %f\n", (float)data_bits/(word_bits + PARITY_WORD_BITS));


assert(word_bits + PARITY_WORD_BITS < sizeof(uint32_t) * 8);

//10 fake memory slots
uint32_t *memory = (uint32_t*)malloc(sizeof(uint32_t) *10);
uint8_t result;
int r = 0;

//Test 1
printf("\nTest1-------------------\n");
write_w_hamming(0xff, &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}
printf("word parity = %d\n", word_parity(&memory[1], READ_F));

result = 0;
read_w_hamming(&result, &memory[1]);
printf("Read back %u\n", result);

//Test 2 - single bit error
printf("\nTest2-------------------\n");
write_w_hamming(0xff, &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}

r = 6;
printf("flip %dth bit!\n", r + 1);
memory[1] ^= (1L << r);
print_binary("flip", &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}

result = 0;
read_w_hamming(&result, &memory[1]);
printf("Read back %u\n", result);

//Test 3 - double bit error
printf("\nTest3-------------------\n");
write_w_hamming(0xff, &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}

r = 5;
printf("flip %dth bit!\n", r + 1);
memory[1] ^= (1L << r);
print_binary("flip", &memory[1]);

r = 9;
printf("flip %dth bit!\n", r + 1);
memory[1] ^= (1L << r);
print_binary("flip", &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}

result = 0;
read_w_hamming(&result, &memory[1]);
printf("Read back %u\n", result);

//Test 4 - corrupt word parity bit 
printf("\nTest4-------------------\n");
write_w_hamming(0xff, &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}

r = 31;
printf("flip %dth bit!\n", r + 1);
memory[1] ^= (1L << r);
print_binary("flip", &memory[1]);

if (apply_parity(memory, READ_F) > 0) {
    printf("check bits failed!\n");
}

result = 0;
read_w_hamming(&result, &memory[1]);
printf("Read back %u\n", result);


}

int write_w_hamming(uint8_t value, uint32_t *memory) {
    hamming_data(value, memory, WRITE_F);
    print_binary("written", memory);
    return apply_parity(memory, WRITE_F);
}

int read_w_hamming(uint8_t *value, uint32_t *memory) {


    int c = apply_parity(memory, READ_F);

    if (c == 0) {
        if (word_parity(memory, READ_F) != get_parity(memory)) {
            printf("word parity bit error, correcting\n");
            word_parity(memory, WRITE_F);
        }
        *value = hamming_data(0, memory, READ_F);
        return 1; 
    } else {
        printf("parity error! c=%d\n", c);
        if (word_parity(memory, READ_F) == get_parity(memory)) {
            printf("Double bit error!!\n");
            return -1;
        }
        uint32_t mask = (uint32_t)(1 << (c - 1));
        print_binary("mask", &mask);
        *memory ^= mask; 
        print_binary("corrected", memory);
        *value = hamming_data(0, memory, READ_F); 
        return 1;
    }
        
return -1;
}

int hamming_data(uint8_t value, uint32_t *memory, int write) {

    int word_pos = 1;
    int value_pos = 0;

    uint8_t return_value = 0;

    for (int p=0; p < M; p++) {
        int current_p = pow(2, p);

        //There are p-1 data bits in each set
        for (int i = 0; i < (current_p - 1); i++) {

            if (write) {
                //If the bit it set in value, set the appropriate memory bit
                if (value & (1 << value_pos)) { // bit is 1
                        *memory |=  (uint32_t)(1 << word_pos);
                } //else bit is 0 
            } else { //READ
               //printf("read bit %d\n", *memory & (uint32_t)(1 << word_pos));
               if (*memory & (uint32_t)(1 << word_pos)) {
                    return_value |=  (uint8_t)(1 << value_pos);
                } //else bit is 0
            }

            word_pos++;
            value_pos++;
        }
        word_pos++;

    }

return return_value; 
}

int apply_parity(uint32_t *memory, int write) {

    int total_c = 0;

    for (int p=0; p < M; p++) {

        int xor = 0;
        int parity_count = 0;
        int bit;

        for (int word_pos=0; word_pos < word_bits; word_pos++) {

            if ( (word_pos + 1) & (1 << p)) { //this bit is used in XOR
                bit = (*memory & (1 << word_pos)) ? 1 : 0; 
                xor ^= bit; 
                parity_count += bit; 
                //printf("index:%d (%u)\n", word_pos, bit);
            }
        }

        assert ( parity_count % 2 == xor);

        int p_pos = (int)pow(2,p);

        if (write == 1) { //write
            //printf("XOR of p%d is %d\n", p_pos, xor);
            //printf("parity count is %d\n", parity_count);
            if (xor) {
                //printf("setting parity at pos %d to 1\n", p_pos);
                *memory |= (1 << (p_pos -1)); //zero indexed
            } else {
                //printf("setting parity at pos %d to 0\n", p_pos);
            }
        } else { //verify
            //printf("c %d is %d\n", p_pos, xor);
            if (xor) {
                total_c += p_pos;
            }
        } 
    }

    if (write) {
        word_parity(memory, WRITE_F);
        print_binary("add parity", memory);
    }

return total_c;
}

int word_parity(uint32_t *memory, int write) {

    int xor = 0;
    int bit = 0;
    int parity_count = 0;

    for (int word_pos=0; word_pos < word_bits; word_pos++) {
            bit = (*memory & (1 << word_pos)) ? 1 : 0; 
            xor ^= bit; 
            parity_count += bit; 
    }

    if (write) {
        printf("write word_parity = %d\n", parity_count);
        if (xor) {
            *memory |= PARITY_WORD_BIT; 
        }
    } else {
        printf("read word_parity = %d\n", parity_count);
    }
            
return xor; 
}

int get_parity(uint32_t *memory) {
    return (*memory & PARITY_WORD_BIT) ? 1 : 0;
}



void print_binary(char* s, uint32_t *memory) {
    printf("%-15s", s);
    for (int i=0; i < word_bits; i++) {
        if (*memory & (uint64_t)(1L << i)) {
            printf("[ 1 ]");
        } else {
            printf("[ 0 ]");
        }
    }

printf("\n");
}
