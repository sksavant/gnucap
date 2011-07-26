

// used to inject things to the vvp runtime...
// renumbering possible

#include "e_ivl_compile.h"

intptr_t ARG_E::offset = 0;
intptr_t ARG_V::offset = 0;

/*-----------------------------------------------*/
ARG_O::operator string() const{
	stringstream x;
	x << "o0x" << hex  << i;
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
	param_offset = 0;// offset to used internally
	param_top = 0;
	signal_offset = 0;
	signal_top = 0;
}
/*-----------------------------------------------*/
void COMPILE_WRAP::flush(){
	param_offset += param_top;
	param_top = 0;
	signal_offset += signal_top;
	signal_top = 0;
}
/*-----------------------------------------------*/
void COMPILE_WRAP::param_logic(intptr_t label, char* name, char*value, bool signed_flag,
		long file_idx, long lineno){
	assert(label>=0);
	param_top = max(param_top,label);

	char* buf = (char*)malloc(64*sizeof(char));
	snprintf(buf, 64*sizeof(char), "P_0x%lx", label+param_offset);

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
