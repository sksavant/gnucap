

// used to inject things to the vvp runtime...
// renumbering possible

#include "e_ivl_compile.h"

intptr_t ARG_E::offset = 0;
intptr_t ARG_V::offset = 0;
intptr_t ARG_T::offset = 0;
intptr_t ARG_S::offset = 0;
intptr_t ARG_P::offset = 0;

intptr_t ARG_E::top = 0;
intptr_t ARG_V::top = 0;
intptr_t ARG_T::top = 0;
intptr_t ARG_S::top = 0;
intptr_t ARG_P::top = 0;

/*-----------------------------------------------*/
ARG_O::operator string() const{
	stringstream x;
	x << "o" << hex  << i;
	trace0("ARG_O::operator " + x.str());
	return x.str();
}
/*-----------------------------------------------*/
ARG_C4::operator string() const{
	stringstream x;
	assert( type>0); //for now...
	x << "C4<" << s << ">";
	return x.str();
}
/*-----------------------------------------------*/
ARG_S::operator string() const{
	stringstream x;
	x << "S_0x" << hex << i;
	return x.str();
}
/*-----------------------------------------------*/
COMPILE_WRAP::COMPILE_WRAP():COMPILE()
{
/*	param_offset = 0;// offset used internally
	param_top = 0;
	signal_offset = 0;
	signal_top = 0;
	*/
}
/*-----------------------------------------------*/
void COMPILE_WRAP::flush(){
	// hmmm... probably unneeded.
	assert(false);
	param_offset += param_top;
	param_top = 0;
	signal_offset += signal_top;
	signal_top = 0;
}
/*-----------------------------------------------*/
void COMPILE_WRAP::param_logic(ARG_BASE* label, char* name, char*value, bool signed_flag,
		long file_idx, long lineno){
	char* buf = strdup(string(*label).c_str());
	trace0("COMPILE_WRAP::param_logic " + string(*label));
	COMPILE::param_logic(buf, name, value, signed_flag,
			file_idx,  lineno);
};
/*-----------------------------------------------*/
//depr.
void COMPILE_WRAP::variable(intptr_t label, int app, char*name,
			     int msb, int lsb, int vpi_type_code,
			     bool signed_flag, bool local_flag){
	assert(label>=0);
	signal_top = max(signal_top,label);

	char* buf = (char*)malloc(64*sizeof(char));
	snprintf(buf, 64*sizeof(char), "v0x%lx_%i", label+signal_offset, app);
	trace3("COMPILE_WRAP::variable "+string(buf), label, signal_offset, app);

	COMPILE::variable(buf, name, msb, lsb, vpi_type_code, signed_flag, local_flag);
}
void COMPILE_WRAP::variable(ARG_BASE* label,  char*name,
			     int msb, int lsb, int vpi_type_code,
			     bool signed_flag, bool local_flag){

	char* buf = strdup(string(*label).c_str());

	COMPILE::variable(buf, name, msb, lsb, vpi_type_code, signed_flag, local_flag);
}
/*-----------------------------------------------*/
void COMPILE_WRAP::net( ARG_BASE* l, char*name, int msb, int lsb,
				int vpi_type_code, bool signed_flag, bool local_flag,
				unsigned argc, ... ){

	va_list args;
	va_start(args, argc);

	char* buf = strdup(string(*l).c_str());

	COMPILE::net( buf, name,  msb,  lsb,
			vpi_type_code, signed_flag, local_flag,
			argc, arg_symbols(argc,args) );
}
/*-----------------------------------------------*/
void COMPILE_WRAP::net( intptr_t label, int app, char*name, int msb, int lsb,
				int vpi_type_code, bool signed_flag, bool local_flag,
				unsigned argc, ... ){

	assert(label>=0);
	va_list args;
	va_start(args, argc);

	char* buf = (char*)malloc(64*sizeof(char));
	snprintf(buf, 64*sizeof(char), "v0x%lx_%i", label+signal_offset, app);

	COMPILE::net( buf, name,  msb,  lsb,
			vpi_type_code, signed_flag, local_flag,
			argc, arg_symbols(argc,args) );
}
/*-----------------------------------------------*/
void COMPILE_WRAP::codelabel( ARG_BASE* label) {
	char* buf = strdup(string(*label).c_str());
	COMPILE::codelabel(buf);
}
/*-----------------------------------------------*/
// taken from compile.c
// FIXME: propagate *double instead of double ( not needed for now. )
void COMPILE_WRAP::param_real( ARG_BASE* l, char*name, const double*dvp,
                        long file_idx, long lineno)
{
		char* label = strdup(string(*l).c_str());
      // double dvalue = crstring_to_double(value);
      vpiHandle obj = vpip_make_real_param(name, *dvp, file_idx, lineno);
		           //             FIXME            ^^
      vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);

      delete[](label);
      // free(value);
}
