
#ifndef MMATRIXEXTRA__
#define MMATRIXEXTRA__
#include "m_matrix.h"
#include <complex>
using namespace std;

template<class T>
BSMATRIX<T>::BSMATRIX(const BSMATRIX<T>& m) :
	_changed(new bool[m.size()+1]),
	_lownode(new unsigned[m.size()+1]),
	_space(new T[m._nzcount]),
	_rowptr(new T*[m._size+1]),
	_colptr(new T*[m._size+1]),
	_diaptr(new T*[m._size+1]),
	_nzcount(m._nzcount),
	_size(m._size),
	_zero(0),
	_trash(m._trash),
	_min_pivot(m._min_pivot)
{
	memcpy(_changed,m._changed, (_size+1) * sizeof(bool) );
	memcpy(_lownode,m._lownode, (_size+1) * sizeof(unsigned) );
	memcpy(_space,m._space, (_nzcount)  * sizeof(T));

	for(unsigned i=0; i<=_size; i++){
		_rowptr[i]= _space - intptr_t(m._space - m._rowptr[i]);
		_colptr[i]= _space - intptr_t(m._space - m._colptr[i]);
		_diaptr[i]= _space - intptr_t(m._space - m._diaptr[i]);
	}

}

/*-----------------------------------*/
template<>
inline BSMATRIX<double>::BSMATRIX(BSMATRIX<double>::REAL, const
		BSMATRIX<complex<double> >& m ) :
	_changed(new bool[m.size()+1]),
	_lownode(new unsigned[m.size()+1]),
	_space(new double[m._nzcount]),
	_rowptr(new double*[m._size+1]),
	_colptr(new double*[m._size+1]),
	_diaptr(new double*[m._size+1]),
	_nzcount(m._nzcount),
	_size(m._size),
	_zero(0.)
{
	memcpy(_changed,m._changed, (_size+1) * sizeof(bool) );
	memcpy(_lownode,m._lownode, (_size+1) * sizeof(unsigned) );

	_trash = abs (m._trash);
	_min_pivot = abs(m._min_pivot) ;
	_space = new double[_nzcount];
	for(unsigned i=0; i<_nzcount; i++ ){
		_space[i]=(m._space[i]).real();
	}

	for(unsigned i=0; i<_size+1; i++){
		_rowptr[i]= _space - intptr_t(m._space - m._rowptr[i]);
		_colptr[i]= _space - intptr_t(m._space - m._colptr[i]);
		_diaptr[i]= _space - intptr_t(m._space - m._diaptr[i]);
	}
}
/*-----------------------------------*/
template<>
inline BSMATRIX<double>::BSMATRIX(BSMATRIX<double>::IMAG, const
		BSMATRIX<complex<double> >& m ) :
	_changed(new bool[m.size()+1]),
	_lownode(new unsigned[m.size()+1]),
	_rowptr(new double*[m._size+1]),
	_colptr(new double*[m._size+1]),
	_diaptr(new double*[m._size+1]),
	_nzcount(m._nzcount),
	_size(m._size),
	_zero(0.)
{
	memcpy(_changed,m._changed, (_size+1) * sizeof(bool) );
	memcpy(_lownode,m._lownode, (_size+1) * sizeof(unsigned) );

	_trash = abs (m._trash);
	_min_pivot = abs(m._min_pivot) ;
	_space = new double[_nzcount];
	for(unsigned i=0; i<_nzcount; i++ ){
		_space[i]=(m._space[i]).imag();
	}

	for(unsigned i=0; i<_size+1; i++){
		_rowptr[i]= _space - intptr_t(m._space - m._rowptr[i]);
		_colptr[i]= _space - intptr_t(m._space - m._colptr[i]);
		_diaptr[i]= _space - intptr_t(m._space - m._diaptr[i]);
	}
}
/*-----------------------------------*/
template<>
inline BSMATRIX<double>::BSMATRIX(BSMATRIX<double>::SUM, const
		BSMATRIX<complex<double> >& m ) :
	_changed(new bool[m.size()+1]),
	_lownode(new unsigned[m.size()+1]),
	_space(new double[m._nzcount]),
	_rowptr(new double*[m._size+1]),
	_colptr(new double*[m._size+1]),
	_diaptr(new double*[m._size+1]),
	_nzcount(m._nzcount),
	_size(m._size),
	_zero(0.)
{
	memcpy(_changed,m._changed, (_size+1) * sizeof(bool) );
	memcpy(_lownode,m._lownode, (_size+1) * sizeof(unsigned) );

	_trash = abs (m._trash);
	_min_pivot = abs(m._min_pivot) ;
	_space = new double[_nzcount];
	for(unsigned i=0; i<_nzcount; i++ ){
		_space[i]= (m._space[i]).real() + (m._space[i]).imag() ;
	}

	for(unsigned i=0; i<_size+1; i++){
		_rowptr[i]= _space - intptr_t(m._space - m._rowptr[i]);
		_colptr[i]= _space - intptr_t(m._space - m._colptr[i]);
		_diaptr[i]= _space - intptr_t(m._space - m._diaptr[i]);
	}
}
/*-----------------------------------*/
template<class T>
inline T* BSMATRIX<T>::row(T* x, unsigned n)
{
	for(unsigned i=0; i<=_size; ++i)
	  x[i]=s(n,i);	
	return x;
}
/*-----------------------------------*/
template<class T>
inline T* BSMATRIX<T>::col(T* x, unsigned n)
{
	for(unsigned i=0; i<=_size; ++i)
	  x[i]=s(i,n);	
	return x;
}
/*-----------------------------------*/

//{}
template<class T>
inline T*  BSMATRIX<T>::rmul(T* b, const T* x)const{
// probably possible to exploit sparse structure here
	for(unsigned i=0; i<=_size; ++i){
		b[i]=0;
		for(unsigned j=0; j<=_size; ++j){
			b[i] += s(i,j) * x[j];
		}
	}

	return b;
}
#endif
