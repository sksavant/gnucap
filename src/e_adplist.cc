/*$Id: e_adplist.cc,v 1.10 2010-09-20 08:21:54 felix Exp $ -*- C++ -*-
 * vim:sw=2:ts=8:et:
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
 * Base class for "adp" in the circuit description file
 * This file contains functions that process a list of adp
 */
//testing=script 2006.07.10
#include "e_node.h"
#include "u_nodemap.h"
#include "e_model.h"
#include "e_adplist.h"
#include "e_adp.h"
#include "u_opt.h"
#include "u_time_pair.h"
#include "omp.h"
/*--------------------------------------------------------------------------*/
ADP_NODE_LIST::ADP_NODE_LIST()
  :_parent(NULL),
   _nm(new NODE_MAP),
   _params(NULL),
   _language(NULL)
{
}
/*--------------------------------------------------------------------------*/
ADP_NODE_LIST::~ADP_NODE_LIST()
{
}
/*--------------------------------------------------------------------------*/
ADP_NODE_LIST& ADP_NODE_LIST::erase(ADP_NODE* c)
{
  delete c;
  _al.remove(c);
  return *this;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* ADP_NODE_LIST::params()
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* ADP_NODE_LIST::params()const
{
  if (_params) {
    return _params;
  }else{ //BUG//const
    static PARAM_LIST empty_params;
    return &empty_params;
  }
}
/*--------------------------------------------------------------------------*/
//ADP_NODE_LIST::iterator ADP_NODE_LIST::find_again(const std::string& short_name,
//					  ADP_NODE_LIST::iterator Begin)
//{
//  return notstd::find_ptr(Begin, end(), short_name);
//}
///*--------------------------------------------------------------------------*/
//ADP_NODE_LIST::const_iterator ADP_NODE_LIST::find_again(const std::string& short_name,
//						ADP_NODE_LIST::const_iterator Begin)const
//{
//  return notstd::find_ptr(Begin, end(), short_name);
//}
/*--------------------------------------------------------------------------*/
/* erase_all: empty the list, destroy contents
 * Beware: something else may be pointing to them, leaving dangling ptr.
 */
ADP_NODE_LIST& ADP_NODE_LIST::erase_all()
{
  while (!_al.empty()) {
    delete _al.back();
    _al.pop_back();
  }
  return *this;
}
/* tr_iwant_matrix: allocate solution matrix
 * also sets some flags for mixed-mode
 */
/*--------------------------------------------------------------------------*/
/* apply things to all cards */
ADP_NODE_LIST& ADP_NODE_LIST::do_forall( void (ADP_NODE::*thing)( )  )
{
// #pragma omp parallel for
  for (iterator ci=begin(); ci!=end(); ++ci) {
    ((*ci)->*thing)( );
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_accept: final acceptance of a time step, before moving on
 * (ab) used to collect stress...
 */
ADP_LIST& ADP_LIST::tr_accept()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    // trace_func_comp();
    (**ci).tr_accept();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR ADP_NODE_LIST::tt_review()
{

  //incomplete();
  trace0(("ADP_NODE_LIST::tt_review\n"));
  // put review into DEVICE?
  //
  TIME_PAIR time_by;
  ADP_NODE* minnode=NULL;
  double event = 0;
  for (iterator ci=begin(); ci!=end(); ++ci) {

    TIME_PAIR tmp  = (*ci)->tt_review();
    trace1(("ADP_NODE_LIST::tt_review at " + (*ci)->short_label()).c_str(), tmp._event );
    if (tmp<time_by){
      minnode = *ci;
      event = tmp._event;
      trace1(("ADP_NODE_LIST::tt_review min at" +minnode->short_label()).c_str(), tmp._event );
    }

    time_by.min(tmp);
        
  }

  if(minnode){
    if (OPT::tracewdtt){
      std::cout << "* wdT min by " << minnode->short_label() << ": " <<
        event << " gain " <<   event/CKT_BASE::_sim->_dTmin << "\n";
    }
  }

  return time_by;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR ADP_NODE_LIST::tt_preview()
{
  TIME_PAIR time_by;
  for (iterator ci=begin(); ci!=end(); ++ci) {
    //time_by.min((**ci).tt_preview());
  }
  return time_by;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ADP_LIST::ADP_LIST()
  :_parent(NULL),
   _nm(new NODE_MAP),
   _params(NULL),
   _language(NULL)
{
}
/*--------------------------------------------------------------------------*/
ADP_LIST::~ADP_LIST()
{
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* ADP_LIST::params()
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }else{
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* ADP_LIST::params()const
{
  if (_params) {
    return _params;
  }else{ //BUG//const
    static PARAM_LIST empty_params;
    return &empty_params;
  }
}
/*--------------------------------------------------------------------------*/
//ADP_LIST::iterator ADP_LIST::find_again(const std::string& short_name,
//					  ADP_LIST::iterator Begin)
//{
//  return notstd::find_ptr(Begin, end(), short_name);
//}
///*--------------------------------------------------------------------------*/
//ADP_LIST::const_iterator ADP_LIST::find_again(const std::string& short_name,
//						ADP_LIST::const_iterator Begin)const
//{
//  return notstd::find_ptr(Begin, end(), short_name);
//}
/*--------------------------------------------------------------------------*/
/* erase_all: empty the list, destroy contents
 * Beware: something else may be pointing to them, leaving dangling ptr.
 */
ADP_LIST& ADP_LIST::erase_all()
{
  while (!_al.empty()) {
    delete _al.back();
    _al.pop_back();
  }
  return *this;
}
/* tr_iwant_matrix: allocate solution matrix
 * also sets some flags for mixed-mode
 */
/*--------------------------------------------------------------------------*/
/* apply things to all cards
 */
ADP_LIST& ADP_LIST::do_forall( void (ADP_CARD::*thing)( )  )
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    ((*ci)->*thing)( );
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR ADP_LIST::tt_review() const
{
  TIME_PAIR time_by;
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    TIME_PAIR buf=(**ci).tt_review();
    time_by.min(buf);
    if (buf._event<=0){
      std::cerr << "ADP_LIST::tt_review 0 -- not implemented?\n";
    }
  }

  return time_by;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR ADP_LIST::tt_preview()
{
  TIME_PAIR time_by;
  for (iterator ci=begin(); ci!=end(); ++ci) {
    time_by.min((**ci).tt_preview());
  }
  return time_by;
}
/*--------------------------------------------------------------------------*/
