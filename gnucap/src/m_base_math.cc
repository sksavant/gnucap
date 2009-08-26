/*$Id: m_base_math.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
#include "m_base.h"
/*--------------------------------------------------------------------------*/
std::string new_name()
{
  static int c = 0;
  char s[30];
  sprintf(s, "EX%04u", ++c);
  return s;
}
/*--------------------------------------------------------------------------*/
Base* Base::logic_not()const
{
  if (to_bool()) {
    return new Float(0.);
  }else{
    return new Float(1.);
  }
}
/*--------------------------------------------------------------------------*/
Base* Base::logic_or(const Base* X)const
{
  if ((to_bool()) || (X && X->to_bool())) {
    return new Float(1.);
  }else{
    return new Float(0.);
  }
}
/*--------------------------------------------------------------------------*/
Base* Base::logic_and(const Base* X)const
{
  if (!to_bool() || !X || !X->to_bool()) {
    return new Float(0.);
  }else{
    return new Float(1.);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
