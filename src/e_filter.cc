/* * Copyright (C) 2012 Felix Salfelder
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
 * impulse response filter coefficients and the like
 */

#include "e_filter.h"


const double _default_time_step(OPT::dtmin * 10.0);
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
MODEL_IR::MODEL_IR(const MODEL_IR& p):
	MODEL_CARD(p), _time_step(p._time_step), _coef(p._coef),
_lazy(p._lazy){}
/*-------------------------------------------------------------*/
void MODEL_IR::precalc_first(){
	MODEL_CARD::precalc_first();

	const CARD_LIST* par_scope = scope();
	assert(par_scope);
	_step.e_val( OPT::dtmin*10, par_scope);
	_time_step = _step;
}
/*-------------------------------------------------------------*/
double MODEL_IR::timestep(double x) const{
	if (!x) return _time_step;
	_time_step = min(_time_step,x);
	assert(_time_step);
	return _time_step;
}
/*-------------------------------------------------------------*/
void MODEL_IR::tr_begin()
{
	MODEL_CARD::tr_begin();
	trace4("MODEL_IR::tr_begin", _time_step, _time_step, _sim->_time0, _coef.size());
	_timer = _sim->_time0 + _time_step;

	// if lazy
//	_sim->_evalq_uc->push_back(this);
}
/*-------------------------------------------------------------*/
void MODEL_IR::expand()
{
	trace1("MODEL_IR::expand", dev_type());
	if (size()>2)
		_a1 = ++begin();
}
/*-------------------------------------------------------------*/
double MODEL_IR::tr_filt( const EVAL_BM_IR* c, double u)const
{
	// pushes u as u_0
	// returns \sum_{i>0} u_i a_i
	
	const MODEL_IR* m = this;
	trace2("EVAL_BM_IR::tr_filt .", m->size(), hp(c->_hist));
	assert(m);

	EVAL_BM_IR::hlist* new_h;
	if(c->_garbage){
		new_h = c->_garbage;
		c->_garbage = new_h->prev;
	} else {
		new_h = new hlist;
	}
	// new(u) -> last -> .... -> first

	new_h->prev = c->_hist;
	c->_hist = new_h;
	new_h->coef = (m->begin()); // begin with c_1
	new_h->d = u; // u_0

	hlist* h = new_h;
	u = 0;

	trace0("EVAL_BM_IR::tr_filt");
	size_t i = 0;
	while(h->prev){
		assert(h->prev->coef != m->end());
		//double cdelta = *(h->coef) - *(h->prev->coef);
		trace2("coef comp", *(h->coef), *(h->prev->coef) );

		(h->prev->coef)++;
		if(h->prev->coef == m->end()){
			h->prev->prev = c->_garbage;
			c->_garbage = h->prev;
			h->prev = 0;
			break;
		}

		h = h->prev;
		trace4("finite filter", h->d, *(h->coef), _sim->_time0,c->time_step());
		u += h->d * *(h->coef);
		i++;
	}
	trace2("EVAL_BM_IR::tr_filt", i, size());
	assert(i<=size());
//	assert(!h->prev);

	return u;
}
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
EVAL_BM_IR::EVAL_BM_IR(const EVAL_BM_IR& p):
		  EVAL_BM_ACTION_BASE(p),
		  _time_step(p._time_step),
		  _hist(p._hist) { }
/*-------------------------------------------------------------*/
EVAL_BM_IR::hlist* EVAL_BM_IR::_garbage; // garbage
/*-------------------------------------------------------------*/
EVAL_BM_IR::EVAL_BM_IR(int c):EVAL_BM_ACTION_BASE(c),
	_time_step(_default_time_step)
{}
/*-------------------------------------------------------------*/
void EVAL_BM_IR::set_param_by_name(string Name, string Value)
{
	if (Umatch (Name,"step")) { _time_step = Value; }
	else{ EVAL_BM_ACTION_BASE::set_param_by_name(Name,Value); }
}
/*-------------------------------------------------------------*/
bool EVAL_BM_IR::parse_params_obsolete_callback(CS& cmd)
{
	return ONE_OF
		|| Get(cmd, "step",	&_time_step)
		|| EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd) ;
}
/*-------------------------------------------------------------*/
void EVAL_BM_IR::precalc_first(const CARD_LIST*Scope)
{
	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());
	assert(m);

	EVAL_BM_ACTION_BASE::precalc_first(Scope);
	_time_step.e_val(m->timestep(), Scope);
}
/*-------------------------------------------------------------*/
void EVAL_BM_IR::precalc_last(const CARD_LIST*Scope)
{
	EVAL_BM_ACTION_BASE::precalc_last(Scope);
	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());
	assert(m);
	m->timestep( _time_step );
	_last = -inf;
	_v1 = _v0 = 0;
	assert(!_hist); // hmm
}
/*-------------------------------------------------------------*/
namespace FILTER_DISPATCHER { 
  //static MODEL_IR p1(0);
  //static DISPATCHER<MODEL_CARD>::INSTALL
  //  d1(&model_dispatcher, "filter", &p1);

