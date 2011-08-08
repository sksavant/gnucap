// add another interface to vvp compiler
#ifndef IVLC__H
#define IVLC__H

#include "vvp/compile.h"
#include <string>
#include <sstream>
#include <iostream>
#include "io_trace.h"
#include "vvp/codes.h"
using namespace std;
// #include <iostream>
#define sd(a) strdup(a)
#define none (const char*)0
class COMPONENT;

class ARG_BASE{
	public:
		virtual operator string() const=0;
		// needed to circumvent temporary address warning
		ARG_BASE* operator &(){ return(this); }
};
/*-----------------------------------------------------*/
class ARG_I : public ARG_BASE{
	private:
		intptr_t i;
	public:
		ARG_I(intptr_t x): ARG_BASE(),i(x){}
		operator string() const{
			stringstream x;
			x << hex << i;
			return x.str();
		}
};
/*-----------------------------------------------------*/
class ARG_E : public ARG_BASE{
	private:
		intptr_t i;
	public:
		static intptr_t offset;
		static intptr_t top;
		ARG_E(intptr_t x): ARG_BASE(),i(x)
		{ assert(x>=0);}
		operator string() const{
			stringstream x;
			x << "E_0x" << hex << i-offset;
			return x.str();
		}
};
/*-----------------------------------------------------*/
class ARG_O : public ARG_BASE{
	private:
		intptr_t i;
	public:
		static intptr_t offset;
		static intptr_t top;
		ARG_O(intptr_t x): ARG_BASE(),i(x){}
		operator string() const;
};
/*-----------------------------------------------------*/
class ARG_RAW : public ARG_BASE{
	private:
		string s;
	public:
		ARG_RAW(string x): ARG_BASE(),s(x){}
		ARG_RAW(const char* x): ARG_BASE(),s(x){}
		operator string() const{
			stringstream x;
			x << s;
			trace0(x.str());
			return x.str();
		}
};
/*-----------------------------------------------------*/
class ARG_S : public ARG_BASE{
	private:
		intptr_t i;
	public:
		static intptr_t offset;
		static intptr_t top;
		ARG_S(intptr_t x): ARG_BASE(),i(x){}
		operator string() const;
};
/*-----------------------------------------------------*/
class ARG_T : public ARG_BASE{
	private:
		intptr_t i;
		unsigned app;
		bool apped;
	public:
		static intptr_t offset;
		static intptr_t top;
		ARG_T(intptr_t x): ARG_BASE(),i(x),apped(false){}
		ARG_T(intptr_t x, unsigned y): ARG_BASE(),i(x),app(y),apped(true){}
		operator string() const{
			stringstream x;
			x << "T_" << i - offset;
			if (apped)
				x<<"." << app;
			return x.str();
		}
};
/*-----------------------------------------------------*/
class ARG_C4 : public ARG_BASE{
	private:
		unsigned i;
		string s;
		char c;
		int type;
	public:
		// just reproduce the string...
		// obsolete at the end??
		ARG_C4(const char* x): ARG_BASE(),s(strdup(x)),type(0){}

		ARG_C4(unsigned x): ARG_BASE(),i(x),type(1){}
		ARG_C4(char x, unsigned wid): ARG_BASE(),i(wid),c(x),type(2){}

		operator string() const;
};
/*------------------------------------------------*/
class ARG_P : public ARG_BASE{
	private:
		intptr_t i;
	public:
		static intptr_t offset;
		static intptr_t top;
		ARG_P(intptr_t x): ARG_BASE(),i(x){}
		operator string() const{
			stringstream x;
			x << "P_" << hex << i - offset;
			return x.str();
		}
};
/*------------------------------------------------*/
class ARG_V : public ARG_BASE{
	private:
		intptr_t i;
		int app;
	public:
		static intptr_t offset;
		static intptr_t top;
		ARG_V(intptr_t x, int y): ARG_BASE(),i(x), app(y){}
		operator string() const{
			stringstream x;
			x << "v0x" << hex << i - offset << "_" << app;
			return x.str();
		}
};
/*------------------------------------------------*/

// compile iverilogish argument list from ARG_BASE va_list...

