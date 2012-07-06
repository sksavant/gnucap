/*
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
#ifndef D_COMPONENT
#define D_COMPONENT
#include "e_node.h"
#include "d_subckt.h"
/*--------------------------------------------------------------------------*/
#define PORTS_PER_DEVICE 100
/*--------------------------------------------------------------------------*/
class INTERFACE MODEL_COMPONENT : public MODEL_SUBCKT {
private:
    explicit MODEL_COMPONENT(const MODEL_COMPONENT &p){} ;
public:
    explicit MODEL_COMPONENT();
            ~MODEL_COMPONENT();
public:
  char      id_letter()const    {untested(); return '\0';}
  CARD*     clone_instance()const;
  bool      print_type_in_spice()const {unreachable(); return false;}
  std::string value_name()const   {incomplete(); return "";}
  std::string dev_type()const     {untested(); return "";}
  int       max_nodes()const    {return PORTS_PER_DEVICE;}
  int       min_nodes()const    {return 0;}
  int       matrix_nodes()const {untested();return 0;}
  int       net_nodes()const    {return _net_nodes;}
  CARD*     clone()const        {return new MODEL_COMPONENT(*this);}
  bool      is_device()const    {return false;}
  void      precalc_first()     {}
  void      expand()            {}
  void      precalc_last()      {}
  bool      makes_own_scope()const  {return true;}
  void      map_nodes() {}
  CARD_LIST*       scope()      {return subckt();}
  const CARD_LIST* scope()const {return subckt();}

  std::string port_name(int)const {
    return "";
  }
public:
  static int    count()         {return _count;}

private:
  node_t    _nodes[PORTS_PER_DEVICE];
  static int    _count;
};

MODEL_COMPONENT p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "component", &p1);














#endif
