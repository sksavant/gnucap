/* (c) felix salfelder 2012
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
 * this measurement device implements online measurements
 * 
 */
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_MEAS : public ELEMENT {
	private:
		double _m, _v1;
	protected:
		explicit DEV_MEAS(const DEV_MEAS& p) :ELEMENT(p) {}
	public:
		explicit DEV_MEAS()	:ELEMENT() {}
	protected: // override virtual
		std::string value_name()const {return "what";}
		std::string dev_type()const	{return "meas1";}
		uint_t	   max_nodes()const	{return 2;}
		uint_t	   min_nodes()const	{return 2;}
		uint_t	   matrix_nodes()const	{return 2;}
		uint_t	   net_nodes()const	{return 2;}
		//bool	   has_iv_probe()const  {return true;}
		bool	   use_obsolete_callback_parse()const {return false;}
		CARD*	   clone()const		{return new DEV_MEAS(*this);}
		void     precalc_last();
		hp_float_t   tr_involts()const	{return tr_outvolts();}
		hp_float_t   tr_involts_limited()const {return tr_outvolts_limited();}
		COMPLEX  ac_involts()const	{untested();return ac_outvolts();}
		virtual void tr_iwant_matrix(){}
		virtual void ac_iwant_matrix(){}
		std::string port_name(uint_t i)const {
			assert(i != INVALID_NODE);
			assert(i < 2);
			static std::string names[] = {"p", "n"};
			return names[i];
		}
	public:

	public:// tt overrides
		double tt_probe_num(const std::string& )const;
		void tt_advance(){ 
			ELEMENT::tt_advance();
			trace0("DEV_MEAS::tt_advance()");
			_m = _v1 = 0; q_accept(); 
		}
	private:
		double _last;

	public:// overrides
		bool tr_needs_eval()const{return true;}
		double tr_probe_num(const std::string& )const;
//		void tr_begin(){q_eval();}
		void tr_begin(){q_accept(); _m = _v1 = 0; }
		bool do_tr(){ 
			q_accept();
			return ELEMENT::do_tr(); // true.
		}
		void tr_accept(){
			ELEMENT::tr_accept();
			tr_stress();
		}
		void tr_stress(){
			if(_sim->is_initial_step()){
				_m = 0;
				_v1 = tr_involts();
			} else if( _last<_sim->_time0){
				_m += _sim->_dt0 * ( tr_involts() + _v1 ) ;
				_v1 = tr_involts();
			}else{
				// untested();
			}
			_last= _sim->_time0;

			q_eval();
		}
		void tr_stress_last(){ _m /= _sim->last_time()*2.0; q_eval(); }

};
/*--------------------------------------------------------------------------*/
double DEV_MEAS::tr_probe_num(const std::string&x) const{
  if (Umatch(x, "v ")) {
    return  tr_involts();
  }else if (Umatch(x, "last ")) {
    return  _last;
  }else if (Umatch(x, "avg ")) { 
	  return _m/(_sim->_time0*2.0);
  }else if (Umatch(x, "m ")) { 
	  return _m;

  }
  return ELEMENT::tr_probe_num(x);
}
/*--------------------------------------------------------------------------*/
double DEV_MEAS::tt_probe_num(const std::string&) const{
	return _m;
}
/*--------------------------------------------------------------------------*/
void DEV_MEAS::precalc_last()
{
	ELEMENT::precalc_last();
	set_constant(true);
	set_converged(true);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_MEAS p1;
DISPATCHER<CARD>::INSTALL
	d1(&device_dispatcher, "meas1|measure1", &p1);
}
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
// experimental freq meas.

