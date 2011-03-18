/* $Id: d_bti.h,v 1.7 2010-09-07 07:46:20 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
 *
 * (c) Felix Salfelder 2010
 * 
 * GPLv3
 */


#ifndef D_BTI_H_INCLUDED
#define D_BTI_H_INCLUDED

#include "d_diode.h"
#include "e_aux.h"
#include "d_rcd.h"
#include "u_limit.h"
#include "u_sdp.h"
#include "e_node.h"
#include "e_subckt.h"
#include "e_model.h"
class COMMON_BUILT_IN_RCD;
/*--------------------------------------------------------------------------*/
class SDP_BUILT_IN_BTI
  :public SDP_CARD{
public:
  explicit SDP_BUILT_IN_BTI(const COMMON_COMPONENT* c) : SDP_CARD(c) {init(c);}
  void init(const COMMON_COMPONENT*);
public:
};
/*--------------------------------------------------------------------------*/
class DEV_BUILT_IN_BTI;
class TDP_BUILT_IN_BTI{
public:
  explicit TDP_BUILT_IN_BTI(const DEV_BUILT_IN_BTI*);
public:
};
/*--------------------------------------------------------------------------*/
class MODEL_BUILT_IN_BTI
  :public MODEL_CARD{
protected:
  explicit MODEL_BUILT_IN_BTI(const MODEL_BUILT_IN_BTI& p);
public:
  explicit MODEL_BUILT_IN_BTI(const BASE_SUBCKT*);
  ~MODEL_BUILT_IN_BTI() {--_count;}
public: // override virtual
  virtual std::string dev_type()const;
  void      set_dev_type(const std::string& nt);
  CARD*     clone()const {return new MODEL_BUILT_IN_BTI(*this);}
  virtual void      precalc_first();
  virtual void      precalc_last();
  SDP_CARD* new_sdp(COMMON_COMPONENT* c)const;
  ADP_CARD* new_adp( const COMPONENT* c)const;
  virtual void      set_param_by_index(int, std::string&, int);
  virtual bool      param_is_printable(int)const;
  virtual std::string param_name(int)const;
  virtual std::string param_name(int,int)const;
  virtual std::string param_value(int)const;
  virtual int param_count()const {return (11);}
  bool      is_valid(const COMPONENT*)const;
  bool      is_v2()const{return (bool) v2;}
  void      tr_eval(COMPONENT*)const;
  virtual void do_stress_apply(COMPONENT*)const{ std::cerr<<"virtual stress apply(C)\n" ;}
  virtual void attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const;

  //copy from subckt (stupid)

public: // not virtual
  static int count() {return _count;}
protected: // strictly internal
  static int _count;
  double hoch(int i) const;
  double runter(int i) const;
public: // input parameters
  PARAMETER<double> gparallel;	// parallel conductance
  PARAMETER<int> flags;	// 
  PARAMETER<int> mos_level;	// 
  PARAMETER<uint_t> rcd_number;	// 
  PARAMETER<int> bti_type;	// 
  PARAMETER<double> bti_base;	// 
  PARAMETER<bool> anneal;	// 
  PARAMETER<std::string> rcd_model_name;	// 
  PARAMETER<double> weight;	// 
  PARAMETER<double> uref;
  PARAMETER<bool> v2;	// 
//  PARAMETER<MODEL_BUILT_IN_RCD> rcd_model;	// 
public: // calculated parameters
  virtual std::string RCD_name(uint_t)const;
};
/*--------------------------------------------------------------------------*/
class MODEL_BUILT_IN_BTI_MATRIX
  :public MODEL_BUILT_IN_BTI{
protected:
  int foo();
  explicit MODEL_BUILT_IN_BTI_MATRIX(const MODEL_BUILT_IN_BTI_MATRIX& p);
public:
  explicit MODEL_BUILT_IN_BTI_MATRIX(const BASE_SUBCKT*);
public: // override virtual
  virtual void attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const;
  std::string dev_type()const {return "bti_matrix";}
  void      set_dev_type(const std::string& nt);
  CARD*     clone()const {return new MODEL_BUILT_IN_BTI_MATRIX(*this);}
  void      set_param_by_index(int, std::string&, int);
  bool      param_is_printable(int)const;
  std::string param_name(int)const;
  std::string param_name(int,int)const;
  std::string param_value(int)const;
  int param_count()const;
  virtual void     precalc_first();
public: // not virtual
  static int count() {return _count;}
public: // input parameters
  PARAMETER<uint_t> cols;
  PARAMETER<uint_t> rows;
  PARAMETER<double> base;
//  PARAMETER<MODEL_BUILT_IN_RCD> rcd_model;	// 
public: // calculated parameters
};
/*--------------------------------------------------------------------------*/
class MODEL_BUILT_IN_BTI_SINGLE
  :public MODEL_BUILT_IN_BTI{
protected:
  int foo();
  explicit MODEL_BUILT_IN_BTI_SINGLE(const MODEL_BUILT_IN_BTI_SINGLE& p);
public:
  explicit MODEL_BUILT_IN_BTI_SINGLE(const BASE_SUBCKT*);
public: // override virtual
  virtual void attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const ;
  std::string dev_type()const {return "bti_single";}
  void      set_dev_type(const std::string& ) {}
  CARD*     clone()const {return new MODEL_BUILT_IN_BTI_SINGLE(*this);}
  void      set_param_by_index(int, std::string&, int);
  bool      param_is_printable(int)const;
  std::string param_name(int)const;
  std::string param_name(int i,int j)const { if (j) return ""; return param_name(i); }
  std::string param_value(int)const;
  int param_count()const;
  virtual void     precalc_first();
public: // input parameters
  PARAMETER<int> fooo;
//  PARAMETER<MODEL_BUILT_IN_RCD> rcd_model;	// 
public: // calculated parameters
};
/*--------------------------------------------------------------------------*/
class COMMON_BUILT_IN_BTI
  :public COMMON_COMPONENT{
public:
  explicit COMMON_BUILT_IN_BTI(const COMMON_BUILT_IN_BTI& p);
  explicit COMMON_BUILT_IN_BTI(int c=0);
           ~COMMON_BUILT_IN_BTI();
  bool     operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new COMMON_BUILT_IN_BTI(*this);}
  void     set_param_by_index(int, std::string&, int);
  bool     param_is_printable(int)const;
  std::string param_name(int)const;
  std::string param_name(int,int)const;
  std::string param_value(int)const;
  int param_count()const {return (3 + COMMON_COMPONENT::param_count());}
  virtual void     precalc_first(const CARD_LIST*);
  void     expand(const COMPONENT*);
  void     precalc_last(const CARD_LIST*);
  std::string name()const {itested();return "bti";}
  const SDP_CARD* sdp()const {return _sdp;}
  bool     has_sdp()const {untested();return _sdp;}
  static int  count() {return _count;}
