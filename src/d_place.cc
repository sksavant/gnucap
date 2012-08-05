

#include "e_compon.h"
#include "e_node.h"
class DEV_PLACE : public COMPONENT {
    public:
        explicit DEV_PLACE() :COMPONENT()
        {
            _n=_nodes;
        }
        explicit DEV_PLACE(const DEV_PLACE& p) :COMPONENT(p){
            _n=_nodes;
        }
        ~DEV_PLACE(){}
    private:
        bool param_is_printable(int)const;
    protected:
        PARAMETER<double> _xco;
        PARAMETER<double> _yco;
    private:
        std::string param_name(int) const;
        std::string param_name(int, int) const;
        void set_param_by_name(std::string, std::string);
        void set_param_by_index(int, std::string, int);
        std::string param_value(int) const;
    private:
        char        id_letter()const {return 'P';}
        std::string value_name()const {return "place";}
        std::string dev_type()const {untested(); return "place";}
        int         max_nodes() const {return 1;}
        int         min_nodes()const {return 1;}
        int         matrix_nodes()const {return 1;}
        int         net_nodes()const {return 1;}
        bool      has_iv_probe()const {return true;}
        bool        print_type_in_spice()const {return true;}
        bool        is_device() const {return false;}
        CARD*       clone()const    {return new DEV_PLACE(*this);}
        int         param_count()const {return 2;}
        std::string port_name(int i)const{
            assert(i==0);
            return "port";
        }
    public:
        node_t      _nodes[NODES_PER_BRANCH];
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
std::string DEV_PLACE::param_name(int i) const
{
    switch(DEV_PLACE::param_count()-i-1) {
    case 0: return "x";
    case 1: return "y";
    default : return "";
    }
}
/*--------------------------------------------------------------------------*/
std::string DEV_PLACE::param_name(int i,int j) const{
    if (j==0){
        return param_name(i);
    }else{
        return "";
    }
}
/*--------------------------------------------------------------------------*/
void DEV_PLACE::set_param_by_name(std::string Name,std::string Value)
{
    for (int i=DEV_PLACE::param_count()-1; i>=0; --i) {
        for (int j=0; DEV_PLACE::param_name(i,j)!=""; ++j) {
            if(Name==DEV_PLACE::param_name(i,j)) {
                DEV_PLACE::set_param_by_index(i,Value,0);
                return;
            }else{
            }
        }
    }
    throw Exception_No_Match(Name+" "+Value);
}
/*--------------------------------------------------------------------------*/
void DEV_PLACE::set_param_by_index(int i, std::string Value, int offset)
{
    switch(DEV_PLACE::param_count()-1-i) {
    case 0: _xco = Value; break;
    case 1: _yco = Value; break;
    default: throw Exception_Too_Many(i,2,offset);
    }
}
/*--------------------------------------------------------------------------*/
bool DEV_PLACE::param_is_printable(int i)const
{
    switch(DEV_PLACE::param_count()-1-i) {
    case 0: return true;
    case 1: return true;
    default: return COMPONENT::param_is_printable(i);
    }
}
/*--------------------------------------------------------------------------*/
std::string DEV_PLACE::param_value(int i)const
{
    switch(DEV_PLACE::param_count()-1-i) {
    case 0: return _xco.string();
    case 1: return _yco.string();
    }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_PLACE p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher,"place",&p1);
/*--------------------------------------------------------------------------*/
