#ifndef IMS_OBCE
#define IMS_OBCE

#include "simlib.h"


class CasovacZasedani;
class Obce : public Process
{
public:
	/**
	 * Creates work shift process.
	 *
	 * @param cars Number of cars.
	 * @param foodAverage Average number of food to be distributed.
	 * @param foodDeviation Deviation from average number of food.
	 */
	explicit Obce(int i, Stat *uhlikZPapiruStat, int min, int max);

	/**
	 * Desctructs work shift process.
	 */
	~Obce() override;

	/**
	 * Work shift process behavoir.
	 */
	void Behavior() override;


	
	int releaseFac(Facility *fac);
	double vratMnozstviUhlikuZPapiru();
	double vratMnozstviUhlikuZMobilu();
	double vratMnozstviUhlikuZMobiluSVyrobou();
	
	int vratPocetPapiru();

	int pocet_zastupitelu;

	int index_obce;

private:

	double vyprodukovanyUhlikZPapiru;
	double vyprodukovanyUhlikZMobilu;
	double vyprodukovanyUhlikZVyrobyMobilu;

	int papiry;

	Stat *uhlikZPapiruStat;

	void pridatUhlikZPapiru(int pocet_pritomnych_zastupitelu, int pocetPapiru);
	int vratPocetPritomnych();
	int skonci;
};
#endif