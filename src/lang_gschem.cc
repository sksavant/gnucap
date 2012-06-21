/* This file is the plugin for the gEDa/gschem plugin
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

/*----------------------------------------------------------------------*/
// A net is in form N x0 y0 x1 y1 c
// Need to get x0 y0 ; x1 y1 ;
// Need to go through all the nets. Anyway? 
// Need to save them in other forms? How to go through all cards?
// Need to specify a name for a card?
static void parse_net(CS& cmd,CARD* x)
{
    assert(x);
    int index=0
    if(cmd>>"N"){
        std::string name,value;
        while(cmd>>" " and index<4) {
            unsigned here=cmd.cursor();
            name=((index%2==0) ? "x" : "y") + (index/2==0) ? "0" : "1");
                        // the nets are saved as x0,y0 x1,y1;
            cmd>>value; // same as cmd.ctos() : To find what ctos does
            x->set_param_by_name(name,value);
            index++;
        }
        if (index==4) {
            name="color";
            cmd>>value; //use variant of cmd.ctos...
            x->set_param_by_name(name,value);
            index++;
        }
        else if (index<4) {
            // not correct syntax of net
        }
        else{
            // over.. got all of the net.
        }
    }
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
    graphical=["v","L","G","B","V","A","H","T"]
    if (cmd >> "v" || cmd >> "L" || cmd >> "G" || cmd >> "B" || cmd >>"V" ||
        cmd >> "A" || cmd >> "H" || cmd >> "T"){ type="dev_comment"}
    else if (cmd >> "N"){ type="net";}
    else if (cmd >> "U"){ type="bus";}
    else if (cmd >> "P"){ type="pin";}
    else if (cmd >> "C"){ type="component";}
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
  new__instance(cmd, NULL, Scope);
}
/*----------------------------------------------------------------------*/
class CMD_GSCHEM : public CMD {
public:
    void do_it(CS&, CARD_LIST* Scope)
    {
    command("options lang=gschem", Scope);
    }
} p8;
DISPATCHER<CMD>::INSTALL 
    d8(&command_dispatcher, "gschem", &p8);
/*----------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

