#include "My_QAM_Modulator.h"
#include <stdio.h>
#include <math.h>

My_QAM_Modulator::My_QAM_Modulator()
{
	// Initialize values
	init = false;
	Es = 1; // Unitary energy
	m = 0;
	M = 0;
}


My_QAM_Modulator::~My_QAM_Modulator()
{
	// Delete memory allocations
	delete[] constellation;
	delete[] mapping;
}
int My_QAM_Modulator::SetParameters(const int input_m) {
	if (input_m < 2) {
		printf("Error: m must be an integer greater or equal to 2\n");
		return -1;
	}
	if ((input_m % 2) != 0) {
		printf("Error: m must be even (square QAM constellation)\n");
		return -1;
	}
	m = input_m;
	M = 1;
	for (int i = m; i > 0; i--) { // Fast loop to compute 2^M
		M = M * 2;
	}
	// Allocate constellation and mapping
	// check if SetParameter has already been called once
	// If so, deallocate
	if (init == true) {
		delete[] constellation;
		delete[] mapping;
	}
	constellation = new double[2 * M]; // 2^m = M constellation points, each with real and immaginary parts
	mapping = new int[M * m]; // We have one sequence of m-bits mapped to each of the constellation's symbols
	// Generate constellation
	Generate_Constellation(m, constellation, mapping);
	return 0;
}

void My_QAM_Modulator::grayEncode(int* input, int* output, int length) {
	// Most significant bit is the first bit
	// Input Binary sequence [BN, BN-1,...,B0] where BN is the MSB, idx 0 
	// Output Gray sequence  [GN, GN-1,...,G0] where GN is the MSB
	// GN = BN
	// GN-1 = BN ^ BN-1
	// G0 = B1 ^ B0
	int MSB = 0;
	// The MSB is the same
	output[MSB] = input[MSB];
	if (length > 1) { // 
		// Loop until we finish 
		for (int i = 1; i < length; i++) {
			output[i] = input[i - 1] ^ input[i]; // Current output is the previous input bit xored with the current input bit
		}
	}
}

void My_QAM_Modulator::grayDecode(int* input, int* output, int length) {
	// Most significant bit is the first bit
	// Input Gray sequence  [GN, GN-1,...,G0] where GN is the MSB
	// Output Binary sequence [BN, BN-1,...,B0] where BN is the MSB, idx 0 
	// BN = GN
	// BN-1 = GN ^ GN-1 = BN ^ GN-1
	// BN-2 = GN ^ GN-1 ^ GN-2 = (associative XOR) (GN ^ GN-1) ^ GN-2 = BN-1 ^ GN-2
	// G0 = B1 ^ B0
	int MSB = 0;
	// The MSB is the same
	output[MSB] = input[MSB];
	if (length > 1) { // 
		// Loop until we finish 
		for (int i = 1; i < length; i++) {
			output[i] = output[i - 1] ^ input[i]; // Current output is the previous output bit xored with the current input bit
		}
	}
}

void My_QAM_Modulator::dec2bin(int n, int* mb, int length) {
	// Converts a decimal number to binary
	// We do the flipping even thought without it, the QAM mapping would be flipped without BER difference in the commsys
	int aux, r;
	int i = length - 1; // To have MSB at mb[0]
	aux = n;
	//if (aux == 0) {
	for (int j = 0; j < length; j++) { // Zero the entire vector
		mb[j] = 0;
	}
	//}
	while (aux != 0) {
		r = aux % 2;
		aux = aux / 2;
		mb[i] = r;
		i = i - 1;
	}
	//printf("N: %d , bin: %d\n", n, mb[0]); // m = 2 for our test now
}

