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
//namespace {
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
protected:
  PARAMETER<double> _xco0;//Not needed?
  PARAMETER<double> _yco0;
  PARAMETER<double> _xco1;
  PARAMETER<double> _yco1;
  PARAMETER<double> _color;
private:
  std::string param_name(int) const;
  std::string param_name(int,int) const;
  void      set_param_by_name(std::string,std::string);
  void      set_param_by_index(int,std::string,int);
  bool      param_is_printable(int)const;
  std::string param_value(int)const;
private:
  char      id_letter()const {return 'N';}
  std::string value_name()const   {return "";}
  std::string dev_type()const {untested(); return "net";}
  int       max_nodes()const {return 2;}
  int       min_nodes()const {return 2;}
  int       matrix_nodes()const {return 2;}
  int       net_nodes()const {return 2;}
  bool      has_iv_probe()const {return true;}
  CARD*     clone()const     {return new DEV_NET(*this);}
  bool      print_type_in_spice()const {return false;}
  int       param_count()const  {return 5;}
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
std::string DEV_NET::param_name(int i) const
{
    switch(DEV_NET::param_count()-i-1) {
    case 0: return "x0";
    case 1: return "x1";
    case 2: return "y0";
    case 3: return "y1";
    case 4: return "color";
    default : return "";
    }
}
/*--------------------------------------------------------------------------*/
std::string DEV_NET::param_name(int i,int j) const{
    if (j==0){
        return param_name(i);
    }else{
        return "";
    }
}
/*--------------------------------------------------------------------------*/
void DEV_NET::set_param_by_name(std::string Name,std::string Value)
{
    for (int i=DEV_NET::param_count()-1; i>=0; --i) {
        for (int j=0; DEV_NET::param_name(i,j)!=""; ++j) {
            if(Name==DEV_NET::param_name(i,j)) {
                DEV_NET::set_param_by_index(i,Value,0);
                return;
            }else{
            }
        }
    }
    throw Exception_No_Match(Name+" "+Value);
}
/*--------------------------------------------------------------------------*/
void DEV_NET::set_param_by_index(int i, std::string Value, int offset)
{
    switch(DEV_NET::param_count()-1-i) {
    case 0: _xco0 = Value; break;
    case 1: _yco0 = Value; break;
    case 2: _xco1 = Value; break;
    case 3: _yco1 = Value; break;
    case 4: _color = Value; break;
    default: throw Exception_Too_Many(i,4,offset);
    }
}
/*--------------------------------------------------------------------------*/
bool DEV_NET::param_is_printable(int i)const
{
    switch(DEV_NET::param_count()-1-i) {
    case 0: return false;
    case 1: return false;
    case 2: return false;
    case 3: return false;
    case 4: return false;
    default: return COMPONENT::param_is_printable(i);
    }
}
/*--------------------------------------------------------------------------*/
std::string DEV_NET::param_value(int i)const
{
    switch(DEV_NET::param_count()-1-i) {
    case 0: return _xco0.string();
    case 1: return _yco0.string();
    case 2: return _xco1.string();
    case 3: return _yco1.string();
    case 4: return _color.string();
    default: return "";
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_NET p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher,"N|net",&p1);
/*--------------------------------------------------------------------------*/
//}
/*--------------------------------------------------------------------------*/
