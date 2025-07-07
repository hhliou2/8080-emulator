#include "EmulatePCB.h"

const double MICROSECONDS_PER_FRAME = (double) 1e6/60.0;
const double MICROSECONDS_PER_HALF_FRAME = (double) 1e6/120.0;

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

ExtInstructions* InitExt(void) {
	struct ExtInstructions* ins = malloc(sizeof(ExtInstructions));
	ins->lastTimer = 0.0;
	ins->nextInterrupt = 0.0;
	ins->whichInterrupt = 1;

	ins->shift0 = 0;
	ins->shift1 = 0;
	ins->shift_offset = 0;

	return ins;
}

double GetPreciseTimeMicroseconds() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double) ts.tv_sec * 1e6 +  (double) ts.tv_nsec/1e3;
}

// IN port handling
uint8_t MachineIn(uint8_t port, struct ExtInstructions* ins) {
	unsigned char a = 0;
	switch(port) {
		case 0:
			return 0x0e;
		case 1:
			return 0;
		case 3:
			{
				uint16_t v = (ins->shift1<<8) | ins->shift0;
				a = ((v >> (8-ins->shift_offset)) & 0xff);
			}
			break;
	} 

	return 0;
}

//OUT port handling
void MachineOut(uint8_t port, uint8_t value, struct ExtInstructions* ins) {
	switch(port) {
		case 2:
			ins->shift_offset = value & 0x7;
			break;
		case 4:
			ins->shift0 = ins->shift1;
			ins->shift1 = value;
			break;
	}
}

// Get time elapsed, perform all opcodes that would have been executed in that time relative to 2MHz
void CPUIncrement(State8080* state, ExtInstructions* ins) {
	double now = GetPreciseTimeMicroseconds();

	if (ins->lastTimer == 0.0) {
		ins->lastTimer = now;
		ins->nextInterrupt = ins->lastTimer + MICROSECONDS_PER_FRAME;
	}

	// Interrupt handling
	if ((state->int_enable) && (now > ins->nextInterrupt)) {
		if (ins->whichInterrupt == 1) {
			GenerateInterrupt(state, 1);
			ins->whichInterrupt = 2;
		} else {
			GenerateInterrupt(state, 2);
			ins->whichInterrupt = 1;
		}
		ins->nextInterrupt = now + MICROSECONDS_PER_HALF_FRAME;
	}

	// Compute x CPU cycles for time elapsed
	double timeElapsed = now - ins->lastTimer;
	int cycles_needed = timeElapsed * 2; // Intel 8080 operates at 2MHz (10e6), so time in us (10e-6) converts nicely
	int cycles = 0;

	while (cycles < cycles_needed) {
		unsigned char *op = &state->memory[state->pc];

		if (*op == 0xdb) {
		// Emulating machine IN
			state->a = MachineIn(op[1], ins);
			state->pc += 2;
			cycles+=10;
		} else if (*op == 0xd3) {
		// Emulating machine OUT
			MachineOut(op[1], state->a, ins);
			state->pc += 2;
			cycles+=10;
		} else {
			cycles += Emulate8080Op(state);
		}
	}

	ins->lastTimer = now;
}


/*
int main(int argc, char**argv) {

        int done = 0;

        State8080* state = InitMachine();
	ExtInstructions* ins = InitExt();

        while (done == 0) {
                CPUIncrement(state, ins);
        }

        return 0;
}

*/
