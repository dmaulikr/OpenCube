/**
 * \file dct.c
 */

#include "ejpgl.h"
#include "zzq.h"

extern signed int weights[512];
signed short dctresult[MATRIX_SIZE][MATRIX_SIZE];

int dct_init_start() {

	return 0;

}

int dct_stop_done() {

	return 0;

}

/*
 Function Name: dct

 Operation: Find the 8x8 DCT of an array using separable DCT
 First, finds 1-d DCT along rows, storing the result in inter[][]
 Then, 1-d DCT along columns of inter[][] is found

 Input: pixels is the 8x8 input array

 Output: dct is the 8x8 output array
*/

void dct(signed char pixels[8][8], int color) {
	int inr, inc;   /* rows and columns of input image */
	int intr, intc;  /* rows and columns of intermediate image */
	int outr, outc;  /* rows and columns of dct */
	int f_val;  /* cumulative sum */
	int inter[8][8]; /* stores intermediate result */
	int i, j, k;
	k = 0;

	for (intr = 0; intr < 8; intr++)
		for (intc = 0; intc < 8; intc++) {
			for (i = 0, f_val = 0; i < 8; i++) {
				f_val += (pixels[intr][i] * weights[k]);//cos((double)(2*i+1)*(double)intc*PI/16);
				k++;
			}
			if (intc != 0) inter[intr][intc] =  f_val >> 15;
			else inter[intr][intc] = (11585 * (f_val >> 14)) >> 15;

		}

	/* find 1-d dct along columns */

	for (outc = 0, k = 0; outc < 8; outc++)
		for (outr = 0; outr < 8; outr++) {
			for (i = 0, f_val = 0; i < 8; i++) {
				f_val += (inter[i][outc] * weights[k]);
				k++;
			}
			if (outr != 0) dctresult[outr][outc] = f_val >> 15;
			else dctresult[outr][outc] = (11585 * (f_val >> 14) >> 15);
		}

	zzq_encode(dctresult, color);
	return;

}


signed int weights[512] = {
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16384,
	16069,
	13623,
	9102,
	3196,
	-3196,
	-9102,
	-13623,
	-16069,
	15137,
	6270,
	-6270,
	-15137,
	-15137,
	-6270,
	6270,
	15137,
	13623,
	-3196,
	-16069,
	-9103,
	9102,
	16069,
	3196,
	-13623,
	11585,
	-11585,
	-11585,
	11585,
	11585,
	-11585,
	-11585,
	11585,
	9102,
	-16069,
	3196,
	13623,
	-13623,
	-3197,
	16069,
	-9102,
	6270,
	-15137,
	15137,
	-6270,
	-6270,
	15137,
	-15137,
	6270,
	3196,
	-9103,
	13623,
	-16069,
	16069,
	-13623,
	9102,
	-3196
};