inline symb_s* arg_symbols( unsigned argc, va_list argv ){
	assert (argc>0);

	struct symbv_s obj;
   symbv_init(&obj);
	for(unsigned i=0; i<argc; i++){
		ARG_BASE* w = va_arg ( argv, ARG_BASE* );
		assert(w);
		symb_s s;
		string sw = string(*w);
		assert(string(*w).c_str());
		char* ww = strdup(string(*w).c_str());
		trace0("wrapping" + string(*w));
		s.text=ww;
		s.idx=0;
		symbv_add(&obj, s);
	//	delete[]ww;
	}

	trace0("wrapped " + string(obj.vect[0].text));
	return (obj.vect);

}
inline symb_s* arg_symbols( unsigned argc, ... ){
	va_list arguments;
	va_start ( arguments, argc );
	return arg_symbols(argc,arguments);
}


class COMPILE_WRAP : public COMPILE {
	private:
		intptr_t param_offset;
		intptr_t param_top;
		intptr_t signal_offset;
		intptr_t signal_top;


	public:
		COMPILE_WRAP();
		void flush();

		void param_logic(ARG_BASE* label, char* name, char*value, bool signed_flag,
				long file_idx, long lineno);

		// this is a READ ONLY pointer. so it is ok.
		void param_real(ARG_BASE*label, char*name, const double* value,
											 long file_idx, long lineno);

		void variable(intptr_t, int app, char*name,
				int msb, int lsb, int vpi_type_code,
				bool signed_flag, bool local_flag);
		void variable(ARG_BASE* label, char*name,
				int msb, int lsb, int vpi_type_code,
				bool signed_flag, bool local_flag);


		/*
		void code( intptr_t label, const char *mnem );
		void code( intptr_t label, const char *mnem,
			  	unsigned long l);
		void code( intptr_t label, const char *mnem,
			  	const char* cc);
		void code( intptr_t label, const char *mnem,
			  	unsigned long l, unsigned long m);
		void code( intptr_t label, const char *mnem,
			  	unsigned long l, const char* m, unsigned long n);
		void code( intptr_t label, const char *mnem,
				unsigned long l, unsigned long m, unsigned long n);
		void code( intptr_t label, const char *mnem,
				const char * l, unsigned long m, unsigned long n);
				*/

	   void vpi_call( const char* label, const char* cmd, bool b0, bool
		b1, long l0, long l1, ... );

		//deprecated.
		void net( intptr_t label, int app, char*name, int msb, int lsb,
				int vpi_type_code, bool signed_flag, bool local_flag,
				unsigned argc, ... );
		void net( ARG_BASE* label, char*name, int msb, int lsb,
				int vpi_type_code, bool signed_flag, bool local_flag,
				unsigned argc, ... );

		void functor(const char*label, const char*type, unsigned width,
				unsigned ostr0, unsigned ostr1,
				unsigned argc, ...) {
			va_list arguments,argv;
			va_start ( arguments, argc );
			va_copy(argv,arguments);

//			if (argc > 0 ){
//				ARG_BASE* w = va_arg ( arguments, ARG_BASE* );
//				trace0("passing arguments 1, "+ string(*w) );
//			}

			COMPILE::functor(label, type, width,
					ostr0,  ostr1,
					argc, arg_symbols( argc, argv));
		}
	// compile->functor( &ARG_O(0), s("BUFZ"), 1, 6, 6, 1, &ARG_RAW("C4<z>") ); // o0x2b9ed4fce018
		void functor(const ARG_BASE* label, const char*type, unsigned width,
				unsigned ostr0, unsigned ostr1,
				unsigned argc, va_list argv) {

			const char* l = strdup(string(*label).c_str());
			trace0("calling arg_symbols");
			struct symb_s*args = arg_symbols(argc, argv );
			COMPILE::functor(l, type, width,
					ostr0,  ostr1,
					argc, args);
		}
		void functor(const ARG_BASE* label, const char*type, unsigned width,
				unsigned ostr0, unsigned ostr1,
				unsigned argc, ...) {
			va_list argv;
			va_start( argv, argc );

			functor(label, type, width,
					ostr0,  ostr1,
					argc, argv);
		}
		void event(int l, char* t, int argc, ... ){
			va_list argv;
			va_start(argv,argc);
			COMPILE::event(strdup(string(ARG_E(l)).c_str()), t, argc, arg_symbols(argc, argv));
		}
		void event(ARG_BASE* l, char* t, int argc, ... ){
			va_list argv;
			va_start(argv,argc);
			COMPILE::event(strdup(string(*l).c_str()), t, argc, arg_symbols(argc, argv));
		}

