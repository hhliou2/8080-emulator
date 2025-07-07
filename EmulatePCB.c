#include <stdlib.h>
#include <sys/time.h>
#include "Emulate8080.h"

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

int main(int argc, char**argv) {

        int done = 0;
        State8080* state = InitMachine();

        while (done == 0) {
                done = Emulate8080Op(state);
        }

        return 0;
}


