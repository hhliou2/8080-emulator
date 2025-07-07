#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Emulate8080.h"

typedef struct ExtInstructions {
        double lastTimer;
        double nextInterrupt;
        double whichInterrupt;

        uint8_t shift0;
        uint8_t shift1;
        uint8_t shift_offset;
} ExtInstructions;

void CPUIncrement(State8080* state, ExtInstructions* ins);
