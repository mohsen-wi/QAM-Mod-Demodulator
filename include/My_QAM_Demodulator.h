#pragma once
#include "My_QAM_Modulator.h"
class My_QAM_Demodulator
{
public:
	My_QAM_Demodulator(); // Hard decoder (minimum distance)
	virtual ~My_QAM_Demodulator();
	// Set the parameters of the demodulator
	void SetParameters(const My_QAM_Modulator* mod);
	// Run
	void Run(
		const int ntics,
		const double* inp,
		int* out,
		const My_QAM_Modulator* mod
	);

	// Gray to Binary (convert m-bit Gray code to m-bit binary)
	void grayDecode(int* input, int* output, int length);
};
/*
1) User starts by calling new Demodulator
2) Demodulator SetParameter is called with Modulator (to provide constellation and mapping)
3) Demodulator Run (N (number of samples/ticks), input (2*N), out (N*m))
3.a) We receive N symbols (real and immaginary parts)
3.b) We iterate 1 symbol at a time and store the decoded output in a N*m-bit vector until all bits have been demodulated
	3.b.1) We compute the euclidean distance to all constellation points and identify the minimum distance point
	3.b.2) We obtain its index and the corresponding mapping
	3.b.3) We decode the mapped gray code
	3.b.3) We write the binary sequence to the output
*/

