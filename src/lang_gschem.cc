/*$Id: lang_gschem.cc,v xx.xxx xxxx/xx/xx xx:xx:xx al Exp $ -*- C++ -*-
 * This file is the plugin for the gEDA-gschem plugin
 * The documentation of how this should work is at bit.ly(slash)gnucapwiki
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package.
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
 *
 */
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "u_lang.h"

/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class LANG_GSCHEM : public LANGUAGE {

public:
    //
    enum MODE {mDEFAULT, mPARAMSET} _mode;
    mutable int arg_count;
    enum {INACTIVE = -1};
    //
    std::string name()const {return "gschem";}
    bool case_insensitive()const {return false;}
    UNITS units()const {return uSI;}

public: //functions to be declared
    std::string arg_front()const {
        return " "; //arbitrary
    }
    std::string arg_mid()const {
        return "="; //arbitrary
    }
    std::string arg_back()const {
        return "";  //arbitrary
    }

public:
    void		  parse_top_item(CS&, CARD_LIST*);
    DEV_COMMENT*  parse_comment(CS&, DEV_COMMENT*);
    DEV_DOT*	  parse_command(CS&, DEV_DOT*);
    MODEL_CARD*	  parse_paramset(CS&, MODEL_CARD*);
    MODEL_SUBCKT* parse_module(CS&, MODEL_SUBCKT*);
    COMPONENT*	  parse_instance(CS&, COMPONENT*);
    std::string	  find_type_in_string(CS&);
    std::vector<CARD*> nets;

private:
    void print_paramset(OMSTREAM&, const MODEL_CARD*);
    void print_module(OMSTREAM&, const MODEL_SUBCKT*);
    void print_instance(OMSTREAM&, const COMPONENT*);
    void print_comment(OMSTREAM&, const DEV_COMMENT*);
    void print_command(OMSTREAM& o, const DEV_DOT* c);

private:
    void print_args(OMSTREAM&, const MODEL_CARD*);
    void print_args(OMSTREAM&, const COMPONENT*);
}lang_gschem;

DISPATCHER<LANGUAGE>::INSTALL
    d(&language_dispatcher, lang_gschem.name(), &lang_gschem);
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
//functions to be defined

  //Finds type from find_type_in_string
  //If component, then assign type as device=.. ?
