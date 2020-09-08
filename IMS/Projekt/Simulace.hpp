#ifndef IMS_SIMULACE
#define IMS_SIMULACE

#include "simlib.h"
#include "Obce.hpp"

#define POCET_OBCI 6258
#define POCET_OBCI_VELIKOSTI1 3424
#define POCET_OBCI_VELIKOSTI2 2382
#define POCET_OBCI_VELIKOSTI3 321
#define POCET_OBCI_VELIKOSTI4 113
#define POCET_OBCI_VELIKOSTI5 14
#define POCET_OBCI_VELIKOSTI6 4

class Simulace : public Process
{
public:
	/**
	 * Creates work shift process.
	 *
	 * @param cars Number of cars.
	 * @param foodAverage Average number of food to be distributed.
	 * @param foodDeviation Deviation from average number of food.
	 */
	Simulace();

	/**
	 * Desctructs work shift process.
	 */
	~Simulace() override;

	/**
	 * Work shift process behavoir.
	 */
	void Behavior() override;

	double vratMnozstviUhlikuZPapiru();
	double vratMnozstviUhlikuZMobilu();
	double vratMnozstviUhlikuZMobiluSVyrobou();

private:

	Stat *uhlikZPapiruStat;

	double vyprodukovanyUhlikZPapiru;
	double vyprodukovanyUhlikZMobilu;
	double celkvyprodukovanyUhlikZVyrobyMobilu;
};

#endif