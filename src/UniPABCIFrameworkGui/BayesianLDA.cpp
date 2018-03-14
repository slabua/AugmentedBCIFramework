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

#include "BayesianLDA.h"
#include <iostream>
#include <fstream>
#include <gsl/gsl_eigen.h>
#include "rootpaths.h" //SLB

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;


BayesianLDA::BayesianLDA(int p_channels, int p_samples, int p_trial, float** input, int* p_labels, int* p_stimuli, string p_user) {

	n_channels = p_channels;
	n_samples = p_samples;
	n_trial = p_trial;
	n_feature = n_channels*n_samples+1;
	username = p_user;

	x = gsl_matrix_float_alloc(n_feature, n_trial);				
	for (int i=0; i<n_feature; i++)
		if (i!=n_feature-1) {
			for (int j=0;j<n_trial;j++) 
				gsl_matrix_float_set(x,i,j,input[i][j]); 
		}
		else {
			for (int j=0;j<n_trial;j++) 
				gsl_matrix_float_set(x,i,j,1);
		}
				
	yi = gsl_vector_int_alloc(n_trial);

	y = gsl_vector_float_alloc(n_trial);

	stimuli = gsl_vector_int_alloc(n_trial);
	for (int i=0; i<n_trial; i++) {
		gsl_vector_int_set(yi,i,p_labels[i]);
		gsl_vector_int_set(stimuli,i,p_stimuli[i]);
	}

	d_beta = 100000;                    
	d_alpha = 100000;                     
	alpha = 25;                     
	biasalpha = 0.00000001;            
	beta = 1;                      
	stopeps = 0.0001;                
	iteration = 1;                      
	maxit = 500;         
	err = 0;
	gamma = 0;

	e = gsl_vector_float_alloc(n_feature);
	gsl_vector_float_set_all(e,1);
	gsl_vector_float_set(e,n_feature-1,biasalpha);		//aggiungere ultimo valore all'alloc

	vxy = gsl_vector_float_alloc(n_feature);

	evec = gsl_matrix_float_alloc (n_feature,n_feature);
    eval = gsl_vector_float_alloc (n_feature);
	
}

gsl_vector_float* BayesianLDA::vectorXmatrix(gsl_vector_float* a, gsl_matrix_float* b) {
	
	int dim = a->size;
	int n_col=b->size2;
	double prod=0;
	gsl_vector_float* res = gsl_vector_float_alloc(n_col);
	gsl_vector_float_set_all(res,0);

	for (int i=0; i<n_col; i++) {
		for (int j=0;j<dim;j++) 
			prod += gsl_vector_float_get(a,j)*gsl_matrix_float_get(b,j,i);
		gsl_vector_float_set(res,i,prod);
		prod = 0;
	}
	return res;
}

gsl_vector_float* BayesianLDA::matrixXvector(gsl_matrix_float* a, gsl_vector_float* b) {

	int dim = b->size;
	int n_row = a->size1;
	double prod=0;
	gsl_vector_float* res = gsl_vector_float_alloc(n_row);
	gsl_vector_float_set_all(res,0);

	for (int i=0; i<n_row; i++) {
		for (int j=0;j<dim;j++) 
			prod += gsl_matrix_float_get(a,i,j)*gsl_vector_float_get(b,j); 
		gsl_vector_float_set(res,i,prod);
		prod = 0;
	}

	return res;
}

gsl_matrix_float* BayesianLDA::matrixXmatrix(gsl_matrix_float* a, gsl_matrix_float* b) {

	int n_row = a->size1;
	int dim = a->size2;
	int n_col = b->size2;
	double prod=0;
	gsl_matrix_float* res = gsl_matrix_float_alloc (n_row,n_col);
	gsl_matrix_float_set_all(res,0);

	
	for (int i=0; i<n_row; i++) {
		for (int j=0;j<n_col;j++) {
			for (int k=0;k<dim;k++) 
				prod += gsl_matrix_float_get(a,i,k) * gsl_matrix_float_get(b,k,j);
			gsl_matrix_float_set(res,i,j,prod);
			prod = 0;
		}
	}

	return res;
}

