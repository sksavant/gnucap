/*$Id: d_logic.h,v 1.4 2009-12-13 17:55:01 felix Exp $ -*- C++ -*-
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
 * data structures and defaults for logic model.
 */
//testing=script,sparse 2006.07.17
#ifndef D_VVP_H
#define D_VVP_H
#include "e_model.h"
#include "e_subckt.h"
#include "d_logic.h"
#include "extlib.h"
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_VVP = 10};
enum direction_t {pIN=0, pOUT, pBUS};

inline OMSTREAM& operator<<(OMSTREAM& o, direction_t t) {
  const std::string s[] = {"", "input", "output", "bus"};
  return (o << s[t]);
}
/*--------------------------------------------------------------------------*/
struct port_info_t {
  string name;
  direction_t direction;
};
/*--------------------------------------------------------------------------*/
class DEV_LOGIC_VVP : public BASE_SUBCKT  {
private:
  static int	_count;
  node_t	nodes[PORTS_PER_VVP];	/* PORTS_PER_VVP <= PORTSPERSUBCKT */
public:
  explicit	DEV_LOGIC_VVP();
  explicit	DEV_LOGIC_VVP(const DEV_LOGIC_VVP& p);
		~DEV_LOGIC_VVP()		{--_count;}
private: // override virtuals
  char	   id_letter()const	{return '\0';}
  std::string value_name()const	{return "#";}
  bool	      print_type_in_spice()const {return false;}
  CARD*	   clone()const		{return new DEV_LOGIC_VVP(*this);}
  void	   precalc_first();
  void	   expand();
  void	   precalc_last();
  //void   map_nodes();

  static int count()			{return _count;}
  std::vector<port_info_t> _ports;
  ExtLib* _extlib; 
  ExtRef* _extref; //initialized Extlib...

public:
  virtual std::string port_name(uint_t)const ;
};
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC_VVP : public MODEL_LOGIC {
private:
  explicit	MODEL_LOGIC_VVP(const MODEL_LOGIC_VVP& p);
public:
  explicit MODEL_LOGIC_VVP(const DEV_LOGIC_VVP*);
	   ~MODEL_LOGIC_VVP()		{--_count;}
private: // override virtuals
  std::string	dev_type()const		{return "logic_vvp";}
  CARD*		clone()const		{return new MODEL_LOGIC_VVP(*this);}
  void		precalc_first();
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
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
public: // calculated parameters
  double range;			/* vmax - vmin */
private:
  static int _count;
};
/*--------------------------------------------------------------------------*/
class COMMON_LOGIC_VVP : public COMMON_COMPONENT {
protected:
  explicit	COMMON_LOGIC_VVP(int c=0)
    :COMMON_COMPONENT(c), incount(0) {++_count;}
  explicit	COMMON_LOGIC_VVP(const COMMON_LOGIC_VVP& p)
    :COMMON_COMPONENT(p), incount(p.incount) {++_count;}
public:
		~COMMON_LOGIC_VVP()			{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  static  int	count()				{return _count;}
  virtual LOGICVAL logic_eval(const node_t*)const	= 0;
public:
  int		incount;
protected:
  static int	_count;
};
/*--------------------------------------------------------------------------*/
//class LOGIC_AND : public COMMON_LOGIC {
//private:
//  explicit LOGIC_AND(const LOGIC_AND& p) :COMMON_LOGIC(p){untested();++_count;}
//  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_AND(*this);}
//public:
//  explicit LOGIC_AND(int c=0)		  :COMMON_LOGIC(c) {untested();}
//  LOGICVAL logic_eval(const node_t* n)const {untested();
//    LOGICVAL out(n[0]->lv());
//    for (int ii=1; ii<incount; ++ii) {untested();
//      out &= n[ii]->lv();
//    }
//    return out;
//  }
//  virtual std::string name()const	  {itested();return "and";}
//};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
