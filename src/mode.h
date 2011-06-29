/*$Id: mode.h,v 1.3 2010-09-07 07:46:24 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
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
 * several enumerated types to identify various modes
 */
//testing=script,complete 2006.07.14
#ifndef MODE_H
#define MODE_H
#include "io_.h"
/*--------------------------------------------------------------------------*/
enum smode_t   {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};
const std::string smode_t_names[] = {"unknown", "analog", "digital", "mixed"};
inline OMSTREAM& operator<<(OMSTREAM& o, smode_t t) {
  assert(t >= int(moUNKNOWN));
  assert(t <= int(moMIXED));
  return (o << smode_t_names[t]);
}

enum SIM_MODE { // simulation types
  s_NONE = 0,	/* not doing anything, reset by cmd interpreter	*/
  s_AC,  	/* AC analysis					*/
  s_OP,  	/* op command					*/
  s_DC,  	/* dc sweep command				*/
  s_TRAN,	/* transient command				*/
  s_TTT,	/* two time transient command			*/
  s_FOURIER	/* fourier command				*/
};
const int sSTART = s_NONE;
const int sCOUNT = s_FOURIER + 1;
const std::string SIM_MODE_label[] = {"ALL", "AC", "OP", "DC", "TRAN", "TTT",
  "FOURIER"};
inline OMSTREAM& operator<<(OMSTREAM& o, SIM_MODE t) {
  trace0( "mode stream" );
  assert(t >= int(s_NONE));
  assert(t <= int(s_FOURIER));
  return (o << SIM_MODE_label[t]);
}

enum SIM_PHASE { // which of the many steps...
  p_NONE,	/* not doing anything, reset by cmd interpreter */
  p_INIT_DC,	/* initial DC analysis				*/
  p_DC_SWEEP,	/* DC analysis sweep, in progress		*/
  p_TRAN, 	/* transient, in progress			*/
  p_RESTORE	/* transient restore after stop			*/
};
const std::string SIM_PHASE_label[] = {"NONE", "INIT_DC", "DC_SWEEP", "TRAN",
  "RESTORE"};


enum PROBE_INDEX { // iter probes (continue after SIM_MODE)
  iPRINTSTEP = sCOUNT,	/* iterations for this printed step		*/
  iSTEP,		/* iterations this internal step		*/
  iTOTAL		/* total iterations since startup		*/
};
const int iCOUNT = iTOTAL + 1;	/* number of iteration counters		*/

/* control probes							*/
#define cSTEPCAUSE (0)	/* what caused this time step			*/
#define cSTEPS	   (1)	/* count of hidden steps (+ this unhidden)	*/
#define cCOUNT	   (2)	/* number of control probes			*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
