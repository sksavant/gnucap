/*$Id: e_base.h,v 26.107 2008/12/19 06:13:23 al Exp $ -*- C++ -*-
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
 * real base for anything to do with a circuit
 */
//testing=script 2007.07.13
#ifndef E_BASE_H
#define E_BASE_H
#include "l_compar.h"
#include "u_opt.h"
#include "u_status.h"
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
// this file
class CKT_BASE;
/*--------------------------------------------------------------------------*/
// external
class XPROBE;
class CS;
/*--------------------------------------------------------------------------*/
class INTERFACE CKT_BASE {
private:
  mutable int	_probes;		/* number of probes set */
  std::string	_label;
public:
  static double _vmax;
  static double _vmin;
  enum TRI_STATE {tsNO=0, tsYES=1, tsBAD=-1};
private:
  static TRI_STATE inc_mode;	/* flag: make incremental changes (3 state) */
public:
  static SIM_MODE _mode;	/* simulation type (AC, DC, ...) */
  static SIM_PHASE _phase;	/* phase of simulation (iter, init-dc,) */
  static BSMATRIX<double>  aa;	/* raw matrix for DC & tran */
  static BSMATRIX<double>  lu;	/* decomposed matrix for DC & tran */
  static BSMATRIX<COMPLEX> acx;	/* raw & decomposed matrix for AC */
protected:
  explicit CKT_BASE()			  :_probes(0), _label() {}
  explicit CKT_BASE(const std::string& s) :_probes(0), _label(s) {}
  explicit CKT_BASE(const CKT_BASE& p)	  :_probes(0), _label(p._label) {}
  virtual  ~CKT_BASE();
public:
  bool operator!=(const std::string& n)const
    {return strcasecmp(_label.c_str(),n.c_str())!=0;}
  virtual const std::string long_label()const;
  const std::string&  short_label()const {return _label;}
	  double      probe_num(const std::string&)const;
	  double      ac_probe_num(const std::string&)const;
  virtual double      tr_probe_num(const std::string&)const;
  virtual XPROBE      ac_probe_ext(const std::string&)const;
	  void	      inc_probes()const	{++_probes;}
	  void	      dec_probes()const	{assert(_probes>0); --_probes;}
	  bool	      has_probes()const	{return _probes > 0;}
  static  void	      set_limit(double v);
  static  void	      clear_limit();

  void	set_label(const std::string& s) {_label = s;}
protected:
  //--------------------------------------------------------------------
  // tamed access to the environment
  void	reset_iteration_counter(int i)
    {assert(up_order(0,i,iCOUNT-1)); ::status.iter[i] = 0;}
  void	count_iterations(int i)
    {assert(up_order(0,i,iCOUNT-1)); ++::status.iter[i];}
public:
  int	iteration_tag()const
    {return ::status.iter[iTOTAL];}
  int	iteration_number()const
    {return ::status.iter[iSTEP];}

  static bool initial_step()
    {return (::status.iter[_mode] <= 1  && analysis_is_static());}
  bool is_advance_iteration()const
    {return (::status.iter[iSTEP] == 0);}
  bool	is_advance_or_first_iteration()const
    {assert(::status.iter[iSTEP] >= 0); return (::status.iter[iSTEP] <= 1);}
  bool	is_first_iteration()const
    {assert(::status.iter[iSTEP] > 0); return (::status.iter[iSTEP] == 1);}
  bool	is_second_iteration()const
    {assert(::status.iter[iSTEP] > 0); return (::status.iter[iSTEP] == 2);}
  bool	is_iteration_number(int n)const
    {return (::status.iter[iSTEP] == n);}
  bool	exceeds_iteration_limit(OPT::ITL itlnum)const
    {return(::status.iter[iSTEP] > OPT::itl[itlnum]);}

  static bool command_is_ac()	   {itested();return _mode == s_AC;}
  static bool command_is_dc()	   {itested();return _mode == s_DC;}
  static bool command_is_op()	   {itested();return _mode == s_OP;}
  //static bool command_is_tran()    {untested();return _mode == s_TRAN;}
  //static bool command_is_fourier() {untested();return _mode == s_FOURIER;}

  static bool analysis_is_ac()     {return _mode == s_AC;}
  static bool analysis_is_dcop()   {return _mode == s_DC || _mode == s_OP;}
  //bool analysis_is_ic()const

  static bool analysis_is_static()
  	{return _phase == p_INIT_DC || _phase == p_DC_SWEEP;}
  static bool analysis_is_restore()
	{return _phase == p_RESTORE;}

  static bool analysis_is_tran()
	{return _mode == s_TRAN || _mode == s_FOURIER;}
  static bool analysis_is_tran_static()
  	{itested();return analysis_is_tran() && _phase == p_INIT_DC;}
  static bool analysis_is_tran_restore()
  	{itested();itested(); return analysis_is_tran() && _phase == p_RESTORE;}
  static bool analysis_is_tran_dynamic()
	{return analysis_is_tran() && _phase == p_TRAN;}

  static void set_command_none()  {_mode = s_NONE;}
protected:
  void set_command_ac()const	  {_mode = s_AC;}
  void set_command_dc()const	  {_mode = s_DC;}
  void set_command_op()const	  {_mode = s_OP;}
  void set_command_tran()const	  {_mode = s_TRAN;}
  void set_command_fourier()const {_mode = s_FOURIER;}
  //void set_analysis_ic()const
  //void set_analysis_static()const

public:
  static bool is_inc_mode()	 {return inc_mode;}
  static bool inc_mode_is_no()	 {return inc_mode == tsNO;}
  static bool inc_mode_is_bad()	 {return inc_mode == tsBAD;}
protected:
  static void set_inc_mode_bad() {inc_mode = tsBAD;}
  static void set_inc_mode_yes() {inc_mode = tsYES;}
  static void set_inc_mode_no()  {inc_mode = tsNO;}
  static void mark_inc_mode_bad() {
    switch (inc_mode) {
    case tsYES: inc_mode = tsBAD; break;
    case tsBAD: break;
    case tsNO:  break;
    }
  }
};
/*--------------------------------------------------------------------------*/
inline bool exists(const CKT_BASE* c) {return c!=0;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
