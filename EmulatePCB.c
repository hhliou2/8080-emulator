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

void ReadFileIntoMemoryAt(State8080* state, char* filename, uint32_t offset) {
        FILE *f = fopen(filename, "rb");
        if (f==NULL) {
                printf("error: Couldn't open file %s\n", filename);
                exit(1);
        }
        fseek(f, 0L, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0L, SEEK_SET);

        uint8_t *buffer = &state->memory[offset];
        fread(buffer, fsize, 1, f);
        fclose(f);
}

State8080* InitMachine(void) {
        State8080* state = calloc(1, sizeof(State8080)); // allocate memory for State8080 struct
        state->memory = malloc(0x10000); // allocate 16k bytes for state memory (game needs 16-bit per ComputerArchaeology)
	
	// read in reverse due to little endian (per ComputerArchaeology)
	ReadFileIntoMemoryAt(state, "invaders.h", 0);
	ReadFileIntoMemoryAt(state, "invaders.g", 0x800);
	ReadFileIntoMemoryAt(state, "invaders.f", 0x1000);
	ReadFileIntoMemoryAt(state, "invaders.e", 0x1800);

        return state;
}

void InitExt(struct ExtInstructions* ins) {
	ins->lastTimer = 0;
	ins->nextInterrupt = 16000;
	ins->whichInterrupt = 1;
}

double GetPreciseTimeMicroseconds() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double) ts.tv_sec * 1e6 +  (double) ts.tv_nsec/1e3;
}

// Get time elapsed, perform all opcodes that would have been executed in that time relative to 2MHz
void CPUIncrement(State8080* state, ExtInstructions* ins) {
	double now = GetPreciseTimeMicroseconds();
	double timeElapsed = now - ins->lastTimer;
	int cycles_needed = timeElapsed * 2; // Intel 8080 operates at 2MHz (10e6), so time in us (10e-6) converts nicely
	int cycles = 0;

	while (cycles < cycles_needed) {
		cycles += Emulate8080Op(state);
	}

	ins->lastTimer = now;
}

int main(int argc, char**argv) {

        int done = 0;
	struct ExtInstructions ins;

        State8080* state = InitMachine();
	InitExt(&ins);

        while (done == 0) {
                CPUIncrement(state, &ins);
        }

        return 0;
}


