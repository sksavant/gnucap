/*$Id: io_.h,v 1.2 2010-07-09 12:14:23 felix Exp $ -*- C++ -*-
 * :vim:ts=8:sw=2:et:
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
 * shared data for all io functions
 * other io related stuff, like files and formatting
 */
//testing=script 2006.07.17
#ifndef IO_H
#define IO_H
#include "l_lib.h"
#include <complex>
/*--------------------------------------------------------------------------*/
class CS;
const int MAXHANDLE = CHAR_BIT*sizeof(int)-1;
/*--------------------------------------------------------------------------*/
class INTERFACE OMSTREAM {
private:
  int _mask;
  int _fltdig;			/* max precision for float/double numbers */
  int _fltwid;			/* fixed(min)width for float/double numbers */
  int _format;			/* how to format io.  Basic option. */
  static unsigned _cpos[MAXHANDLE+1];/* character counter */
  bool _cipher;			/* flag: encrypt output file */
  bool _pack;			/* flag: convert whitespace to tabs on out */
  FILE* fn;
  FILE* to_pipe;

  OMSTREAM(int m)
    :_mask(m),_fltdig(7),_fltwid(0),_format(0),_cipher(false),_pack(false) {}
public:
  explicit OMSTREAM(FILE* f = 0)
    :_mask(0),_fltdig(7),_fltwid(0),_format(0),_cipher(false), _pack(false)
    {_mask = (f) ? 1<<fileno(f) : 0;}
  OMSTREAM* outset(CS& cmd);
  void outreset();
  OMSTREAM& operator=(const OMSTREAM& x)  {_mask = x._mask; return *this;}
  OMSTREAM& attach(const OMSTREAM& x)	{itested();_mask |= x._mask; return *this;}
  OMSTREAM& attach(FILE* f)		{itested();return attach(OMSTREAM(f));}
  OMSTREAM& detach(const OMSTREAM& x)	{_mask &= ~(x._mask); return *this;}
  OMSTREAM& detach(FILE* f)		{itested();return detach(OMSTREAM(f));}

  OMSTREAM operator-(const OMSTREAM& y) {OMSTREAM x = *this; return x.detach(y);}
  OMSTREAM operator+(const OMSTREAM& y) {OMSTREAM x = *this; return x.attach(y);}

  //OMSTREAM& operator<<=(const OMSTREAM& x) {untested();_mask <<= x._mask; return *this;}
  bool	    any()const			{return _mask != 0;}
  int	    mask()const			{return _mask;}
  bool	    cipher()const		{return _cipher;}
  bool	    pack()const			{return _pack;}
  int	    format()const		{return _format;}
  OMSTREAM& setcipher(bool x=true)	{untested(); _cipher=x; return *this;}
  OMSTREAM& setpack(bool x=true)	{itested(); _pack=x;   return *this;}
  OMSTREAM& setfloatwidth(int d,int w=0) {_fltdig=d; _fltwid=w; return *this;}
  OMSTREAM& setformat(int f)		{_format=f; return *this;}
  OMSTREAM& reset()
    {_fltdig=7;_fltwid=0;_format=0; _cipher=_pack=false; return *this;}
  /* out */
  OMSTREAM& tab(unsigned);
  OMSTREAM& tab(int p)			{return tab(static_cast<unsigned>(p));}
  OMSTREAM& form(const char*,...);

  template<class T>
	  OMSTREAM& operator<<(T);
  //
  //
//OMSTREAM& operator<< <>(char c);
//OMSTREAM& operator<<(const char* s);
  OMSTREAM& ostream_char(char );
  OMSTREAM& ostream_const_char_p(const char *);

};


/*--------------------------------------------------------------------------*/
/* mputs: multiple puts.
 * puts to "m" style files.
 * also....
 * starts new line, prefixes it with + if it would exceed width
 * width is handled separately for each file to support different widths
 * (which are not currently handled by .options)
 * and it is possible that current contents of lines may be different
 */
template<>
inline
OMSTREAM & OMSTREAM::operator<< <>(const char *str)
{
  assert(str);
  return ostream_const_char_p(str);
}
/*--------------------------------------------------------------------------*/
/* mputc: multiple putc
 * multiple putc
 * also....
 * crunch spaces, if selected
 * encripts, if selected
 * keeps track of character count
 */
template<>
inline OMSTREAM & OMSTREAM::operator<< <>(char chr)
{
  return ostream_char(chr);
}
/*--------------------------------------------------------------------------*/

template<>
inline OMSTREAM& OMSTREAM::operator<< <>(double x)
{
	return (*this)<<ftos(x,_fltwid,_fltdig,_format);
}

template<>
inline OMSTREAM& OMSTREAM::operator<< <>(COMPLEX c)
{ 
	*this << c.real();
	if(c.imag() <0){
		*this << "-" << -c.imag();
	}
	else{
		*this << "+" << c.imag();
	}
	return 	  *this	<< "* i";
}

template<>
inline OMSTREAM& OMSTREAM::operator<< <>(bool x)		{return form("%d", x);}
template<>
inline OMSTREAM& OMSTREAM::operator<< <>(int x)		{return form("%d", x);}
template<>
inline OMSTREAM& OMSTREAM::operator<< <>(long int x)	{return form("%li", x);}
template<>
inline OMSTREAM& OMSTREAM::operator<< <>(unsigned x)	{return form("%u", x);}
template<>
inline OMSTREAM& OMSTREAM::operator<< <>(const std::string& s) {return operator<<(s.c_str());}

template<class T>
inline OMSTREAM& OMSTREAM::operator<< (T t){
	stringstream a;
	a << t;
	untested();
	return *this<<a.str().c_str();
}
/*--------------------------------------------------------------------------*/

const char* octal(int x);
/*--------------------------------------------------------------------------*/
class INTERFACE IO {
public:
  static OMSTREAM mstdout;
  static OMSTREAM error;
  static OMSTREAM plotout;		/* where to send ascii graphics */
  static bool	  plotset;		/* plot on by default flag */
  static int	  formaat;		/* how to format io.  Basic option. */
  static bool	  incipher;		/* flag: decrypt input file */
  static FILE*	  stream[MAXHANDLE+1];	/* reverse of fileno() */
};
/*--------------------------------------------------------------------------*/
/* contrl */ INTERFACE void	   initio(OMSTREAM&);
		       void	   outreset();
	     INTERFACE OMSTREAM*   outset(CS&,OMSTREAM*);
/* findf */	       std::string findfile(const std::string&,const std::string&,int);
/* xopen */	       void	   xclose(FILE**);
		       FILE*	   xopen(CS&,const char*,const char*);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