gsl_matrix* BayesianLDA::matrixXtmatrix(gsl_matrix_float* a) {

	int n_col = a->size2;
	int n_row = a->size1;
	double prod=0;
	gsl_matrix* res = gsl_matrix_alloc (n_row,n_row);
	gsl_matrix_set_all(res,0);

	
	for (int i=0; i<n_row; i++) {
		for (int j=0;j<n_row;j++) {
			for (int k=0;k<n_col;k++) 
				prod += gsl_matrix_float_get(a,i,k) * gsl_matrix_float_get(a,j,k);
			gsl_matrix_set(res,i,j,prod);
			prod = 0;
		}
	}

	return res;
}

gsl_matrix* BayesianLDA::matrixXmatrix(gsl_matrix* a, gsl_matrix* b) {

	int n_row = a->size1;
	int dim = a->size2;
	int n_col = b->size2;
	double prod=0;
	gsl_matrix* res = gsl_matrix_alloc (n_row,n_col);
	gsl_matrix_set_all(res,0);

	
	for (int i=0; i<n_row; i++) {
		for (int j=0;j<n_col;j++) {
			for (int k=0;k<dim;k++) 
				prod += gsl_matrix_get(a,i,k) * gsl_matrix_get(b,k,j);
			gsl_matrix_set(res,i,j,prod);
			prod = 0;
		}
	}

	return res;
}

void BayesianLDA::compute_eigenvectors_and_eigenvalues() {

	gsl_vector_complex *evalCP;
	gsl_matrix_complex *evecCP;

    evecCP = gsl_matrix_complex_alloc (n_feature,n_feature);
    evalCP = gsl_vector_complex_alloc (n_feature);

	gsl_eigen_nonsymmv_workspace * w = gsl_eigen_nonsymmv_alloc (n_feature);

	gsl_matrix* res = matrixXtmatrix(x);

	gsl_eigen_nonsymmv (res, evalCP, evecCP, w);

	gsl_eigen_nonsymmv_free (w);

	gsl_eigen_nonsymmv_sort (evalCP, evecCP, GSL_EIGEN_SORT_ABS_ASC);


	for (int i=0; i<n_feature; i++) {
		gsl_vector_complex_view evec_i = gsl_matrix_complex_row (evecCP, i);
		gsl_complex eval_i = gsl_vector_complex_get (evalCP, i);
		gsl_vector_float_set(eval,i, GSL_REAL(eval_i));
		for (int j = 0; j < n_feature; ++j) {
			gsl_complex z = gsl_vector_complex_get(&evec_i.vector, j);
			gsl_matrix_float_set(evec,i,j, -GSL_REAL(z));
		}
   }

   gsl_vector_complex_free(evalCP);
   gsl_matrix_complex_free(evecCP);
}

gsl_vector_float* BayesianLDA::get_eigenvalues() {


	return eval;
}

gsl_matrix_float* BayesianLDA::get_eigenvectors() {

	return evec;
}

void BayesianLDA::compute_regression_targets() {

	int n_pos = 0;
	int n_neg = 0;
	ntot = 0;

	for (int i=0; i<yi->size; i++) {
		if (gsl_vector_int_get(yi, i) == 1)
			n_pos++;
		else
			n_neg++;
	}

	ntot = n_pos+n_neg;

	for (int i=0; i<yi->size; i++) {
		if (gsl_vector_int_get(yi, i) == 1)
			gsl_vector_float_set(y,i,(float)ntot/n_pos);
		else
		    gsl_vector_float_set(y,i,-(float)ntot/n_neg);
	}

}

void BayesianLDA::compute_vxy() {
	ifstream file;
	//file.open("./OutputFileLDA/vxy.txt");
	file.open((outputFilesRoot + "LDA/vxy.txt").c_str()); // SLB
	//file.open(outputFilesRoot + "LDA/vxy.txt");
	
	gsl_matrix_float* transEvec = gsl_matrix_float_alloc(evec->size1, evec->size2);
	gsl_matrix_float* temp = gsl_matrix_float_alloc(evec->size1, evec->size2);

	gsl_matrix_float_memcpy(transEvec, evec);
	gsl_matrix_float_transpose(transEvec);
	temp = matrixXmatrix(transEvec, x);

	//cout << temp->size1 << " " << temp->size2 << " " << y->size << endl;
	vxy = matrixXvector(temp, y);
}

