
#include <stdio.h>
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

	// a.set_min_pivot(0.0);
	//a.lu_decomp();

	double v[4] = {1,1,1,0};
	double x[4] = {1,1,1,0};

	cout << "v: "<< v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";

	cout << "lu: " << lu << "\n";
	lu.lu_decomp(a, false);
	cout << "lu: " << lu << "\n";

//	lu.fbsub(x,v,x);
	//lu.fbsub(v-1);

	cout << "v: " << v[0] << ", " << v[1] << ", " << v[2]<< ", " << v[3] << "\n";
	cout << "x: " << x[0] << ", " << x[1] << ", " << x[2]<< ", " << x[3] << "\n";

	BSMATRIX<double>* C = a.copy();

	cout << "orig\n" << a << "\n";
	cout << "copy\n" << *C;
}