		/*---------------------------*/
		inline void code( const char *label,  const char *mnem ){
			comp_operands_s *opa = 0;
			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}
		inline void code( const char *label, const char *mnem, unsigned long l){
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			opa->argc = 1;

			opa->argv[0].ltype = L_NUMB;
			opa->argv[0].numb = l;

			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}
		inline void code( const char *label, const char *mnem, const char* cc){
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			//= new comp_operands_s();
			opa->argc = 1;

			symb_s s;// = new symb_s();
			s.text = strdup(cc);
			s.idx = 0;
			opa->argv[0].ltype = L_SYMB;
			opa->argv[0].symb = s;

			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}


		inline void code( const char *label, const char *mnem,
			  	unsigned long l,
			  	unsigned long m){
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			opa->argc = 2;

			opa->argv[0].ltype = L_NUMB;
			opa->argv[0].numb = l;
			opa->argv[1].ltype = L_NUMB;
			opa->argv[1].numb = m;


			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}

		/*inline void code( const char *label, const char *mnem,
			  	unsigned long l,
			  	const char* m,
			  	unsigned long n){
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			opa->argc = 3;
			symb_s M;
			M.text = strdup(m);
			M.idx = 0;

			opa->argv[0].ltype = L_NUMB;
			opa->argv[0].numb = l;
			opa->argv[1].ltype = L_SYMB;
			opa->argv[1].symb = M;
			opa->argv[2].ltype = L_NUMB;
			opa->argv[2].numb = n;

			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}*/

		inline void code( const char *label, const char *mnem,
				unsigned long l,
			  	unsigned long m,
			  	unsigned long n){
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			opa->argc = 3;

			opa->argv[0].ltype = L_NUMB;
			opa->argv[0].numb = l;
			opa->argv[1].ltype = L_NUMB;
			opa->argv[1].numb = m;
			opa->argv[2].ltype = L_NUMB;
			opa->argv[2].numb = n;

			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}

//	compile->code( 0, "\%set/v", &ARG_V(0,0), 0, 32);
		inline void code( const char *label, const char *mnem,
			  	const ARG_BASE* l,
			  	unsigned long m,
			  	unsigned long n){
			assert(l);
			trace0("COMPILE_WRAP::code " + string(*l) );
			char *ll = strdup(string(*l).c_str());
			assert(ll);
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			symb_s L;
			opa->argc = 3;
			L.text = ll;
			L.idx = 0;

			opa->argv[0].ltype = L_SYMB;
			opa->argv[0].symb = L;
			opa->argv[1].ltype = L_NUMB;
			opa->argv[1].numb = m;
			opa->argv[2].ltype = L_NUMB;
			opa->argv[2].numb = n;

			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}

		void thread( const ARG_BASE* l, char* c ){
			return COMPILE::thread ( strdup(string(*l).c_str()), c);
		}
		void scope_recall( const ARG_BASE* l ){
			return COMPILE::scope_recall ( strdup(string(*l).c_str()));
		}
		void code( const char*label, const char *mnem,
			  	const ARG_BASE* l ){
			return code (label, mnem, strdup(string(*l).c_str()));
		}

		void code( const char*label, const char *mnem,
			  	unsigned long l,
			  	const ARG_BASE* m,
			  	unsigned long n){
			char *mm = strdup(string(*m).c_str());

			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			opa->argc = 3;
			symb_s M;
			M.text = mm;
			M.idx = 0;

			opa->argv[0].ltype = L_NUMB;
			opa->argv[0].numb = l;
			opa->argv[1].ltype = L_SYMB;
			opa->argv[1].symb = M;
			opa->argv[2].ltype = L_NUMB;
			opa->argv[2].numb = n;

			COMPILE::code(label?sd(label):0, sd(mnem), opa);
		}

		void scope_decl(ARG_BASE*typ, char*lab, char*nam, char*tnam,
				char*par, long file_idx, long lineno,
				long def_file_idx, long def_lineno,
				long is_cell){

			return
				COMPILE::scope_decl(strdup(string(*typ).c_str()), lab, nam, tnam,
						par, file_idx, lineno,
						def_file_idx, def_lineno,
						is_cell);


		}

