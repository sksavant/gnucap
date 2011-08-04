/*$Id: u_parameter.h,v 1.8 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2005 Albert Davis
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
 * A class for parameterized values
 * Used for spice compatible .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script 2006.07.14
#ifndef U_PARAMETER_H
#define U_PARAMETER_H
#include "md.h"
#include "globals.h"
#include "m_expression.h"
#include "u_opt.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
class LANGUAGE;
class PARAM_LIST; //unnnec?
//class MODEL_BUILT_IN_RCD;
//class MODEL_BUILT_IN_BTI;
/*--------------------------------------------------------------------------*/


template <class T>
class PARAMETER {
private:
  mutable T _v;
  std::string _s;
  T my_infty() const;
public:
  T _NOT_INPUT();

  explicit PARAMETER() :_v(_NOT_INPUT()), _s() {}
  PARAMETER(const PARAMETER<double>& p) :_v(p._v), _s(p._s) {}
  explicit PARAMETER(T v) :_v(v), _s() {}
  //explicit PARAMETER(T v, const std::string& s) :_v(v), _s(s) {untested();}
  ~PARAMETER() {}
  
  bool	has_hard_value()const {return (_s != "");}
  bool	has_good_value()const {return (_v != NOT_INPUT);}
  //bool has_soft_value()const {untested(); return (has_good_value() && !has_hard_value());}

  operator T()const {return _v;}
  const T*	pointer()const	 {return &_v;}
  T	e_val(const T& def, const CARD_LIST* scope)const;
  void	parse(CS& cmd);

  std::string debugstring()const {
    return(_s + " -> " + to_string(_v));
  }
  std::string string()const {
    if (_s == "#") {
      return to_string(_v);
    }else if (_s == "") {
      return "NA(" + to_string(_v) + ")";
    }else{
      return _s;
    }
  }
  void	print(OMSTREAM& o)const		{o << string();}
  void	set_default(const T& v)		{_v = v; _s = "";}
  void	operator=(const PARAMETER& p)	{_v = p._v; _s = p._s;}
  void	operator=(const T& v)		{_v = v; _s = "#";}
  //void	operator=(const std::string& s)	{untested();_s = s;}

  void	operator=(const std::string& s)	{
    if (strchr("'\"{", s[0])) {
      CS cmd(CS::_STRING, s);
      _s = cmd.ctos("", "'\"{", "'\"}");
    }else if (s == "NA") {
      _s = "";
    }else{
      _s = s;
    }
  }
  bool  operator==(const PARAMETER& p)const {
    return (_v == p._v  &&  _s == p._s);
  }
  bool  operator==(const T& v)const {
    if (v != NOT_INPUT) {
      return _v == v;
    }else{
      return (_v == NOT_INPUT || !has_hard_value());
    }
  }
  //bool	operator!=(const PARAMETER& p)const {untested();
  //  return !(*this == p);
  //}
  //bool	operator!=(const T& v)const {untested();
  //  return !(*this == v);
  //}
  T*	pointer_hack()	 {return &_v;}

private:
  T lookup_solve(const T& def, const CARD_LIST* scope)const;
};
/*--------------------------------------------------------------------------*/
template <class T>
inline T PARAMETER<T>::_NOT_INPUT(){ return NOT_INPUT ;}
/*--------------------------------------------------------------------------*/
template <>
inline int PARAMETER<int>::_NOT_INPUT(){ untested(); return 0;} //BUG. magic number?
/*--------------------------------------------------------------------------*/
template <>
inline unsigned int PARAMETER<unsigned int>::_NOT_INPUT(){ untested(); return 0;} //BUG. magic number?
/*--------------------------------------------------------------------------*/
template <> 
inline std::vector<double> PARAMETER< std::vector<double> >::_NOT_INPUT(){
  untested();
  return std::vector<double>(0);
}
/*--------------------------------------------------------------------------*/
template <> 
inline std::list<double> PARAMETER< std::list<double> >::_NOT_INPUT(){
  untested();
  return std::list<double>(0);
}
/*--------------------------------------------------------------------------*/

