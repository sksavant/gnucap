/*$Id: bm.h,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
 * behavioral modeling base
 */
//testing=script 2006.07.13
#ifndef E_BM_H
#define E_BM_H
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class SPLINE;
class FPOLY1;
/*--------------------------------------------------------------------------*/
class EVAL_BM_BASE : public COMMON_COMPONENT {
protected:
  explicit	EVAL_BM_BASE(int c=0) 
    :COMMON_COMPONENT(c) {}
  explicit	EVAL_BM_BASE(const EVAL_BM_BASE& p)
    :COMMON_COMPONENT(p) {}
		~EVAL_BM_BASE() {}
protected: // override virtual
  bool operator==(const COMMON_COMPONENT&)const{/*incomplete();*/return false;}
  bool		has_tr_eval()const	{return true;}
  bool		has_ac_eval()const	{return true;}
  bool use_obsolete_callback_parse()const {return true;}
  bool use_obsolete_callback_print()const {return true;}
  bool has_parse_params_obsolete_callback()const {return true;}
};
/*--------------------------------------------------------------------------*/
class INTERFACE EVAL_BM_ACTION_BASE : public EVAL_BM_BASE {
protected:
  PARAMETER<double> _bandwidth;
  PARAMETER<double> _delay;
  PARAMETER<double> _phase;
  PARAMETER<double> _ooffset;
  PARAMETER<double> _ioffset;
  PARAMETER<double> _scale;
  PARAMETER<double> _tc1;
  PARAMETER<double> _tc2;
  PARAMETER<double> _ic;
  bool	 _has_ext_args;
protected:
  explicit	EVAL_BM_ACTION_BASE(int c=0);
  explicit	EVAL_BM_ACTION_BASE(const EVAL_BM_ACTION_BASE& p);
		~EVAL_BM_ACTION_BASE() {}
  double	temp_adjust()const;
  void		tr_final_adjust(FPOLY1* y, bool f_is_value)const;
  void		tr_finish_tdv(ELEMENT* d, double val)const;
  void		ac_final_adjust(COMPLEX* y)const;
  void		ac_final_adjust_with_temp(COMPLEX* y)const;
  double	uic(double x)const	{return (SIM::uic_now()) ? _ic : x;}
  double	ioffset(double x)const	{return uic(x) + _ioffset;}	
public: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  //COMPONENT_COMMON* clone()const;	//COMPONENT_COMMON=0
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  //void	expand(const COMPONENT*);	//COMPONENT_COMMON/nothing
  //COMMON_COMPONENT* deflate();		//COMPONENT_COMMON/nothing
  //void	precalc_last(const CARD_LIST*);	//COMPONENT_COMMON

  //void	tr_eval(ELEMENT*)const; //COMPONENT_COMMON
  void		ac_eval(ELEMENT*)const;
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  virtual bool	ac_too()const = 0;
protected: // override virtual
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing
  bool  	parse_params_obsolete_callback(CS&);
public:
  bool		has_ext_args()const;
  static COMMON_COMPONENT* parse_func_type(CS&);
};
/*--------------------------------------------------------------------------*/
class INTERFACE EVAL_BM_COND : public EVAL_BM_BASE {
private:
  COMMON_COMPONENT* _func[sCOUNT];
  bool _set[sCOUNT];
  explicit	EVAL_BM_COND(const EVAL_BM_COND& p);
public:
  explicit	EVAL_BM_COND(int c=0);
		~EVAL_BM_COND();
private: // override virtual
  bool  operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_COND(*this);}
  void  parse_common_obsolete_callback(CS&);
  void  print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  
  void  	precalc_first(const CARD_LIST*);
  void		expand(const COMPONENT*);
  COMMON_COMPONENT* deflate();
  void  	precalc_last(const CARD_LIST*);

  void  tr_eval(ELEMENT*d)const{assert(_func[SIM::_mode]); _func[SIM::_mode]->tr_eval(d);}
  void  ac_eval(ELEMENT*d)const{assert(_func[s_AC]);	   _func[s_AC]->ac_eval(d);}
  TIME_PAIR tr_review(COMPONENT*d)
		{assert(_func[SIM::_mode]); return _func[SIM::_mode]->tr_review(d);}
  void  tr_accept(COMPONENT*d) {assert(_func[SIM::_mode]); _func[SIM::_mode]->tr_accept(d);}
  //bool has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool has_ac_eval()const;	//EVAL_BM_BASE/true
  std::string name()const		{unreachable(); return "????";}
  //bool parse_numlist(CS&);	//COMPONENT_COMMON/nothing/ignored
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EVAL_BM_MODEL : public EVAL_BM_ACTION_BASE {
private:
  std::string	_arglist;
  COMMON_COMPONENT* _func;
  explicit	EVAL_BM_MODEL(const EVAL_BM_MODEL& p);
public:
  explicit      EVAL_BM_MODEL(int c=0);
		~EVAL_BM_MODEL()	{detach_common(&_func);}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_MODEL(*this);}
  void		parse_common_obsolete_callback(CS&);
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  
  //void  	precalc_first(const CARD_LIST*); //EVAL_BM_ACTION_BASE
  void		expand(const COMPONENT*);
  COMMON_COMPONENT* deflate() {return (_func) ? _func->deflate() : this;}
  //void  	precalc_last(const CARD_LIST*); //EVAL_BM_ACTION_BASE

  void		tr_eval(ELEMENT*d)const {assert(_func); _func->tr_eval(d);}
  void		ac_eval(ELEMENT*d)const {assert(_func); _func->ac_eval(d);}
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  std::string	name()const		{untested();return modelname();}
  bool		ac_too()const		{return true;}
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing/ignored
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_VALUE : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _value;
  explicit	EVAL_BM_VALUE(const EVAL_BM_VALUE& p);
public:
  explicit      EVAL_BM_VALUE(int c=0);
		~EVAL_BM_VALUE()	{}
  const PARAMETER<double>& value()const {return _value;}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_VALUE(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  //void  	expand(const COMPONENT*);	//COMPONENT_COMMON/nothing
  //COMMON_COMPONENT* deflate();		//COMPONENT_COMMON/nothing
  //void	precalc_last(const CARD_LIST*);	//COMPONENT_COMMON

  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  std::string	name()const		{untested();return "VALUE";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool  	parse_params_obsolete_callback(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
