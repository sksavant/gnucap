/*$Id: m_cpoly.h,v 1.2 2009-12-10 14:34:44 felix Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
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
 * structs for fixed order polynomials, in 2 different forms
 * FPOLY is by function -- f0 = eval of function
 *			   f1 = 1st derivative
 *			   f2 = 2nd derivative
 *			   etc.
 *			f(x) = f0
 *			f(t) = f0 + f1*(t-x) + f2*(t-x)^2 + ...
 * CPOLY is by series -- c0 = coeff of x^0 term (constant)
 *			 c1 = coeff of x^1 term (1st derivative)
 *			 c2 = coeff of x^2 term
 *			 etc.
 *			f(x) = c0 + f1*x + f2*x^2 + ...
 *			f(t) = c0 + f1*t + f2*t^2 + ...
 */
//testing=script,sparse 2006.07.13
#ifndef M_CPOLY_H
#define M_CPOLY_H
#include "constant.h"
#include "io_.h"
/*--------------------------------------------------------------------------*/
struct FPOLY1;
struct CPOLY1;
/*--------------------------------------------------------------------------*/
struct FPOLY1{		/* first order polynomial	*/
  hp_float_t   x;		/* the argument			*/
  hp_float_t   f0;		/* the function (c0 + x*f1)	*/
  hp_float_t   f1;		/* the first derivative		*/
  explicit FPOLY1() : x(0), f0(0), f1(0) {}
           FPOLY1(const FPOLY1& p) : x(p.x), f0(p.f0), f1(p.f1) {}
  explicit FPOLY1(hp_float_t X,hp_float_t F0,hp_float_t F1) : x(X), f0(F0), f1(F1) {}
  explicit FPOLY1(const CPOLY1& p);
  ~FPOLY1() {}

  bool	   operator==(const FPOLY1& p)const
				{return (f1==p.f1 && f0==p.f0 && x==p.x);}
  FPOLY1&  operator*=(hp_float_t s)	{f0*=s; f1*=s; return *this;}
  FPOLY1&  operator*=(const FPOLY1& s);
  FPOLY1&  operator+=(hp_float_t f) {untested(); f0+=f; return *this;}
  FPOLY1&  operator+=(const FPOLY1& s)
		{untested(); assert(x==s.x); f0+=s.f0; f1+=s.f1; return *this;}
  FPOLY1   operator-()const	{untested(); return FPOLY1(x, -f0, -f1);}
  hp_float_t   c1()const		{assert(f1 == f1); return f1;}
  hp_float_t   c0()const		
  {assert(f0==f0); assert(f1==f1); assert(x==x); assert(f0!=LINEAR); return (f0 - x * f1);}
};
/*--------------------------------------------------------------------------*/
struct CPOLY1{		/* first order polynomial	*/
  hp_float_t   x;		/* the argument			*/
  hp_float_t   c0;		/* f(x) - x*f'(x), or f0 - x*f1 */
  hp_float_t   c1;		/* the first derivative		*/
  explicit CPOLY1() : x(0), c0(0), c1(0) {}
           CPOLY1(const CPOLY1& p) : x(p.x), c0(p.c0), c1(p.c1) {untested();}
  explicit CPOLY1(hp_float_t X,hp_float_t C0,hp_float_t C1) : x(X), c0(C0), c1(C1) {}
  explicit CPOLY1(const FPOLY1& p);
  ~CPOLY1() {}

  bool	   operator==(const CPOLY1& p)const
				{return (c1==p.c1 && c0==p.c0 && x==p.x);}
  CPOLY1&  operator*=(hp_float_t s)	{c0*=s; c1*=s; return *this;}
  hp_float_t   f1()const		{return c1;}
  hp_float_t   f0()const		{return (c0 + x * c1);}
};
/*--------------------------------------------------------------------------*/
inline FPOLY1::FPOLY1(const CPOLY1& p)
  :x(p.x),
   f0(p.f0()),
   f1(p.f1())
{
  assert(p == p);
  assert(*this == *this);
}
/*--------------------------------------------------------------------------*/
inline CPOLY1::CPOLY1(const FPOLY1& p)
  :x(p.x),
   c0(p.c0()),
   c1(p.c1())
{
  assert(p == p);
  assert(x == x);
  assert(c1 == c1);
  assert(c0 == c0);
  assert(*this == *this);
}
/*--------------------------------------------------------------------------*/
inline FPOLY1& FPOLY1::operator*=(const FPOLY1& s)
{
  untested();
  assert(x == s.x);
  *this *= s.f0;
  f1 += f0 * s.f1;
  return *this;
}
/*--------------------------------------------------------------------------*/
inline FPOLY1 operator*(FPOLY1 a, const FPOLY1& b)
{
  untested();
  a *= b;
  return a;
}
/*--------------------------------------------------------------------------*/
inline FPOLY1 operator+(FPOLY1 a, const FPOLY1& b)
{
  untested();
  a += b;
  return a;
}
/*--------------------------------------------------------------------------*/
inline FPOLY1 operator+(FPOLY1 a, hp_float_t b)
{
  untested();
  a += b;
  return a;
}
/*--------------------------------------------------------------------------*/
inline FPOLY1 operator-(hp_float_t a, const FPOLY1& b)
{
  untested();
  return -b + a;
}
/*--------------------------------------------------------------------------*/
template<class T>
inline T& operator<<(T& o, const CPOLY1& x)
{
  	return o << "C " <<  x.x << " " << x.c0 << " " << x.c1;
}
/*--------------------------------------------------------------------------*/
template<class T>
inline T& operator<<(T& o, const FPOLY1& x)
{
  	return o << "F " << x.x << " " << x.f0 << " " << x.f1;
}
#endif