// ugly hack, probably
template <>
class PARAMETER<std::string> {
private:
  mutable std::string _v; //value
  std::string _s;  //backend string
  std::string my_infty() const;
private:
  std::string lookup_solve(const std::string& def, const CARD_LIST* scope)const;
public:
  explicit PARAMETER() :_v("") {}
  PARAMETER(const PARAMETER<std::string>& p) :_v(p._v), _s(p._s) {}
  explicit PARAMETER(std::string v) :_s(v) {}
  //explicit PARAMETER(T v, const std::string& s) :_v(v), _s(s) {untested();}
  ~PARAMETER() {}
  
  bool	has_hard_value()const {return (true);}
  bool	has_good_value()const {return (true);}

  operator std::string()const {
    trace0(("PARAMETER::string " + _s + " -> " + _v ).c_str());
    return _v;
  }

  std::string e_val(const std::string& def, const CARD_LIST* scope)const;
  std::string e_val_strange(const std::string& def, const CARD_LIST* scope)const;
  void	parse(CS& cmd);

  std::string value()const;
  std::string string()const;
  void	print(OMSTREAM& o)const	{o << string();}
  void	set_default(const std::string& v)		{_v = v;}
  void	operator=(const PARAMETER& p)	{ _v = p._v; _s = p._s; }
 // argh
  void	operator=(const std::string& s)	{
    if (strchr("'\"{", s[0])) {
      CS cmd(CS::_STRING, s);
      _s = cmd.ctos("", "'\"{", "'\"}");
    }else if (s == "NA") {
      _s = "";
    }else{
      _s = s;
    }
  }
  //void	operator=(const std::string& s)	{untested();_s = s;}

  bool  operator==(const PARAMETER& p)const;
//  {
//    return (_v == p._v && _s == p._s );
//  }
  bool  operator==(const std::string& v)const {
      return _v == v;
  }
  //bool	operator!=(const PARAMETER& p)const {untested();
  //  return !(*this == p);
  //}
  //bool	operator!=(const T& v)const {untested();
  //  return !(*this == v);
  //}
  std::string*	pointer_hack()	 {return &_v;}
};
/*--------------------------------------------------------------------------*/
//template <>
//std::string PARAMETER<std::string>::e_val(const std::string& def, const CARD_LIST* scope)const
/*--------------------------------------------------------------------------*/
/* non-class interface, so non-paramaters can have same syntax */
/* It is needed by the model compiler */
#if 0
inline bool operator==(const PARAMETER<int>& p, double v)
{untested();
  if (v != NOT_INPUT) {untested();
    return p.operator==(static_cast<int>(v));
  }else{untested();
    return (!(p.has_value()));
  }
}
#endif

template <class T>
bool has_hard_value(const PARAMETER<T>& p)
{
  return p.has_hard_value();
}

#if 0
inline bool has_hard_value(double x)
{untested();
  return (x != NOT_INPUT);
}
inline bool has_good_value(double x)
{untested();
  return (x != NOT_INPUT);
}
#endif

template <class T>
bool has_good_value(const PARAMETER<T>& p)
{
  return p.has_good_value();
}

#if 0
template <class T>
bool has_soft_value(const PARAMETER<T>& p)
{untested();
  return p.has_soft_value();
}
#endif

template <class T>
bool has_nz_value(const T& p)
{
  return (has_good_value(p) && p != 0);
}

template <class T>
void set_default(PARAMETER<T>* p, const T& v)
{
  assert(p);
  p->set_default(v);
}

template <class T>
void set_default(T* p, const T& v)
{
  assert(p);
  *p = v;
}

template <class T>
void e_val(PARAMETER<T>* p, const PARAMETER<T>& def, const CARD_LIST* scope)
{
  assert(p);
  p->e_val(def, scope);
}

template <class T>
void e_val(PARAMETER<T>* p, const T& def, const CARD_LIST* scope)
{
  assert(p);
  p->e_val(def, scope);
}

//e_val(PARAMETER<MODEL_BUILT_IN_BTI>*, NULL, const CARD_LIST*&)’      

