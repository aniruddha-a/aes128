#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "aes.h"
/*
 * A 128 bit AES implementation
 *
 * Thu Dec 31 11:23:12 IST 2009
 *
 * Aniruddha. A (aniruddha.a@gmail.com)
 * Gururaj      (gururajl@gmail.com)
 *
 */

/* Rotate left a 32 bit value by N */
#define ROL32(X,N)  ( ((X) << N) | ((X) >> (32 - N)) )
#define ROR32(X,N)  ( ((X) >> N) | ((X) << (32 - N)) )

typedef union {
    uint32_t w;
    uint8_t b[4];
} u_t;

void convert_str(uint32_t * num, unsigned char *str)
{
    int i, j;

    i = 0;
    for (j = 0; j < 4; j++) {
	num[j] = (str[i] << 24) |
	    (str[i + 1] << 16) | (str[i + 2] << 8) | (str[i + 3]);
	i += 4;
    }
}

uint32_t apply_s_box(uint8_t s[], uint32_t in)
{
    u_t u;

    u.w = in;
    u.b[0] = s[u.b[0]];
    u.b[1] = s[u.b[1]];
    u.b[2] = s[u.b[2]];
    u.b[3] = s[u.b[3]];
    return u.w;
}

uint8_t ffm(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    uint8_t counter;
    uint8_t hi_bit_set;
    for (counter = 0; counter < 8; counter++) {
	if (b & 1)
	    p ^= a;
	hi_bit_set = (a & 0x80);
	a <<= 1;
	if (hi_bit_set)
	    a ^= 0x1b;		/* x^8 + x^4 + x^3 + x + 1 */
	b >>= 1;
    }
    return p;
}

uint32_t expanded_key[44];

/* This is returning the expanded key transposing 4 at a time  */
void get_round_key(int r, uint32_t * res)
{
    u_t uin[4] = { {0}, {0}, {0}, {0} };
    u_t ures[4] = { {0}, {0}, {0}, {0} };
    int j, i;

    for (i = 0; i < 4; i++)
	uin[i].w = htonl(expanded_key[(r * 4) + i]);

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    ures[i].b[j] = uin[j].b[i];

    for (i = 0; i < 4; i++)
	res[i] = htonl(ures[i].w);
}

/* In simple terms - this is transposing the text matrix */
void build_state_matrix(uint32_t *text)
{
    u_t uin[4] = { {0}, {0}, {0}, {0} };
    u_t ures[4] = { {0}, {0}, {0}, {0} };
    int j, i;

    for (i = 0; i < 4; i++)
	uin[i].w = htonl(text[i]);

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    ures[i].b[j] = uin[j].b[i];

    /* copy back */
    for (i = 0; i < 4; i++)
	text[i] = htonl(ures[i].w);
}

void transpose(uint32_t *text)
{
    u_t uin[4] = { {0}, {0}, {0}, {0} };
    u_t ures[4] = { {0}, {0}, {0}, {0} };
    int j, i;

    for (i = 0; i < 4; i++)
	uin[i].w = htonl(text[i]);

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    ures[i].b[j] = uin[j].b[i];

    /* copy back */
    for (i = 0; i < 4; i++)
	text[i] = (ures[i].w);
}

void hex_2_str(uint32_t c[])
{
    u_t uin;
    int i;

    for (i = 0; i < 4; i++) {
	uin.w = c[i];
	printf("%c%c%c%c", uin.b[0], uin.b[1], uin.b[2], uin.b[3]);
    }
}

