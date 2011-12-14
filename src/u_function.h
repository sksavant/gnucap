/*$Id: u_function.h,v 1.3 2010-09-07 07:46:26 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et
 * Copyright (C) 2008 Albert Davis
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
 */
//testing=none
#ifndef _U_FUNC
#define _U_FUNC
#include "md.h"
/*--------------------------------------------------------------------------*/
class CS;
class CARD_LIST;
class WAVE;
/*--------------------------------------------------------------------------*/
// more general function... 
class FUNCTION_BASE{
public:
  virtual void expand(CS&, const CARD_LIST*) = 0;
  virtual fun_t eval(CS&, const CARD_LIST*)const =0; // const {return 888;}
  virtual string label() const =0;
  virtual FUNCTION_BASE* clone()const=0;
};
/*--------------------------------------------------------------------------*/
// ordinary function. as in gnucap-2009-12
class FUNCTION : public FUNCTION_BASE {
public:
  virtual void expand(CS&, const CARD_LIST*){ } // ordinary functions dont need to be expanded.
  virtual fun_t eval(CS&, const CARD_LIST*)const =0; // const {return 888;}
  virtual string label() const {return "ordinary";}
  virtual FUNCTION_BASE* clone()const {assert(0); return 0;}
};
/*--------------------------------------------------------------------------*/
class WAVE_FUNCTION : public FUNCTION_BASE {
  protected:
    WAVE* w;
  public:
//    virtual FUNCTION* clone()const {return NULL;}
    WAVE_FUNCTION():w(0),probe_name("unset"){}
    // virtual fun_t eval(CS&, const CARD_LIST*)const{ incomplete(); return 0; }
    virtual fun_t wave_eval() const = 0;
    string probe_name;
    void set_wave(WAVE* ww);// { w=ww; }

    fun_t eval(CS&, const CARD_LIST*) const {return wave_eval();} 

  protected:
    WAVE* find_wave(const std::string& probe_name)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
