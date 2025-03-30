#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

const uint32_t init_h[] = {
	0b01101010000010011110011001100111,
	0b10111011011001111010111010000101,
	0b00111100011011101111001101110010,
	0b10100101010011111111010100111010,
	0b01010001000011100101001001111111,
	0b10011011000001010110100010001100,
	0b00011111100000111101100110101011,
	0b01011011111000001100110100011001
};

const uint32_t k[] = {
	0b01000010100010100010111110011000,
	0b01110001001101110100010010010001,
	0b10110101110000001111101111001111,
	0b11101001101101011101101110100101,
	0b00111001010101101100001001011011,
	0b01011001111100010001000111110001,
	0b10010010001111111000001010100100,
	0b10101011000111000101111011010101,
	0b11011000000001111010101010011000,
	0b00010010100000110101101100000001,
	0b00100100001100011000010110111110,
	0b01010101000011000111110111000011,
	0b01110010101111100101110101110100,
	0b10000000110111101011000111111110,
	0b10011011110111000000011010100111,
	0b11000001100110111111000101110100,
	0b11100100100110110110100111000001,
	0b11101111101111100100011110000110,
	0b00001111110000011001110111000110,
	0b00100100000011001010000111001100,
	0b00101101111010010010110001101111,
	0b01001010011101001000010010101010,
	0b01011100101100001010100111011100,
	0b01110110111110011000100011011010,
	0b10011000001111100101000101010010,
	0b10101000001100011100011001101101,
	0b10110000000000110010011111001000,
	0b10111111010110010111111111000111,
	0b11000110111000000000101111110011,
	0b11010101101001111001000101000111,
	0b00000110110010100110001101010001,
	0b00010100001010010010100101100111,
	0b00100111101101110000101010000101,
	0b00101110000110110010000100111000,
	0b01001101001011000110110111111100,
	0b01010011001110000000110100010011,
	0b01100101000010100111001101010100,
	0b01110110011010100000101010111011,
	0b10000001110000101100100100101110,
	0b10010010011100100010110010000101,
	0b10100010101111111110100010100001,
	0b10101000000110100110011001001011,
	0b11000010010010111000101101110000,
	0b11000111011011000101000110100011,
	0b11010001100100101110100000011001,
	0b11010110100110010000011000100100,
	0b11110100000011100011010110000101,
	0b00010000011010101010000001110000,
	0b00011001101001001100000100010110,
	0b00011110001101110110110000001000,
	0b00100111010010000111011101001100,
	0b00110100101100001011110010110101,
	0b00111001000111000000110010110011,
	0b01001110110110001010101001001010,
	0b01011011100111001100101001001111,
	0b01101000001011100110111111110011,
	0b01110100100011111000001011101110,
	0b01111000101001010110001101101111,
	0b10000100110010000111100000010100,
	0b10001100110001110000001000001000,
	0b10010000101111101111111111111010,
	0b10100100010100000110110011101011,
	0b10111110111110011010001111110111,
	0b11000110011100010111100011110010
};

void hexPrint(unsigned char* buf, int len) {
	for (int i = 0; i < len; i++) {
		printf("%02X", *(buf + i));
		if (i % 4 == 3) printf("\n");
	}
	printf("\n");
}

void binPrint(unsigned char* buf, int len) {
	for (int i = 0; i < len; i++) {
		unsigned char temp = buf[i];
		for (int j = 0; j < 8; j++) {
			printf("%d", temp / 128);
			temp *= 2;
		}
		if (i % 4 == 3) printf("\n");
	}
	printf("\n");
}

void binPrint32(uint32_t num) {
	for (int i = 0; i < 32; i++) {
		printf("%d", num / (1 << 31));
		num <<= 1;
	}
	printf("\n");
}

void printSchedule(uint32_t* schedule) {
	for (int i = 0; i < 64; i++) {
		binPrint32(schedule[i]);
	}
}

uint32_t rtr(uint32_t num, unsigned int n) {
	return (num >> n) | (num << (32 - n));
}

uint32_t shr(uint32_t num, unsigned int n) {
	return (num >> n);
}

void copyBlockToSchedule(uint32_t* schedule, unsigned char* block) {
	for (int i = 0; i < 16; i++) {
		schedule[i] = 0;
		for (int j = 0; j < 4; j++) {
			schedule[i] <<= 8;
			schedule[i] += block[i * 4 + j];
		}
	}
}

void processBlock(uint32_t* h, unsigned char* block) {
	uint32_t schedule[64];
	copyBlockToSchedule(schedule, block);
	for (int i = 16; i < 64; i++) {
		uint32_t sigma0 = rtr(schedule[i - 15], 7) ^ rtr(schedule[i - 15], 18) ^ shr(schedule[i - 15], 3);
		uint32_t sigma1 = rtr(schedule[i - 2], 17) ^ rtr(schedule[i - 2], 19) ^ shr(schedule[i - 2], 10);
		schedule[i] = schedule[i - 16] + sigma0 + schedule[i - 7] + sigma1;
	}
	uint32_t working[8];
	memcpy(working, h, 8 * sizeof(uint32_t));
	for (int i = 0; i < 64; i++) {
		uint32_t majority = (working[0] & working[1]) ^ (working[0] & working[2]) ^ (working[1] & working[2]);
		uint32_t choice = (working[4] & working[5]) ^ ((~working[4]) & working[6]);
		uint32_t sigma0 = rtr(working[0], 2) ^ rtr(working[0], 13) ^ rtr(working[0], 22);
		uint32_t sigma1 = rtr(working[4], 6) ^ rtr(working[4], 11) ^ rtr(working[4], 25);
		uint32_t temp1 = working[7] + sigma1 + choice + k[i] + schedule[i];
		uint32_t temp2 = sigma0 + majority;

		working[7] = working[6];
		working[6] = working[5];
		working[5] = working[4];
		working[4] = working[3] + temp1;
		working[3] = working[2];
		working[2] = working[1];
		working[1] = working[0];
		working[0] = temp1 + temp2;
	}
	for (int i = 0; i < 8; i++) {
		h[i] += working[i];
	}
}

void loadSize(unsigned char* block, uint64_t size) {
	for (int i = 0; i < 8; i++) {
		block[64 - i - 1] = size % 256;
		size >>= 8;
	}
}

int main(int argc, char** argv) {
	if (argc > 2) {
		printf("Usage: sha256 <file>\n");
		return 1;
	}

	FILE* file;
	if (argc == 1) {
		file = stdin;
	} else {
		file = fopen(argv[1], "r");
	}
	if (file == NULL) {
		printf("Failed to open the file\n");
		return 2;
	}

	unsigned char block[64];
	unsigned int bytesRead = 0;
	uint64_t messageBits = 0;

	uint32_t h[8];
	memcpy(h, init_h, 8 * sizeof(uint32_t));
	
	while ((bytesRead = fread(block, 1, 64, file)) == 64) {
		messageBits += 512;
		processBlock(h, block);
	}

	messageBits += bytesRead * 8;
	block[bytesRead] = 0b10000000;
	if (bytesRead < 56) {
		memset(block + bytesRead + 1, 0, 56 - (bytesRead + 1));
		loadSize(block, messageBits);
		processBlock(h, block);
	} else {
		memset(block + bytesRead + 1, 0, 64 - (bytesRead + 1));
		processBlock(h, block);
		memset(block, 0, 56);
		loadSize(block, messageBits);
		processBlock(h, block);
	}

	for (unsigned int i = 0; i < 8; i++) {
		printf("%08x", h[i]);
	}
	printf("\n");

	return 0;
}