  static ALPHA_COEF p2(0);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d2(&model_dispatcher, "alpha_iir", &p2);
}
/*-------------------------------------------------------------*/
void ALPHA_COEF::precalc_first()
{
	_lazy = 1;
	MODEL_IR::precalc_first();
	const CARD_LIST* par_scope = scope();
	_alpha.e_val( 0, par_scope);
	trace1("ALPHA_COEF::precalc_first", _alpha);

	_ha=_alpha*.5;

	if(_ha<=0 || _ha==1)
		error(bWARNING, "doesnt make sense.");
}
/*-------------------------------------------------------------*/
// with history compression...
double ALPHA_COEF::tr_filt( const EVAL_BM_IR* c, double u )const
{
	trace4("ALPHA_COEF::tr_filt", size(), u, dev_type(), _alpha );
	double tol = OPT::reltol; //sqrt(OPT::abstol);
	tol = sqrt(OPT::abstol);
//	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());

	//fixme: allocate at the end.
	hlist* new_h;
	if(c->_garbage){
		new_h = c->_garbage;
		c->_garbage = new_h->prev;
	} else {
		new_h = new hlist;
	}

	new_h->prev = c->_hist;
	c->_hist = new_h;
	new_h->coef = (begin());
	new_h->d = u;
	hlist* h = new_h;

	u = 0;

	size_t i = 0;
	while (h->prev){
		assert(h->prev->coef != end());
		trace2("coef comp", *(h->coef), *(h->prev->coef) );
		double e = fabs(*(h->coef)* h->d - *(h->prev->coef) * h->prev->d);
		e = fabs(( *(h->coef)* *(h->prev->coef) ) * (h->d -  h->prev->d));

		(h->prev->coef)++;
		assert(h->prev->coef != end()); // check if lazy evaluation works.
		u += h->prev->d * *(h->prev->coef);

		if(  OPT::filtercomp && e < tol && i ){
			// this does speed up things but doesnt maintain
			// tolerances in any useful way.
			hlist* tmp = h->prev->prev;
			h->prev->prev = c->_garbage;
			c->_garbage = h->prev;
			h->prev = tmp;

			h->d += c->_garbage->d * (*(c->_garbage->coef) / *(h->coef));
		//	cout << "... " << *(c->_garbage->coef) / *(h->coef) << "," << c->_garbage->d  << "\n";
			assert( *(c->_garbage->coef) - *(h->coef) );
			u += c->_garbage->d * *(c->_garbage->coef);

			if (!h->prev){
				trace0("tail break");
			  	break;
			}
			(h->prev->coef)++;
		} else {
//			cout << ".";
		}

		h = h->prev;
		i++;
	}
	// BUG: last?
	trace4("ALPHA_COEF::tr_filt", size(), i, dev_type(), _alpha );
	assert(h->coef != end());

	return u;
}
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
namespace INTEGRATE_FILTER {
class FILTER_INT : public MODEL_IR{
	public:
		FILTER_INT(COMPONENT* c=0);
		FILTER_INT(const FILTER_INT& p): MODEL_IR(p){}
		string value_name()const{ return "none";}
		string dev_type()const{ return "ir";}
		CARD* clone()const {return new FILTER_INT(*this);}
	public:
		double tr_filt(const EVAL_BM_IR* c, double u ) const;
		void tr_begin(){
			if(!size())
				_coef.push_front( 1 ); 

			assert (size()==1);
		}
};
/*-------------------------------------------------------------*/
FILTER_INT::FILTER_INT(COMPONENT* c) : MODEL_IR(c){}
/*-------------------------------------------------------------*/
double FILTER_INT::tr_filt(const EVAL_BM_IR* c, double u ) const
{
	// stash u as u_0
	// return \sum_+ c_i u_i
	
	// FIXME: a bit late...
	if (!c->_hist){
		c->_hist = new hlist();
		c->_hist->d=0;
		c->_hist->prev = 0;
	}
	if (!c->_hist->prev){
		c->_hist->prev = new hlist();
		c->_hist->prev->d = 0;
	}

	c->_hist->prev->d += c->_hist->d;
	c->_hist->d = u;

	assert(c->_hist);
	return c->_hist->prev->d ;
}
/*-------------------------------------------------------------*/
static FILTER_INT p1;
static DISPATCHER<MODEL_CARD>::INSTALL d1(&model_dispatcher, "int_filter", &p1);
}
/*-------------------------------------------------------------*/
namespace NOFILTER_MODEL {
class MODEL_NOFILT : public MODEL_IR{
	public:
		MODEL_NOFILT(COMPONENT* c=0);
		MODEL_NOFILT(const MODEL_NOFILT& p): MODEL_IR(p){}
		string value_name()const{ return "none";}
		string dev_type()const{ return "nofilt";}
		CARD* clone()const {return new MODEL_NOFILT(*this);}
	public:
		double tr_filt(const EVAL_BM_IR* c, double u ) const;
		void tr_begin(){
			if(!size())
				_coef.push_front( 1 ); 
			assert (size()==1);
		}
};
/*-------------------------------------------------------------*/
MODEL_NOFILT::MODEL_NOFILT(COMPONENT* c) : MODEL_IR(c){}
/*-------------------------------------------------------------*/
double MODEL_NOFILT::tr_filt(const EVAL_BM_IR* c, double u ) const
{ 
	if (!c->_hist){
		c->_hist = new hlist();
		c->_hist->d = 0;
	}

	assert(c->_hist);
	c->_hist->d = u;

	return 0; // \sum_{i>0} a_i u_i
}
/*-------------------------------------------------------------*/
static MODEL_NOFILT p1;
static DISPATCHER<MODEL_CARD>::INSTALL d1(&model_dispatcher, "nofilt", &p1);
}
