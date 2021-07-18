#pragma once
#include <stdio.h>
class My_BER_Meter
{
public:  //you access to all of them (place the method and in the private section put the data) 
	My_BER_Meter(); //constructor method  (when want to create the object)
	~My_BER_Meter(); //deconstructor method (delete the object)

	//! Set the BER parameters
	void SetParameters(
		const int delay,	//!< Delay of second stream 
		const int n = 30	//!< Number of errors to declare realiable results (we take the value 30 if we don't specify it during the code)
		);

	//! Compare two bit streams. Returns 1 if errors are present (the most important part)
	int Run1(
		const int sizein,		//!< Number of compared bits. 
		const int* ref,			//!< First stream (source). (pointer to the buffer contain the bits)
		const int* decoded		//!< Second bit stream (decoded). (pointer to storage)(can not modify the data)(two input signal no output)
		);

	int Run(
		const int sizein,		//!< Number of compared bits. 
		const int* ref,			//!< First stream (source). (pointer to the buffer contain the bits)
		const int* decoded		//!< Second bit stream (decoded). (pointer to storage)(can not modify the data)(two input signal no output)
	);

	bool IsReliable();	//!< Return true if enough bits or frame errors have been counted.
	void Reset();		//!< Reset the error counters (no parameter)

	//! Display the BER statistics on specified output.
	void Display(FILE* stream = stdout //!< Output stream
		);

	//! Display the BER statistics on a single line of the specified output.
	void Display_on_Line(FILE* stream = stdout);

private: // Place here variable of the class (store the data(object))
	int nerr;		//!< Number of counted errors
	int nbits;	    //!< Number of counted bits
	int nmin;       //!< Minimum number of error to declare reliable the measure
	int delay;       //!< delay of the second stream

	int* line;		//! delay line (pointer to the space(not declare it now))
	int  pointer;	//! Pointer within the delay line
	bool count;		//! Flag for counting
};



