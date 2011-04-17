/*$Id$
 * vim:sw=2:et:ts=8:
 * Copyright (C) 2009 Kevin Cameron
 * Author: Kevin Cameron
 *
 * This file is a part of plugin "Gnucap-Icarus" to "Gnucap", 
 * the Gnu Circuit Analysis Package
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
 * Dynamic binding of PWL signal sources to external simulators 
 */
#include "u_lang.h"
#include "globals.h"
#include "e_card.h"
#include "e_cardlist.h"
#include "s_tr.h"
#include "extlib.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/

// static std::list<ExtLib*> Libs;

void ExtLib::set_active(void *,double time)
{
  if (active) {
    if (time >= next_time) {
      return;
    }
  } else {
    active    = 1;
  }
  next_time = time;
  if (time > now) {
    CKT_BASE::_sim->new_event(time);
  }
}
/*--------------------------------------------------------------------------*/
void ExtLib::SetActive(void *dl,void *handle,double time)
  //                      extLib ^^
{
  SpcDllData *spd = (SpcDllData *)dl;
  ExtLib *lib = ExtLib::Sdd2El(spd);
  lib->set_active(handle,time);
}

int ExtLib::init(const char *args) {
  size_t i=strlen(args)+1;
  char buff[i],*bp=buff;
  const char *argv[i];
  argv[0]=name.c_str();
  argv[i=1]=bp;
  while ((*bp=*args++)) {
    if (' ' == *bp) {
      *bp++ = 0;
      argv[++i]=bp;
    } else {
      bp++;
    }
  }

  trace0("ExtLib::init");

  so_main =  (typeof(so_main))dlsym(handle,"main");
  assert(so_main);
  startsim = (typeof(startsim))dlsym(handle,"startsim");
  assert(startsim);
  endsim   = (typeof(endsim))dlsym(handle,"endsim");
  assert(endsim);
  bindnet  = (typeof(bindnet))dlsym(handle,"bindnet");
  assert(bindnet);
  contsim  = (typeof(contsim))dlsym(handle,"contsim");
  assert(contsim);
  activate = (typeof(activate))SetActive;
  assert(activate);

  argv[++i] = 0;
  return( (*so_main)((int)i,argv) );
}


static inline double FixTable(double *dp,std::vector<DPAIR> *num_table,
                              double now)
{
  double t = -1,nxt = -1;
  unsigned int s;
  int rsz = 1;
  for (s = 0;; s++) {
    double t2 = *dp++;
    if (t2 <= t) break;
      t = t2;
      if (nxt < now) nxt = t;
      double v = *dp++;
      if (s >= num_table->size()) {
        DPAIR p(t,v);
	num_table->push_back(p); rsz = 0;
      } else {
	DPAIR &p((*num_table)[s]);
	p.first = t;
	p.second = v;
      }
  }
  if (rsz) num_table->resize(s);

  return nxt;
}


void ExtSig::SetActive(void *,void *sig,double time) {
  ((ExtSig*)sig)->set_active(time);
  trace1("ExtSig::SetActive", (intptr_t)sig );
}

void ExtSig::set_active(double ) {
}

