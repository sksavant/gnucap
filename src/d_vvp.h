/*$Id: d_logic.cc,v 1.2 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2011 Felix Salfelder
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
 *
 *
 */
//testing=script,sparse 2006.07.17
//#include "d_subckt.h"
//#include "u_xprobe.h"
// #include "d_vvp.h"
//#include "extlib.h"
//#include "d_logic.h"
//#include "ap.h"
//#include "u_lang.h"
//#include "m_interp.h"
//#include "bm.h"
//#include "l_lib.h"
//#include "io_trace.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC_VVP;
class COMMON_LOGIC_VVP;

class COMMON_LOGIC_VVP : public COMMON_COMPONENT {
  public:
    explicit	COMMON_LOGIC_VVP(int c=0)
      :COMMON_COMPONENT(c), 
      _extlib(0),
      incount(0),
      file(""),
      module(""),
      status(0)
    {++_count;}
    explicit	COMMON_LOGIC_VVP(const COMMON_LOGIC_VVP& p)
      :COMMON_COMPONENT(p),
      _extlib(p._extlib),
      incount(p.incount),
      file(p.file),
      module(p.module),
      status(p.status)
      {++_count;}
    COMMON_COMPONENT* clone()const; //  { return new COMMON_LOGIC_VVP(*this);}
    void precalc_first(const CARD_LIST*);
    void precalc_last(const CARD_LIST*);
  public:
    ~COMMON_LOGIC_VVP()	{--_count;}
    bool operator==(const COMMON_COMPONENT&)const {return false;}
    static  int	count()	{return _count;}
    // virtual LOGICVAL logic_eval(const node_t*)const	= 0;
    std::string name()const;
    void expand(const COMPONENT* d);
    bool param_is_printable(int)const;
    std::string	param_name(int)const;
    void set_param_by_index(int, std::string&, int);
    std::string	param_name(int,int)const;
    std::string	param_value(int)const;
    int	param_count()const	{return (2 + COMMON_COMPONENT::param_count());}

    void startsim();
    //private:?
    ExtLib* _extlib;
  public:
    int		incount;
    PARAMETER<std::string> file;
    PARAMETER<std::string> module;
  protected:
    static int	_count;
  private:
   uint_t status;
};

