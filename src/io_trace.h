/*$Id: io_trace.h,v 1.7 2010-09-07 07:46:23 felix Exp $ -*- C++ -*-
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
 * trace macros for model debugging
 */
//testing=trivial 2006.07.17
/* allow multiple inclusions with different DO_TRACE */
#include <iostream>

#undef trace_line
#undef trace
#undef trace0
#undef trace1
#undef trace2
#undef trace3
#undef trace4
#undef trace5
#undef trace6
#undef trace7
#undef untested
#undef untested0
#undef unreachable
#undef incomplete
/*--------------------------------------------------------------------------*/
// using namespace std;
#ifdef DO_TRACE
#ifndef hashpointer_
#define hashpointer_

class hp{
	intptr_t p;
	public:
	hp(const void* x){
		p = (intptr_t)x %30011;
	}
	operator int(){
		return static_cast<int>(p);
	}
};

#endif


#define trace_line() (fprintf(stderr, "@@#\n@#@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))
#define trace0(s) ( cerr << "@#@" << s << "\n")   
// #define trace0(s) (fprintf(stderr, "@#@%s\n", s )) // needs c_str()...
#define trace1(s,x) ( cerr <<  "@#@" << s << "  " << #x << "=" << (x) << endl )
// #define trace1(s,x) (fprintf(stderr, "@#@%s  %s=%g\n", s, #x, (double)(x)))
#define trace2(s,x,y) ( cerr <<  "@#@" << s << "  " << #x << "=" << (double)(x)  \
		                                      << "  " << #y << "=" << (double)(y)  \
		                                      << endl )
#define trace3(s,x,y,z) ( cerr <<  "@#@" << s << "  " << #x << "=" << (double)(x)  \
		                                      << "  " << #y << "=" << (double)(y)  \
		                                      << "  " << #z << "=" << (double)(z)  \
		                                      << endl )
#define trace4(s,w,x,y,z) ( cerr <<  "@#@" << s << "  " << #w << "=" << (double)(w)  \
		                                          << "  " << #x << "=" << (double)(x)  \
		                                          << "  " << #y << "=" << (double)(y)  \
		                                          << "  " << #z << "=" << (double)(z)  \
		                                          << endl )
#define trace5(s,v,w,x,y,z)\
	(fprintf(stderr, "@#@%s  %s=%g  %s=%g  %s=%g  %s=%g  %s=%g\n",\
	s, #v, (double)(v), #w, (double)(w), #x, (double)(x),\
	#y, (double)(y), #z, (double)(z)))
#define trace6(s,u,v,w,x,y,z)\
	(fprintf(stderr, "@#@%s  %s=%g  %s=%g  %s=%g  %s=%g  %s=%g  %s=%g\n",\
    s, #u, (double)(u),#v, (double)(v), #w, (double)(w), #x, (double)(x),\
	#y, (double)(y), #z, (double)(z)))
#define trace7(s,t,u,v,w,x,y,z)\
	(fprintf(stderr, "@#@%s  %s=%g  %s=%g  %s=%g  %s=%g  %s=%g  %s=%g %s=%g\n",\
    s, #t, (double)(t), #u, (double)(u),#v, (double)(v), #w, (double)(w),\
    #x, (double)(x), #y, (double)(y), #z, (double)(z)))
#else
#define trace_line()
#define trace0(s)
#define trace1(s,x)
#define trace2(s,x,y)
#define trace3(s,x,y,z)
#define trace4(s,w,x,y,z)
#define trace5(s,v,w,x,y,z)
#define trace6(s,u,v,w,x,y,z)
#define trace7(s,t,u,v,w,x,y,z)
#endif

#define unreachable() (fprintf(stderr, "@@#\n@@@unreachable:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))

#define incomplete() (fprintf(stderr, "@@#\n@@@incomplete:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))

#ifdef TRACE_UNTESTED
#define untested() (fprintf(stderr, "@@# untested \n@@@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))
#define untested0(s) (fprintf(stderr, "@@#\n@@@:%s:%u:%s: %s\n", \
			   __FILE__, __LINE__, __func__, s))
#define untested1(s,x) (fprintf(stderr, "@@#\n@@@:%s:%u:%s: %s  %s=%g\n", \
			   __FILE__, __LINE__, __func__, s, #x, (double)(x)))
#define untested2(s,x,y) (fprintf(stderr, "@@#\n@@@:%s:%u:%s: %s  %s=%g, %s=%g\n", \
			   __FILE__, __LINE__, __func__, s, #x, (double)(x), #y, (double)(y) ))
#else
#define untested()
#define untested0(s)
#define untested1(s,x)
#define untested2(s,x,y)
#endif

#ifdef TRACE_ITESTED
#define itested() (fprintf(stderr, "@i#\n@i@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))
#else
#define itested()
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
