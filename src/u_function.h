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
class FUNCTION {
public:
  virtual void expand(CS&, const CARD_LIST*){}
  virtual fun_t eval(CS&, const CARD_LIST*)const = 0;
};
/*--------------------------------------------------------------------------*/
class WAVE_FUNCTION : public FUNCTION{
  protected:
    WAVE* w;
  public:
    virtual fun_t eval(CS&, const CARD_LIST*)const{ incomplete(); return 0; }
    virtual fun_t wave_eval()const = 0;
    WAVE_FUNCTION():w(0),probe_name("unset"){}
    string probe_name;
    void set_wave(WAVE* ww);// { w=ww; }

  protected:
    WAVE* find_wave(const std::string& probe_name)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