gsl_vector_float* BayesianLDA::vecXconst(gsl_vector_float* a, double b) {

	gsl_vector_float* res = gsl_vector_float_alloc(a->size);
	for (int i=0; i<a->size; i++) 
		gsl_vector_float_set(res,i,gsl_vector_float_get(a,i)*b);
	
	return res;
}

gsl_matrix* BayesianLDA::inverse(gsl_matrix* m) {

	int s;
	gsl_matrix* inverse = gsl_matrix_alloc (m->size1,m->size2);
	gsl_permutation* perm = gsl_permutation_alloc (m->size1);

	// Make LU decomposition of matrix m
	gsl_linalg_LU_decomp (m, perm, &s);

	// Invert the matrix m
	gsl_linalg_LU_invert (m, perm, inverse);

	return inverse;

}

gsl_vector_float* BayesianLDA::sumVector(gsl_vector_float* a, gsl_vector_float* b) {

	gsl_vector_float* res = gsl_vector_float_alloc(a->size);
	for (int i=0; i<a->size; i++) 
		gsl_vector_float_set(res,i,(gsl_vector_float_get(a,i)+gsl_vector_float_get(b,i)));

	return res;
}

void BayesianLDA::compute_w() {

	gsl_vector_float* a = vecXconst(eval,beta);		
	
	gsl_vector_float* b = vecXconst(e,alpha);
	
	gsl_vector_float* res = sumVector(a,b);			//d+e
	
	float tmp=0;
	for (int i=0; i<res->size; i++) {					//d+e^-1
	    tmp = gsl_vector_float_get(res,i);
		gsl_vector_float_set(res,i,1/tmp);
	}

	gsl_matrix_float* t = gsl_matrix_float_alloc(evec->size1,evec->size2);
	for (int i=0; i<t->size1; i++)
		for (int j=0; j<t->size2; j++)
			gsl_matrix_float_set(t,i,j,gsl_matrix_float_get(evec,i,j)*beta);

	
	for (int i=0; i<res->size; i++)
		gsl_vector_float_set(res,i,(gsl_vector_float_get(res,i)*gsl_vector_float_get(vxy,i)));

	gsl_vector_float* temp = matrixXvector(t,res);

	w = temp;

	ofstream test;
	username.erase(0, username.find_first_not_of(' '));       //prefixing spaces
	username.erase(username.find_last_not_of(' ')+1);   
	test.open((outputFilesRoot + "/w_"+username+".txt").c_str()); // SLB
	//test.open(outputFilesRoot + "/w_"+username+".txt");
	//for (int i=0; i<201; i++) {
	for (int i = 0; i < res->size; i++) { // SLB // ST
		test << gsl_vector_float_get(w, i) << endl;
	}
}

void BayesianLDA::compute_err() {

	gsl_vector_float* t = vectorXmatrix(w,x);
	err=0;
	for (int i=0; i<t->size; i++) {
		gsl_vector_float_set(t,i,(gsl_vector_float_get(y,i)-gsl_vector_float_get(t,i)));
		gsl_vector_float_set(t,i,(gsl_vector_float_get(t,i)*gsl_vector_float_get(t,i)));
		err += gsl_vector_float_get(t,i);
	}
}

void BayesianLDA::compute_gamma() {

	gamma=0;
	gsl_vector_float* t1 = vecXconst(eval,beta);
	gsl_vector_float* t2 = vecXconst(e,alpha);
	for (int i=0; i<t1->size; i++) {
		gsl_vector_float_set(t2,i,(gsl_vector_float_get(t2,i)+gsl_vector_float_get(t1,i)));
		gsl_vector_float_set(t1,i,(gsl_vector_float_get(t1,i)/gsl_vector_float_get(t2,i)));
		gamma += gsl_vector_float_get(t1,i);
	}
}

void BayesianLDA::compute_alpha(float alphaold) {

	float temp = 0;
	for (int i=0; i<w->size; i++)
		temp += gsl_vector_float_get(w, i) * gsl_vector_float_get(w, i);

	alpha = gamma/temp;
	d_alpha = abs(alpha-alphaold);
}

