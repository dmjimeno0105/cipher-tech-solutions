#include <stdint.h>
#include <stdlib.h>
#include "lfsr.h"

unsigned char *Crypt(unsigned char *data, int datalength, unsigned int initialValue)
{
    unsigned int state = initialValue;
    unsigned int feedback = 0x87654321;
    const int STEPS = 8;
    for (int i = 0; i < datalength; i++)
    {
        for (int j = 0; j < STEPS; j++)
        {
            if (state & 1)
            {
                state = state >> 1;
                state = state ^ feedback;
            }
            else
            {
                state = state >> 1;
            }
        }
        data[i] = data[i] ^ state;
    }
    return data;
}