		void notify( const ARG_BASE* l,
			  	unsigned long m,
			  	unsigned long n, COMPONENT* daport){

			trace0("COMPILE_WRAP::notify");
			assert(l);
			char *ll = strdup(string(*l).c_str());
			assert(ll);
			comp_operands_s *opa = (comp_operands_t) calloc(1, sizeof(comp_operands_s));
			symb_s L;
			opa->argc = 3;
			L.text = ll;
			L.idx = 0;

			opa->argv[0].ltype = L_SYMB;
			opa->argv[0].symb = L;
			opa->argv[1].ltype = L_NUMB;
			opa->argv[1].numb = m;
			opa->argv[2].ltype = L_NUMB;
			opa->argv[2].numb = n;

			notify( opa, daport );
		}

		void notify( comp_operands_t opa, COMPONENT* daport );
		void load_real_immediate( const double*, unsigned reg );
		void load_number_immediate( const int64_t*, unsigned reg );
		void codelabel( ARG_BASE* label);


}; // COMPILE_WRAP






//#define OPERAND_MAX 3
//enum ltype_e { L_NUMB, L_SYMB };
//
//struct comp_operands_s {
//	unsigned argc;
//	struct {
//		enum ltype_e ltype;
//		union {
//			unsigned long numb;
//			struct symb_s symb;
//		};
//	} argv[OPERAND_MAX];
//};

// void compile_code(char*label, char*mnem, va_list a);
//
class COMPILE;

/* 
symb_s* operands_wrap( unsigned argc, ... ){
   va_list arguments;
   va_start ( arguments, argc );
   assert (argc>0);

   struct symbv_s obj;
   symbv_init(&obj);

	for(unsigned i=0; i<argc; i++){
	const char* w = va_arg ( arguments, const char* );

	comp_operands_s* opa = (comp_operands_s*)
	calloc(1, sizeof(struct comp_operands_s)); 

	char* ww=strdup(w);
   }
}
*/


inline void compile_vpi_call(COMPILE* c, const char* label, const char* cmd, bool b0, bool
		b1, long l0, long l1, ... ){
  	unsigned i=0; char* w;
	va_list arguments;

	va_start ( arguments, l1 );
	struct argv_s argv;

	w = va_arg ( arguments, char* );
	if (!w){
		argv_init( &argv);
	} else {


		while (1) {
			i++;
			cerr << "argument" <<  w << "\n";

			if (w[0]=='\"'){


			} else if (1) {

			}


			w = va_arg ( arguments, char* );
			if (!w) break;
		}
		// argv_sym_lookup(c, &$2);
	}

	c->vpi_call(label?sd(label):0, sd(cmd), b0, b1, l0, l1, i, (vpiHandle*)&argv);

}
/*
void compile_net(COMPILE* c, char*label, char*name, int msb, int lsb,
			int vpi_type_code, bool signed_flag, bool local_flag,
			unsigned argc, ... ){
	va_list arguments;
	va_start ( arguments, argc );
	assert (argc>0);

	struct symbv_s obj;
   symbv_init(&obj);
	for(unsigned i=0; i<argc; i++){
		char* w = va_arg ( arguments, char* );
		symb_s s;
		s.text=w;
		s.idx=0;
		symbv_add(&obj, s);

	}

	assert(obj.cnt==argc);
	c->net(label, name, msb, lsb,
			 vpi_type_code, signed_flag, local_flag,
			 argc, obj.vect);

}*/




inline symb_s* symbols_wrap( unsigned argc, ... ){
	va_list arguments;
	va_start ( arguments, argc );
	assert (argc>0);


	struct symbv_s obj;
   symbv_init(&obj);
	for(unsigned i=0; i<argc; i++){
		const char* w = va_arg ( arguments, const char* );
		symb_s s;
		char* ww=strdup(w);
		trace0("wrapping" + string(w));
		s.text=ww;
		s.idx=0;
		symbv_add(&obj, s);
	//	delete[]ww;

	}

	trace0("wrapped " + string(obj.vect[0].text));
	return (obj.vect);

}


#endif
