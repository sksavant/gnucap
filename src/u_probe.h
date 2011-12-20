/*$Id: u_probe.h,v 1.6 2010-09-22 13:19:51 felix Exp $ -*- C++ -*-
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
 * single probe item
 */
//testing=script,complete 2006.07.14
#ifndef U_PROBE_H
#define U_PROBE_H
#include "l_compar.h" // inorder
#include "e_cardlist.h" // inorder
#include "m_expression.h" 
#include "u_function.h" 
/*--------------------------------------------------------------------------*/
class CKT_BASE;
/*--------------------------------------------------------------------------*/
#define MATH_OP_CONST 0
#define MATH_OP_UNARY 1 * 32
#define MATH_OP_BINARY 2 * 32
#define MATH_OP_OTHER 3 * 32
#define MATH_OP_SET  4 * 32

#define MATH_OP_MASK  7 * 32


// this is c-ish. fixme
// use parameter expressions and a list of probes instead.
typedef enum{
  op_null = MATH_OP_CONST, // constant value.
  op_pi,
  op_sum = MATH_OP_SET, // argument is a set
  op_prod,
  op_exp = MATH_OP_UNARY,
  op_neg,
  op_abs,
  op_inv,
  op_square,
  op_cos,
  op_sin,
  op_tan,
  op_artan,
  op_diff = MATH_OP_BINARY,
  op_quot
} MATH_OP;

MATH_OP strtotype( std::string );
char typetochar(MATH_OP );

inline bool is_const(MATH_OP x){
  return  x<32;
}

inline bool is_unary(MATH_OP x){
  return  x>=MATH_OP_UNARY && x<MATH_OP_BINARY ;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class INTERFACE PROBE {
  public:
    PROBE(): _brh(0),_next(0) { itested();}
    explicit  PROBE(const std::string& what, const CKT_BASE *brh);
    PROBE(const PROBE& p);
    virtual ~PROBE();

    virtual double value()const;

    void        set_override_label(const std::string _override) { _override_label=_override; }  // ???

    PROBE* arg() const {return _arg;}
    std::string  override_label() const {return _override_label;}
    virtual PROBE* clone()const { return (new PROBE(*this));}
    virtual void expand(){}
    virtual void precalc_last(){}
  private:
  protected:
    std::string	_what;    
    std::string	_override_label;    
  private:
    const CKT_BASE* _brh;
    double _lo,_hi;
  public:

    void	    set_limit(double Lo,double Hi)	{_lo = Lo; _hi = Hi;}
    void	    set_next( PROBE* n  )	{ assert(this); _next=n;}
    void	    detach();
  PROBE&    operator=(const PROBE& p);

  virtual const std::string label()const;
  const CKT_BASE* object()const	 {return _brh;}
  double	  lo()const	 {return _lo;}
  double	  hi()const	 {return _hi;}
  double	  range()const	 {return hi()-lo();}
  bool		  in_range()const{return in_order(lo(),value(),hi());}
  PROBE* next()const {return _next;}
private:
  double	  probe_node()const;
protected:
  PROBE* _next; 
  PROBE* _arg; // ->MATH_PROBE

public: // compare probes.
  bool operator==(const PROBE& p)const
  { return ( ( _what == p._what )
       //    &&( _override_label == p._override_label )  ???
           &&(  _brh == p._brh  )); 
  
  }

public: // compare (for STL)
  bool operator==(const CKT_BASE& brh)const;
  bool operator!=(const CKT_BASE& brh)const
  { untested(); return (object() != &brh); }
  bool operator==(const std::string& par)const
  { return wmatch(label(), par); }
  bool operator!=(const std::string& par)const
  { untested(); return !( *this == par); }
  
};

/*--------------------------------------------------------------------------*/
class MATH_PROBE : public PROBE {
  public:
    MATH_PROBE(): PROBE() {_next=0;untested(); _arg=0;};
    MATH_PROBE(const MATH_OP type) { _type=type; _next=NULL; _arg=0; }
    MATH_PROBE(const MATH_PROBE& p);
    virtual PROBE* clone()const { return new MATH_PROBE(*this);}
    MATH_OP type()const{return(_type);}
    MATH_PROBE&    operator=(const MATH_PROBE& p);
    void	   set_arg( PROBE*  );//	{ assert(this); _arg=n;}
    ~MATH_PROBE();
  private:
    MATH_OP _type;
    unsigned _arity;
  public:
    double value()const;
    const std::string label()const;
    void push( PROBE* ); 
};
/*--------------------------------------------------------------------------*/
class EVAL_PROBE: public PROBE {
  public:
    EVAL_PROBE(): PROBE() {_cmd="none";untested();};
    EVAL_PROBE(const EVAL_PROBE& p);
    explicit  EVAL_PROBE(const std::string& what, const CARD_LIST* scope);
    virtual PROBE* clone()const { return new EVAL_PROBE(*this);}
    EVAL_PROBE&    operator=(const EVAL_PROBE& p);
  private:
    // FIXME carry expression. not _cmd and _scope
    std::string _cmd;
    Expression _exp;
    const CARD_LIST* _scope;
  public:
    double value()const;
    const std::string label()const { return _cmd; }
};
/*--------------------------------------------------------------------------*/
class MEAS_PROBE: public PROBE {
  public:
    MEAS_PROBE(): PROBE() {_cmd="none";untested();};
    MEAS_PROBE(const MEAS_PROBE& p);
    explicit  MEAS_PROBE(const std::string& what);
    explicit  MEAS_PROBE(const std::string& what, const CARD_LIST* scope);
    virtual PROBE* clone()const { return new MEAS_PROBE(*this);}
    MEAS_PROBE&    operator=(const MEAS_PROBE& p);
  private:
    std::string _cmd;
    const CARD_LIST* _scope;
    WAVE_FUNCTION* f;
    WAVE* w;
  public:
    string probe_name;
    void precalc_first();
    void expand();
    double value()const;
    const std::string label()const;
    void precalc_last();
};
/*--------------------------------------------------------------------------*/
#endif
