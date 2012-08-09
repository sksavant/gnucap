/* 
 * Written by Savant Krishna <savant.2020@gmail.com>
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
 * This is the device 'net' : a connection between components.
 */
#include "e_compon.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_NET : public COMPONENT {

public:
  explicit DEV_NET() :COMPONENT()
    {
        _n=_nodes;
    }
  explicit DEV_NET(const DEV_NET& p) :COMPONENT(p){
    _n=_nodes;
  }
  ~DEV_NET(){}
private:
  bool      param_is_printable(int)const {return false;}
private:
  char      id_letter()const {return 'N';}
  std::string value_name()const   {return "color";}
  std::string dev_type()const {untested(); return "net";}
  int       max_nodes()const {return 2;}
  int       min_nodes()const {return 2;}
  int       matrix_nodes()const {return 2;}
  int       net_nodes()const {return 2;}
  bool      has_iv_probe()const {return true;}
  CARD*     clone()const     {return new DEV_NET(*this);}
  bool      print_type_in_spice()const {return false;}
  int       param_count()const  {return COMPONENT::param_count();}
  std::string port_name(int i)const{
    assert(i>=0);
    assert(i<2);
    static std::string names[]={"p","n"};
    return names[i];
  }
public:
  node_t    _nodes[NODES_PER_BRANCH];
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_NET p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher,"net",&p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
