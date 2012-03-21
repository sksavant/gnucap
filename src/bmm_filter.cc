/* Copyright (C) 2012 Felix Salfelder
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
 * finite impulse response filter
 */

#include "e_filter.h"
#include "e_elemnt.h"
#include "bm.h"
#include "u_para_vec.h"

class EVAL_BM_FIR : public EVAL_BM_IR {
	protected:
		explicit EVAL_BM_FIR(const EVAL_BM_FIR& p):
		  EVAL_BM_IR(p){
		}
	public:
		explicit EVAL_BM_FIR(int c=0);
		~EVAL_BM_FIR() {}
	private: // override virtual
		bool		operator==(const COMMON_COMPONENT&)const { return false;} // the bm stores history...
		COMMON_COMPONENT* clone()const {return new EVAL_BM_FIR(*this);}
		void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
		void  	expand(const COMPONENT*);
//		void		tr_eval(ELEMENT*)const; //use _IR
		std::string	name()const	{untested();return modelname().c_str();}
		bool		ac_too()const		{untested();return false;} // how to do this?
	private:
//		mutable double _last;
	//	double _time_step;
	public:

};
/*--------------------------------------------------------------------------*/
class MODEL_FIR : public MODEL_IR {
	public:
		PARAMETER< dpv > _coeff;
		SPLINE* _spline;
	private:
		void new_coef(){} // hmmm not necessary?
		explicit MODEL_FIR(const MODEL_FIR& p): MODEL_IR(p),
		_coeff(p._coeff) {
			trace2("cloned MODEL_FIR", _coeff, dpv(_coeff));
		}
	public:
		explicit MODEL_FIR();
		virtual ~MODEL_FIR();
		void set_param_by_name(std::string Name, std::string Value);
		//void tr_begin();
	private: // override virtual
		std::string dev_type()const		{return "fir";}
		void  precalc_first();
		void  expand() { MODEL_IR::expand();}
		COMMON_COMPONENT* new_common()const	{return new EVAL_BM_FIR;}
		CARD* clone()const {return new MODEL_FIR(*this);}

		bool use_obsolete_callback_print()const {return false;}
		bool use_obsolete_callback_parse()const {return false;}
		// void print_args_obsolete_callback(OMSTREAM&,LANGUAGE*)const;
		// bool parse_params_obsolete_callback(CS&);

		// void tr_eval(COMPONENT*)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void EVAL_BM_IR::expand(const COMPONENT*d)
{
	attach_model(d);
	EVAL_BM_ACTION_BASE::expand(d);
	trace1("EVAL_BM_IR::expand", _time_step);
	assert(_time_step);

	// not here...?
	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());
	if (!m) {untested();
		throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "ir");
	}else{
	}
}
/*-------------------------------------------------------------*/
TIME_PAIR EVAL_BM_IR::tr_review(COMPONENT* d)
{
	d->_time_by.min_event(_last + time_step());
	return d->_time_by;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_FIR::EVAL_BM_FIR(int c):EVAL_BM_IR(c){}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIR::print_common_obsolete_callback( OMSTREAM&, LANGUAGE* )const
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIR::expand(const COMPONENT*d)
{
  EVAL_BM_IR::expand(d);

  const MODEL_FIR* m = dynamic_cast<const MODEL_FIR*>(model());
  if (!m) {untested();
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "fir");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_IR::tr_eval(ELEMENT*d)const
{
	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());
	assert(time_step());
	trace4("EVAL_BM_FIR::tr_eval", time_step(), _last, d->_sim->_time0, _v1);
	double off = 0 ;
	double value = 0;
	double input = 0;
	// BUG: what if time steps backwards?
	if ( _last + time_step() <= d->_sim->_time0 + 0.01*OPT::dtmin ){

		if (_hist)
			_hist->d = d->tr_involts();

		if(_hist)
			input = _hist->d;
		_v1 = _v0 + input * m->a0();

		// tr_involts is _NOT_ the future value...
		_v0 = m->tr_filt(this, d->tr_involts());
		_last = d->_sim->_time0;
		// event(now+sampletime); tr_review
		off =  _v1 + (_v0-_v1)* (d->_sim->_time0 - _last) / time_step() ;

		trace5("EVAL_BM_FIR::tr_eval got new", _v1, _v0, d->_sim->_time0, d->tr_involts(), off);
	}

	_hist->d = d->tr_involts();
	assert(m->a0()==1);
	double q = (d->_sim->_time0 - _last) / time_step(); // history fraction of frame 

	assert(!d->f_is_value());
	if(!d->f_is_value()){ // vcvs and the like
		value = q * m->a0();
		off = _v1 + (_v0 + value * d->tr_involts() - _v1) * q;
		d->_y[0] = FPOLY1(CPOLY1(ioffset(d->_y[0].x), 0 , value*(1.-q)));
		tr_final_adjust(&(d->_y[0]), d->f_is_value());
		d->_y[0].f0 += off;
	} else { untested();
		// does this even make sense?

	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_FIR::MODEL_FIR() : MODEL_IR(NULL),
  _coeff( PARAMETER< dpv>() ) {}
/*--------------------------------------------------------------------------*/
MODEL_FIR::~MODEL_FIR(){}
/*--------------------------------------------------------------------------*/
void MODEL_FIR::precalc_first()
{
	MODEL_IR::precalc_first();
	const CARD_LIST* par_scope = scope();
	assert(par_scope);
	dpv tmp ( _coeff.e_val(dpv(), par_scope) );

	_coef.clear();

	dpv a = (dpv)_coeff;
	dpv::iterator i = a.begin();
	trace1("MODEL_FIR::precalc_first 1", *i);

	// argh. invalid read??
	//i = dpv(_coeff).begin();
	//trace1("MODEL_FIR::precalc_first 2", *i);
	//i = ((dpv)_coeff).begin();
	//trace1("MODEL_FIR::precalc_first 3", *i);
	   
	for (dpv::const_iterator i = a.begin(); 
			i!=a.end(); i++){
		_coef.push_back( *i); untested();
	}

	assert(_coef.size() || ! _coeff.size());
	assert(_coef.size() == _coeff.size());

	assert(timestep());
}
/*--------------------------------------------------------------------------*/
//void MODEL_FIR::tr_begin(){
//	MODEL_IR::tr_begin();
//}
/*--------------------------------------------------------------------------*/
//void MODEL_FIR::tr_eval(COMPONENT*)const{
//	trace0("MODEL_FIR::tr_eval");
//}
/*--------------------------------------------------------------------------*/
void MODEL_FIR::set_param_by_name(std::string Name, std::string Value)
{
  if (Umatch (Name,"coef{fs}")) { _coeff = Value; }
  else{ MODEL_IR::set_param_by_name(Name,Value);}
}
/*--------------------------------------------------------------------------*/
namespace FIR_DISP{

//	static MODEL_IR p1(0);
//	static DISPATCHER<MODEL_CARD>::INSTALL
//	d1(&model_dispatcher, "ir|ir_filter", &p1);

	static MODEL_FIR p2;
	static DISPATCHER<MODEL_CARD>::INSTALL
	d2(&model_dispatcher, "fir|fir_filter", &p2);
}
/*--------------------------------------------------------------------------*/
