
#include "m_matrix.h"
#include "io_.h"
#include <complex>

using namespace std;

/* template <>
ostream& operator<<( ostream& o, const BSMATRIX<double> &m)
{
  std::string s;
  s = "Matrix of size ";
  int size=m.size();
  o << s << m.size();
  o << " space " << m._nzcount;
  o << "\n";
  int i,j;
  double x;

  for(i = 1; i <=size ; i++ ){
    for(j = 1; j <=size ; j++ ){
      x = ((m).s(i,j));
      o << x << " ";
    }
    if (i<size ) o << "\n";
  }
  return o;
}

template <>
OMSTREAM& operator<<( OMSTREAM& o, const BSMATRIX<double> &m)
{
  std::string s;
  s = "Matrix of size ";
  int size=m.size();
  o << s << m.size();
  o << " space " << m._nzcount;
  o << "\n";
  int i,j;
  double x;

  for(i = 1; i <=size ; i++ ){
    for(j = 1; j <=size ; j++ ){
      x = ((m).s(i,j));
      o << x << " ";
    }
    if (i<size ) o << "\n";
  }
  return o;
}

*/

//inline OMSTREAM& operator<<( OMSTREAM& o, const std::complex<double> &c){
//	return (o << c.real() << "+" << c.imag() << "I"  );
//}

template <class T, class S>
inline S& operator<<( S& o, const BSMATRIX<T> &m)
{
  std::string s;
  s = "Matrix of size ";
  int size=m.size();
  o << s << m.size();
  o << " space " << m._nzcount;
  o << "\n";
  int i,j;
  T x;

  for(i = 1; i <=size ; i++ ){
    for(j = 1; j <=size ; j++ ){
      x = ((m).s(i,j));
      o << x << " ";
    }
    if (i<size ) o << "\n";
  }
  return o;
}


template <class T>
inline OMSTREAM& operator<<( OMSTREAM& o, const BSMATRIX<T> &m)
{
  std::string s;
  s = "Matrix of size ";
  int size=m.size();
  o << s << m.size();
  o << " space " << m._nzcount;
  o << "\n";
  int i,j;
  T x;

  for(i = 1; i <=size ; i++ ){
    for(j = 1; j <=size ; j++ ){
      x = ((m).s(i,j));
      o << x << " ";
    }
    if (i<size ) o << "\n";
  }
  return o;
}
