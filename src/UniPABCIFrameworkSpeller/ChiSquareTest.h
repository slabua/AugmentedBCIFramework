
#ifndef CHISQUARETEST_H
#define CHISQUARETEST_H

#include <vector>


/*ChiSquare si occupa di effettuare la sogliatura dell'albero per ridurre il problema del sovraddatamento della rete, le
soglie ottimali sono:(dal piu alto al piu basso)
/**
	 * Chi-square test with threshold .001.
			* ChiSquareTest(16.27);

	 * Chi-square test with threshold .01.
			ChiSquareTest(11.34);

	 * Chi-square test with threshold .5
	         ChiSquareTest(7.82)
*/
class ChiSquareTest
{
    public:
        ChiSquareTest(double threshold);
        virtual ~ChiSquareTest();
        double Get() { return threshold; }
        void Set(double val) { threshold = val; }
       bool test(std::vector<std::vector <int> > count,int x,int y);
		// bool test(std::vector<std::vector <int> > count,int x,int y);
    protected:
    private:
        double threshold;
};

#endif // CHISQUARETEST_H
