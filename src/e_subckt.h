/*$Id: e_subckt.h,v 1.10 2010-09-07 07:46:23 felix Exp $ -*- C++ -*-
 * vim:et:ts=8:sw=2
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
 * base class for elements made of subcircuits
 */
//testing=script 2006.07.12
#ifndef E_SUBCKT_H
#define E_SUBCKT_H
#include "e_compon.h"
#include "io_trace.h"
/*--------------------------------------------------------------------------*/
class CARD;
class BASE_SUBCKT : public COMPONENT {
protected:
  explicit BASE_SUBCKT()
    :COMPONENT() {}
  explicit BASE_SUBCKT(const BASE_SUBCKT& p)
    :COMPONENT(p) {}
  ~BASE_SUBCKT() {}

  virtual  void tr_save_amps(int n){
	  trace1( ("BASE_SUBCKT::tr_save_amps: " +  short_label()).c_str(), n );
	  COMPONENT::tr_save_amps(n);
  }
protected: // override virtual
	virtual double* tr_ampsp(){ 
	  std::cerr << "BASE_SUBCKT::tr_ampsp " << short_label() << "\n";
	
	  return NULL;
	};

  //char  id_letter()const		//CARD/null
  std::string dev_type()const {assert(common()); return common()->modelname();}
  uint_t	  tail_size()const		{return 1;}
  //int	  max_nodes()const		//COMPONENT/null
  //int	  num_nodes()const		//COMPONENT/null
  //int	  min_nodes()const		//COMPONENT/null
  uint_t     matrix_nodes()const		{return 0;}
  uint_t     net_nodes()const		{return _net_nodes;}
  //CARD* clone()const			//CARD/null
  //void  precalc_first()	{assert(subckt()); subckt()->precalc();}
  //void  expand()			//COMPONENT
  //void  precalc_last()	{assert(subckt()); subckt()->precalc();}
  //void  map_nodes();
  void	  tr_begin()	{assert(subckt()); subckt()->tr_begin();}
  void	tr_stress()
         { assert(subckt()); subckt()->do_forall( &CARD::tr_stress ); }
  virtual void	tr_stress_last()
         { assert(subckt()); subckt()->do_forall( &CARD::tr_stress_last ); }
  // not const?
  virtual void	stress_apply()
         { assert(subckt()); subckt()->do_forall( &CARD::stress_apply );}

  void	  tr_restore()	{assert(subckt()); subckt()->tr_restore();}
  void	  dc_advance()	{assert(subckt()); subckt()->dc_advance();}
  void	  tr_advance()	{assert(subckt()); subckt()->tr_advance();}
  void	  tr_regress()	{assert(subckt()); subckt()->tr_regress();}
  bool	  tr_needs_eval()const
	{ //  untested0(long_label().c_str());
		assert(subckt()); return subckt()->tr_needs_eval();}
  void	  tr_queue_eval() {assert(subckt()); subckt()->tr_queue_eval();}
  bool	  do_tr()
	{ 
          trace1("do_tr" + short_label(), (intptr_t) common() %1000 );

          
          // untested0(long_label().c_str());
		assert(subckt());set_converged(subckt()->do_tr());return converged();}
  void	  tr_load()	{assert(subckt()); subckt()->tr_load();}
  TIME_PAIR tr_review()	{
    assert(subckt()); 
    _time_by = subckt()->tr_review();
    if (_time_by._event < 1e-5 ) {
      trace1(("BASE_SUBCKT::tr_review " + short_label()).c_str() , _time_by._event );
    }
    return  _time_by;
  }
  void	  tr_accept()	{assert(subckt()); subckt()->tr_accept();}
  void	  tr_unload()	{assert(subckt()); subckt()->tr_unload();}
  void	  ac_begin()	{assert(subckt()); subckt()->ac_begin();}
  void	  do_ac()	{assert(subckt()); subckt()->do_ac();}
  void	  ac_load()	{assert(subckt()); subckt()->ac_load();}

  void	  tt_prepare() 
         { COMPONENT::tt_prepare();  // behaviour (not implemented)
           assert(subckt()); subckt()->do_forall( &CARD::tt_prepare );}
  //                  why   ^^^^^^^^^^^^^^ ? ( .subctk ) !
  void	  tt_next()         
         { assert(subckt()); subckt()->do_forall( &CARD::tt_next );}

  virtual void	tt_begin()  // FIXME do_tt? 
         { assert(subckt()); subckt()->do_forall( &CARD::tt_begin );}
  virtual void	tt_commit() 
         { assert(subckt()); subckt()->do_forall( &CARD::tt_commit );}
  virtual void	tt_accept() // !const (beh)
         { assert(subckt()); subckt()->do_forall( &CARD::tt_accept );}
  public:
  // virtual void	tr_stress_last() {untested(); }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
