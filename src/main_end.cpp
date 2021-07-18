// Include TOPCOM++ headers of used blocks
#include "Parameter_Manager.h"
#include "PN_Source.h"
#include "Modulator.h"
#include "My_QAM_Modulator.h"
#include "AWGN_Channel.h"
#include "My_QAM_Demodulator.h"
#include "Demodulator.h"
#include "BER_meter.h"

int main()
{
	/* Declaration of global variables								   */
	/***********************************************************/
	double ebn0db;
	int m;
	int N;
	char nameout[132];

	/* Compile-time specified parameters					   */
	/***********************************************************/

	/* Instantiation of blocks NOT affected by users parameter */
	/***********************************************************/

	/* Allocation of buffers NOT affected by users parameter */
	/***********************************************************/

	/* Parameter specified by the user (using Parameter_Manager)*/
	/***********************************************************/
	Parameter_Manager a(40);
	FILE *input;
	char nameinput[132] = "input.txt";
	input = fopen(nameinput, "r+");
	if (input == NULL)input = fopen(nameinput, "w+");

	 a.Add_Parameter_From_File(input,"Name output file",nameout);	
	 a.Add_Parameter_From_File(input,"Frame size (symbols)",&N);
	 a.Add_Parameter_From_File(input,"Eb/N0 [dB]",&ebn0db);
	 a.Add_Parameter_From_File(input,"Number of modulation bits",&m);
	// ...
	fclose(input);
	if (a.error)
	{
		printf("Main: The file %s was not ready. Fill up all the required parameters.\n", nameinput);
		return 1;
	}

	FILE* file = fopen(nameout, "w");

	/***********************************************************/
	/* Parameters loop  */
	/***********************************************************/
	do // 
	{
		a.Print_Parameters();

		/* Compute derived parameters */
		/***********************************************************/

		/* Instantiation of blocks affected by users parameter    */
		/***********************************************************/
		PN_Source* Source	= new PN_Source;

		//Modulator* Mod = QAM_Modulator(m);
	
		AWGN_Channel* AWGN	= new AWGN_Channel;
		AWGN->Set_EsN0dB(ebn0db+10.*log10((double)m));

		//Demodulator* Demod	= new Demodulator;
		//Demod->SetParameters(Mod);

		BER_meter* BER	= new BER_meter;
		BER->SetParameters(0, 30);  

		My_QAM_Modulator* My_Mod = new My_QAM_Modulator();
		My_Mod->SetParameters(m);
		
		My_QAM_Demodulator* My_Demod = new My_QAM_Demodulator;
		
		


		/* Allocation of buffers affected by users parameter	   */
		/***********************************************************/
		int* data	= new int	 [N*m];
		double* tx = new double[2 * N];
		double* rx = new double[2 * N];
		int* dataRX	= new int	 [N*m];

		/* Initializations										   */
		/***********************************************************/

		/***********************************************************/
		/* Simulation loop										   */
		/***********************************************************/
		int f;
		for (f = 0;;f++)
		{
			Source	->Run(N*m, data);
			//Mod		->Run(N, data, tx);
			My_Mod  ->Run(N, data, tx);
			AWGN	->Run(2*N, tx, rx);
			//Demod	->Run(N, rx, dataRX);
			My_Demod->Run(N, rx, dataRX, My_Mod);
			BER->Run(N*m, data, dataRX);
			if (BER->IsReliable())break;
		}

		/* Post processing single run							   */
		/***********************************************************/
		BER->Display();

		a.Print_Parameters_on_Line(file);
		BER->Display_on_Line(file);
		fprintf(file, "\n");
		fprintf(file, "\n");
		fflush(file);

		//BER[0]->Display(outfile);
		//..

		/* Delete blocks and buffers affected by users parameter   */
		/***********************************************************/
		delete Source;
		delete AWGN;
		delete BER;
		delete[] data;
		delete[] tx;
		delete[] rx;
		delete[] dataRX;
		//delete Mod;
		//delete Demod;
		delete My_Mod;
		delete My_Demod;

		//...
	} while (a.Update_Parameters());

		/* Global Post processing								   */
		/***********************************************************/

		/* Delete blocks and buffers NOT affected by users parameter   */
		/***********************************************************/
		return 0;
}