int main(int argc, char *argv[])
{
    unsigned char *key_str = (unsigned char *) "Thats my Kung Fu";
    unsigned char *text_str = (unsigned char *) "Two One Nine Two";
    uint32_t key[4], text[4];
    int round, i, j;
    uint32_t temp;
    u_t ur[4] = { {0}, {0}, {0}, {0} };
    u_t uin[4] = { {0}, {0}, {0}, {0} };
    uint32_t round_key[4];

    /*  conversion from text to numeric key, text */
    convert_str(key, key_str);
    convert_str(text, text_str);

    build_state_matrix(text);

    /*  Key expansion  */
    for (i = 0; i < 4; i++)
	expanded_key[i] = key[i];	/* just copy  */

    for (i = 4; i < 44; i++) {
	temp = expanded_key[i - 1];
	if (!(i % 4))
	    temp = apply_s_box(s_box, ROL32(expanded_key[i - 1], 8))
		^ (rcon[i / 4] << 24);
	expanded_key[i] = expanded_key[i - 4] ^ temp;
    }

    get_round_key(0, round_key);
    /*  Add round key  */
    for (i = 0; i < 4; i++)
	text[i] ^= round_key[i];

    for (round = 0; round < 10; round++) {
	/*  round 1 */
	for (i = 0; i < 4; i++)
	    text[i] = apply_s_box(s_box, text[i]);

	/*  shift row - start from 1st */
	for (i = 1; i < 4; i++)
	    text[i] = ROL32(text[i], 8 * i);

	/* Only for the first 9 rounds */
	if (round < 9) {
	    /*  Mix column transformation  */
	    for (i = 0; i < 4; i++)
		uin[i].w = htonl(text[i]);	/* put MSB first */

	    for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++) {
		    /*  populate the j th column in result set  */
		    ur[i].b[4 - j - 1] =
			ffm(uin[0].b[j], mixcol[i][0]) ^
			ffm(uin[1].b[j], mixcol[i][1]) ^
			ffm(uin[2].b[j], mixcol[i][2]) ^
			ffm(uin[3].b[j], mixcol[i][3]);
		}
	    for (i = 0; i < 4; i++)
		text[i] = ur[i].w;
	}

	/* Add round key */
	get_round_key(round + 1, round_key);
	for (i = 0; i < 4; i++)
	    text[i] ^= round_key[i];
	/* 1 round complete */
    }				/* end for {round} */

    // Cipher
    for (i = 0; i < 4; i++)
	printf("%08X", text[i]);
    printf("\n");

    /* Decrypt */
    /* Add round key */
    get_round_key(10, round_key);
    for (i = 0; i < 4; i++)
	text[i] ^= round_key[i];

    for (round = 9; round >= 0; round--) {
	/*  shift row - start from 1st */
	for (i = 1; i < 4; i++)
	    text[i] = ROR32(text[i], 8 * i);

	/* apply s box inverse */
	for (i = 0; i < 4; i++)
	    text[i] = apply_s_box(s_box_inv, text[i]);

	/* Add round key */
	get_round_key(round, round_key);
	for (i = 0; i < 4; i++)
	    text[i] ^= round_key[i];

	/* Only for the first 9 rounds in reverse order  */
	if (round > 0) {
	    /*  Mix column transformation  */
	    for (i = 0; i < 4; i++)
		uin[i].w = htonl(text[i]);	/* put MSB first */

	    for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++) {
#if 0
		    uint32_t x = 0;
		    x = ffm(mixcol[0][j], mixcol_inverse[i][0]) ^
			ffm(mixcol[1][j], mixcol_inverse[i][1]) ^
			ffm(mixcol[2][j], mixcol_inverse[i][2]) ^
			ffm(mixcol[3][j], mixcol_inverse[i][3]);
		    printf("\n %04X ", x);
#endif
		    /*  populate the j th column in result set  */
		    ur[i].b[4 - j - 1] =
			ffm(uin[0].b[j], mixcol_inverse[i][0]) ^
			ffm(uin[1].b[j], mixcol_inverse[i][1]) ^
			ffm(uin[2].b[j], mixcol_inverse[i][2]) ^
			ffm(uin[3].b[j], mixcol_inverse[i][3]);
		}
	    for (i = 0; i < 4; i++)
		text[i] = ur[i].w;
	}
    }				// end inverse rounds
    printf("\n ANI: Text back!\n");
    transpose(text);		// dumb transpose to print easily (inverse build_state_matrix)
    hex_2_str(text);
    printf("\n");

    return 0;
}