#if 0
template <class T>
void e_val(T* p, const T& def, const CARD_LIST*)
{untested();
  assert(p);
  if (*p == NOT_INPUT) {untested();
    *p = def;
  }else{untested();
  }
}
#endif
/*--------------------------------------------------------------------------*/
class INTERFACE PARAM_LIST {
private:
  mutable std::map<const std::string, PARAMETER<double> > _pl;
public:
  PARAM_LIST* _try_again; // if you don't find it, also look here
public:
  typedef std::map<const std::string, PARAMETER<double> >::const_iterator
		const_iterator;
  typedef std::map<const std::string, PARAMETER<double> >::iterator
		iterator;
  explicit PARAM_LIST() :_try_again(NULL) {}
  explicit PARAM_LIST(const PARAM_LIST& p) 
				:_pl(p._pl), _try_again(p._try_again) {}
  //explicit PARAM_LIST(PARAM_LIST* ta) :_try_again(ta) {untested();}
  ~PARAM_LIST() {}
  void	parse(CS& cmd);
  void	print(OMSTREAM&, LANGUAGE*)const;
  
  size_t size()const {return _pl.size();}
  bool	 is_empty()const {return _pl.empty();}
  bool	 is_printable(int)const;
  std::string name(int)const;
  std::string value(int)const;

  void	eval_copy(PARAM_LIST&, const CARD_LIST*);
  bool  operator==(const PARAM_LIST& p)const {return _pl == p._pl;}
  const PARAMETER<double>& deep_lookup(std::string)const;
  const PARAMETER<double>& operator[](std::string i)const {return deep_lookup(i);}

//  const PARAMETER<std::string>& operator[](std::string i)const {return string_lookup(i);}

  void set(std::string, const std::string&);
  void set(std::string, const double);
  void set_try_again(PARAM_LIST* t) {_try_again = t;}

