
#include <iostream>
#include "Obce.hpp"
//#include "Casovac.cc"
//#include "CasovacZasedani.cc"
#include <math.h> 


using namespace std;


Obce::Obce(int i, Stat *uhlikZPapiruStat, int min, int max)
{


	this->pocet_zastupitelu = min;
	while(this->pocet_zastupitelu != max){
		double r=Random();
		if(r < 0.2){
			break;
		}
		this->pocet_zastupitelu++;
	}
	this->index_obce = i;

	this->uhlikZPapiruStat = uhlikZPapiruStat;
	this->vyprodukovanyUhlikZMobilu = 0;
	this->vyprodukovanyUhlikZPapiru = 0;
	this->vyprodukovanyUhlikZVyrobyMobilu = 0;
	this->skonci = 0;

	this->papiry = 0;
}


Obce::~Obce()
{
	this->skonci = 1;
}


void Obce::Behavior()
{

	int pocetPritomnych = 0;
	int pocetPapiru = 0;
	int pocetTelefonu = pocet_zastupitelu;
	int doba=0;
	this->vyprodukovanyUhlikZVyrobyMobilu = ((pocetTelefonu * (44000/2)));
	while(this->skonci != 1){
		pocetPritomnych = this->vratPocetPritomnych();
		//printf("POCET PRITOMNYCH: %d\n", pocetPritomnych);
		doba = ((int)ceil(Random() * 4))*7;

		Wait(doba); //v tydnech

		pocetPapiru = ceil(Random() * 4) + 1;
		this->papiry += (pocetPapiru*pocetPritomnych);
		this->pridatUhlikZPapiru(pocetPritomnych, pocetPapiru);
		/*
		printf("Zastupitele jsou na zasedani\n");
		printf("Zastupitele skoncili zasedani\n");*/
	}


}


void Obce::pridatUhlikZPapiru(int pocet_pritomnych_zastupitelu, int pocetPapiru){
	this->vyprodukovanyUhlikZPapiru += (pocet_pritomnych_zastupitelu*pocetPapiru*60); // v gramech
	this->vyprodukovanyUhlikZMobilu += (pocet_pritomnych_zastupitelu*530); // za jedmu hodinu v gramech
	//printf("Doposud vyprodukovany uhlik obce %d: %f kilogramu\n", index_obce, vyprodukovanyUhlikZPapiru );
}

int Obce::vratPocetPritomnych(){
	int pocet = 0;
	for(int i = 0; i < pocet_zastupitelu; i++){
		if(Random() < 0.9){
			pocet++;
		}
	}
	return pocet;
}

double Obce::vratMnozstviUhlikuZPapiru(){
	return vyprodukovanyUhlikZPapiru;
}

double Obce::vratMnozstviUhlikuZMobilu(){
	return vyprodukovanyUhlikZMobilu;
}

double Obce::vratMnozstviUhlikuZMobiluSVyrobou(){
	return vyprodukovanyUhlikZMobilu+vyprodukovanyUhlikZVyrobyMobilu;
}


int Obce::vratPocetPapiru(){
	return this->papiry;
}


