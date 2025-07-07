#include "Emulate8080.h"

// Cycle values taken from Emulator101, can be found on Wikipedia
unsigned char cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
};

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
	value = (value & ((1<<8)-1));
	for (i=0; i<8; i++)
	{
		if (value & 0x1) p++;
		value = value >> 1;
	}
	return (0 == (p & 0x1));
}

// Space invaders RAM must be between 0x2000 and 0x4000
static void writeMem(State8080* state, uint16_t address, uint8_t value) {
	if (address >= 0x2000 && address <= 0x4000) {
		state->memory[address] = value;
	}
}

// Common functions
static uint8_t readHL(State8080* state) {
	uint16_t offset = (state->h << 8) | state->l;
	return state->memory[offset];
}

static void writeHL(State8080* state, uint8_t value) {
	uint16_t offset = (state->h << 8) | state->l;
	writeMem(state, offset, value);
}

static void push(State8080* state, uint8_t high, uint8_t low) {
	writeMem(state, state->sp-1, high); //going backwards due to little endian
	writeMem(state, state->sp-2, low);
	state->sp = state->sp-2;
}

static void pop(State8080* state, uint8_t *high, uint8_t *low) {
	*high = state->memory[state->sp+1];
	*low = state->memory[state->sp];
	state->sp += 2;
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
		case 0x03: 
			   state->c++;
			   if (state->c == 0)
				   state->b++;
			   break;
		case 0x04: 
			   {
			   uint8_t res = state->b + 1;
			   state->cc.z = flag_z(res);
			   state->cc.s = flag_s(res);
			   state->cc.p = flag_p(res);
			   state->b = state->b + 1;
			   break;
			   }
		case 0x05: 
			   {
			   uint8_t res = state->b - 1;
			   state->cc.z = flag_z(res);
			   state->cc.s = flag_s(res);
			   state->cc.p = flag_p(res);
			   state->b = state->b - 1;
			   break;
			   }
		case 0x06: 
			   state->b = opcode[1];
			   state->pc++;
			   break;
		case 0x07: 
			   {
			   uint8_t x = state->a;
			   state->a = ((x & 0x80) >> 7) | (x << 1);
			   state->cc.cy = (0x80 == (x&0x80));
			   }
			   break;
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
		case 0x0a: 
			   {
			   uint16_t bc = (state->b << 8) | state->c;
			   state->a = state->memory[bc];
			   }
			   break;
		case 0x0b: UnimplementedInstruction(state); break;
		case 0x0c: 
			   state->c += 1;
			   state->cc.z = flag_z(state->c);
			   state->cc.s = flag_s(state->c);
			   state->cc.p = flag_p(state->c);
		case 0x0d: 
			   {
			   uint8_t res = state->c - 1;
			   state->cc.z = flag_z(res);
			   state->cc.s = flag_s(res);
			   state->cc.p = flag_p(res);
			   state->c = state->c - 1;
			   }
			   break;
		case 0x0e: 
			   state->c = opcode[1];
			   state->pc++;
			   break;
		case 0x0f: 
			   uint8_t prev_bit = state->a&1;
			   state->a = prev_bit<<7 | state->a>>1;
			   state->cc.cy = prev_bit;
			   break;

		case 0x10: break;
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
		case 0x16: 
			   state->d = opcode[1];
			   state->pc++;
			   break;
		case 0x17: 
			   {
			   uint8_t x = state->a;
			   state->a = state->cc.cy | (x<<1);
			   state->cc.cy = flag_s(x);
			   }
			   break;
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
		case 0x1c: 
			   state->e +=1; 
			   state->cc.z = flag_z(state->e);
			   state->cc.s = flag_s(state->e);
			   state->cc.p = flag_p(state->e);
			   break;
		case 0x1d: UnimplementedInstruction(state); break;
		case 0x1e: UnimplementedInstruction(state); break;
		case 0x1f: 
			   {
			   uint8_t x = state->a;
			   state->a = (state->cc.cy << 7) | (x >> 1);
			   state->cc.cy = (1 == (x&1));
			   }
			   break;

		case 0x20: UnimplementedInstruction(state); break;
		case 0x21: 
			   state->h = opcode[2];
			   state->l = opcode[1];
			   state->pc += 2;
			   break;
		case 0x22: 
			   {
			   uint16_t offset = opcode[2]<<8 | opcode[1];
			   writeMem(state, offset, state->l);
			   writeMem(state, offset+1, state->h);
			   state->pc+=2;
			   }
			   break;
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
		case 0x27: 
			   if ((state->a &0xf) > 9)
				   state->a += 6;
			   if ((state->a&0xf0) > 0x90) {
				   uint16_t res = (uint16_t) state->a + 0x60;
				   state->a = res & 0xff;
				   state->cc.cy = (res > 0xff);
				   state->cc.z = flag_z(res&0xff);
				   state->cc.s = flag_s(res&0xff);
				   state->cc.p = flag_p(res&0xff);
			   }
			   break;
		case 0x28: break;
		case 0x29: 
			   {
			   uint32_t hl = (state->h << 8) | state->l;
			   uint32_t res32 = hl + hl;
			   state->h = (res32 & 0xff00) >> 8;
			   state->l = res32 & 0xff;
			   state->cc.cy = ((res32 & 0xffff0000) != 0);
			   break;
			   }
		case 0x2a: 
			   {
			   uint16_t offset = opcode[1] | (opcode[2] << 8);
			   state->l = state->memory[offset];
			   state->h = state->memory[offset+1];
			   state->pc += 2;
			   }
			   break;
		case 0x2b: 
			   {
			   uint16_t hl = readHL(state);
			   writeHL(state, hl-1);
			   }
			   break;
		case 0x2c: UnimplementedInstruction(state); break;
		case 0x2d: UnimplementedInstruction(state); break;
		case 0x2e: 
			   state->l = opcode[1];
			   state->pc++;
			   break;
		case 0x2f: state->a = ~state->a; break;

		case 0x30: break;
		case 0x31: 
			   state->sp = (opcode[2]<<8) | opcode[1];
			   state->pc+=2;
			   break;
		case 0x32: 
			   {
				   uint16_t offset = (opcode[2]<<8) | opcode[1];
				   state->memory[offset] = state->a;
				   state->pc += 2;
			   }
			   break;
		case 0x33: UnimplementedInstruction(state); break;
		case 0x34: UnimplementedInstruction(state); break;
		case 0x35: 
			   {
				   uint8_t res = readHL(state)-1;
			   	   state->cc.cy = state->cc.ac = 0;
				   state->cc.z = flag_z(res);
				   state->cc.s = flag_s(res);
				   state->cc.p = flag_p(res);
				   writeHL(state, res);
			   }
			   break;
		case 0x36: 
			   {
			   writeHL(state, opcode[1]);
			   state->pc++;
			   break;
			   }
		case 0x37: 
			   state->cc.cy = 1;
			   break;
		case 0x38: UnimplementedInstruction(state); break;
		case 0x39: UnimplementedInstruction(state); break;
		case 0x3a: 
			   {
				uint16_t offset = (opcode[2]<<8) | (opcode[1]);
				state->a = state->memory[offset];
				state->pc+=2;
				break;
			   }
		case 0x3b: UnimplementedInstruction(state); break;
		case 0x3c: UnimplementedInstruction(state); break;
		case 0x3d: 
			   state->a = state->a - 1;
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;
		case 0x3e: state->a = opcode[1];
			   state->pc++; 
			   break;
		case 0x3f: UnimplementedInstruction(state); break;

		case 0x40: UnimplementedInstruction(state); break;
		case 0x41: UnimplementedInstruction(state); break;
		case 0x42: UnimplementedInstruction(state); break;
		case 0x43: UnimplementedInstruction(state); break;
		case 0x44: UnimplementedInstruction(state); break;
		case 0x45: UnimplementedInstruction(state); break;
		case 0x46: state->b = readHL(state); break;
		case 0x47: state->b = state->a; break;
		case 0x48: UnimplementedInstruction(state); break;
		case 0x49: UnimplementedInstruction(state); break;
		case 0x4a: UnimplementedInstruction(state); break;
		case 0x4b: UnimplementedInstruction(state); break;
		case 0x4c: UnimplementedInstruction(state); break;
		case 0x4d: UnimplementedInstruction(state); break;
		case 0x4e: state->c = readHL(state); break;
		case 0x4f: state->c = state->a; break;

		case 0x50: UnimplementedInstruction(state); break;
		case 0x51: UnimplementedInstruction(state); break;
		case 0x52: UnimplementedInstruction(state); break;
		case 0x53: UnimplementedInstruction(state); break;
		case 0x54: UnimplementedInstruction(state); break;
		case 0x55: UnimplementedInstruction(state); break;
		case 0x56: state->d = readHL(state); break;
		case 0x57: state->d = state->a; break;
		case 0x58: UnimplementedInstruction(state); break;
		case 0x59: UnimplementedInstruction(state); break;
		case 0x5a: UnimplementedInstruction(state); break;
		case 0x5b: UnimplementedInstruction(state); break;
		case 0x5c: UnimplementedInstruction(state); break;
		case 0x5d: UnimplementedInstruction(state); break;
		case 0x5e: state->e = readHL(state); break;
		case 0x5f: state->e = state->a; break;

		case 0x60: UnimplementedInstruction(state); break;
		case 0x61: state->h = state->c; break;
		case 0x62: UnimplementedInstruction(state); break;
		case 0x63: UnimplementedInstruction(state); break;
		case 0x64: UnimplementedInstruction(state); break;
		case 0x65: UnimplementedInstruction(state); break;
		case 0x66: state->h = readHL(state); break;
		case 0x67: state->h = state->a; break;
		case 0x68: UnimplementedInstruction(state); break;
		case 0x69: UnimplementedInstruction(state); break;
		case 0x6a: UnimplementedInstruction(state); break;
		case 0x6b: UnimplementedInstruction(state); break;
		case 0x6c: UnimplementedInstruction(state); break;
		case 0x6d: state->l = state->l; break;
		case 0x6e: UnimplementedInstruction(state); break;
		case 0x6f: state->l = state->a;	break;

		case 0x70: writeHL(state, state->b); break;
		case 0x71: UnimplementedInstruction(state); break;
		case 0x72: UnimplementedInstruction(state); break;
		case 0x73: UnimplementedInstruction(state); break;
		case 0x74: UnimplementedInstruction(state); break;
		case 0x75: UnimplementedInstruction(state); break;
		case 0x76: UnimplementedInstruction(state); break;
		case 0x77: writeHL(state, state->a); break;
		case 0x78: state->a = state->b; break;
		case 0x79: state->a = state->c; break;
		case 0x7a: state->a = state->d;	break;
		case 0x7b: state->a = state->e; break;
		case 0x7c: state->a = state->h;	break;
		case 0x7d: state->a = state->l; break;
		case 0x7e: state->a = readHL(state); break;
		case 0x7f: UnimplementedInstruction(state); break;

		case 0x80: 
			   state->a = ((uint16_t)state->a + (uint16_t)state->b)&0xff;
			   state->cc.cy = (state->a > 0xff);
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_z(state->a);
			   state->cc.p = flag_z(state->a);
			   break;
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

		case 0x90: state->a = (uint16_t) state->a - (uint16_t) state->b;
			   state->cc.cy = (state->a > 0xff);
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_z(state->a);
			   state->cc.p = flag_z(state->a);
			   break;
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
		case 0xa7: 
			   state->a = state->a & state->a;
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;
		case 0xa8: 
			   state->a = state->a ^ state->b;
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;
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
		case 0xb0: 
			   state->a = state->a | state->b; 
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;
		case 0xb1: UnimplementedInstruction(state); break;
		case 0xb2: UnimplementedInstruction(state); break;
		case 0xb3: UnimplementedInstruction(state); break;
		case 0xb4: UnimplementedInstruction(state); break;
		case 0xb5: UnimplementedInstruction(state); break;
		case 0xb6: 
			   state->a = state->a | readHL(state);
			   state->cc.cy = state->cc.ac = 0;
			   state->cc.z = flag_z(state->a);
			   state->cc.s = flag_s(state->a);
			   state->cc.p = flag_p(state->a);
			   break;
		case 0xb7: UnimplementedInstruction(state); break;
		case 0xb8: UnimplementedInstruction(state); break;
		case 0xb9: UnimplementedInstruction(state); break;
		case 0xba: UnimplementedInstruction(state); break;
		case 0xbb: UnimplementedInstruction(state); break;
		case 0xbc: UnimplementedInstruction(state); break;
		case 0xbd: UnimplementedInstruction(state); break;
		case 0xbe: UnimplementedInstruction(state); break;
		case 0xbf: UnimplementedInstruction(state); break;

		case 0xc0: 
			   if (state->cc.z == 0) {
				   state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
				   state->sp+=2;
			   }
			   break;
		case 0xc1: 
			   pop(state, &state->b, &state->c);
			   break;
		case 0xc2: 
			   if (state->cc.z == 0)
				   state->pc = (opcode[2] << 8) | opcode[1];
			   else
			           state->pc = state->pc + 2;
			   break;
		case 0xc3: 
			   state->pc = (opcode[2] << 8) | opcode[1]; //reconstructs address little endian format
			   break;
		case 0xc4: 
			   if (state->cc.z) {
				   uint16_t ret = state->pc+2;
				   writeMem(state, state->sp-1, (ret>>8) & 0xff);
				   writeMem(state, state->sp-2, (ret&0xff));
				   state->sp = state->sp - 2;
				   state->pc = opcode[2]<<8 + opcode[1];
			   }
			   else
				   state->pc+=2;
			   break;
		case 0xc5: 
			   push(state, state->b, state->c);
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
		case 0xc8: 
			   if (state->cc.z) {
				   state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
				   state->sp += 2;
			   }
			   break;
		case 0xc9: 
			   state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
			   state->sp += 2;
			   break;
		case 0xca: 
			   if (state->cc.z != 0)
				   state->pc = (opcode[2] << 8) | opcode[1];
			   else
			           state->pc = state->pc + 2;
			   break;
		case 0xcb: UnimplementedInstruction(state); break;
		case 0xcc: 
			   if (state->cc.z) {
				   uint16_t ret = state->pc+2;
				   writeMem(state, state->sp-1, (ret>>8)&0xff);
				   writeMem(state, state->sp-2, (ret&0xff));
				   state->sp = state->sp-2;
				   state->pc = (opcode[2]<<8) | opcode[1];
			   }
			   break;
		case 0xcd: 
			   {
			   // store current location to memory stack before starting subroutine
			   uint16_t ret = state->pc + 2;
			   writeMem(state, state->sp - 1, (ret >> 8) & 0xff); //0xff ensures only 8 byte storage (no garbage from shift)
			   writeMem(state, state->sp - 2, ret & 0xff);
			   
			   // shift SP down
			   state->sp = state->sp - 2;

			   // Set PC to address in provided bytes
			   state->pc = (opcode[2] << 8) | opcode[1];
			   }
			   break;
		case 0xce: UnimplementedInstruction(state); break;
		case 0xcf: UnimplementedInstruction(state); break;

		case 0xd0: 
			   if (state->cc.cy == 0) {
				   state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
				   state->sp+=2;
			   }
			   break;
		case 0xd1: 
			   pop(state, &state->d, &state->e);
			   break;
		case 0xd2: 
			   if (state->cc.cy == 0)
				   state->pc = (opcode[2] << 8) | opcode[1];
			   else
			           state->pc = state->pc + 2;
			   break;
		case 0xd3: state->pc++;	break; //special??
		case 0xd4: 
			   {
			   if (state->cc.cy) {
				   uint16_t ret = state->pc+2;
				   writeMem(state, state->sp-1, (ret>>8) & 0xff);
				   writeMem(state, state->sp-2, (ret&0xff));
				   state->sp = state->sp-2;
				   state->pc = (opcode[2]<<8) | opcode[1];
			   }
			   }
			   break;
		case 0xd5: 
			   push(state, state->d, state->e);
			   break;
		case 0xd6: 
			   {
			   uint8_t x = state->a - opcode[1];
			   state->cc.z = flag_z(x&0xff);
			   state->cc.s = flag_s(x&0xff);
			   state->cc.p = flag_p(x&0xff);
			   state->cc.cy = (state->a < opcode[1]);
			   state->a = x;
			   state->pc++;
			   }
			   break;
		case 0xd7: UnimplementedInstruction(state); break;
		case 0xd8: 
			   if (state->cc.cy) {
				   state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
				   state->sp+=2;
			   }
			   break;
		case 0xd9: UnimplementedInstruction(state); break;
		case 0xda: 
			   if (state->cc.cy != 0)
				   state->pc = (opcode[2] << 8) | opcode[1];
			   else
			           state->pc = state->pc + 2;
			   break;
		case 0xdb: state->pc++; break;
		case 0xdc: UnimplementedInstruction(state); break;
		case 0xdd: UnimplementedInstruction(state); break;
		case 0xde: UnimplementedInstruction(state); break;
		case 0xdf: UnimplementedInstruction(state); break;

		case 0xe0: UnimplementedInstruction(state); break;
		case 0xe1: 
			   pop(state, &state->h, &state->l);
			   break;
		case 0xe2: UnimplementedInstruction(state); break;
		case 0xe3: 
			   {
		   	   uint8_t temp1 = state->l;
			   uint8_t temp2 = state->h;

			   state->l = state->memory[state->sp];
			   state->h = state->memory[state->sp+1];
			   writeMem(state, state->sp, temp1);
			   writeMem(state, state->sp+1, temp2);
			   }
			   break;
		case 0xe4: UnimplementedInstruction(state); break;
		case 0xe5: 
			   push(state, state->h, state->l);
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
		case 0xe9: 
			   state->pc = state->h<<8 | state->l;
			   break;
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

		case 0xf0: 
			   if (state->cc.s == 0) {
				   state->pc = state->memory[state->sp+1]<<8 | state->memory[state->sp];
				   state->sp+=2;
			   }
			   break;
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
		case 0xf6: 
			   {
			   uint8_t x = state->a | opcode[1];
			   state->cc.z = flag_z(x);
			   state->cc.s = flag_s(x);
			   state->cc.p = flag_p(x);
			   state->cc.cy = 0;
			   state->a = x;
			   state->pc++;
			   }
			   break;
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
		case 0xff: 
			   {
			   uint16_t ret = state->pc+2;
			   writeMem(state, state->sp-1, (ret>>8)&0xff);
			   writeMem(state, state->sp-2, (ret&0xff));
			   state->sp = state->sp - 2;
			   state->pc - 0x38;
			   }
			   break;
	}

	// Print current state for debugging
	#ifdef DEBUG
	Disassemble8080Op(state->memory, state->pc);
	printf("\t");
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
	#endif

	return cycles8080[*opcode];
}

void GenerateInterrupt(State8080* state, int interrupt_num) {
	// push current pc to stack for future return
	push(state, (state->pc & 0xff00) >> 8, (state->pc & 0xff));

	// set pc to ret call address
	state -> pc = 8 * interrupt_num;

	//DI
	state->int_enable = 0;
}

/*

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
*/
