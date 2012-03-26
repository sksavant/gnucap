/* * Copyright (C) 2012 Felix Salfelder
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * fftw interface and helpers
 */

#ifndef MFFT_HH
#ifdef HAVE_FFTW_H
#define MFFT_HH

#include <rfftw.h> 

class spec_const_iterator{
	private:
		fftw_real *_r, *_i;
	public:
		spec_const_iterator(fftw_real* r, fftw_real* i):
			_r(r), _i(i){}
		COMPLEX operator*(){ return COMPLEX(*_r,*_i); }
		void operator++(){ _r++; _i--; };
		bool operator==(const spec_const_iterator& x) const
		{ return _r == x._r; }
};
/*-------------------------------------------------------------------------------*/
class spec_container{
		fftw_real * const _data;
		const size_t _N;
		const size_t _n;

	public:
		spec_container( fftw_real* const out, unsigned N):
		_data(out), _N(N), _n(N/2+1) { };

		spec_const_iterator begin()const
		{ return( spec_const_iterator(_data,_data+_N-1) ); }
		spec_const_iterator end()const
		{ return( spec_const_iterator(_data+_n , 0 ) ); }
		COMPLEX operator[](size_t x){ return *(spec_const_iterator(_data+x,_data+_N-1-x )) ; }

		size_t size()const{return _n;}
		COMPLEX interpolate(double x);
};
/*-------------------------------------------------------------------------------*/
inline COMPLEX spec_container::interpolate(double x)
{
	assert(x>=0);

	size_t lower = (size_t) floor(x)%(size());
	size_t upper = (size_t) ceil(x)%(size());
	x = fmod (x, 1.);


	assert(lower < size());
	assert( upper <= size());

	COMPLEX  ret = (*this)[lower]+((*this)[upper]-(*this)[lower])*(x);
	trace7("spec_container::interpolate", lower, upper, size(), x, (*this)[0], (*this)[size()-1], ret );
	return ret;
}
/*--------------------------------------------------------------------------*/
#endif // HAVE_FFTW_H
#endif