static void parse_type(CS& cmd, CARD* x)
{
  assert(x);
  std::string new_type;
  new_type=lang_gschem.find_type_in_string(cmd);
  if (new_type=="net") {
    x->set_dev_type(new_type);
  }
  else{
    return;
  }
}
/*--------------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
static void parse_args_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);

  while (cmd >> '.') {
    unsigned here = cmd.cursor();
    std::string name, value;
    try{
      cmd >> name >> '=' >> value >> ';';
      x->set_param_by_name(name, value);
    }catch (Exception_No_Match&) {untested();
      cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
    }
  }
}
/*--------------------------------------------------------------------------*/
static void parse_args_instance(CS& cmd, CARD* x)
{
  assert(x);

  if (cmd >> "#(") {
    if (cmd.match1('.')) {
      // by name
      while (cmd >> '.') {
	unsigned here = cmd.cursor();
	std::string name  = cmd.ctos("(", "", "");
	std::string value = cmd.ctos(",)", "(", ")");
	cmd >> ',';
	try{
	  x->set_param_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
	}
      }
    }else{
      // by order
      int index = 1;
      while (cmd.is_alnum()) {
	unsigned here = cmd.cursor();
	try{
	  std::string value = cmd.ctos(",)", "", "");
	  x->set_param_by_index(x->param_count() - index++, value, 0/*offset*/);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }
    cmd >> ')';
  }else{
    // no args
  }
}
/*--------------------------------------------------------------------------*/
static void parse_label(CS& cmd, CARD* x)
{
  assert(x);
  std::string my_name;
  cmd >> my_name;
  x->set_label(my_name);
}
/*--------------------------------------------------------------------------*/
static void parse_ports(CS& cmd, COMPONENT* x)
{
  assert(x);

  if (cmd >> '(') {
    if (cmd.is_alnum()) {
      // by order
      int index = 0;
      while (cmd.is_alnum()) {
	unsigned here = cmd.cursor();
	try{
	  std::string value;
	  cmd >> value;
	  x->set_port_by_index(index++, value);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }else{
      // by name
      while (cmd >> '.') {
	unsigned here = cmd.cursor();
	try{
	  std::string name, value;
	  cmd >> name >> '(' >> value >> ')' >> ',';
	  x->set_port_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, "mismatch, ignored");
	}
      }
    }
    cmd >> ')';
  }else{untested();
    cmd.warn(bDANGER, "'(' required");
  }
}

/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_GSCHEM::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_GSCHEM::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset();
  CMD::cmdproc(cmd, scope);
  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
/* "paramset" <my_name> <base_name> ";"
 *    <paramset_item_declaration>*
 *    <paramset_statement>*
 *  "endparamset"
 */
//BUG// no paramset_item_declaration, falls back to spice mode
//BUG// must be on single line

MODEL_CARD* LANG_GSCHEM::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset();
  cmd >> "paramset ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  cmd >> ';';
  parse_args_paramset(cmd, x);
  cmd >> "endparamset ";
  cmd.check(bWARNING, "what's param this?");
  return x;
}
/*--------------------------------------------------------------------------*/
/* "module" <name> "(" <ports> ")" ";"
 *    <declarations>
 *    <netlist>
 * "endmodule"
 */
//BUG// strictly one device per line

MODEL_SUBCKT* LANG_GSCHEM::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);
  cmd.reset();
  std::string type = find_type_in_string(cmd);
  assert(type=="subckt");
  int component_x, component_y, mirror, angle;
  std::string dump,basename;
  bool isgraphical=true;
  cmd>>"C";
  std::vector<std::string> paramname;
  std::vector<std::string> paramvalue;
  cmd>>component_x>>" ">>component_y>>" ">>dump>>" ">>angle>>" ">>mirror>>" ">>basename;
  cmd.get_line("gnucap-gschem-"+basename+">");
  cmd>>"{";
  for (;;) {
    cmd.get_line("gnucap-gschem-"+basename+">");
    if (cmd >> "}") {
      break;
    }else{
        if(cmd>>"T"){
            cmd>>dump;
        }
        else {
            std::string _paramname=cmd.ctos("=","",""),_paramvalue;
            cmd>>"=">>_paramvalue;
            paramname.push_back (_paramname);
            paramvalue.push_back(_paramvalue);
            std::cout<<_paramname<<" "<<_paramvalue<<std::endl;
            if(_paramname=="device"){
                isgraphical=false;
            }
        }
    }
  }
  if (isgraphical==true) {
    return NULL;
  }
  else{

  }
  return x;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// A net is in form N x0 y0 x1 y1 c
// Need to get x0 y0 ; x1 y1 ;
// Need to go through all the nets. Anyway?
// Need to save them in other forms? How to go through all cards?
// Need to specify a name for a card?
static void parse_net(CS& cmd,CARD* x)
{
    assert(x);
    assert(lang_gschem.find_type_in_string(cmd)=="net");
    cmd>>"N";     //Got N
    std::string x0,y0,x1,y1,color;
    unsigned here=cmd.cursor();
    std::string paramnames[5]={"x0","x1","y0","y1","color"};
    std::string paramvalue[5];
    int i=0;
    bool gotthenet=true;
    while (i<5) {
        if (cmd.is_alnum()){
            cmd>>" ">>paramvalue[i];
            try{
                x->set_param_by_name(paramnames[i],paramvalue[i]);
            }catch (Exception_No_Match&) {untested();
                cmd.warn(bDANGER, here, x->long_label() + ": bad parameter "+ paramvalue[i] + "ignored");
            }
        }else{
            gotthenet=false;
            cmd.warn(bDANGER, here, x->long_label() +": Not correct format for net");
            break;
        }
        ++i;
    }
    if (gotthenet){
        //lang_gschem.nets.push_back(x);
    }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_GSCHEM::parse_instance(CS& cmd, COMPONENT* x)
{
  cmd.reset();
  //parse_type(cmd, x); //parse type will parse the component type and set_dev_type
  if (x->dev_type()=="net") {
    parse_net(cmd,x);
  }
  cmd.check(bWARNING, "what's ins this?");
  return x;
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* find_type_in_string : specific to each language which gives the type of the
 * cmd Eg: verilog : comment ,resistor etc
 * In Gschem the broad high level types are
 * The following don't mean anything to the electrical part of the circuit and
 * they are of type graphical
 *  version  v    * circle   V
 *  line     L    * arc      A
 *  picture  G    * path     H
 *  box      B    * text     T (not attributes)
 * The following have electrical meaning and are of type
 *  net          N
 *  bus          U
 *  pin          P
 *  component    C
 *  attributes   T (enclosed in {})  // don't include in find_type_in_string
 * Will check for type graphical (type=dev_comment) else type will be
 * net or bus or pin or component\
 */
 std::string LANG_GSCHEM::find_type_in_string(CS& cmd)
 {
    unsigned here = cmd.cursor(); //store cursor position to reset back later
    std::string type;   //stores type : should check device attribute..
    // (**)to find how to check the type.. (**)
    /*
     *if((cmd>>"//")){
     *   assert(here==0);
     *   type="dev_comment";
     *}
     *else{ cmd>>type;}
     */
    //graphical=["v","L","G","B","V","A","H","T"]
    if (cmd >> "v" || cmd >> "L" || cmd >> "G" || cmd >> "B" || cmd >>"V" ||
        cmd >> "A" || cmd >> "H" || cmd >> "T"){ type="dev_comment";}
    else if (cmd >> "N"){ type="net";}
    else if (cmd >> "U"){ type="bus";}
    else if (cmd >> "P"){ type="pin";}
    else if (cmd >> "C"){ type="subckt";}
    else { } //Not matched with the type. What now?
    cmd.reset(here);//Reset cursor back to the position that
                    //has been started with at beginning
    return type;    // returns the type of the string

 }

/*----------------------------------------------------------------------*/
/*parse_top_item : To know:
 *What does CS& refer to
 *  CS stands for command string which is defined in ap.h and various functions
 *  on it are defined in ap*.cc
 *what does get_line method on CS& do ?
 *  get_line will ‘getlines’ from the file ptr, else if no file, get from
 *  keyboard (stdin) using getcmd (which will get a command, also log, echo etc)
 *what is the class CARD_LIST* ?
 *
 *what does new__instance of cmd mean?
 *	It is defined in u_lang.cc. It does the following :
 *	/11/find_type_in_string : specific to each language which gives the type of the  *cmd
		Eg: verilog : comment ,resistor etc
 * Then find_proto for the given type.
 * Clone the instance : new_instance=proto->clone_instance()
 * set the owner of the instance : new_instance->set_owner(owner)
 * Then we parse the command using new_instance : x=parse_item(cmd,new_instance)
 *		for und. parse_item : // See Stroustrup 15.4.5
 *	 	if MODEL_SUBCKT, return parse_module()
 *		elsif COMPONENT, return parse_instance()
 * 		elsif MODEL_CARD, return parse_paramset()
 *		elsif DEV_COMMENT, return parse_comment()
 *		elsif DEV_DOT, return parse_command()
 *		else return NULL
 *	Now Scope->push_back(x) which will add this to the list “Scope._cl”.
 */
void LANG_GSCHEM::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-gschem>");
  std::cout<<"Got the line\n";
  new__instance(cmd, NULL, Scope);
  for(CARD_LIST::const_iterator ci=Scope->begin();ci!= Scope->end();++ci) {
    std::cout<<(*ci)->dev_type()<<" "<<(*ci)->param_value(4)<<std::endl;
  }
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
void LANG_GSCHEM::print_args(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " ." + x->param_name(ii) + "=" + x->param_value(ii) + ";";
	o << arg;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_GSCHEM::print_args(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << " #(";
  if (x->use_obsolete_callback_print()) {
    arg_count = 0;
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
    arg_count = INACTIVE;
  }else{
    std::string sep = ".";
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	o << sep << x->param_name(ii) << "(" << x->param_value(ii) << ")";
	sep = ",.";
      }else{
      }
    }
  }
  o << ") ";
}
/*--------------------------------------------------------------------------*/
static void print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->dev_type();
}
/*--------------------------------------------------------------------------*/
static void print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->short_label();
}
/*--------------------------------------------------------------------------*/
static void print_ports_long(OMSTREAM& o, const COMPONENT* x)
{
  // print in long form ...    .name(value)
  assert(x);

  o << " (";
  std::string sep = ".";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_name(ii) << '(' << x->port_value(ii) << ')';
    sep = ",.";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_name(ii) << '(' << x->current_port_value(ii) << ')';
    sep = ",.";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
static void print_ports_short(OMSTREAM& o, const COMPONENT* x)
{
  // print in short form ...   value only
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_value(ii);
    sep = ",";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_value(ii);
    sep = ",";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_GSCHEM::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  _mode = mPARAMSET;
  o << "paramset " << x->short_label() << ' ' << x->dev_type() << ";\\\n";
  print_args(o, x);
  o << "\\\n"
    "endparmset\n\n";
  _mode = mDEFAULT;
}
/*--------------------------------------------------------------------------*/
void LANG_GSCHEM::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  o << "module " <<  x->short_label();
  print_ports_short(o, x);
  o << ";\n";

  for (CARD_LIST::const_iterator
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }

  o << "endmodule // " << x->short_label() << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_GSCHEM::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_type(o, x);
  print_args(o, x);
  print_label(o, x);
  print_ports_long(o, x);
  o << ";\n";
}
/*--------------------------------------------------------------------------*/
void LANG_GSCHEM::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_GSCHEM::print_command(OMSTREAM& o, const DEV_DOT* x)
{
  assert(x);
  o << x->s() << '\n';
}




/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

class CMD_GSCHEM : public CMD {
public:
    void do_it(CS& cmd, CARD_LIST* Scope)
    {
      command("options lang=gschem", Scope);
    }
} p8;
DISPATCHER<CMD>::INSTALL
    d8(&command_dispatcher, "gschem", &p8);

class CMD_COMPONENT : public CMD {
    void do_it(CS& cmd, CARD_LIST* Scope)
    {
      std::cout<<"Command Dispatched\n";
      MODEL_SUBCKT* new_compon = new MODEL_SUBCKT;
      assert(new_compon);
      assert(!new_compon->owner());
      assert(new_compon->subckt());
      assert(new_compon->subckt()->is_empty());
      lang_gschem.parse_module(cmd, new_compon);
      Scope->push_back(new_compon);
    }
} p2;
DISPATCHER<CMD>::INSTALL
    d2(&command_dispatcher, "C", &p2);

/*----------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

