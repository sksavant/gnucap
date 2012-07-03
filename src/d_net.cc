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
#include "e_elemnt.h"

namespace {

class DEV_NET : public ELEMENT {
private:
  explicit DEV_NET(const DEV_NET& p) :ELEMENT(p){}
public:
  explicit DEV_NET() :ELEMENT(){}
public:
  std::string param_name(int) const;
  std::string param_name(int,int) const;
private:
  char      id_letter()const {return 'N';}
  std::string value_name()const   {return "n";}
  std::string dev_type()const {untested(); return "net";}
  int       max_nodes()const {return 2;}
  int       min_nodes()const {return 2;}
  int       matrix_nodes()const {return 2;}
  int       net_nodes()const {return 2;}
  bool      has_iv_probe()const {return true;}
  CARD*     clone()const     {return new DEV_NET(*this);}
  void      precalc_last();
  void      tr_iwant_matrix()   {tr_iwant_matrix_passive();}
  void      tr_begin();
  double    tr_involts()const	{return tr_outvolts();}
  double    tr_involts_limited()const {return tr_outvolts_limited();}
  void	    ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void      ac_begin()          {_ev=0.;}
  COMPLEX   ac_involts()const	{return ac_outvolts();} 
  int       param_count()const  {return 5+ELEMENT::param_count();}
  std::string port_name(int i)const{
    assert(i>=0);
    assert(i<2);
    static std::string names[]={"p","n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
std::string DEV_NET::param_name(int i) const{
    switch(i) {
    case 0: return "n";
    case 1: return "y1";
    case 2: return "x1";
    case 3: return "y0";
    case 4: return "color";
    default : return ELEMENT::param_name(i);
    }
}

std::string DEV_NET::param_name(int i,int j) const{
    if (j==0){
        return param_name(i);
    }else if (i>=ELEMENT::param_count()) {
        return "";
    }else{
        return ELEMENT::param_name(i,j);
    }
}

void DEV_NET::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}

void DEV_NET::tr_begin()
{
  ELEMENT::tr_begin();
  _y1.f1 = _y[0].f1 = 0.;
  _m0.x  = _y[0].x;
}

DEV_NET p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher,"N|net",&p1);

}
