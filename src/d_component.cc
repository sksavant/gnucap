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
#include "d_subckt.h"
/*--------------------------------------------------------------------------*/
#define PORTS_PER_DEVICE 100
/*--------------------------------------------------------------------------*/
class DEV_COMPONENT : public COMPONENT {
private:
    explicit DEV_COMPONENT(const DEV_COMPONENT &p) :COMPONENT(p){}
public:
    explicit DEV_COMPONENT(): COMPONENT(){}
protected:
    PARAM_LIST* _params;
private:
    std::string param_name(int) const;
    std::string param_name(int, int) const;
    void	set_param_by_name(std::string, std::string);
    void	set_param_by_index(int, std::string, int);
    bool 	param_is_printable(int) const;
    std::string	param_value(int) const;
public:
  char      id_letter()const    {untested(); return '\0';}
  //CARD*     clone_instance()const;
  bool      print_type_in_spice()const {unreachable(); return false;}
  std::string value_name()const   {incomplete(); return "";}
  std::string dev_type()const     {untested(); return "";}
  int       max_nodes()const    {return PORTS_PER_DEVICE;}
  int       min_nodes()const    {return 0;}
  int       matrix_nodes()const {untested();return 0;}
  int       net_nodes()const    {return _net_nodes;}
  CARD*     clone()const        {return new DEV_COMPONENT(*this);}
  bool      is_device()const    {return false;}
  void      precalc_first()     {}
  void      expand()            {}
  void      precalc_last()      {}
  bool      makes_own_scope()const  {return true;}
  void      map_nodes() {}
  CARD_LIST*       scope()      {return subckt();}
  const CARD_LIST* scope()const {return subckt();}
  int 	param_count()const 	{ return (static_cast<int>(_params->size()));}

  std::string port_name(int)const {
    return "";
  }
public:
  static int    count()         {return _count;}

private:
  node_t    _nodes[PORTS_PER_DEVICE];
  static int    _count;
};
/*--------------------------------------------------------------------------*/
void DEV_COMPONENT::set_param_by_name(std::string Name, std::string Value)
{
	_params->set(Name,Value);
}
/*--------------------------------------------------------------------------*/
std::string DEV_COMPONENT::param_name(int i)const
{
	assert(i<DEV_COMPONENT::param_count());
	return _params->name(DEV_COMPONENT::param_count() - 1 - i);
}
/*--------------------------------------------------------------------------*/
std::string DEV_COMPONENT::param_name(int i, int j)const
{
	assert(i<DEV_COMPONENT::param_count());
	if (j == 0) {
       		return param_name(i);
	}else {
		return	"";
	}
}
/*--------------------------------------------------------------------------*/
std::string DEV_COMPONENT::param_value(int i)const
{
	assert(i<DEV_COMPONENT::param_count());
	return _params->value(DEV_COMPONENT::param_count() - 1 - i);
}
/*--------------------------------------------------------------------------*/
bool DEV_COMPONENT::param_is_printable(int i)const
{
	assert(i<DEV_COMPONENT::param_count());
	return _params->is_printable(DEV_COMPONENT::param_count() -1 -i);
}
/*--------------------------------------------------------------------------*/
/*CARD* DEV_COMPONENT::clone_instance()const
{
    DEV_COMPONENT* new_instance = dynamic_cast<DEV_COMPONENT*>(p1.clone());
    new_instance-> _parent = this;
    return new_instance;
}*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMPONENT p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "component", &p1);
/*--------------------------------------------------------------------------*/