private: // strictly internal
  static int _count;
public: // input parameters
  PARAMETER<double> lambda;	// lambda
  PARAMETER<int>    number;	// number
  PARAMETER<int>    type;	// type
  PARAMETER<double> base;	
  PARAMETER<double> weight;	// weight
  PARAMETER<polarity_t> polarity;	// polarity
public: // calculated parameters
  SDP_CARD* _sdp;
public: // attached commons
  COMMON_COMPONENT** _RCD;
};
/*--------------------------------------------------------------------------*/
class DEV_BUILT_IN_BTI : public BASE_SUBCKT {
private:
  explicit DEV_BUILT_IN_BTI(const DEV_BUILT_IN_BTI& p);
public:
  explicit DEV_BUILT_IN_BTI();
           ~DEV_BUILT_IN_BTI() {--_count; if (_RCD){ delete [] _RCD; _RCD=NULL; } }
private: // override virtual
  char      id_letter()const     {untested();return 'B';}
  bool      print_type_in_spice()const {return true;}
  std::string value_name()const  {return "area";}
  //std::string dev_type()const;   //BASE_SUBCKT
  uint_t       max_nodes()const     {return 2;}
  uint_t       min_nodes()const     {return 2;}
  //int     matrix_nodes()const; //BASE_SUBCKT
  uint_t       net_nodes()const     {return 2;}
  uint_t       int_nodes()const     {return 1;}
  CARD*     clone()const         {return new DEV_BUILT_IN_BTI(*this);}
  void      precalc_first() {COMPONENT::precalc_first(); if(subckt()) subckt()->precalc_first();}
  void      expand();
  void      precalc_last()  {COMPONENT::precalc_last(); assert(subckt()); subckt()->precalc_last();}
  //void    map_nodes();         //BASE_SUBCKT
  //void    tr_begin();          //BASE_SUBCKT
  //void    tr_restore();        //BASE_SUBCKT
  //void    tt_commit();         //BASE_SUBCKT
  void      stress_apply(); 
  void      tt_commit( );   
  void      tt_prepare();         //BASE_SUBCKT
  //void    dc_advance();        //BASE_SUBCKT
  //void    tr_advance();        //BASE_SUBCKT
  //void    tr_regress();        //BASE_SUBCKT
  //bool    tr_needs_eval()const;//BASE_SUBCKT
  //void    tr_queue_eval();     //BASE_SUBCKT
  bool    do_tr();
  //void    tr_load();           //BASE_SUBCKT
  //double  tr_review();         //BASE_SUBCKT
  //void    tr_accept();         //BASE_SUBCKT
  //void    tr_unload();         //BASE_SUBCKT
  double    tr_probe_num(const std::string&)const;
  double    tt_probe_num(const std::string&)const;
  //void    ac_begin();          //BASE_SUBCKT
  //void    do_ac();             //BASE_SUBCKT
  //void    ac_load();           //BASE_SUBCKT
  //XPROBE  ac_probe_ext(CS&)const;//CKT_BASE/nothing
  virtual void	tr_stress();
  virtual void	tr_stress_last();
public:
  static int  count() {return _count;}
  double vw()const;
  double dvth()const;
public: // may be used by models
private: // not available even to models
  static int _count;
public: // input parameters
public: // calculated parameters
  bool cutoff;	// flag: in cut off region
public: // netlist
  COMPONENT* _Ubti;
  COMPONENT** _RCD;
  COMPONENT* _RCD1;
  COMPONENT* _RCD2;
  COMPONENT* _RCD3;
private: // node list
  enum {n_g, n_b, n_iu};
  node_t _nodes[3];
  std::string port_name(uint_t i)const {
    assert(i !=INVALID_NODE);
    assert(i < 2);
    static std::string names[] = {"g", "b", ""};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
// h_direct


class ADP_BUILT_IN_BTI
  :public ADP_CARD{
public:
  explicit ADP_BUILT_IN_BTI(const COMPONENT* c) : ADP_CARD(c)
  //. Hgone(0),
    {init(c);}

  void init(const COMPONENT*);
public:

// hier steckt die 'effektive gleichspannung' drin
  ADP_NODE* bti_stress; // FIXME, BTI_?
  double tr_probe_num(const std::string& x)const;
  double tt_probe_num(const std::string& x)const;

private:

public:
  virtual void tt_accept();
  virtual void tt_commit();
  double eff(){return 0.0; // value*voltage?
}
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
