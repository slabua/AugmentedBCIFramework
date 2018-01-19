/*****************************************************************************
* 
* UniPA BCI Framework: an Augmented BCI Framework
* 
* Copyright (C) 2016-2017 Salvatore La Bua (slabua@gmail.com)
* RoboticsLab, Department of Industrial and Digital Innovation (DIID),
* Universita' degli Studi di Palermo, V. delle Scienze, Palermo, Italy.
* 
* http://www.slblabs.com/projects/unipa-bci-framework
* https://github.com/slabua/UniPABCIFramework
* 
* Based on a previous work -P300 core- of (in alphabetical order):
* Rosario Misuraca, Walter Tranchina, Giuseppe Trubia.
* 
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
*****************************************************************************/

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
