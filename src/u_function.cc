/*$Id: u_function.cc,v 1.1 2009-12-14 10:41:30 felix Exp $ -*- C++ -*-
 * Copyright (C) 2009 Albert Davis
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
#include "e_base.h"
#include "u_function.h"
/*--------------------------------------------------------------------------*/
WAVE* WAVE_FUNCTION::find_wave(const std::string& probe_name)const
{
  return CKT_BASE::find_wave(probe_name);
}
/*--------------------------------------------------------------------------*/
void WAVE_FUNCTION::set_wave(WAVE* ww)
{
	if (!ww){
		trace0("WAVE_FUNCTION::set_wave no wave " + probe_name );
		untested();
	}
  	w = ww;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
