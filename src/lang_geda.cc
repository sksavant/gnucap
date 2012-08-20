/* This file is the plugin for the gEDA-gschem plugin
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
#include <fts.h>
#include "io_trace.h"
#ifdef HAVE_GEDA
#include <gmpxx.h> // to workaround bug in gmp header about __cplusplus
#define COMPLEX NOCOMPLEX // COMPLEX already came from md.h
extern "C"{
# include <libgeda/libgeda.h>
}
#undef COMPLEX
#endif
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class LANG_GEDA : public LANGUAGE {

public:
    //
    enum MODE {mATTRIBUTE, mCOMMENT} _mode;
    mutable int _no_of_lines;
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
    MODEL_SUBCKT* parse_componmod(CS&, MODEL_SUBCKT*);

private:
    void print_paramset(OMSTREAM&, const MODEL_CARD*);
    void print_module(OMSTREAM&, const MODEL_SUBCKT*);
    void print_instance(OMSTREAM&, const COMPONENT*);
    void print_comment(OMSTREAM&, const DEV_COMMENT*);
    void print_command(OMSTREAM& o, const DEV_DOT* c);

}lang_geda;

DISPATCHER<LANGUAGE>::INSTALL
    d(&language_dispatcher, lang_geda.name(), &lang_geda);
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
//Finds type from find_type_in_string
static void parse_type(CS& cmd, CARD* x)
{
      assert(x);
      std::string new_type;
      new_type=lang_geda.find_type_in_string(cmd);
      x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
static std::string* parse_pin(CS& cmd, COMPONENT* x, int index, bool ismodel)
{
    //assert(x); can parse NULL also
    std::string type=OPT::language->find_type_in_string(cmd),dump;
    assert(type=="pin");
    cmd>>"P";
    std::string* coord = new std::string[3];
    if (!ismodel){
        std::string pinattributes[7];
        for(int i=0;i<7;i++){
            cmd>>" ">>pinattributes[i];
        }
        if (pinattributes[6]=="1"){
            coord[0]=pinattributes[2];
            coord[1]=pinattributes[3];
        }else if (pinattributes[6]=="0"){
            coord[0]=pinattributes[0];
            coord[1]=pinattributes[1];
        }
    }
    else{
        cmd>>dump;
    }
    std::string    _portvalue="_";
    cmd.get_line("");
    cmd>>"{";
    for(;;){
        cmd.get_line("");
        if(cmd>>"}"){
            break;
        }else{
            if (cmd>>"T"){
                cmd>>dump;
            }
            else{
                std::string _pname=cmd.ctos("=","",""),_pvalue;
                cmd>>"=">>_pvalue;
                if(_pname=="pinlabel"){
                    _portvalue=_pvalue+_portvalue;
                }else if (_pname=="pintype"){
                    _portvalue=_portvalue+_pvalue;
                }
            }
        }
    }
    if(ismodel and x){
        x->set_port_by_index(index,_portvalue);
        return NULL;
    }else{
        return coord;
    }
}
/*--------------------------------------------------------------------------*/
static std::string find_file_given_name(std::string basename)
{
#ifdef HAVE_GEDA
    const char* geda = s_path_sys_data();
#else
    const char* geda = GEDA_DATA;
#endif
    char gedasym[5+strlen(geda)];
    sprintf(gedasym, "%s/sym", geda);
    char *p[] = {gedasym,NULL};
    FTS* dir=fts_open(p,FTS_NOCHDIR, NULL);
    if(!dir){
        std::cout<<"Not opened\n";
        return "";
    }
    assert(dir);
    FTSENT* node;
    std::string dirname="";
    while(node=fts_read(dir)){
        if(node->fts_info & FTS_F && basename==node->fts_name){
            dirname=node->fts_path;
        }
    }
    if(dirname==""){
        std::cout<<"No symbol file for "+basename<<std::endl;
    }
    return dirname;
}
/*--------------------------------------------------------------------------*/
static std::vector<std::string*> parse_symbol_file(COMPONENT* x, std::string basename)
{
    std::string filename=find_file_given_name(basename),dump;
    CS sym_cmd(CS::_INC_FILE, filename);
    //Now parse the sym_cmd which will get lines
    int index=0;
    std::vector<std::string*> coord;
    while(true){
        try{
            sym_cmd.get_line("");
        }catch (Exception_End_Of_Input&){
            break;
        }
        std::string linetype=OPT::language->find_type_in_string(sym_cmd);
        bool ismodel=false;
        if (x){
            if(x->short_label()=="!_"+basename){
                ismodel=true;
            }
        }
        if (linetype=="pin"){
            coord.push_back(parse_pin(sym_cmd,x,index++,ismodel));
        }else if(linetype=="graphical"){
            sym_cmd>>"graphical=";
            std::string value;
            sym_cmd>>value;
            if(value=="1"){
                return coord;
            }
        }else{
            sym_cmd>>dump;
        }
    }
    return coord;
}
/*--------------------------------------------------------------------------*/
//place <nodename> x y
static void parse_place(CS& cmd, COMPONENT* x)
{
    assert(x);
    assert(OPT::language->find_type_in_string(cmd)=="place");
    cmd>>"place";
    std::string _portname,_x,_y;
    cmd>>" ">>_portname>>" ">>_x>>" ">>_y;
    x->set_param_by_name("x",_x);
    x->set_param_by_name("y",_y);
    x->set_port_by_index(0,_portname);
}
/*--------------------------------------------------------------------------*/
static void create_place(std::string cmdstr,COMPONENT* x)
{
    CS place_cmd(CS::_STRING,cmdstr);
    OPT::language->new__instance(place_cmd,NULL,x->scope());
}
/*--------------------------------------------------------------------------*/
static std::string findplacewithsameposition(COMPONENT* x,std::string xco,std::string yco)
{
    for (CARD_LIST::const_iterator ci = x->scope()->begin(); ci != x->scope()->end(); ++ci) {
        if((*ci)->dev_type()=="place"){
            if(xco==(*ci)->param_value(1) && yco==(*ci)->param_value(0)){
                return static_cast<COMPONENT*>(*ci)->port_value(0);
            }
        }
    }
    return "";
}
/*--------------------------------------------------------------------------*/
static std::string* findnodeonthisnet(CARD *x, std::string x0, std::string y0, std::string x1, std::string y1)
{
    for(CARD_LIST::const_iterator ci = x->scope()->begin(); ci != x->scope()->end(); ++ci) {
        if((*ci)->dev_type()=="place"){
            std::string _x=(*ci)->param_value(1),_y=(*ci)->param_value(0);
            if (y0==y1){
                if(  ( ( (atoi(x1.c_str()) < atoi(_x.c_str())) and (atoi(_x.c_str())<atoi(x0.c_str())) ) || ( (atoi(x0.c_str()) < atoi(_x.c_str())) and (atoi(_x.c_str())<atoi(x1.c_str())) ) ) and _y==y0 and _x!=x0 and _x!=x1){
                    trace0("true0");
                    std::string* coord=new std::string[2];
                    coord[0]=_x;
                    coord[1]=_y;
                    return coord;
                }
                else{
                    return NULL;
                }
            }else if (x0==x1){
                if(  ( ( (atoi(y1.c_str()) < atoi(_y.c_str())) and (atoi(_y.c_str())<atoi(y0.c_str())) ) || ( (atoi(y0.c_str()) < atoi(_y.c_str())) and (atoi(_y.c_str())<atoi(y1.c_str())) ) ) and _x==x0 and _y!=y0 and _y!=y1){
                    trace0("true1");
                    std::string* coord=new std::string[2];
                    coord[0]=_x;
                    coord[1]=_y;
                    return coord;
                }else{
                    return NULL;
                }
            }
            else{
                return NULL;
            }
        }
    }
}
/*--------------------------------------------------------------------------*/
// A net is in form N x0 y0 x1 y1 c
// Need to get x0 y0 ; x1 y1 ;
// Need to go through all the nets. Anyway?
// Need to save them in other forms? How to go through all cards?
// Need to specify a name for a card?
static void parse_net(CS& cmd, COMPONENT* x)
{
    assert(x);
    assert(lang_geda.find_type_in_string(cmd)=="net");
    cmd>>"N";     //Got N
    unsigned here=cmd.cursor();
    // x0 y0 x1 y1 color
    std::string parsedvalue[5];
    int i=0;
    bool gotthenet=true;
    while (i<5) {
        if (cmd.is_alnum()){
            cmd>>" ">>parsedvalue[i];
        }else{
            gotthenet=false;
            cmd.warn(bDANGER, here, x->long_label() +": Not correct format for net");
            break;
        }
        ++i;
    }
    if (gotthenet){
        //lang_geda.nets.push_back(x);
        //To check if any of the previous nodes have same placement.
        x->set_param_by_name("color",parsedvalue[4]);
        x->set_label("net"+to_string(rand()%10000)); //BUG : names may coincide!. Doesn't matter? Or try some initialisation method. (latch like digital)

        std::string _portvalue=findplacewithsameposition(x,parsedvalue[0],parsedvalue[1]);
        if(_portvalue==""){
            _portvalue="node"+to_string(rand()%10000);
            create_place("place "+_portvalue+" "+parsedvalue[0]+" "+parsedvalue[1],x);
        }
        x->set_port_by_index(0,_portvalue);

        _portvalue=findplacewithsameposition(x,parsedvalue[2],parsedvalue[3]);
        if(_portvalue==""){
            _portvalue="node"+to_string(rand()%10000);
            create_place("place "+_portvalue+" "+parsedvalue[2]+" "+parsedvalue[3],x);
        }
        x->set_port_by_index(1,_portvalue);

        std::string* nodeonthisnet=findnodeonthisnet(x,parsedvalue[0],parsedvalue[1],parsedvalue[2],parsedvalue[3]);
        if (!nodeonthisnet)
        {
        }else{
            //create new net from nodeonthisnet to one of edges of net.
            std::string netcmdstr="N "+parsedvalue[0]+" "+parsedvalue[1]+" "+nodeonthisnet[0]+" "+nodeonthisnet[1]+" 5";
            CS net_cmd(CS::_STRING,netcmdstr);
            OPT::language->new__instance(net_cmd,NULL,x->scope());
        }
        //To check if there are any attributes
        cmd.get_line("gnucap-geda>");
        std::string temp=cmd.fullstring();
        std::string _paramvalue,_paramname,dump;
        if(temp!="{"){
            OPT::language->new__instance(cmd,NULL,x->scope());
            return;
        }
        cmd>>"{";
        for (;;) {
            cmd.get_line("gnucap-geda-net>");
            if (cmd >> "}") {
                break;
            }else{
                if(cmd>>"T"){
                    cmd>>dump;
                }
                else {
                    std::string _paramname=cmd.ctos("=","",""),_paramvalue;
                    cmd>>"=">>_paramvalue;
                    if (_paramname=="netname" && _paramvalue!="?"){
                        x->set_label(_paramvalue);
                    }else{
                        x->set_param_by_name(_paramname,_paramvalue);
                    }
                }
            }
        } 
    }
}
/*--------------------------------------------------------------------------*/
static void parse_component(CS& cmd,COMPONENT* x){
    assert(x);
    std::string component_x, component_y, mirror, angle, dump,basename;
    std::string type=lang_geda.find_type_in_string(cmd);
    cmd>>type;
    std::vector<std::string> paramname;
    std::vector<std::string> paramvalue;
    cmd>>component_x>>" ">>component_y>>" ">>dump>>" ">>angle>>" ">>mirror>>" ">>basename;
    //To get port names and values from symbol?
    //Then set params below
    //Search for the file name
    std::vector<std::string*> coordinates=parse_symbol_file(x,basename);
    char    newx[10],newy[10];
    int index=0;
    for (std::vector<std::string*>::const_iterator i=coordinates.begin();i<coordinates.end();++i){
        //to do integer casting, addition and then reconverting to string
        if(mirror=="0"){
            switch(atoi(angle.c_str())){
            case 0:
                sprintf(newx,"%d",atoi(component_x.c_str())+atoi((*i)[0].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())+atoi((*i)[1].c_str()));
                break;
            case 90:
                sprintf(newx,"%d",atoi(component_x.c_str())-atoi((*i)[1].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())+atoi((*i)[0].c_str()));
                break;
            case 180:
                sprintf(newx,"%d",atoi(component_x.c_str())-atoi((*i)[0].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())-atoi((*i)[1].c_str()));
                break;
            case 270:
                sprintf(newx,"%d",atoi(component_x.c_str())+atoi((*i)[1].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())-atoi((*i)[0].c_str()));
                break;
            }
        }else if(mirror=="1"){
            switch(atoi(angle.c_str())){
            case 0:
                sprintf(newx,"%d",atoi(component_x.c_str())-atoi((*i)[0].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())+atoi((*i)[1].c_str()));
                break;
            case 90:
                sprintf(newx,"%d",atoi(component_x.c_str())-atoi((*i)[1].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())-atoi((*i)[0].c_str()));
                break;
            case 180:
                sprintf(newx,"%d",atoi(component_x.c_str())+atoi((*i)[0].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())-atoi((*i)[1].c_str()));
                break;
            case 270:
                sprintf(newx,"%d",atoi(component_x.c_str())+atoi((*i)[1].c_str()));
                sprintf(newy,"%d",atoi(component_y.c_str())+atoi((*i)[0].c_str()));
                break;
            }
        }else{
        //not correct mirror!
        }
        //delete (*i);
        //setting new place devices for each node searching for .
        //new__instance(cmd,NULL,Scope); //cmd : can create. Scope? how to get Scope? Yes!
        std::string _portvalue=findplacewithsameposition(x,newx,newy);
        if (_portvalue==""){
            _portvalue="node_"+to_string(rand()%10000);
            create_place("place "+_portvalue+" "+newx+" "+newy,x);
        }
        x->set_port_by_index(index,_portvalue);
        ++index;
    }
    x->set_param_by_name("basename",basename);
    CS new_cmd(CS::_STDIN);
    new_cmd.get_line("gnucap-geda>");
    std::string temp=new_cmd.fullstring();
    if(temp!="{"){
        OPT::language->new__instance(new_cmd,NULL,x->scope());
        return;
    }
    cmd>>"{";
    for (;;) {
        new_cmd.get_line("gnucap-geda- "+basename+">");
        if (new_cmd >> "}") {
            break;
        }else{
            if(new_cmd>>"T"){
                new_cmd>>dump;
            }
            else {
                std::string _paramname=new_cmd.ctos("=","",""),_paramvalue;
                new_cmd>>"=">>_paramvalue;
                paramname.push_back (_paramname);
                paramvalue.push_back(_paramvalue);
                if(_paramname=="device"){
                    x->set_dev_type(_paramvalue);
                }else if (_paramname=="refdes" && _paramvalue!="?"){
                    x->set_label(_paramvalue);
                }else{
                    x->set_param_by_name(_paramname,_paramvalue);
                }
            }
        }
    }
    if(x->short_label()==""){
        x->set_label(type+to_string(rand()));
    }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_GEDA::parse_comment(CS& cmd, DEV_COMMENT* x)
{
    assert(x);
    x->set(cmd.fullstring());
    std::string dump,no_of_lines;
    if (cmd >> "T "){
        _mode=mCOMMENT;
        for(int i=0; i<8; ++i){
            cmd >> dump >> " ";
        }
        cmd>>no_of_lines;
        _no_of_lines=atoi(no_of_lines.c_str());
    }else{
        if(_no_of_lines!=0){
            --_no_of_lines;
            if(_no_of_lines==0){
                _mode=mATTRIBUTE;
            }
        }
    }
    return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_GEDA::parse_command(CS& cmd, DEV_DOT* x)
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
MODEL_CARD* LANG_GEDA::parse_paramset(CS& cmd, MODEL_CARD* x)
{
    assert(x);
    return NULL;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT* LANG_GEDA::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  //To parse heirarchical schematics
  return NULL;
}

MODEL_SUBCKT* LANG_GEDA::parse_componmod(CS& cmd, MODEL_SUBCKT* x)
{
    assert(x);
    cmd.reset();
    std::string type = find_type_in_string(cmd);
    assert(type=="C");
    std::string component_x, component_y, mirror, angle, dump,basename;
    bool isgraphical=false;
    cmd>>"C";
    unsigned here=cmd.cursor();
    cmd>>component_x>>" ">>component_y>>" ">>dump>>" ">>angle>>" ">>mirror>>" ">>basename;
    //open the basename to get the ports and their placements
    //parse_ports(newcmd,x);
    x->set_label("!_"+basename);
    std::vector<std::string*> coord=parse_symbol_file(x,basename);
    if(coord.size()==0){
        isgraphical=true;
    }
    if (isgraphical==true) {
        return NULL;
    }
    else{
        cmd.reset(here);
    }
    /*type = "graphical";
    x->set_dev_type(type);
    std::cout<<x->dev_type()<<" is the dev type\n";
    */
    return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_GEDA::parse_instance(CS& cmd, COMPONENT* x)
{
    cmd.reset();
    parse_type(cmd, x); //parse type will parse the component type and set_dev_type
    if (x->dev_type()=="net") {
    parse_net(cmd,x);
    }else if(x->dev_type()=="place"){
    parse_place(cmd,x);
    }else {
    parse_component(cmd,x);
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
std::string LANG_GEDA::find_type_in_string(CS& cmd)
{
    unsigned here = cmd.cursor(); //store cursor position to reset back later
    std::string type;   //stores type : should check device attribute..
    //graphical=["v","L","G","B","V","A","H","T"]
    if (cmd >> "v " || cmd >> "L " || cmd >> "G " || cmd >> "B " || cmd >>"V "
        || cmd >> "A " || cmd >> "H " || cmd >> "T " ){ type="dev_comment";}
    else if (cmd >> "N "){ type="net";}
    else if (cmd >> "U "){ type="bus";}
    else if (cmd >> "P "){ type="pin";}
    else if (cmd >> "C "){ type="C";}
    else if (cmd >> "place "){ type="place";}
    else {  
        switch(_mode){
            case mCOMMENT: return "dev_comment";
            default : cmd >> type;
        } 
    } //Not matched with the type. What now?
    cmd.reset(here);//Reset cursor back to the position that
                    //has been started with at beginning
    return type;    // returns the type of the string
}
/*----------------------------------------------------------------------*/
/* parse_top_item :
 * The top default thing that is parsed. Here new__instances are
 * created and (TODO)post processing of nets is done
 */
void LANG_GEDA::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
    cmd.get_line("gnucap-geda>");
    new__instance(cmd, NULL, Scope);
}
/*----------------------------------------------------------------------*/
// Code for Printing schematic follows
/*----------------------------------------------------------------------*/
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
static void print_node_xy(OMSTREAM& o,const COMPONENT* x,int _portindex)
{
    std::string _nodename=x->port_value(_portindex);
    for(CARD_LIST::const_iterator ci=x->scope()->begin(); ci!=x->scope()->end(); ++ci) {
        if((*ci)->dev_type()=="place"){
            if(static_cast<COMPONENT*>(*ci)->port_value(0)==_nodename){
                trace0("Got some place!");
                o << (*ci)->param_value(1) << " " << (*ci)->param_value(0) << " ";
            }
        }
    }
}
/*--------------------------------------------------------------------------*/
static std::string findcomponentposition(std::string* absxy, std::string* pinxy, std::string angle, std::string mirror)
{
    char newx[10];
    char newy[10];
    if(mirror=="0"){
        switch(atoi(angle.c_str())){
        case 0:
            sprintf(newx,"%d",atoi(absxy[0].c_str())-atoi(pinxy[0].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())-atoi(pinxy[1].c_str()));
            break;
        case 90:
            sprintf(newx,"%d",atoi(absxy[0].c_str())+atoi(pinxy[1].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())-atoi(pinxy[0].c_str()));
            break;
        case 180:
            sprintf(newx,"%d",atoi(absxy[0].c_str())+atoi(pinxy[0].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())+atoi(pinxy[1].c_str()));
            break;
        case 270:
            sprintf(newx,"%d",atoi(absxy[0].c_str())-atoi(pinxy[1].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())+atoi(pinxy[0].c_str()));
            break;
        }
    }else if(mirror=="1"){
        switch(atoi(angle.c_str())){
        case 0:
            sprintf(newx,"%d",atoi(absxy[0].c_str())+atoi(pinxy[0].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())-atoi(pinxy[1].c_str()));
            break;
        case 90:
            sprintf(newx,"%d",atoi(absxy[0].c_str())+atoi(pinxy[1].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())+atoi(pinxy[0].c_str()));
            break;
        case 180:
            sprintf(newx,"%d",atoi(absxy[0].c_str())-atoi(pinxy[0].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())+atoi(pinxy[1].c_str()));
            break;
        case 270:
            sprintf(newx,"%d",atoi(absxy[0].c_str())-atoi(pinxy[1].c_str()));
            sprintf(newy,"%d",atoi(absxy[1].c_str())-atoi(pinxy[0].c_str()));
            break;
        }
    }else{
    //not correct mirror!
    }
    std::string sx=newx;
    std::string sy=newy;
    return sx+" "+sy;
}
/*--------------------------------------------------------------------------*/
static std::string* find_place(const COMPONENT* x, std::string _portvalue)
{
    for(CARD_LIST::const_iterator ci=x->scope()->begin(); ci!=x->scope()->end(); ++ci) {
        if((*ci)->dev_type()=="place"){
            if(static_cast<COMPONENT*>(*ci)->port_value(0)==_portvalue){
                trace0("Got the coord");
                std::string*  a= new std::string[2];
                a[0]=(*ci)->param_value(1);     
                a[1]=(*ci)->param_value(0);
                trace0("Got out of find_place");
                return a;
            }
        }
    }
    return NULL;
    //To return place coordinates after searching for it.
}
/*--------------------------------------------------------------------------*/
static void print_net(OMSTREAM& o, const COMPONENT* x)
{
    assert(x);
    assert(x->dev_type()=="net");
    o << "N ";
    //print_coordinates(node); //Will search through the CARD_LIST to find the node
    //o<< node0x << node1x
    //o<< node1x << node2x
    print_node_xy(o,x,0);
    print_node_xy(o,x,1);
    if(x->value().string()!=""){
        o  << x->value().string()<<"\n"; //The color
    }else{
        o << "4\n";
    }
}
/*--------------------------------------------------------------------------*/
/* C x y selectable angle mirror basename
 * {
 *  <params>
 * }
 */
static void print_component(OMSTREAM& o, const COMPONENT* x)
{
    assert(x);
    trace0("Got into print_component");
    std::string _x,_y,_angle,_mirror;
    o <<  "C ";
    std::string _basename=x->param_value(x->param_count()-1);
    //go through the symbol file and get the relative pin positions
    std::vector<std::string*> coordinates=parse_symbol_file(NULL, _basename);
    std::vector<std::string*> abscoord;
    for(int ii=0; ii<coordinates.size(); ++ii){
        abscoord.push_back(find_place(x,x->port_value(ii)));
    }
    std::string angle[4]={"0","90","180","270"};
    int index=0; 
    std::string xy="";
    bool gottheanglemirror=false;
    for(int ii=0; ii<4 ; ++ii){
        if(not gottheanglemirror){
            _mirror="0";
            _angle=angle[ii];
            xy="";
            gottheanglemirror=true;
            for(int pinind=0; pinind<coordinates.size(); ++pinind){
                if (xy==""){
                    xy=findcomponentposition(abscoord[pinind],coordinates[pinind],_angle,_mirror);
                }else if(xy!=findcomponentposition(abscoord[pinind],coordinates[pinind],_angle,_mirror)){
                        gottheanglemirror=false;
                        break;
                }
            }
            if (gottheanglemirror) {
                o << xy << " " << "1" << " " << _angle << " " << _mirror << " " << _basename<< "\n";
            }
            else{
                _mirror="1";
                xy="";
                gottheanglemirror=true;
                for(int pinind=0; pinind<coordinates.size(); ++pinind){
                    if (xy==""){
                        xy=findcomponentposition(abscoord[pinind],coordinates[pinind],_angle,_mirror);
                    }else if(xy!=findcomponentposition(abscoord[pinind],coordinates[pinind],_angle,_mirror)){
                            gottheanglemirror=false;
                            break;
                    }
                }
                if (gottheanglemirror) {
                    o << xy << " " << "1" << " " << _angle << " " << _mirror << " " << _basename<< "\n";
                }
            }
        }
    }
    //map those with the absolute positions of nodes and place the device
    //such that it is in between the nodes.
    //std::vector<std::string*> coord = parse_symbol_file(static_cast<COMPONENT*>(x) , _basename);

    //Got the x and y
    //To print angle mirror etc, to get from the intelligent positioning

    //*To check if there are any attributes at all first
    //    If not return;
    if(x->param_count()>6){
        o << "{\n" ;
        if(x->short_label()!=""){
            o << "T "<< xy << " 5 10 0 1 0 0 1\n";
            o << "refdes=" << x->short_label() << "\n";
        }
        for(int i=x->param_count()-1; i>=0 ; --i){
            if (!(x->param_value(i)=="NA( 0.)" or x->param_value(i)=="NA( NA)" or x->param_value(i)=="NA( 27.)" or x->param_name(i)=="basename")){
                o << "T "<< xy << " 5 10 0 1 0 0 1\n";
                o << x->param_name(i) << "=" << x->param_value(i) << "\n";
            }
        }
        o << "}\n";
    }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_GEDA::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
    trace0("Got into print paramset");
    return;
}
/*--------------------------------------------------------------------------*/
void LANG_GEDA::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  //o<<x->short_label();
  //o<<"\n";
  assert(x->subckt());
  if(x->short_label().find("!_")!=std::string::npos){
    trace0("Got a placeholding model");
    return;
  }
}
/*--------------------------------------------------------------------------*/
void LANG_GEDA::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  trace0("Got into print_instance in geda!");
 // print_type(o, x);
 // print_label(o, x);
  if(x->dev_type()=="net"){
    print_net(o, x);
  }
  else if(x->dev_type()=="place"){
 }else{
    //Component
    print_component(o ,x);
  }
  }
/*--------------------------------------------------------------------------*/
void LANG_GEDA::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_GEDA::print_command(OMSTREAM& o, const DEV_DOT* x)
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
      lang_geda._mode=lang_geda.mATTRIBUTE;
      lang_geda._no_of_lines=0;
      srand(time(NULL));
      command("options lang=gschem", Scope);
    }
} p8;
DISPATCHER<CMD>::INSTALL
    d8(&command_dispatcher, "gschem", &p8);
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
class CMD_C : public CMD {
    void do_it(CS& cmd, CARD_LIST* Scope)
    {
      MODEL_SUBCKT* new_compon = new MODEL_SUBCKT;
      MODEL_SUBCKT* temp= new MODEL_SUBCKT;
      assert(new_compon);
      assert(!new_compon->owner());
      assert(new_compon->subckt());
      assert(new_compon->subckt()->is_empty());
      temp=lang_geda.parse_componmod(cmd, new_compon);
      if(temp){
        Scope->push_back(new_compon);
        std::string s=new_compon->short_label()+" "+cmd.tail();
        CS cmd(CS::_STRING,s);
        lang_geda.new__instance(cmd,NULL,Scope);
      }
    }
} p2;
DISPATCHER<CMD>::INSTALL
    d2(&command_dispatcher, "C", &p2);

/*----------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
