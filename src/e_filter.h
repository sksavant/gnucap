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
 * trying to implement FIR filter coefficients and the like
 */
#ifndef FILT_H__
#define FILT_H__

#include "e_model.h"
#include "bm.h"
#include "m_fft.h"
// impulse response filter
class EVAL_BM_IR : public EVAL_BM_ACTION_BASE {
	private:
		PARAMETER<double> _time_step;
	public:
		struct hlist{
			hlist* prev;
			double d;
			list<double>::const_iterator coef;
		};
	protected:
		explicit EVAL_BM_IR(const EVAL_BM_IR& p);
		virtual void set_param_by_name(string Name, string Value);
	public:
		explicit EVAL_BM_IR(int c=0);
		~EVAL_BM_IR() {}
		bool		operator==(const COMMON_COMPONENT&)const;
		double time_step()const{return _time_step;}
		double freq_step()const ; // {return 1./_time_step * size();}
		virtual bool parse_params_obsolete_callback(CS&);
		//virtual?
		TIME_PAIR tr_review(COMPONENT*);

	private: // override virtual
		COMMON_COMPONENT* clone()const {return new EVAL_BM_IR(*this);}
	//	void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
	protected:
		virtual void precalc_first(const CARD_LIST*);
		virtual void expand(const COMPONENT*);
		virtual void precalc_last(const CARD_LIST*);
		virtual void tr_eval(ELEMENT*)const;
		std::string	name()const	{untested();return modelname().c_str();}
		bool ac_too()const {untested();return false;} // how to do this?
	private:
		// double _time_step;
	public: // evalutaion stuff
		static hlist* _garbage; // garbage
		mutable hlist* _hist; // history
		mutable double _last;
		mutable double _v0;
		mutable double _v1;
		//mutable double* _coef;
	protected: // filter evaluators
		double infinite(double) const;
		//template<size_t O>
		double finite(double) const;
};
/*-------------------------------------------------------------*/
class MODEL_IR : public MODEL_CARD {
	protected:
		typedef EVAL_BM_IR::hlist hlist;
	private:
		//MODEL_IR(); // needed?
		PARAMETER<double>_step; // default ts for this model? can be overridden by common?
	protected:
	public:
		MODEL_IR(COMPONENT* d = 0) : MODEL_CARD(d),_lazy(0){} // needed?
		MODEL_IR(const MODEL_IR& p);
		virtual ~MODEL_IR();
		// string value_name()const{ return "none";}
		string dev_type()const{ return "none";}
		string short_label(){ return "_filter_coeffs";}
		virtual COMMON_COMPONENT* new_common()const	{return new EVAL_BM_IR;}
		CARD* clone()const {return new MODEL_IR(*this);}
	private:
		mutable double _time_step; // timestep for coeff generator.
#ifdef HAVE_FFTW_H
		spec_container* _spec;
#endif
	public:// tr analysis
		virtual bool do_tr();
		virtual void tr_begin();
		virtual TIME_PAIR tr_review();
		virtual double tr_filt(const EVAL_BM_IR*, double)const;
	public:
		void set_param_by_name(std::string Name, std::string Value);

		list<double>::const_iterator begin() const 
		{return _coef.begin();}
		list<double>::const_iterator end() const 
		{return _coef.end();}
		size_t size() const 
		{return _coef.size();}
	public:
		virtual void precalc_first( );
		virtual void expand( );

	public: // interface to BM_EVAL
		//	static MODEL_IR* dispatch( const COMMON_COMPONENT* );
		double timestep(double x=0) const;
		double a0()const{assert(_coef.size()); return *(_coef.begin());}
		list<double>::const_iterator a1()const{assert(_coef.size()>1); return _a1;}


	protected:
		virtual void new_coef() {unreachable();} // lazy coef generation.
		/*forward-*/list<double> _coef;
		double _timer;
		bool _lazy;
	private:
		list<double>::const_iterator _a1;

};
/*-------------------------------------------------------------*/
inline void MODEL_IR::set_param_by_name(std::string Name, std::string Value)
{
  if (Umatch (Name,"step{size}")) { _step = Value; }
  else{ throw Exception_No_Match(Name); }
}
/*-------------------------------------------------------------*/
inline MODEL_IR::~MODEL_IR(){
	incomplete();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline bool EVAL_BM_IR::operator==(const COMMON_COMPONENT& x)const
{
	const EVAL_BM_IR* p = dynamic_cast<const EVAL_BM_IR*>(&x);
	bool rv = p
		&& _time_step == p->_time_step
		&& !OPT::trnoise
		&& EVAL_BM_ACTION_BASE::operator==(x);
	if (rv) {untested();
	}else{
	}
	return rv;
}
/*-------------------------------------------------------------*/
//inline MODEL_IR::MODEL_IR(): MODEL_CARD(0)
//{
//	trace0("MODEL_IR::MODEL_IR()");
//}
/*-------------------------------------------------------------*/
inline TIME_PAIR MODEL_IR::tr_review()
{
	//	trace2("NOISE_COEF::tr_review", _ha, _time_step);
	return TIME_PAIR();
}
/*-------------------------------------------------------------*/
inline bool MODEL_IR::do_tr(){
	if(_timer < _sim->_time0 + 0.01*OPT::dtmin){
		new_coef();
		_timer = _sim->_time0 + _time_step;
	}

	_sim->_evalq_uc->push_back(this);
	return true;
}
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
// 1/f^alpha spectrum filter.
/*-------------------------------------------------------------*/
class ALPHA_COEF: public MODEL_IR{
	public:
		ALPHA_COEF(double);
		ALPHA_COEF(const ALPHA_COEF& p): MODEL_IR(p), _ha(p._ha){}
		string value_name()const{ return "alpha";}
		CARD* clone()const {return new ALPHA_COEF(*this);}
	private:
		PARAMETER<double> _alpha;
		void new_coef();
		double _ha; // half of alpha
	public:
		void set_param_by_name(std::string Name, std::string Value);
		void precalc_first();
		double ha()const{return _ha;}
		void tr_begin();
		virtual double tr_filt(const EVAL_BM_IR*, double)const;
};
/*-------------------------------------------------------------*/
inline void ALPHA_COEF::new_coef(){
	assert((double)_ha && _ha!=1);
	size_t k = _coef.size();
	double h_k = *(_coef.rbegin());
	double h_kp1 = h_k * (_ha + double(k-1))/(double(k));
	trace3("ALPHA_COEF::new_coef", k, h_k, h_kp1 );
	_coef.push_back( h_kp1 );
}
/*--------------------------------------------------------------------------*/
inline void ALPHA_COEF::tr_begin(){
	MODEL_IR::tr_begin();
	assert(!size()); //fixme.
	_coef.push_front( 1 ); // bug?
	new_coef(); // to be sure...
	new_coef(); // to be sure...
	_sim->_evalq_uc->push_back(this);
}
/*-------------------------------------------------------------------------------*/
inline ALPHA_COEF::ALPHA_COEF(double alpha): MODEL_IR(0), _ha(.5*alpha)
{
	unreachable();
	set_label(".alpha_noise_"+to_string((double)alpha));
}
/*-------------------------------------------------------------------------------*/
inline void ALPHA_COEF::set_param_by_name(std::string Name, std::string Value)
{
  if (Umatch (Name,"alpha")) { _alpha = Value; }
  else{ MODEL_IR::set_param_by_name(Name,Value);}
}
/*-------------------------------------------------------------------------------*/

#endif