namespace blah {
/*--------------------------------------------------------------------------*/
enum	st_t{ DUNNO=0, TOP=1, BOTTOM=-1 };

class DEV_FMEAS : public ELEMENT {
	private:
		double _hi, _lo;
		double _v1;
		double _f;
		double _trig;
		st_t _p;
		unsigned _c; // counter
	protected:
		explicit DEV_FMEAS(const DEV_FMEAS& p) :ELEMENT(p) {}
	public:
		explicit DEV_FMEAS()	:ELEMENT() {}
	protected: // override virtual
		std::string value_name()const {return "what";}
		std::string dev_type()const	{return "fmeas1";}
		uint_t	   max_nodes()const	{return 2;}
		uint_t	   min_nodes()const	{return 2;}
		uint_t	   matrix_nodes()const	{return 2;}
		uint_t	   net_nodes()const	{return 2;}
		//bool	   has_iv_probe()const  {return true;}
		bool	   use_obsolete_callback_parse()const {return false;}
		CARD*	   clone()const		{return new DEV_FMEAS(*this);}
		void     precalc_last();
		hp_float_t   tr_involts()const	{return tr_outvolts();}
		hp_float_t   tr_involts_limited()const {return tr_outvolts_limited();}
		COMPLEX  ac_involts()const	{untested();return ac_outvolts();}
		virtual void tr_iwant_matrix(){}
		virtual void ac_iwant_matrix(){}
		std::string port_name(uint_t i)const {
			assert(i != INVALID_NODE);
			assert(i < 2);
			static std::string names[] = {"p", "n"};
			return names[i];
		}
	public:

	public:// tt overrides
		double tt_probe_num(const std::string& )const;
		void tt_advance(){ 
			ELEMENT::tt_advance();
			trace0("DEV_MEAS::tt_advance()");
			_v1 = 0; q_accept(); 
			_c = 0;
			_hi = -inf;
			_lo = inf;
			_trig=0;
		}
	private:
		double _last;

	public:// overrides
		bool tr_needs_eval()const{return true;}
		double tr_probe_num(const std::string&x) const;
//		void tr_begin(){q_eval();}
		void tr_begin(){q_accept(); _v1 = 0; }
		bool do_tr(){ 
			q_accept();
			return ELEMENT::do_tr(); // true.
		}
		void tr_accept(){
			ELEMENT::tr_accept();

			if(_sim->is_initial_step()){
				_v1 = _hi = _lo = tr_involts();
				_p = DUNNO;
			} else if( _last<_sim->_time0){
				double in = tr_involts();
				switch (_p){
					case DUNNO:
						if( in > _v1 ){
							_p = TOP;
							_c++;
							_trig = in - _hi;
						}else if(in < _v1){
							_p = BOTTOM;
							_c++;
							_trig = _lo - in;
						}
						break;
					case TOP:
						if( in > 0.2*_hi+0.8*_lo ){
							// no transition to B
							if(in > _lo+2*_trig){
								_c = 1;
								_trig = in - _lo;
							}
						}else{
							_p = BOTTOM;
							_c++;
							_trig = _hi - in;
						}
						break;
					case BOTTOM:
						if( in < 0.2*_lo+0.8*_hi  ){
							if( in < _hi-2*_trig){
								_c = 1;
								_trig = _hi - in;
							}
						}else{
							_p = TOP;
							_c++;
							_trig = in-_lo;
						}
				}
				// _m += _sim->_dt0 * ( tr_involts() + _v1 ) ;
				_v1 = in;
				_hi = max(_v1,_hi);
				_lo = min(_v1,_lo);
			}else{
				// untested();
			}
			_last= _sim->_time0;

			q_eval();
		}
		void tr_stress_last(){ _f =  double((_c+1)/2) / (_sim->last_time()); q_eval(); }

	/*--------------------------------------------------------------------------*/
};
/*--------------------------------------------------------------------------*/
double DEV_FMEAS::tt_probe_num(const std::string&x) const{
	if (Umatch(x, "f ")) {
		return _f;
	}
	return -1;
}
/*--------------------------------------------------------------------------*/
// FIXME: cleanup.
double DEV_FMEAS::tr_probe_num(const std::string&x) const{
	if (Umatch(x, "v ")) {
		return  tr_involts();
	}else if (Umatch(x, "c ")) {
		return  _c;
	}else if (Umatch(x, "t ")) { 
		return _trig;
	}else if (Umatch(x, "s |p ")) { 
		return _p;
	}else if (Umatch(x, "last ")) { 
		return _last;

	}
	return ELEMENT::tr_probe_num(x);
}
/*--------------------------------------------------------------------------*/
void DEV_FMEAS::precalc_last()
{
	ELEMENT::precalc_last();
	set_constant(true);
	set_converged(true);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_FMEAS p1;
DISPATCHER<CARD>::INSTALL
	d1(&device_dispatcher, "fmeas1|fmeasure1", &p1);
}
/*--------------------------------------------------------------------------*/
