#include "Emulate8080.h"

#define DEBUG 0

// Error call for opcode
void UnimplementedInstruction(State8080* state) {
	printf("Error: Unimplemented instruction\n");
	state->pc--; //go to previous program counter
	printf("Opcode of error: %02x", state->memory[state->pc]);
	printf("\n");
	exit(1);
}

// Flag functions
uint8_t flag_z(int value) {
	return value == 0;
}
uint8_t flag_s(int value) {
	return (0x80== (value & 0x80));
}
uint8_t flag_p(int value) {
	int i;
	int p = 0;
	value = (value & ((1<<size)-1));
	for (i=0; i<size; i++)
	{
		if (value & 0x1) p++;
		value = value >> 1;
	}
	return (0 == (p & 0x1));
}

// Emulate opcodes programatically
int Emulate8080Op(State8080* state) {
	unsigned char *opcode = &state->memory[state->pc];

	state->pc+=1;

	switch(*opcode) {
		case 0x00: break;
		case 0x01: 
			   state->b = opcode[2];
			   state->c = opcode[1];
			   state->pc += 2;
			   break;
		case 0x02: UnimplementedInstruction(state); break;
		case 0x03: UnimplementedInstruction(state); break;
		case 0x04: UnimplementedInstruction(state); break;
		case 0x05: 
			   {
			   uint8_t res8 = state->b - 1;
			   state->cc.z = flag_z(res8);
			   state->cc.s = flag_s(res8);
			   state->cc.p = flag_p(res8);
			   state->b = state->b - 1;
			   break;
			   }
		case 0x06: 
			   state->b = opcode[1];
			   state->pc++;
			   break;
		case 0x07: UnimplementedInstruction(state); break;
		case 0x08: UnimplementedInstruction(state); break;
		case 0x09: 
			   {
			   uint32_t hl = (state->h << 8) | state->l;
			   uint32_t bc = (state->b << 8) | state->c;
			   uint32_t res32 = hl + bc;
			   state->h = (res32 & 0xff00) >> 8;
			   state->l = res32 & 0xff;
			   state->cc.cy = ((res32 & 0xffff0000) != 0);
			   break;
			   }
		case 0x0a: UnimplementedInstruction(state); break;
		case 0x0b: UnimplementedInstruction(state); break;
		case 0x0c: UnimplementedInstruction(state); break;
		case 0x0d: 
			   {
			   uint8_t res8 = state->c - 1;
			   state->cc.z = flag_z(res8);
			   state->cc.s = flag_s(res8);
			   state->cc.p = flag_p(res8);
			   state->c = state->c - 1;
			   break;
			   }
		case 0x0e: 
			   state->c = opcode[1];
			   state->pc++;
			   break;
		case 0x0f: 
			   uint8_t prev_bit = state->a&1;
			   state->a = prev_bit<<7 | state->a>>1;
			   state->cc.cy = prev_bit;
			   break;

		case 0x10: UnimplementedInstruction(state); break;
		case 0x11: 
			   state->d = opcode[2];
			   state->e = opcode[1];
			   state->pc += 2;
			   break;
		case 0x12: UnimplementedInstruction(state); break;
		case 0x13: 
			   state->e++;
			   if (state->e == 0)
				   state->d++;
			   break;
		case 0x14: UnimplementedInstruction(state); break;
		case 0x15: UnimplementedInstruction(state); break;
		case 0x16: UnimplementedInstruction(state); break;
		case 0x17: UnimplementedInstruction(state); break;
		case 0x18: UnimplementedInstruction(state); break;
		case 0x19: 
			   {
			   uint32_t hl = (state->h << 8) | state->l;
			   uint32_t de = (state->d << 8) | state->e;
			   uint32_t res32 = hl + de;
			   state->h = (res32 & 0xff00) >> 8;
			   state->l = res32 & 0xff;
			   state->cc.cy = ((res32 & 0xffff0000) != 0);
			   break;
			   }
		case 0x1a: 
			   {
			   uint16_t offset = (state->d<<8) | state->e;
			   state->a = state->memory[offset];
			   break;
			   }
		case 0x1b: UnimplementedInstruction(state); break;
		case 0x1c: UnimplementedInstruction(state); break;
		case 0x1d: UnimplementedInstruction(state); break;
		case 0x1e: UnimplementedInstruction(state); break;
		case 0x1f: UnimplementedInstruction(state); break;

		case 0x20: UnimplementedInstruction(state); break;
		case 0x21: 
			   state->h = opcode[2];
			   state->l = opcode[1];
			   state->pc += 2;
			   break;
		case 0x22: UnimplementedInstruction(state); break;
		case 0x23: 
			   state->l++;
			   if (state->l == 0) 
				   state->h++;
			   break;
		case 0x24: UnimplementedInstruction(state); break;
		case 0x25: UnimplementedInstruction(state); break;
		case 0x26: 
			   state->h = opcode[1];
			   state->pc++;
			   break;
		case 0x27: UnimplementedInstruction(state); break;
		case 0x28: UnimplementedInstruction(state); break;
		case 0x29: 
			   {
			   uint32_t hl = (state->h << 8) | state->l;
			   uint32_t res32 = hl + hl;
			   state->h = (res32 & 0xff00) >> 8;
			   state->l = res32 & 0xff;
			   state->cc.cy = ((res32 & 0xffff0000) != 0);
			   break;
			   }
		case 0x2a: UnimplementedInstruction(state); break;
		case 0x2b: UnimplementedInstruction(state); break;
		case 0x2c: UnimplementedInstruction(state); break;
		case 0x2d: UnimplementedInstruction(state); break;
		case 0x2e: UnimplementedInstruction(state); break;
		case 0x2f: UnimplementedInstruction(state); break;

		case 0x30: UnimplementedInstruction(state); break;
		case 0x31: 
			   state->sp = (opcode[2]<<8) | opcode[1];
			   state->pc+=2;
			   break;
		case 0x32: 
			   uint16_t offset = (opcode[2] << 8) | opcode[1];
			   state->memory[offset] = state->a;
			   state->pc+=2;
			   break;
		case 0x33: UnimplementedInstruction(state); break;
		case 0x34: UnimplementedInstruction(state); break;
		case 0x35: UnimplementedInstruction(state); break;
		case 0x36: 
			   {
			   uint16_t offset = (state->h<<8) | state->l;
			   state->memory[offset] = opcode[1];
			   state->pc++;
			   break;
			   }
		case 0x37: UnimplementedInstruction(state); break;
		case 0x38: UnimplementedInstruction(state); break;
		case 0x39: 
			   
			   break;
		case 0x3a: 
			   {
			   uint16_t offset = (opcode[2]<<8) | opcode[1];
			   state->a = state->memory[offset];
			   state->pc = state->pc+2;
			   break;
			   }
		case 0x3b: UnimplementedInstruction(state); break;
		case 0x3c: UnimplementedInstruction(state); break;
		case 0x3d: UnimplementedInstruction(state); break;
		case 0x3e: 
			   state->a = opcode[1];
			   state->pc++;
			   break;
		case 0x3f: UnimplementedInstruction(state); break;

		case 0x40: UnimplementedInstruction(state); break;
		case 0x41: UnimplementedInstruction(state); break;
		case 0x42: UnimplementedInstruction(state); break;
		case 0x43: UnimplementedInstruction(state); break;
		case 0x44: UnimplementedInstruction(state); break;
		case 0x45: UnimplementedInstruction(state); break;
		case 0x46: UnimplementedInstruction(state); break;
		case 0x47: UnimplementedInstruction(state); break;
		case 0x48: UnimplementedInstruction(state); break;
		case 0x49: UnimplementedInstruction(state); break;
		case 0x4a: UnimplementedInstruction(state); break;
		case 0x4b: UnimplementedInstruction(state); break;
		case 0x4c: UnimplementedInstruction(state); break;
		case 0x4d: UnimplementedInstruction(state); break;
		case 0x4e: UnimplementedInstruction(state); break;
		case 0x4f: UnimplementedInstruction(state); break;

		case 0x50: UnimplementedInstruction(state); break;
		case 0x51: UnimplementedInstruction(state); break;
		case 0x52: UnimplementedInstruction(state); break;
		case 0x53: UnimplementedInstruction(state); break;
		case 0x54: UnimplementedInstruction(state); break;
		case 0x55: UnimplementedInstruction(state); break;
		case 0x56: 
			   {
			   uint16_t offset = (state->h << 8) | state->l;
			   state->d = state->memory[offset];
			   break;
			   }
		case 0x57: UnimplementedInstruction(state); break;
		case 0x58: UnimplementedInstruction(state); break;
		case 0x59: UnimplementedInstruction(state); break;
		case 0x5a: UnimplementedInstruction(state); break;
		case 0x5b: UnimplementedInstruction(state); break;
		case 0x5c: UnimplementedInstruction(state); break;
		case 0x5d: UnimplementedInstruction(state); break;
		case 0x5e: 
			   {
			   uint16_t offset = (state->h << 8) | state->l;
			   state->e = state->memory[offset];
			   break;
			   }
		case 0x5f: UnimplementedInstruction(state); break;

		case 0x60: UnimplementedInstruction(state); break;
		case 0x61: UnimplementedInstruction(state); break;
		case 0x62: UnimplementedInstruction(state); break;
		case 0x63: UnimplementedInstruction(state); break;
		case 0x64: UnimplementedInstruction(state); break;
		case 0x65: UnimplementedInstruction(state); break;
		case 0x66: 
			   {
			   uint16_t offset = (state->h << 8) | state->l;
			   state->h = state->memory[offset];
			   break;
			   }
		case 0x67: UnimplementedInstruction(state); break;
		case 0x68: UnimplementedInstruction(state); break;
		case 0x69: UnimplementedInstruction(state); break;
		case 0x6a: UnimplementedInstruction(state); break;
		case 0x6b: UnimplementedInstruction(state); break;
		case 0x6c: UnimplementedInstruction(state); break;
		case 0x6d: UnimplementedInstruction(state); break;
		case 0x6e: UnimplementedInstruction(state); break;
		case 0x6f: state->l = state->a;	break;

		case 0x70: UnimplementedInstruction(state); break;
		case 0x71: UnimplementedInstruction(state); break;
		case 0x72: UnimplementedInstruction(state); break;
		case 0x73: UnimplementedInstruction(state); break;
		case 0x74: UnimplementedInstruction(state); break;
		case 0x75: UnimplementedInstruction(state); break;
		case 0x76: UnimplementedInstruction(state); break;
		case 0x77: 
			   {
			   uint16_t offset = (state->h<<8) | (state->l);
			   state->memory[offset] = state->a;
			   break;
			   }
		case 0x78: UnimplementedInstruction(state); break;
		case 0x79: UnimplementedInstruction(state); break;
		case 0x7a: state->a = state->d;	break;
		case 0x7b: state->a = state->e; break;
		case 0x7c: state->a = state->h;	break;
		case 0x7d: UnimplementedInstruction(state); break;
		case 0x7e: 
			   {
			   uint16_t offset = (state->h << 8) | state->l;
			   state->a = state->memory[offset];
			   break;
			   }
		case 0x7f: UnimplementedInstruction(state); break;

		case 0x80: UnimplementedInstruction(state); break;
		case 0x81: UnimplementedInstruction(state); break;
		case 0x82: UnimplementedInstruction(state); break;
		case 0x83: UnimplementedInstruction(state); break;
		case 0x84: UnimplementedInstruction(state); break;
		case 0x85: UnimplementedInstruction(state); break;
		case 0x86: UnimplementedInstruction(state); break;
		case 0x87: UnimplementedInstruction(state); break;
		case 0x88: UnimplementedInstruction(state); break;
		case 0x89: UnimplementedInstruction(state); break;
		case 0x8a: UnimplementedInstruction(state); break;
		case 0x8b: UnimplementedInstruction(state); break;
		case 0x8c: UnimplementedInstruction(state); break;
		case 0x8d: UnimplementedInstruction(state); break;
		case 0x8e: UnimplementedInstruction(state); break;
		case 0x8f: UnimplementedInstruction(state); break;

		case 0x90: UnimplementedInstruction(state); break;
		case 0x91: UnimplementedInstruction(state); break;
		case 0x92: UnimplementedInstruction(state); break;
		case 0x93: UnimplementedInstruction(state); break;
		case 0x94: UnimplementedInstruction(state); break;
		case 0x95: UnimplementedInstruction(state); break;
		case 0x96: UnimplementedInstruction(state); break;
		case 0x97: UnimplementedInstruction(state); break;
		case 0x98: UnimplementedInstruction(state); break;
		case 0x99: UnimplementedInstruction(state); break;
		case 0x9a: UnimplementedInstruction(state); break;
		case 0x9b: UnimplementedInstruction(state); break;
		case 0x9c: UnimplementedInstruction(state); break;
		case 0x9d: UnimplementedInstruction(state); break;
		case 0x9e: UnimplementedInstruction(state); break;
		case 0x9f: UnimplementedInstruction(state); break;

		case 0xa0: UnimplementedInstruction(state); break;
		case 0xa1: UnimplementedInstruction(state); break;
		case 0xa2: UnimplementedInstruction(state); break;
		case 0xa3: UnimplementedInstruction(state); break;
		case 0xa4: UnimplementedInstruction(state); break;
		case 0xa5: UnimplementedInstruction(state); break;
		case 0xa6: UnimplementedInstruction(state); break;
		case 0xa7: state->a = state->a & state->a;
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;
		case 0xa8: UnimplementedInstruction(state); break;
		case 0xa9: UnimplementedInstruction(state); break;
		case 0xaa: UnimplementedInstruction(state); break;
		case 0xab: UnimplementedInstruction(state); break;
		case 0xac: UnimplementedInstruction(state); break;
		case 0xad: UnimplementedInstruction(state); break;
		case 0xae: UnimplementedInstruction(state); break;
		case 0xaf: 
			   state->a = state->a ^ state->a;
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;

		case 0xb0: UnimplementedInstruction(state); break;
		case 0xb1: UnimplementedInstruction(state); break;
		case 0xb2: UnimplementedInstruction(state); break;
		case 0xb3: UnimplementedInstruction(state); break;
		case 0xb4: UnimplementedInstruction(state); break;
		case 0xb5: UnimplementedInstruction(state); break;
		case 0xb6: UnimplementedInstruction(state); break;
		case 0xb7: UnimplementedInstruction(state); break;
		case 0xb8: UnimplementedInstruction(state); break;
		case 0xb9: UnimplementedInstruction(state); break;
		case 0xba: UnimplementedInstruction(state); break;
		case 0xbb: UnimplementedInstruction(state); break;
		case 0xbc: UnimplementedInstruction(state); break;
		case 0xbd: UnimplementedInstruction(state); break;
		case 0xbe: UnimplementedInstruction(state); break;
		case 0xbf: UnimplementedInstruction(state); break;

		case 0xc0: UnimplementedInstruction(state); break;
		case 0xc1: 
			   state->c = state->memory[state->sp];
			   state->b = state->memory[state->sp+1];
			   state->sp = state->sp+2;
			   break;
		case 0xc2: 
			   if (0 == state->cc.z)
				   state->pc = (opcode[2] << 8) | opcode[1];
			   else
				   state->pc = state->pc + 2;
			   break;
		case 0xc3: 
			   state->pc = (opcode[2] << 8) | opcode[1]; //reconstructs address little endian format
			   break;
		case 0xc4: UnimplementedInstruction(state); break;
		case 0xc5: 
			   state->memory[state->sp-2] = state->c;
			   state->memory[state->sp-1] = state->b;
			   state->sp = state->sp - 2;
			   break;
		case 0xc6: 
			   {
			   uint16_t x = (uint16_t) state->a + (uint16_t) opcode[1];
			   state->cc.z = flag_z(x);
			   state->cc.s = flag_s(x);
			   state->cc.p = flag_p(x&0xff);
			   state->cc.cy = (x > 0xff);
			   state->a = (uint8_t) x;
			   state->pc++;
			   break;
			   }
		case 0xc7: UnimplementedInstruction(state); break;
		case 0xc8: UnimplementedInstruction(state); break;
		case 0xc9: 
			   state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
			   state->sp += 2;
			   break;
		case 0xca: UnimplementedInstruction(state); break;
		case 0xcb: UnimplementedInstruction(state); break;
		case 0xcc: UnimplementedInstruction(state); break;
		case 0xcd: 
			   {
			   // store current location to memory stack before starting subroutine
			   uint16_t ret = state->pc + 2;
			   state->memory[state->sp - 1] = (ret >> 8) & 0xff; //0xff ensures only 8 byte storage (no garbage from shift)
			   state->memory[state->sp - 2] = (ret & 0xff);
			   
			   // shift SP down
			   state->sp = state->sp - 2;

			   // Set PC to address in provided bytes
			   state->pc = (opcode[2] << 8) | opcode[1];
			   break;
			   }
		case 0xce: UnimplementedInstruction(state); break;
		case 0xcf: UnimplementedInstruction(state); break;

		case 0xd0: UnimplementedInstruction(state); break;
		case 0xd1: 
			   state->e = state->memory[state->sp];
			   state->d = state->memory[state->sp+1];
			   state->sp = state->sp+2;
			   break;
		case 0xd2: UnimplementedInstruction(state); break;
		case 0xd3: state->pc++;	break; //special??
		case 0xd4: UnimplementedInstruction(state); break;
		case 0xd5: 
			   state->memory[state->sp-2] = state->e;
			   state->memory[state->sp-1] = state->d;
			   state->sp = state->sp-2;
			   break;
		case 0xd6: UnimplementedInstruction(state); break;
		case 0xd7: UnimplementedInstruction(state); break;
		case 0xd8: UnimplementedInstruction(state); break;
		case 0xd9: UnimplementedInstruction(state); break;
		case 0xda: UnimplementedInstruction(state); break;
		case 0xdb: UnimplementedInstruction(state); break;
		case 0xdc: UnimplementedInstruction(state); break;
		case 0xdd: UnimplementedInstruction(state); break;
		case 0xde: UnimplementedInstruction(state); break;
		case 0xdf: UnimplementedInstruction(state); break;

		case 0xe0: UnimplementedInstruction(state); break;
		case 0xe1: 
			   state->l = state->memory[state->sp];
			   state->h = state->memory[state->sp+1];
			   state->sp = state->sp + 2;
			   break;
		case 0xe2: UnimplementedInstruction(state); break;
		case 0xe3: UnimplementedInstruction(state); break;
		case 0xe4: UnimplementedInstruction(state); break;
		case 0xe5: 
			   state->memory[state->sp-2] = state->l;
			   state->memory[state->sp-1] = state->h;
			   state->sp = state->sp-2;
			   break;
		case 0xe6: 
			   state->a = state->a & opcode[1];
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   state->pc++;
			   break;
		case 0xe7: UnimplementedInstruction(state); break;
		case 0xe8: UnimplementedInstruction(state); break;
		case 0xe9: UnimplementedInstruction(state); break;
		case 0xea: UnimplementedInstruction(state); break;
		case 0xeb:
			   uint8_t temp_d = state->d;
			   uint8_t temp_e = state->e;
			   state->d = state->h;
			   state->e = state->l;
			   state->h = temp_d;
			   state->l = temp_e;
			   break;
		case 0xec: UnimplementedInstruction(state); break;
		case 0xed: UnimplementedInstruction(state); break;
		case 0xee: UnimplementedInstruction(state); break;
		case 0xef: UnimplementedInstruction(state); break;

		case 0xf0: UnimplementedInstruction(state); break;
		case 0xf1: 
			   {
			   uint8_t flags = state->memory[state->sp];
			   state->cc.z = (0x01 == (flags & 0x01));
			   state->cc.s = (0x02 == (flags & 0x02));
			   state->cc.p = (0x04 == (flags & 0x04));
			   state->cc.cy = (0x05 == (flags & 0x05));
			   state->cc.ac = (0x10 == (flags & 0x10));

			   state->a = state->memory[state->sp+1];
			   state->sp = state->sp+2;
			   break;
			   }
		case 0xf2: UnimplementedInstruction(state); break;
		case 0xf3: UnimplementedInstruction(state); break;
		case 0xf4: UnimplementedInstruction(state); break;
		case 0xf5: 
			   {
			   uint8_t flags = (state->cc.z |
					    state->cc.s<<1 |
					    state->cc.p<<2 |
					    state->cc.cy<<3 |
					    state->cc.ac<<4 );
			   state->memory[state->sp-2] = flags;
			   state->memory[state->sp-1] = state->a;
			   state->sp = state->sp-2;
			   break;
			   }
		case 0xf6: UnimplementedInstruction(state); break;
		case 0xf7: UnimplementedInstruction(state); break;
		case 0xf8: UnimplementedInstruction(state); break;
		case 0xf9: UnimplementedInstruction(state); break;
		case 0xfa: UnimplementedInstruction(state); break;
		case 0xfb: state->int_enable = 1; break;
		case 0xfc: UnimplementedInstruction(state); break;
		case 0xfd: UnimplementedInstruction(state); break;
		case 0xfe: 
			   uint8_t x = state->a - opcode[1]; 
			   state->cc.z = flag_z(x);
			   state->cc.s = flag_s(x);
			   state->cc.p = flag_p(x);
			   state->cc.cy = (state->a < opcode[1]);
			   state->pc++;
			   break;
		case 0xff: UnimplementedInstruction(state); break;
	}

	// Print current state for debugging
	
	printf("State: ");
	printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x ", state->a, state->b, state->c,
				state->d, state->e, state->h, state->l, state->sp);
	printf("PC: ");
	printf("%02x ", state->pc);
	printf("Flags: ");
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("%c  ", state->cc.ac ? 'a' : '.');

	printf("\n");
	return 0;
}

State8080* Init8080(void) {
	State8080* state = calloc(1, sizeof(State8080)); // allocate memory for State8080 struct
	state->memory = malloc(0x10000); // allocate 16k bytes for state memory
	return state;
}

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

int main(int argc, char**argv) {

	int done = 0;
	int vblankcycles = 0;
	State8080* state = Init8080();

	// reading in reverse order due to little endian
	ReadFileIntoMemoryAt(state, "invaders.h", 0);
	ReadFileIntoMemoryAt(state, "invaders.g", 0x800);
	ReadFileIntoMemoryAt(state, "invaders.f", 0x1000);
	ReadFileIntoMemoryAt(state, "invaders.e", 0x1800);

	while (done == 0) {
		done = Emulate8080Op(state);
	}

	return 0;
}
