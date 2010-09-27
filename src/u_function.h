/*$Id: u_function.h,v 1.3 2010-09-07 07:46:26 felix Exp $ -*- C++ -*-
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
#include "md.h"
/*--------------------------------------------------------------------------*/
class CS;
class CARD_LIST;
class WAVE;
/*--------------------------------------------------------------------------*/
class FUNCTION {
public:
	// ARGH
  virtual std::string eval(CS&, const CARD_LIST*)const = 0;
protected:
  WAVE* find_wave(const std::string& probe_name)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
