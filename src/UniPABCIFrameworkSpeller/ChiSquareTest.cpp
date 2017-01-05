#include "ChiSquareTest.h"


ChiSquareTest::ChiSquareTest(double threshold)
{
    Set(threshold);
}

bool ChiSquareTest::test(std::vector<std::vector <int> > count,int x,int y) {
	double chi_square = 0;
	const int m_x_l = x;
	const int m_y_l = y;

	std::vector<double> m_x(m_x_l,0);// margin for x
	std::vector<double> m_y(m_y_l,0);// margin for y
	double m = 0;
	for (int i=0; i<m_x_l; i++) {
		for (int j = 0; j < m_y_l; j++) {
			m_x[i] += count[i][j];
			m += count[i][j];
		}
	}
	for (int j = 0; j < m_y_l; j++) {
		for (int i=0; i<m_x_l; i++) {
			m_y[j] += count[i][j];
		}
	}
	for (int i=0; i<m_x_l; i++) {
		for (int j = 0; j < m_y_l; j++) {
			double e_ij = 1.0 * m_x[i] * m_y[j] / m;
			double o_ij = count[i][j];
			if (e_ij > 0) {
				chi_square += (e_ij - o_ij) * (e_ij - o_ij) / e_ij;
			}
		}
	}
	return chi_square > threshold + 1e-8;
}

ChiSquareTest::~ChiSquareTest()
{
    //dtor
}
