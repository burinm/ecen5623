#include <assert.h>
#include "ecclib.h"

void flip_bit(ecc_t *ecc, unsigned char *address, unsigned short bit_to_flip);

int iterate_data_count=0;
int iterate_data_bits();
int bit = 0;

int total_tests_count = 0;

int main(void)
{
    ecc_t ECC;
    unsigned int offset=0; int rc; unsigned char byteToRead;
    unsigned short bitToFlip;
    unsigned char *base_addr=enable_ecc_memory(&ECC);

    // NEGATIVE testing - flip a SINGLE bit, read to correct
    traceOn();
 
    // TEST CASE 1: if Check-Bits == 0 AND pW == pW2  => NO ERRORS 
    printf("**** TEST CASE 1: Check-Bits == 0 AND pW == pW2 (All good, NO ERRORS) ******\n");
    write_byte(&ECC, base_addr+0, (unsigned char)0xFF);
    assert((rc=read_byte(&ECC, base_addr+offset, &byteToRead)) == NO_ERROR);
    printf("**** END TEST CASE 1 *****************************\n\n");

    // TEST CASE 2: If Check-Bits == 0 AND pW != pW2  => pW ERROR 
    printf("**** TEST CASE 2: pW Error - Check-Bits == 0 AND pW != pW2  => pW****\n");
    write_byte(&ECC, base_addr+0, (unsigned char)0x5A);
    bitToFlip=0; //This is the parity word bit
    flip_bit(&ECC, base_addr+0, bitToFlip);
    assert((rc=read_byte(&ECC, base_addr+offset, &byteToRead)) == PW_ERROR);

    total_tests_count++;
    printf("**** END TEST CASE 2 *****************************\n\n");

    for (int i=1; i <= 12; i++) { //Check all single bit errors (skip pW)
            for (int j=1; j <= 12; j++) { //Check all single bit errors (skip pW)
                // TEST CASE 3: If Check-Bits != 0 AND pW == pW2 => DBE 
                printf("**** TEST CASE 3.%d.%d: DBE - Check-Bits != 0 AND pW == pW2 (flip two bits %d,%d)*******\n", i, j);
                write_byte(&ECC, base_addr+0, (unsigned char)0xCC);

                if (i == j) { continue; } //Flip two unique bits
                bitToFlip=i;
                flip_bit(&ECC, base_addr+0, bitToFlip);

                bitToFlip=j;
                flip_bit(&ECC, base_addr+0, bitToFlip);

                assert((rc=read_byte(&ECC, base_addr+offset, &byteToRead)) == DOUBLE_BIT_ERROR);
                printf("**** END TEST CASE 3.%d.%d *****************************\n\n", i, j);
                total_tests_count++;
        }
    }

    for (int i=1; i <= 12; i++) { //Check all single bit errors (skip pW)
        // TEST CASE 4: If Check-Bits != 0 AND pW != pW2 => SBE 
        printf("**** TEST CASE 4.%d: SBE - If Check-Bits != 0 AND pW != pW2 (flip bit %d)******\n", i, i);

        //iterate_data_count=0;
        //while (1) {
       //     bit = iterate_data_bits();
       //     if (bit == 255) {
       //         break;
       //     }

            write_byte(&ECC, base_addr+0, (unsigned char)0xAB);
            bitToFlip=i;
            flip_bit(&ECC, base_addr+0, bitToFlip);
            assert((rc=read_byte(&ECC, base_addr+offset, &byteToRead)) == bitToFlip);

            printf("**** END TEST CASE 4.%d *****************************\n\n", i);
            total_tests_count++;
    }
    traceOff();


    printf("\nTotal tests run = %d\n", total_tests_count);


    return NO_ERROR;
}


// flip bit in encoded word: pW p1 p2 d1 p3 d2 d3 d4 p4 d5 d6 d7 d8
// bit position:             00 01 02 03 04 05 06 07 08 09 10 11 12
void flip_bit(ecc_t *ecc, unsigned char *address, unsigned short bit_to_flip) {
    unsigned int offset = address - ecc->data_memory;
    unsigned char byte=0;
    unsigned short data_bit_to_flip=0, parity_bit_to_flip=0;
    int data_flip=1;

    switch(bit_to_flip)
    {
        // parity bit pW, p01 ... p04
        case 0: 
            parity_bit_to_flip = 4;
            data_flip=0;
            break;
        case 1: 
            parity_bit_to_flip = 0;
            data_flip=0;
            break;
        case 2:
            parity_bit_to_flip = 1;
            data_flip=0;
            break;
        case 4:
            data_flip=0;
            parity_bit_to_flip = 2;
            break;
        case 8:
            data_flip=0;
            parity_bit_to_flip = 3;
            break;

        // data bit d01 ... d08
        case 3: 
            data_bit_to_flip = 0;
            break;
        case 5: 
            data_bit_to_flip = 1;
            break;
        case 6: 
            data_bit_to_flip = 2;
            break;
        case 7: 
            data_bit_to_flip = 3;
            break;
        case 9: 
            data_bit_to_flip = 4;
            break;
        case 10: 
            data_bit_to_flip = 5;
            break;
        case 11: 
            data_bit_to_flip = 6;
            break;
        case 12: 
            data_bit_to_flip = 7; 
            break;


        default:
            printf("flipped bit OUT OF RANGE\n");
            return;
    }

    if(data_flip)
    {
        printf("DATA  : request=%hu\n", bit_to_flip);
        printf("DATA  : bit to flip=%hu\n", data_bit_to_flip);

        byte = ecc->data_memory[offset];
        printf("DATA  : original byte    = 0x%02X\n", byte);
        byte ^= (1 << (data_bit_to_flip));
        printf("DATA  : flipped bit byte = 0x%02X\n\n", byte);
        ecc->data_memory[offset] = byte;

    }
    else
    {
        printf("PARITY: request=%hu\n", bit_to_flip);
        printf("PARITY: bit to flip=%hu\n", parity_bit_to_flip);

        byte = ecc->code_memory[offset];
        printf("PARITY: original byte    = 0x%02X\n", byte);
        byte ^= (1 << (parity_bit_to_flip));
        printf("PARITY: flipped bit byte = 0x%02X\n\n", byte);
        ecc->code_memory[offset] = byte;
    }

}

int iterate_data_bits() {

    switch(iterate_data_count++) {
        case 0:
            return 3;
            break;

        case 1:
            return 5;
            break;

        case 2:
            return 6;
            break;

        case 3:
            return 7;
            break;

        case 4:
            return 9;
            break;

        case 5:
            return 10;
            break;

        case 6:
            return 11;
            break;

        case 7:
            return 12;
            break;

        default:
            return 255;
    };
}
