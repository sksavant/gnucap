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
//testing=script 2006.07.17
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

	public:// overrides
		double tr_probe_num(const std::string& )const;
//		void tr_begin(){q_eval();}
		void tr_begin(){q_accept(); _m = _v1 = 0; }
		bool do_tr(){ 
			q_accept();
			return ELEMENT::do_tr();
		}
		void tr_accept(){
			ELEMENT::tr_accept();
			tr_stress();
		}
		void tr_stress(){
			if(_sim->is_initial_step()){
				_m = 0;
			} else {
				_m += _sim->_dt0 * ( tr_involts() + _v1 ) ;
			}
			_v1 = tr_involts();

			q_eval();
		}
		void tr_stress_last(){ _m /= _sim->last_time()*2.0; q_eval(); }

};
/*--------------------------------------------------------------------------*/
double DEV_MEAS::tr_probe_num(const std::string&) const{
	return _sim->is_initial_step();
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
