
#include <iostream>
#include "Simulace.hpp"


using namespace std;


Simulace::Simulace()
{
	uhlikZPapiruStat = new Stat("Uhlik vyprodukovany pri pouziti papiru");
	vyprodukovanyUhlikZPapiru = 0;
	vyprodukovanyUhlikZMobilu = 0;
	celkvyprodukovanyUhlikZVyrobyMobilu = 0;
}


Simulace::~Simulace()
{

	delete uhlikZPapiruStat;
}


void Simulace::Behavior()
{

	int uhlikZPapiru, uhlikZMobilu;
	Obce *obce[POCET_OBCI];

	int i = 0;
	int pocet_obci_podminka = POCET_OBCI_VELIKOSTI1;
	for(i = 0; i < pocet_obci_podminka; i++){
		obce[i] = new Obce(i, uhlikZPapiruStat, 5,15);
		obce[i]->Activate();

	} 
	pocet_obci_podminka += POCET_OBCI_VELIKOSTI2;
	for(; i < pocet_obci_podminka; i++){
		obce[i] = new Obce(i, uhlikZPapiruStat, 7,15);
		obce[i]->Activate();

	} 
	pocet_obci_podminka += POCET_OBCI_VELIKOSTI3;
	for(; i < pocet_obci_podminka; i++){
		obce[i] = new Obce(i, uhlikZPapiruStat, 11,25);
		obce[i]->Activate();

	} 
	pocet_obci_podminka += POCET_OBCI_VELIKOSTI4;
	for(; i < pocet_obci_podminka; i++){
		obce[i] = new Obce(i, uhlikZPapiruStat, 15,35);
		obce[i]->Activate();

	} 
	pocet_obci_podminka += POCET_OBCI_VELIKOSTI5;
	for(; i < pocet_obci_podminka; i++){
		obce[i] = new Obce(i, uhlikZPapiruStat, 25,45);
		obce[i]->Activate();

	} 
	pocet_obci_podminka += POCET_OBCI_VELIKOSTI6;
	for(; i < pocet_obci_podminka; i++){
		obce[i] = new Obce(i, uhlikZPapiruStat, 35,55);
		obce[i]->Activate();

	} 
	Wait(365);
	int papiry = 0;
	for(i = 0; i < POCET_OBCI; i++){
		papiry += obce[i]->vratPocetPapiru();
		this->celkvyprodukovanyUhlikZVyrobyMobilu += obce[i]->vratMnozstviUhlikuZMobiluSVyrobou();
		this->vyprodukovanyUhlikZPapiru += obce[i]->vratMnozstviUhlikuZPapiru();
		this->vyprodukovanyUhlikZMobilu += obce[i]->vratMnozstviUhlikuZMobilu();
	} 
	int pocet_zachranenych_stromu = papiry/80000;
	int uhlik_vyprodukovany_ztratou_stromu = pocet_zachranenych_stromu * 22000;
	this->vyprodukovanyUhlikZPapiru+=0;
	this->vyprodukovanyUhlikZPapiru += uhlik_vyprodukovany_ztratou_stromu;
	printf("Pocet zachranenych stromu: %d\n", pocet_zachranenych_stromu);

}

double Simulace::vratMnozstviUhlikuZPapiru(){
	return this->vyprodukovanyUhlikZPapiru;
}

double Simulace::vratMnozstviUhlikuZMobilu(){
	return this->vyprodukovanyUhlikZMobilu;
}

double Simulace::vratMnozstviUhlikuZMobiluSVyrobou(){
	return this->celkvyprodukovanyUhlikZVyrobyMobilu;
}

