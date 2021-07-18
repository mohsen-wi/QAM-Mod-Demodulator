#pragma once

class My_QAM_Modulator
{
public:
	My_QAM_Modulator();
	~My_QAM_Modulator();
	// Set parameters. We only support square QAM modulations so we only receive the constellation cardinality
	// We work only with gray mapping
	int SetParameters(
		const int input_m // Constellation order (m = log2(M))
	);
	// Run the modulator
	void Run(const int ntics, // Number of input symbols (clock cycles)
		const int* inp, // Input bits or symbols
		double* out // Output symbols (double vector with real and imag parts)
	);

	// Generate constellation (we do it when SetParameters is called)
	void Generate_Constellation(const int m, double* constellation, int* mapping);

	// Binary to Gray (convert a m-bit sequence to Gray code)
	void grayEncode(int* input, int* output, int length);
	// Gray to Binary (convert m-bit Gray code to m-bit binary)
	void grayDecode(int* input, int* output, int length);
	// Decimal to binary
	void dec2bin(int n, int* mb, int length);


	// Variables
	double* constellation; // Our QAM constellation
	int* mapping; // mapping of bits to our constellation points (symbols)
	int m; // constellation order (m = log2(M))
	int M; // our constellation cardinality
	double Es; // Symbol energy
	double sF; // scaling factor
	bool init; // Flag to control whether we already have allocated the buffers (calling delete on an unallocated buffer is undefined behavior)

private:


};