void BayesianLDA::compute_beta(float betaold) {

	beta = (ntot-gamma)/err;
	d_beta = abs(beta-betaold);
}

void BayesianLDA::train() {

	compute_eigenvectors_and_eigenvalues();
	printmatrixintofile(evec, outputFilesRoot + "LDA/evec.txt");
	printvectorintofile(eval, outputFilesRoot + "LDA/eval.txt");

	ofstream file2;
	file2.open((outputFilesRoot + "LDA/yi.txt").c_str()); // SLB
	//file2.open(outputFilesRoot + "LDA/yi.txt");
	for (int i=0; i<n_trial; i++)
			file2 << gsl_vector_int_get(yi, i) << endl;
	file2.close();

	compute_regression_targets();
	printvectorintofile(y, outputFilesRoot + "LDA/y.txt");

	compute_vxy();
	printvectorintofile(vxy, outputFilesRoot + "LDA/vxy.txt");

	float alphaold = 0;
	float betaold  = 0;

	ofstream filetext;
	filetext.open((outputFilesRoot + "LDA/lda.txt").c_str()); // SLB
	//filetext.open(outputFilesRoot + "LDA/lda.txt");

	while (((d_alpha > stopeps) || (d_beta > stopeps)) && (iteration < maxit)) {
		alphaold = alpha;
		betaold  = beta;
		filetext << "alphaOld: " << alpha << " " << " betaOld: " << beta << endl;
		filetext << "I: " << iteration << endl;
		compute_w();

		for (int i=0; i<n_feature; i++) 
			filetext << gsl_vector_float_get(w, i) << endl;
		filetext << endl << endl;

		compute_err();
		filetext << "ERR: " << err << endl;
		compute_gamma();
		filetext << "GAMMA: " << gamma << endl;
		compute_alpha(alphaold);
		compute_beta(betaold);
		filetext << "alpha: " << alpha << " " << " beta: " << beta << endl;
		//cout << endl << "d_alpha: " << d_alpha << " " << " d_beta: " << d_beta << endl;
		iteration++;
		filetext << endl << endl << endl;
	}

	filetext.close();

}

gsl_vector_float* BayesianLDA::get_w() {

	return w;
}

void BayesianLDA::printvec(gsl_vector_float* v, string nome) {

	cout << endl << nome << endl;
	for (int i=0; i<v->size; i++)
		cout << gsl_vector_float_get(v, i) << endl;
	cout << endl << endl;
}

void BayesianLDA::printmatrix(gsl_matrix_float* m, string nome) {

	cout << endl << nome << endl;
	for (int i=0; i<m->size1; i++) {
		for (int j=0; j<m->size2; j++)
			cout << gsl_matrix_float_get(m, i, j) << " ";
		cout << endl;
	}
	cout << endl << endl;
}

void BayesianLDA::printvectorintofile(gsl_vector_float* vec, string path) {

	ofstream test;
	test.open(path.c_str()); // SLB
	//test.open(path);
	for (int i=0; i<vec->size; i++) 
		test << gsl_vector_float_get(vec, i) << endl;

	test.close();
}

void BayesianLDA::printmatrixintofile(gsl_matrix_float* mat, string path) {

	ofstream test;
	test.open(path.c_str()); // SLB
	//test.open(path);
	for (int i=0; i<mat->size1; i++) {
		for (int j=0; j<mat->size2; j++)
			test << gsl_matrix_float_get(mat, i, j) << endl;
	test << endl;
	}

	test.close();
}

gsl_matrix_float* BayesianLDA::get_evec() {
	
	return evec;
}

gsl_vector_float* BayesianLDA::get_eval() {
	
	return eval;
}

double BayesianLDA::get_alpha() {
	
	return alpha;
}

double BayesianLDA::get_beta() {
	
	return beta;
}

double BayesianLDA::get_dalpha() {
	
	return d_alpha;
}

double BayesianLDA::get_dbeta() {
	
	return d_beta;
}

double BayesianLDA::get_err() {
	
	return err;
}

double BayesianLDA::get_gamma() {
	
	return gamma;
}

double BayesianLDA::get_iteration() {
	
	return iteration; 
}

gsl_vector_float* BayesianLDA::get_vxy() {

	return vxy;
}

