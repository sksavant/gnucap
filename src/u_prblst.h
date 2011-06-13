/*$Id: u_prblst.h,v 1.6 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
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
 * list of probes
 */
//testing=script,complete 2006.09.28
#ifndef U_PRBLST_H
#define U_PRBLST_H
#include <iostream>
#include "mode.h"
#include "u_probe.h"
// using namespace std;

/*--------------------------------------------------------------------------*/
class CARD_LIST;
/*--------------------------------------------------------------------------*/
class INTERFACE PROBELIST {
private:
  typedef std::vector<PROBE*> _container;
  _container bag;

  explicit PROBELIST(const PROBELIST&) {unreachable();incomplete();}
public:
  explicit PROBELIST() {}
  ~PROBELIST();

  typedef _container::iterator	     iterator;
  typedef _container::const_iterator const_iterator;
  void	  listing(const std::string&)const;
  void     clear();
  void	  remove_list(CS&);
  void     remove_one(CKT_BASE*);
  void push_probe(PROBE*);
  void merge_probe(PROBE*);

  PROBE*     add_list(CS& cmd)
  { return add_list( cmd, &CARD_LIST::card_list ); }
  PROBE*     add_list(CS&, const CARD_LIST* scope);
  int	   size()const		{return static_cast<int>(bag.size());}
  const_iterator begin()const	{return bag.begin();}
  const_iterator end()const	{return bag.end();}
  iterator begin()            {return bag.begin();}
  iterator end()              {return bag.end();}
  virtual void        open(CS&)       {incomplete();}
  virtual void        send(OMSTREAM, double)  {incomplete();}
  virtual void        close()         {incomplete();}
  friend std::ostream& operator<< ( std::ostream&, const PROBELIST& );

private:
  void     erase(iterator , iterator );// {bag.erase(b,e);}
  PROBE*	  push_new_probe(const std::string& param, const CKT_BASE* object);
  MEAS_PROBE*	  push_new_meas_probe(const std::string& param);

  PROBE*  add_branches(const std::string&,const std::string&,const CARD_LIST*);
  void    add_all_nodes(const std::string&, const CARD_LIST* scope);

  // FIXME
  PROBE* add_expr(const std::string&,const MATH_OP,const CARD_LIST*,CS&, PROBELIST&);
};
/*--------------------------------------------------------------------------*/
class INTERFACE PROBE_LISTS {
public:
 static PROBELIST alarm[sCOUNT]; // list of alarm probes
 static PROBELIST verify[sCOUNT]; // list of alarm probes
 static PROBELIST plot[sCOUNT];  // list of plot probes
 static PROBELIST print[sCOUNT]; // list of print probes
 static PROBELIST store[sCOUNT]; // list of probes to store for postproc
 static PROBELIST expr[sCOUNT]; // list of probes to store for postproc
 static void purge(CKT_BASE*);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
