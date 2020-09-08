#include "ims.hpp"

using namespace std;

void zacatek();
void konec();

int main(int argc, char *argv[])
{


	Stat *uhlikZPapiruStat = new Stat("Uhlik vyprodukovany pri pouziti papiru [tuna]");
	Stat *uhlikZMobiluStat = new Stat("Uhlik vyprodukovany pri pouziti telefonu [tuna]");
	Stat *uhlikZMobiluSVyrobouStat = new Stat("(pri zahrnuti vyroby telefonu) [tuna]");
	double uhlikZPapiru, uhlikZMobilu, uhlikZMobiluSVyrobou;
	RandomSeed(time(NULL));

	Simulace *simulace;

	zacatek();
	for (int i = 0; i < 6; i++)
	{
		cout << "Simulace cislo: " << i << ".\n";

		Init(SIMULATION_START_TIME, SIMULATION_END_TIME);

		simulace = new Simulace();
		simulace->Activate();
		Run();

		uhlikZPapiru = simulace->vratMnozstviUhlikuZPapiru()/1000000;
		uhlikZMobilu = simulace->vratMnozstviUhlikuZMobilu()/1000000000000;
		uhlikZMobiluSVyrobou = simulace->vratMnozstviUhlikuZMobiluSVyrobou()/1000000;
		if(uhlikZMobiluSVyrobou == 0) continue;
		(*uhlikZPapiruStat)(uhlikZPapiru);
		(*uhlikZMobiluStat)(uhlikZMobilu);
		(*uhlikZMobiluSVyrobouStat)(uhlikZMobiluSVyrobou);

	}
	
	uhlikZPapiruStat->Output();
	uhlikZMobiluStat->Output();
	uhlikZMobiluSVyrobouStat->Output();
	konec();
	return EXIT_SUCCESS;
}

void zacatek(){	
	cout << "+----------------------------------------------------------+\n"
		<< "ZACATEK SIMULACE\n"
		<< "+----------------------------------------------------------+\n"
		<< endl;
}


void konec(){
	cout << "+----------------------------------------------------------+\n"
		<< "SIMULACE UKONCENA\n"
		<< "+----------------------------------------------------------+\n"
		<< endl;
}