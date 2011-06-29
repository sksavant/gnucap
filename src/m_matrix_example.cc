
#include <stdio.h>
#include <complex>
// #include "io_error.h"
#include "m_matrix_extra.h"
#include "io_matrix.h"
#include "m_matrix.h"

using namespace std;

int main(){

	BSMATRIX<double> a;
	BSMATRIX<double> lu;

	trace0("have bsmatrices.");

	a.reinit(3);
	lu.reinit(3);
	trace0("done init.");

	a.iwant(1,1);
	a.iwant(1,2);
	a.iwant(2,1);
	a.iwant(2,2);
	a.iwant(3,3);

	lu.iwant(1,1);
	lu.iwant(1,2);
	lu.iwant(2,1);
	lu.iwant(2,2);
	lu.iwant(3,3);
	trace0("done iwant");

	a.allocate();
	lu.allocate();
	trace0("done alloc");

	a.load_point(1,2, 1.0);
	a.load_point(2,1, 1.0);
	a.load_point(1,1, 5.0);
	a.load_point(2,2, 5.0);
	a.load_point(3,3, 1.0);
	trace0("done load");

	cout << "a: " << a << "\n";

	a.set_min_pivot(0.0);
	//a.lu_decomp();

	double v[4] = {0,1,1,1};
	double w[4] = {1,1,1,0};

	cout << "v: "<< v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";

	cout << "lu: " << lu << "\n";
	lu.lu_decomp(a, false);
	cout << "lu: " << lu << "\n";

	lu.fbsub(w,v,w);
	//lu.fbsub(v-1);

	cout << "v: " << v[0] << ", " << v[1] << ", " << v[2]<< ", " << v[3] << "\n";
	cout << "w: " << w[0] << ", " << w[1] << ", " << w[2]<< ", " << w[3] << "\n";

	BSMATRIX<double>* C = a.copy();

	cout << "orig\n" << a << "\n";
	cout << "copy\n" << *C << "\n";


	BSMATRIX<complex<double> > c;

	c.reinit(3);
	c.iwant(1,1);
	c.iwant(1,2);
	c.iwant(2,1);
	c.iwant(2,2);
	c.iwant(3,3);
	c.allocate();
	c.load_point(3,3, 0.1);
	c.load_point(1,2, 1.0);
	c.load_point(1,1, 1.0);
	complex<double> mycomplex (2.0,2.0);
	c.load_point(2,2, mycomplex);

	BSMATRIX<double> R = c.real();
	// BSMATRIX<double> R( BSMATRIX<double>::_REAL, c);

	cout << "cplx\n" << c << "\n";
	cout << "real\n" << c.real() << "\n";

	double* b = new double[4];
	double x[4] = {0,2,1,5};

	cout << "x: " << x[0] << ", " << x[1] << ", " << x[2]<< ", " << x[3] << "\n";
	b = R.rmul(b,x);
	cout << "Rx=b: " << b[0] << ", " << b[1] << ", " << b[2]<< ", " << b[3] << "\n";

	R.lu_decomp();
	R.fbsub(b);
	cout << "b: " << b[0] << ", " << b[1] << ", " << b[2]<< ", " << b[3] << "\n";

}