  iterator begin() {return _pl.begin();}
  iterator end() {return _pl.end();}
private:
  mutable int _index;
  mutable const_iterator _previous;
};
/*--------------------------------------------------------------------------*/
inline string PARAMETER<string>::lookup_solve(const std::string& def,
    const CARD_LIST* scope)const
{
  CS cmd(CS::_STRING, _s);
  trace0(("lookup_solve " + _s).c_str());
  const PARAM_LIST* pl = scope->params();
  PARAMETER<double> x =  (pl->deep_lookup(_s));
  return x.string();

  return def;
}
/*--------------------------------------------------------------------------*/
template <>
inline bool PARAMETER<bool>::lookup_solve(const bool&, const CARD_LIST*)const
{
  CS cmd(CS::_STRING, _s);
  return cmd.ctob();
}
/*--------------------------------------------------------------------------*/
template <class T>
inline T PARAMETER<T>::lookup_solve(const T& def, const CARD_LIST* scope)const
{
  CS cmd(CS::_STRING, _s);
  Expression e(cmd);
  Expression reduced(e, scope);
  T v = T(reduced.eval());
  if (v != NOT_INPUT) {
    return v;
  }else{
    const PARAM_LIST* pl = scope->params();
    return T(pl->deep_lookup(_s).e_val(def, scope));
  }
}
/*--------------------------------------------------------------------------*/
#if 0
template <class T>
inline T PARAMETER<T>::lookup_solve(const T& def, const CARD_LIST* scope)const
{
  const PARAM_LIST* pl = scope->params();
  return T(pl->deep_lookup(_s).e_val(def, scope));
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <>
inline double PARAMETER<double>::my_infty()const{ return inf; }
/*--------------------------------------------------------------------------*/

template <class T>
inline T PARAMETER<T>::my_infty()const{ untested(); return 0; }
/*--------------------------------------------------------------------------*/
template <>
inline std::vector<double> PARAMETER<std::vector<double> >::e_val(const
    std::vector<double>& , const CARD_LIST* )const
{
  trace0(("PARAMETER dl" + _s).c_str());
  double d;

  CS c(CS::_STRING,_s);
  std::vector<double>::iterator a;
  a=_v.begin();
  _v.erase(_v.begin(),_v.end());
  // FIXME: accept strings and parse...
  while ( c.more() ){
    d=c.ctof();
    trace1("PARAMETER vector add", d);
    _v.push_back( d );
  }
  return _v;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <>
inline std::list<double> PARAMETER<std::list<double> >::e_val(const
    std::list<double>& , const CARD_LIST*)const
{
  trace0(("PARAMETER dl" + _s).c_str());
  double d;

  CS c(CS::_STRING,_s);
  std::list<double>::iterator a;

  if (_v.size()!=0){
    a=_v.begin();
    _v.erase(a);
  }

  while ( c.more() ){
    d=c.ctof();
    trace1("PARAMETER dl", d);

    _v.push_back( d );
  }
  return _v;
}
/*--------------------------------------------------------------------------*/
template <class T>
T PARAMETER<T>::e_val(const T& def, const CARD_LIST* scope)const
{
  assert(scope);

  static int recursion=0;
  static const std::string* first_name = NULL;
  if (recursion == 0) {
    first_name = &_s;
  }else{
  }
  assert(first_name);
 
  if (_v == inf) {
    return my_infty();
  }
  if (_s == "inf") {
    return my_infty();
  }
  ++recursion;
  if (_s == "") {
    // blank string means to use default value
    _v = def;
    if (recursion > 1) {
      error(bWARNING, "parameter " + *first_name + " has no value\n");
    }else{
    }
  }else if (_s != "#") {
    // anything else means look up the value
    if (recursion <= OPT::recursion) {
      _v = lookup_solve(def, scope);
      if (_v == NOT_INPUT) {untested();itested();
	error(bDANGER, "parameter " + *first_name + " has no value\n");
      }else{
      }
    }else{untested();
      _v = def;
      error(bDANGER, "parameter " + *first_name + " recursion too deep\n");
    }
  }else{
    // start with # means we have a final value
  }
  --recursion;
  // trace1("PARAMETER<T>::e_val ", _v);
  return _v;
}
/*--------------------------------------------------------------------------*/
template <>
inline void PARAMETER<bool>::parse(CS& cmd) 
{
  bool new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{untested();
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {untested();
      if (name == "NA") {untested();
	_s = "";
      }else{untested();
	_s = name;
      }
    }else{untested();
    }
  }
  if (!cmd) {untested();
    _v = true;
    _s = "#";
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline void PARAMETER<T>::parse(CS& cmd) 
{
  trace0(("PARAMETER<T>::parse " + cmd.tail()).c_str());
  T new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {
      if (cmd.match1('(')) {
	_s = name + '(' + cmd.ctos("", "(", ")") + ')';
      }else{
	_s = name;
      }
      if (name == "NA") {
        _s = "";
      }else{
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
// hacked e_val. does strange things
inline std::string PARAMETER<std::string>::e_val_strange(const std::string& def, const CARD_LIST* scope)const
{
  trace0("PARAMETER<std::string>::e_val_strange");
  assert(scope);

  static int recursion=0;
  static const std::string* first_name = NULL;
  if (recursion == 0) {
    first_name = &_s;
  }else{
  }
  assert(first_name);

  ++recursion;
  if (_s == "") {
    // blank string means to use default value
    _v = _s; // where does it come from?
    if (recursion > 1) {
      error(bWARNING, "PARAMETER::e_val_strange: " + *first_name + " has no value (" +def+")\n");
    }else{
    }
  }else if (_s != "#") {
    // anything else means look up the value
    trace0(("looking up value for "+_s).c_str());
    if (recursion <= OPT::recursion) {
      _v = lookup_solve(_s, scope);
      if (_v == "") {untested();itested();
        error(bDANGER, "parameter " + *first_name + " has no value\n");
      }else{
      }
    }else{untested();
      _v = _s;
      error(bDANGER, "parameter " + *first_name + " recursion too deep\n");
    }
  }else{
    // start with # means we have a final value
  }
  --recursion;

  // ARGH?
  // // cleanup needed
  if (_v=="NA") _v=_s;
  if (_v=="NA( NA)") _v=_s;

  if (_v=="empty") _v="";
  if (_v=="none") _v="";

  trace0(("Evaluated " +_s + " to " + _v).c_str());
  return _v;
}
/*--------------------------------------------------------------------------*/
INTERFACE bool Get(CS& cmd, const std::string& key, PARAMETER<bool>* val);
INTERFACE bool Get(CS& cmd, const std::string& key, PARAMETER<int>* val);
/*--------------------------------------------------------------------------*/
template <class T>
inline OMSTREAM& operator<<(OMSTREAM& o, const PARAMETER<T> p)
{
  p.print(o);
  return o;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef struct{
  double tr_sum; // sum up stress during tr
  double tt_now; // total stress
  double tt_old; // old total
} stress;

#endif