void My_QAM_Modulator::Generate_Constellation(const int m, double* constellation, int* mapping) {
	// Compute the scaling factor for QAM constellation energy
	// Constellation is a vector of size 2*M
	// Mapping is a vector of size M*m
	int* m1b = new int[m / 2];
	int* m1g = new int[m / 2];
	int* m2b = new int[m / 2];
	int* m2g = new int[m / 2];
	int idx; // linear index for the constellation

	double* real = new double[M];
	double* imag = new double[M];
	int Mp = 1; // Mp = 2^(m/2) = sqrt(M)
	for (int i = m / 2; i > 0; i--) { // Fast loop for computing power of 2
		Mp = Mp * 2;
	}
	sF = sqrt(3 * Es / (2 * (M - 1)));

	// We only support square QAM modulations
	// Re{Symbol} -> constellation[2*i]
	// Im{Symbol} -> constellation[2*i+1]
	// I, Q mapped independently to gray

	printf("Building constellation.\n");


	// Yet another way: build a M vector and then split into 2*M

	for (int m1 = 0; m1 < Mp; m1++) {
		for (int m2 = 0; m2 < Mp; m2++) {
			// The indexes here should depend both on m1 and m2, because we are working with a linear vector

			idx = m1 * Mp + m2; // this idx only works if we build real and immaginary part separately
			real[idx] = sF * (2 * m1 - Mp + 1);
			imag[idx] = sF * (2 * m2 - Mp + 1);

			// Build the gray map (hardest part) from m1, m2
			// We could convert dec2bin (m/2 bits) and gray code I,Q independently.
			// Then we just copy them to a m-bit vector inside the mapping
			dec2bin(m1, m1b, m / 2);
			grayEncode(m1b, m1g, m / 2);

			dec2bin(m2, m2b, m / 2);
			grayEncode(m2b, m2g, m / 2);

			for (int j = 0; j < m / 2; j++) { // Copy the bits to the mapping
				mapping[idx * m + j] = m1g[j];
				mapping[idx * m + j + m / 2] = m2g[j];
			}

		}
	}
	// Now we need to combine real and immaginary parts into the final constellation
	for (int j = 0; j < M; j++) {
		constellation[2 * j] = real[j];
		constellation[2 * j + 1] = imag[j];
		printf("Symbol %d is (%5f %5fj)\n", j, real[j], imag[j]);
	}
	// Let's print the final mapping

	printf("Mapping:\n");
	for (int k = 0; k < M; k++) {
		for (int l = 0; l < m; l++) {
			printf("%d", mapping[k * m + l]);
		}
		printf("\n");
	}
	printf("\n");
	delete[] real;
	delete[] imag;
}

void My_QAM_Modulator::Run(const int ntics, const int* inp, double* out) {
	// We receive inp, which is a m-bits * ntics vector
	// We run for ntics clock cycles
	int idx = 0;
	int offset = 0;
	int* bin = new int[m]; // m-bits
	int* gray = new int[m]; // m-bits
	int matching = 0; // number of matching bits between current gray code and mapping symbol

	for (int i = 0; i < ntics; i++) {
		// Reset variables (makes it easier to identify control flow problems)
		idx = 0;

		// We extract m-bits of input
		for (int j = 0; j < m; j++) {
			bin[j] = inp[j + offset];
		}
		offset = offset + m; // Move the input by m bits

		// We convert these bits to Gray mapping
		grayEncode(bin, gray, m);

		// We search the mapping for a matching gray code to obtain the index of the constellation symbols

		for (int j = 0; j < M; j++) {
			// Check if mapping is equal to current gray code
			for (int k = 0; k < m; k++) {
				if (gray[k] == mapping[k + j * m]) { // Compare number of equal bits
					matching = matching + 1;
				}
			}
			if (matching == m) { // If all bits match, we exit the loop and store the index of the mapping
				idx = j;
				//printf("Input sequence: %d %d.\t Output gray: %d %d.\n", bin[0], bin[1], gray[0], gray[1]);
				break;
			}
			// if they didn't match, move to the next element
			matching = 0;
		}
		// We output the symbols
		out[2 * i] = constellation[2 * idx]; // Real part of the symbol
		out[2 * i + 1] = constellation[2 * idx + 1]; // Immaginary part of the symbol
		//printf("Symbol TX: %d\n (%5f, %5f)", idx, out[2*i], out[2*i+1]);
	}
	// Clear bitSequence
	delete[] bin;
	delete[] gray;
}
