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
 * trying to implement noise models
 *
 */
//testing=script,complete 2005.10.07
#include "e_elemnt.h"
#include "e_model.h"
#include "e_filter.h"
#include "u_lang.h"
#include "bm.h"
#include <gsl/gsl_randist.h>
#include <list>
#include <vector>

using namespace std;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_rms(0);
const double _default_alpha(1);
const double _default_amplitude(1);
const double _default_deviation(1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EVAL_BM_NOISE : public EVAL_BM_IR {
	enum rndtype_t{
		rGAUSS,
		rUNIFORM,
		rEXPRAND,
		rFISH
	};
	private:
		PARAMETER<double> _rms;
	public:// fixme
		PARAMETER<double> _alpha;
	private:
		PARAMETER<double> _amplitude;
		PARAMETER<double> _deviation;
//		mutable double _actual_frequency;
		explicit	EVAL_BM_NOISE(const EVAL_BM_NOISE& p);
		rndtype_t rndtype;
	private: // these are const, but in fact change the filter state.
		double get_rand() const;
		double white(double) const;
	public:
		explicit EVAL_BM_NOISE(int c=0);
		~EVAL_BM_NOISE()		{}
	private: // override vitrual
		bool operator==(const COMMON_COMPONENT&)const;
		COMMON_COMPONENT* clone()const {return new EVAL_BM_NOISE(*this);}
		void print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
		void set_param_by_name(std::string Name, std::string Value);

		void expand(const COMPONENT* );
		void precalc_first(const CARD_LIST*);
		void precalc_last(const CARD_LIST*);
		void tr_eval(ELEMENT*)const;
		void tr_begin(ELEMENT*)const;
		TIME_PAIR tr_review(COMPONENT*);
		std::string	name()const {return "noise";}
		bool ac_too()const {return false;}
		//		bool parse_numlist(CS&);
		bool parse_params_obsolete_callback(CS&);
	private:
		static gsl_rng *_rng;
};
/*--------------------------------------------------------------------------*/
gsl_rng* EVAL_BM_NOISE::_rng;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double EVAL_BM_NOISE::get_rand()const // get a random value.
{
	double r;
	switch (rndtype) {
		case rUNIFORM:
			/* param1: range -param1[ ... +param1[  (default = 1)
			 *                param2: offset  (default = 0)
			 *                            */
			r = 0; //(param1 * drand() + param2);
			break;
		case rGAUSS:
			r = gsl_ran_gaussian(_rng,_amplitude);
			break;
		case rEXPRAND:
			/* param1: mean (default = 1)
			 *                param2: offset  (default = 0)
			 *                            */
			r = 0; // exprand(param1) + param2;
			break;
		case rFISH:
			/* param1: lambda (default = 1)
			 *                param2: offset  (default = 0)
			 *                            */
			r = 0; // (double)poisson(param1) + param2;
			break;
		default:
			r = 0.0;
			break;
	}

	return r;
}
/*--------------------------------------------------------------------------*/
EVAL_BM_NOISE::EVAL_BM_NOISE(int c) :EVAL_BM_IR(c),
	_rms(_default_rms),
	_alpha(_default_alpha),
	_amplitude(_default_amplitude),
	_deviation(_default_deviation)
{
	trace1("EVAL_BM_NOISE::EVAL_BM_NOISE(c)", c);
	const gsl_rng_type * T;
	T = gsl_rng_default;

	if (!_rng){
		gsl_rng_env_setup();
		_rng = gsl_rng_alloc(T);
	}
	_v1 = 0;
	_hist = 0;
}
/*--------------------------------------------------------------------------*/
EVAL_BM_NOISE::EVAL_BM_NOISE(const EVAL_BM_NOISE& p) : EVAL_BM_IR(p),
	_rms(p._rms),
	_alpha(p._alpha),
	_amplitude(p._amplitude),
	_deviation(p._deviation)
{
	trace1("EVAL_BM_NOISE::EVAL_BM_NOISE(p)", hp(model()));
	if(p.model()) assert (model());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_NOISE::operator==(const COMMON_COMPONENT& x)const
{
	const EVAL_BM_NOISE* p = dynamic_cast<const EVAL_BM_NOISE*>(&x);
	bool rv = p
		&& _rms == p->_rms
		&& _alpha == p->_alpha
		&& _amplitude == p->_amplitude
		&& !OPT::trnoise
		&& EVAL_BM_IR::operator==(x);
	if (rv) {untested();
	}else{
	}
	return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_NOISE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
	assert(lang);
	o << name();
	print_pair(o, lang, "rms", _rms);
	print_pair(o, lang, "alpha", _alpha);
	print_pair(o, lang, "amplitude", _amplitude);
	EVAL_BM_IR::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_NOISE::precalc_first(const CARD_LIST* Scope)
{
	assert(Scope);
	EVAL_BM_ACTION_BASE::precalc_first(Scope);
	_rms.e_val(_default_rms, Scope);
	_alpha.e_val(_default_alpha, Scope);
	_amplitude.e_val(_default_amplitude, Scope);
	_deviation.e_val(_default_deviation, Scope);

	trace2("EVAL_BM_NOISE::precalc_first(Scope)", _amplitude, time_step() );

	_hist = 0;
	rndtype = rGAUSS; 
	_last = -inf;

	assert(has_good_value(_alpha)); //?
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_NOISE::expand(const COMPONENT*d)
{
	trace0("EVAL_BM_NOISE::expand");
	trace0("EVAL_BM_NOISE::expand attaxching");

	//	if (modelname) { return };
	//
	//	otherwise: prepare model...
	//
	string basemodel="";

	if ((double)_alpha == 0){
		set_modelname(".nofilt");
		basemodel="nofilt";
	} else if ((double)_alpha == 2){ // brownian noise
		set_modelname(".int_filter");
		basemodel="int_filter";
	} else {
		set_modelname(".alpha_noise_"+to_string((double)_alpha));
		basemodel="alpha_iir";
	}

	try {
		trace1("EVAL_BM_NOISE::expand", modelname());
		d->find_model(modelname());
	}catch (Exception_Cant_Find){
		trace1("EVAL_BM_NOISE::expand new model", basemodel);
		MODEL_IR* p = dynamic_cast<MODEL_IR*>( model_dispatcher[basemodel]);
		if(!p) throw(  Exception_Cant_Find("bm_noise", basemodel) );
		MODEL_IR* m = asserted_cast<MODEL_IR*>(p->clone());
		m->set_label(modelname());

		trace1("EVAL_BM_NOISE::expand new model", m->short_label());
		if ((double)_alpha!=2 && (double)_alpha){
			//ouch.
			m->set_param_by_name("alpha",to_string((double)_alpha));
		}
		CARD_LIST::card_list.push_back((CARD*)m);
		m->precalc_first();
	}

	EVAL_BM_IR::expand(d);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_NOISE::precalc_last(const CARD_LIST* Scope)
{
	EVAL_BM_IR::precalc_last(Scope);
	assert(Scope);
	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());
	assert(m); USE(m);
}
/*--------------------------------------------------------------------------*/
// override EVAL_BM_IR::tr_eval.
// shouldnt be necessary, but...
void EVAL_BM_NOISE::tr_eval(ELEMENT* d)const
{
	const MODEL_IR* m = dynamic_cast<const MODEL_IR*>(model());
	trace5("EVAL_BM_NOISE::tr_eval", time_step(), _last, d->_sim->_time0, _v1, _alpha);
	double value=0;
	double input0 = 0; // input at _last+step
	double input1 = 0; // input at _last
	if(_hist)
		input1 = _hist->d;

	if(OPT::trnoise) {
		if ( _last + time_step() <= d->_sim->_time0 + 0.01*OPT::dtmin ){
			double newinput = get_rand();
			_v1 = _v0 + input1 * m->a0();
			_v0 = m->tr_filt(this, newinput) ;
			_last = d->_sim->_time0;
		}
		trace4("EVAL_BM_NOISE::tr_eval got new", _v0, d->_sim->_time0, value, _alpha);
	}

	assert(_hist);
	input0 = _hist->d;
	double q = (d->_sim->_time0 - _last) / time_step(); // history fraction of frame 

   if(d->f_is_value()){ // vs and the like
		double value_now = _v1 + (_v0 + input0 - _v1) * q;
		// tr_finish_tdv(d, value_now  );
		//	write value_now into f1
		//	and ooffset into f0?
		d->_y[0] = FPOLY1(CPOLY1(ioffset(d->_y[0].x), 0., value_now));
		tr_final_adjust(&(d->_y[0]), false);
	} else { // vcvs and the like
		untested();
		// does this even make sense?
		double off =  _v1 + (_v0+m->a0()*d->tr_involts()*q -_v1) * q;
		value = (d->_sim->_time0 - _last) / time_step() * m->a0();

		d->_y[0] = FPOLY1(CPOLY1(ioffset(d->_y[0].x), 0 , value*(1.-q)));
		tr_final_adjust(&(d->_y[0]), d->f_is_value());
		d->_y[0].f0 += off;
	}
}
/*--------------------------------------------------------------------------*/
//template<unsigned order>
//double EVAL_BM_NOISE::finite<order>( double u) const
//{
////	generic finite order filter
//
//}
/*--------------------------------------------------------------------------*/
double EVAL_BM_NOISE::white(double u) const{
	return u;
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_NOISE::tr_review(COMPONENT* d)
{
	trace2("EVAL_BM_NOISE::tr_review", d->_sim->_time0, _last);
	d->_time_by.min_event(_last + time_step());
	return d->_time_by;
}
/*--------------------------------------------------------------------------*/
//bool EVAL_BM_NOISE::parse_numlist(CS& cmd)
//{
//	unsigned start = cmd.cursor();
//	unsigned here = cmd.cursor();
//	for (PARAMETER<double>* i = &_offset;  i < &_end;  ++i) {
//		PARAMETER<double> val(NOT_VALID);
//		cmd >> val;
//		if (cmd.stuck(&here)) {
//			break;
//		}else{
//			*i = val;
//		}
//	}
//	return cmd.gotit(start);
//}
/*--------------------------------------------------------------------------*/
void EVAL_BM_NOISE::set_param_by_name(std::string Name, std::string Value)
{
	trace2("EVAL_BM_NOISE::set_param_by_name", Name, Value);
	if (Umatch (Name,"r{ms}")) { _rms = Value; }
	else if (Umatch (Name,"a{lpha}")) { _alpha = Value; }
	else if (Umatch (Name,"amp{litude}")) { _alpha = Value; }
	else{ EVAL_BM_IR::set_param_by_name(Name,Value); }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_NOISE::parse_params_obsolete_callback(CS& cmd)
{
	return ONE_OF
		|| Get(cmd, "r{ms}",	&_rms)
		|| Get(cmd, "alpha",	&_alpha) // 1/f exponent
		|| Get(cmd, "amp{litude}",	&_amplitude) // 1/f
		|| EVAL_BM_IR::parse_params_obsolete_callback(cmd) ;
}
/*--------------------------------------------------------------------------*/
EVAL_BM_NOISE p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "noise|trnoise", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
