/*$Id$
 * vim:ts=8:sw=2:et:
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 * Modifications: 2009, Kevin Cameron, 
 *                2009, Gennadiy Serdyuk (GS), <gserdyuk@analograils.com> 
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
 *  PWL extension for extyrenal simulator binding
 *  GS: possibly an be converted to class, derived from EVAL_BM_PWL 
 */
//testing=
#include "u_lang.h"
#include "e_elemnt.h"
#include "m_interp.h"
#include "bm.h"
#include "extlib.h"
#include "l_lib.h"
#include "io_trace.h"

/*--------------------------------------------------------------------------*/
namespace fussel {
/*--------------------------------------------------------------------------*/
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
static int E=0;
/*--------------------------------------------------------------------------*/
class EVAL_BM_EXTPWL : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double>   _delta;
  PARAMETER<int>      _smooth;
  PARAMETER<intptr_t> _ext;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _raw_table;
  std::vector<DPAIR> _num_table;
  explicit	EVAL_BM_EXTPWL(const EVAL_BM_EXTPWL& p);
public:
  explicit      EVAL_BM_EXTPWL(int c=0);
		~EVAL_BM_EXTPWL()		{}

// delete this function (see bm.h), use measurement;
// to find related places use code "ICARCOSIMVOLT"
//  virtual double    voltage(ELEMENT *) const;

private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_EXTPWL(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  //void  	expand(const COMPONENT*);//COMPONENT_COMMON/nothing
  //COMMON_COMPONENT* deflate();	 //COMPONENT_COMMON/nothing
  void		precalc_last(const CARD_LIST*);
  /// gs - 2remove void		precalc(const CARD_LIST*);

  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  TIME_PAIR	tr_review(COMPONENT*);
  std::string	name()const		{return "extpwl";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
  void		skip_type_tail(CS& cmd)const {cmd.umatch("(1)");}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_EXTPWL::EVAL_BM_EXTPWL(int c)
  :EVAL_BM_ACTION_BASE(c),
   _delta(_default_delta),
   _smooth(_default_smooth),
   _ext(-1),
   _raw_table(),
   _num_table()
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_EXTPWL::EVAL_BM_EXTPWL(const EVAL_BM_EXTPWL& p)
  :EVAL_BM_ACTION_BASE(p),
   _delta(p._delta),
   _smooth(p._smooth),
   _ext(p._ext),
   _raw_table(p._raw_table),
   _num_table(p._num_table)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXTPWL::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_EXTPWL* p = dynamic_cast<const EVAL_BM_EXTPWL*>(&x);
  bool rv = p
    && _delta == p->_delta
    && _smooth == p->_smooth
    && _ext == p->_ext
    && _raw_table == p->_raw_table
    && _num_table == p->_num_table
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  incomplete(); // remove obs cb.
  assert(lang);
  o << name() << '(';
  o << _ext.string() << ',' << (long int)(void*)this <<  ')';
  print_pair(o, lang, "delta", _delta, _delta.has_hard_value());
  print_pair(o, lang, "smooth",_smooth,_smooth.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _delta.e_val(_default_delta, Scope);
  _smooth.e_val(_default_smooth, Scope);
  if (_ext) {
      trace1(("EVAL_BM_EXTPWL::precalc_first binding "+_ext.string()).c_str(), _ext); 
      _ext = (some_int)bindExtSigConnect(_ext, _ext.string(), Scope, this);
      trace1(("EVAL_BM_EXTPWL::precalc_first bound"), _ext); 
  }
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::iterator
	 p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    p->first.e_val(0, Scope);
    p->second.e_val(0, Scope);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);
  trace1("EVAL_BM_EXTPWL::precalc_last -- why so often??", E);

  double last = -BIGBIG;
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::iterator
	 p = _raw_table.begin();  p != _raw_table.end();  ++p) {
    if (last > p->first) {
      throw Exception_Precalc("PWL is out of order: (" + to_string(last)
			      + ", " + to_string(p->first) + ")\n");
    }else{
      DPAIR x(p->first, p->second);
      _num_table.push_back(x);
    }
    last = p->first;
  }

  trace1("EVAL_BM_EXTPWL::precalc_last->ExtStartSim",E);
  //if (E==19) 
  ExtStartSim("TRAN");
  E++;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXTPWL::tr_eval(ELEMENT* d)const
{
  trace0("EVAL_BM_EXTPWL::tr_eval->ExtSigTrEval");
  if (_ext) {
     ExtSigTrEval(_ext,const_cast<std::vector<DPAIR>*>(&_num_table),d);
  }
  double ext = (d->is_source()) ? 0. : NOT_INPUT;
  d->_y[0] = interpolate(_num_table.begin(), _num_table.end(), 
		       ioffset(d->_y[0].x), ext, ext);
  tr_final_adjust(&(d->_y[0]), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_EXTPWL::tr_review(COMPONENT* d)
{
  if (_ext) {
    double dt_s = d->_sim->_dtmin;
    double dt = ExtSigTrCheck(_ext,dt_s,
                       const_cast<std::vector<DPAIR>*>(&_num_table),d);
    if (dt < dt_s) {
      d->_time_by.min_event(dt + d->_sim->_time0);
    }
  }
  if (d->is_source()) {
    ELEMENT* dd = prechecked_cast<ELEMENT*>(d);
    assert(dd);
    double x = dd->_y[0].x + d->_sim->_dtmin * .01;
    DPAIR here(x, BIGBIG);
    std::vector<DPAIR>::iterator begin = _num_table.begin();
    std::vector<DPAIR>::iterator end   = _num_table.end();
    std::vector<DPAIR>::iterator upper = upper_bound(begin, end, here);
    std::vector<DPAIR>::iterator lower = upper - 1;
    assert(x > lower->first);
    d->_time_by.min_event((x < upper->first) ? upper->first : NEVER);
  }else{untested();
  }

  return d->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXTPWL::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here  = cmd.cursor();
  std::pair<PARAMETER<double>, PARAMETER<double> > p;
  if (0 == _ext) {
    trace0(("EVAL_BM_EXTPWL::parse_numlist, Init " + (string)cmd).c_str());
    _ext  = (some_int)bindExtSigInit(_ext.string(),cmd.fullstring().c_str());
    p.first  = 0;
    p.second = 0;
    _raw_table.push_back(p);
    p.first  = BIGBIG;
    p.second = 0;
    _raw_table.push_back(p);
  } else {
    _ext = 0;
    for (;;) {
      unsigned start_of_pair = here;
      //cmd >> key >> value;
      cmd >> p.first; // key
      if (cmd.stuck(&here)) {
	// no more, graceful finish
	break;
      }else{
	cmd >> p.second; // value
	if (cmd.stuck(&here)) {
	  // ran out, but already have half of the pair
	  // back up one, hoping somebody else knows what to do with it
	  cmd.reset(start_of_pair);
	  break;
	}else{
	  _raw_table.push_back(p);
	}
      }
    }
    if (cmd.gotit(start)) {
    }else{
      untested();
    }
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool Getptr(CS& cmd, const std::string& key, PARAMETER<some_int>* val)  // consumes key and optional "="
{  // see for "iu_parameter.h" "Get" functions for example and reference
  std::string str;  
  if (cmd.umatch(key + ' ')) {
    if (cmd.skip1b('=')) {
      untested();
      cmd >> str;
    }else{
      str = "";
    }
    *val=0;
    *val=str;
    return true;
  }else {
    str = "";
    *val=str;
    return false;
    }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXTPWL::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "delta",    &_delta)
    || Get(cmd, "smooth",   &_smooth)
    || Getptr(cmd, "external", &_ext) //  was _ext.get_str(cmd, "external") 
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_EXTPWL p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "extpwl", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
