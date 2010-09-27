
#include <stdio.h>
// #include "io_error.h"
#include "m_matrix.h"


int main(){


	BSMATRIX<double> a;
	BSMATRIX<double> lu;

	a.reinit(3);
	lu.reinit(3);

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


	a.allocate();
	lu.allocate();

	a.load_point(1,2, 1.0);
	a.load_point(2,1, 1.0);
	a.load_point(1,1, 5.0);
	a.load_point(2,2, 5.0);
	a.load_point(3,3, 1.0);

	std::cout << "a: " << a << "\n";


	// a.set_min_pivot(0.0);


	//a.lu_decomp();

	double v[4] = {1,1,1,0};
	double x[4] = {1,1,1,0};

	std::cout << "v: "<< v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";

	std::cout << "lu: " << lu << "\n";
	lu.lu_decomp(a, false);
	std::cout << "lu: " << lu << "\n";

//	lu.fbsub(x,v,x);
	lu.fbsub(v-1);

	std::cout << "v: " << v[0] << ", " << v[1] << ", " << v[2]<< ", " << v[3] << "\n";
//	std::cout << "x: " << x[0] << ", " << x[1] << ", " << x[2]<< ", " << x[3] << "\n";
}
