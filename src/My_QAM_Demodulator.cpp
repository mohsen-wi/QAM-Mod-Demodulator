#include "My_QAM_Modulator.h"
#include "My_QAM_Demodulator.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

My_QAM_Demodulator::My_QAM_Demodulator()
{
	// Initialize variables
}


My_QAM_Demodulator::~My_QAM_Demodulator()
{
	// Delete allocated variables
}

void My_QAM_Demodulator::SetParameters(const My_QAM_Modulator* mod) {
	// This is useless, just pass the modulator to the run function
}

void My_QAM_Demodulator::grayDecode(int* input, int* output, int length) {
	// Most significant bit is the first bit
	// Input Gray sequence  [GN, GN-1,...,G0] where GN is the MSB
	// Output Binary sequence [BN, BN-1,...,B0] where BN is the MSB, idx 0 
	// BN = GN
	// BN-1 = GN ^ GN-1
	// BN-2 = GN ^ GN-1 ^ GN-2 = (associative XOR) (GN ^ GN-1) ^ GN-2 = BN-1 ^ GN-2
	// G0 = B1 ^ B0
	int MSB = 0;
	// The MSB is the same
	output[MSB] = input[MSB];
	if (length > 1) {
		// Loop until we finish 
		for (int i = 1; i < length; i++) {
			//output[i] = output[i - 1] ^ input[i]; // Current output is the previous output bit xored with the current input bit
			output[i] = output[i - 1] ^ input[i]; // Current output is the previous output bit xored with the current input bit
		}
	}
}



void My_QAM_Demodulator::Run(const int ntics, const double* inp, int* out, const My_QAM_Modulator* mod) {
	// We run for ntics clock cycles
	int idx = 0;
	int offset = 0;
	int m = mod->m; // Take the modulator parameter
	int M = mod->M;
	int* bin = new int[m]; // m-bits
	int* gray = new int[m]; // m-bits
	double distance, min;
	double sample[2];
	for (int i = 0; i < ntics; i++) {
		// Reset variables (makes it easier to identify control flow problems)
		idx = 0;
		min = DBL_MAX; // start from maximum value
		// Get current sample
		sample[0] = inp[2 * i];
		sample[1] = inp[2 * i + 1];
		// Search for minimum euclidean distance symbol
		for (int j = 0; j < M; j++) { // [0-2*M-1]
			distance = sqrt(pow((mod->constellation[2 * j] - sample[0]), 2) + pow((mod->constellation[2 * j + 1] - sample[1]), 2));
			if (distance < min) { // new minimum
				min = distance;
				idx = j; // store index
			}
		}
		printf("\n");
		printf("Symbol RX: %d\n (%5f, %5f)", idx, sample[0], sample[1]);
		// We identified the index of the minimum distance mapping.
		// Now we extract its m-bits.
		// mapping[idx*m, idx*m + (m-1)], mapping is M*m vector
		// Thus, we want the m-bits in the position from idx*m until idx*m + m - 1
		offset = idx * m;
		for (int j = 0; j < m; j++) {
			gray[j] = mod->mapping[j + offset];
		}

		// We decode the gray mapping
		grayDecode(gray, bin, m);
		printf("Input gray:%d %d.\tOutput sequence: %d %d\n", gray[0], gray[1], bin[0], bin[1]);
		// We copy the binary sequence to the output (N*m)
		for (int j = 0; j < m; j++) {
			out[i * m + j] = bin[j];
		}
	}
	delete[] gray;
	delete[] bin;
}
