#include "My_BER_Meter.h"

My_BER_Meter::My_BER_Meter()
{
	nerr = 0;
	nbits = 0;
	line = 0;
	nmin = 0;
	delay = 0;
}

My_BER_Meter::~My_BER_Meter()
{
	delete[] line;
}

void My_BER_Meter::SetParameters(const int delay, const int n)
{
	this->delay = delay;//because of ambigurity use the this
	nmin = nmin;
	delete[] line;//I call the deconstructor which is not a good way but it work (destroy previous allocation)
	line = 0;
	line = new int[delay];//I create memory in here and I should delete it in deconstructor
	pointer = 0;
	Reset();
}

int My_BER_Meter::Run(const int sizein, const int* ref, const int* decoded)
{
	int i,j;
	for (i = 0; i < sizein; i++) {

		line[pointer] = ref[i];
    	pointer++;
			if (pointer == delay)
				pointer = 0;//pointer=
		nerr += decoded[i] ^ line[delay - 1]; // ^ -> xor
		nbits++;
	}

	return 0;
}

int My_BER_Meter::Run1(const int sizein, const int* ref, const int* decoded)
{
	int i, j;
	for (i = 0; i < sizein; i++) {
		for (j = delay - 1; j > 0; j--) {
			line[j] = line[j - 1];
		}
		line[0] = ref[i];//input at time i
		nerr += decoded[i] ^ line[delay - 1]; // ^ -> xor
		nbits++;
	}

	return 0;
}

bool My_BER_Meter::IsReliable()
{
	if (nerr >= nmin)return true;
	else
	return false;
}

void My_BER_Meter::Reset()
{
	nbits = 0;
	nerr = 0;
}

void My_BER_Meter::Display(FILE* stream)
{
	fprintf(stream, "%d\t%d\tBER = %e\n", nerr, nbits, (double)nerr / nbits);
}

void My_BER_Meter::Display_on_Line(FILE* stream)
{
}
