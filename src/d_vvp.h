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
#ifndef D_VVP_H__
#define D_VVP_H__
//testing=script,sparse 2006.07.17
#include "e_subckt.h"
//#include "u_xprobe.h"
// #include "d_vvp.h"
#include "extlib.h"
#include "d_logic.h"
//#include "ap.h"
//#include "u_lang.h"
//#include "m_interp.h"
//#include "bm.h"
//#include "l_lib.h"
//#include "io_trace.h"
using namespace std;
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_VVP = 100};
enum direction_t {pIN=0, pOUT, pBUS};

struct port_info_t {
  string name;
  direction_t direction;
};
class MODEL_LOGIC_VVP;
class COMMON_LOGIC_VVP;
class DEV_LOGIC_VVP;

class COMMON_LOGIC_VVP : public COMMON_COMPONENT {
  public:
    explicit	COMMON_LOGIC_VVP(int c=0);
    explicit	COMMON_LOGIC_VVP(const COMMON_LOGIC_VVP& p);
    ~COMMON_LOGIC_VVP();
    bool operator==(const COMMON_COMPONENT&)const; 
    COMMON_COMPONENT* clone()const; //  { return new COMMON_LOGIC_VVP(*this);}
    void set_param_by_index(int, std::string&, int);
    bool param_is_printable(int)const;
    string param_name(int)const;
    string param_name(int,int)const;
    string param_value(int)const;
    int	param_count()const	{return (2 + COMMON_COMPONENT::param_count());}
    void precalc_first(const CARD_LIST*);
    void expand(const COMPONENT* d);
    void precalc_last(const CARD_LIST*);
    std::string name()const {itested();return "logic_vvp";}
    static  int	count()	{return _count;}
    COMMON_COMPONENT* deflate();
  private:
    static int _count;
    static list<const COMMON_COMPONENT*> _commons;
  public:
    // virtual LOGICVAL logic_eval(const node_t*)const	= 0;
    void startsim();
    //private:?
    ExtLib* _extlib;
  public: // input parameters
    int		incount; //?
    PARAMETER<std::string> file;
    PARAMETER<std::string> module;
  private:
   uint_t status;

  public:
    std::vector< COMMON_COMPONENT* > _subcommons;
    COMMON_COMPONENT* _logic_out;
    COMMON_COMPONENT* _logic_in;
    COMMON_COMPONENT* _logic_none;
};
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC_VVP : public MODEL_LOGIC {
private:
  explicit	MODEL_LOGIC_VVP(const MODEL_LOGIC_VVP& p);
public:
  explicit MODEL_LOGIC_VVP(const DEV_LOGIC_VVP* );
	   ~MODEL_LOGIC_VVP()		{ --_count; }
private: // override virtuals
  static int _count;
  std::string	dev_type()const		{return "logic_vvp";}
  CARD*	clone()const		{return new MODEL_LOGIC_VVP(*this);}
  void precalc_first();
  void precalc_last();
  void set_param_by_index(int, std::string&, int);
  bool param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int		param_count()const	{return (3 + MODEL_LOGIC::param_count());}
public:
  static int	count()			{return _count;}
  std::string port_name(uint_t)const;
public:
  PARAMETER<string> file;
  PARAMETER<string> input;
  PARAMETER<string> output;
};
/*--------------------------------------------------------------------------*/
class DEV_LOGIC_VVP : public BASE_SUBCKT  {
  private:
    explicit	DEV_LOGIC_VVP(const DEV_LOGIC_VVP& p);
  public:
    explicit	DEV_LOGIC_VVP();
    ~DEV_LOGIC_VVP();// {--_count;}
  private: // override virt.
    char	   id_letter()const	{return '\0';}
    bool print_type_in_spice()const {return true;}
    string value_name()const	{return "#";}
    uint_t max_nodes()const     {return 10;}
    uint_t min_nodes()const     {return 2;}
    //int     matrix_nodes()const; //BASE_SUBCKT
//    uint_t net_nodes()const     {return 5;} //depends...
    uint_t int_nodes()const     {return 0;}
    uint_t tail_size()const     {return 1;}
    node_t _nodes[PORTS_PER_VVP];	/* PORTS_PER_VVP <= PORTSPERSUBCKT */
    CARD* clone()const { return new DEV_LOGIC_VVP(*this); }
    void precalc_first();
    void expand();
    void precalc_last();
    void tr_accept();
    bool do_tr();
    bool tr_needs_eval()const{return true;}
    TIME_PAIR tr_review();

  public:
    static int count()			{return _count;}

  private:
    static int	_count;
    uint_t status;
    std::vector<port_info_t> _ports;
    ExtRef* _extref; //initialized Extlib...
    std::vector< ExtRef* > ExtRefList;
    std::vector< COMPONENT* > _subdevices;

    vector<vpiHandle> _outport;
    vector<vpiHandle> _inport;

  public:
    void register_port(vpiHandle); // data from ivl

    bool has_common()const {return true;}
    std::string port_name(uint_t)const ;
    void tr_begin();
    ExtLib* extlib()const; //{return (((COMMON_LOGIC_VVP*) common())->_extlib);}
};
#endif
