/* $Id: d_bti.cc,v 1.7 2010-09-07 07:46:19 felix Exp $ -*- C++ -*-
vim:ts=8:sw=2:et:
*/

// non-GPL code ??

#define WS_COLS 11
#define WS_ROWS 11

#include "d_bti.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MODEL_BUILT_IN_BTI_INF
:public MODEL_BUILT_IN_BTI{
  protected:
    int foo();
    explicit MODEL_BUILT_IN_BTI_INF(const MODEL_BUILT_IN_BTI_INF& p);
  public:
    explicit MODEL_BUILT_IN_BTI_INF(const BASE_SUBCKT*);
  public: // override virtual
    virtual void attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const ;
    std::string dev_type()const {return "bti_inf";}
    void      set_dev_type(const std::string& ) {}
    CARD*     clone()const {return new MODEL_BUILT_IN_BTI_INF(*this);}
    void      set_param_by_index(int, std::string&, int);
    bool      param_is_printable(int)const;
    std::string param_name(int)const;
    std::string param_name(int i,int j)const { if (j) return ""; return param_name(i); }
    std::string param_value(int)const;
    int param_count()const;
    virtual void     precalc_first();
  public: // input parameters
    PARAMETER<uint_t> rows;
    PARAMETER<uint_t> cols;
    PARAMETER<double> base;
    PARAMETER<std::vector<PARAMETER<double> > > matrix; //?
    PARAMETER<uint_t> total;
  public: // calculated parameters
    virtual std::string RCD_name(uint_t)const;
};
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_INF::MODEL_BUILT_IN_BTI_INF(const BASE_SUBCKT* p)
  :MODEL_BUILT_IN_BTI(p),
  rows(0),
  cols(0),
  base(10)
{
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_INF::MODEL_BUILT_IN_BTI_INF(const
    MODEL_BUILT_IN_BTI_INF& p)
  :MODEL_BUILT_IN_BTI(p),
  rows(p.rows),
  cols(p.cols),
  base(p.base),
  matrix(p.matrix)
{
  if(matrix.size() != rows*cols){
    trace1("MODEL_BUILT_IN_BTI_INF", matrix.size());
    throw(Exception("size mismatch %i %i\n", 
        (int)rows, (int)cols));
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_INF::precalc_first()
{
  const CARD_LIST* par_scope = scope();
  assert(par_scope);
  MODEL_BUILT_IN_BTI::precalc_first();

  matrix.e_val( std::vector<PARAMETER<double> >(0), par_scope);
  total.e_val( 0, par_scope);
  trace2("MODEL_BUILT_IN_BTI::precalc_first", rows,cols);
  e_val(&(this->rows), (uint_t)11, par_scope);
  e_val(&(this->cols), (uint_t)11, par_scope);
  trace2("MODEL_BUILT_IN_BTI::precalc_first", rows,cols);

  base.e_val(10,par_scope);

  if ((int)rcd_number == 0){
    untested();
  }

}
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_BTI_INF::param_count()const
{
  return MODEL_BUILT_IN_BTI::param_count() + 4;
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_INF::param_value(int i)const
{
  switch (param_count() - 1 - i) {
    case 0: return "...";
    case 1: return to_string(rows);
    case 2: return to_string(cols);
    case 3: return string(matrix);
    case 4: return to_string(base);
  }
  return MODEL_BUILT_IN_BTI::param_value(i);
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_INF::param_name(int i)const
{
  switch (param_count() - 1 - i ){
        case 0: return "======";
        case 1: return "rows";
        case 2: return "cols";
        case 3: return "matrix";
        case 4: return "base";
  }
  return MODEL_BUILT_IN_BTI::param_name(i );

}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI_INF::param_is_printable(int i)const
{
  switch (param_count() - 1 - i) {
    case 0:  return (false);
    case 1: 
    case 2: 
    case 3: 
    case 4:  return (true);
    default: return MODEL_BUILT_IN_BTI::param_is_printable( i );
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_INF::set_param_by_index(int i, std::string& value, int offset)
{
  switch (param_count() - 1 - i) {
    case 0: untested(); break;
    case 1: rows = value; break;
    case 2: cols = value; break;
    case 3: matrix = value; break;
    case 4: base = value; break;
    default: MODEL_BUILT_IN_BTI::set_param_by_index(i,value,offset);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_INF::RCD_name(uint_t i) const{

  uint_t row=0;
  uint_t col=0;
  uint_t pos=0;
  uint_t find=0;
  vector<PARAMETER<double> > mat=matrix;

  while( true ){
    row = pos/WS_COLS;
    col = pos%WS_COLS;
    trace4("MODEL_BUILT_IN_BTI_INF::RCD_name", row, col, pos, mat[pos]) ;

    assert( pos == cols * row + col);
    if(mat[pos]==.0){
      pos++;
      continue;
    }else{
      if(find == i) break;
      find++;
    }
    pos++;
  }
  row = pos/WS_COLS;
  col = pos%WS_COLS;

  stringstream a;
  a << "R_" << row << "_" << col ;
  return a.str();
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_INF::attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const
{
  trace0("MODEL_BUILT_IN_BTI_INF::attach_rcds()");
  trace0(rcd_model_name.string().c_str());
  uint_t row, col;
  uint_t cols = WS_COLS;
  uint_t rows = 10;
  vector<PARAMETER<double> > mat=matrix;

  long double up = pow(10, -7.5);
  double base=10;
  //double mu=1;
  uint_t k=0;
  // k
  // 1 2 3
  // 4 5 6
  // 7 8 9
  long double down = pow(base, 4.5);

  for(row=0; row < rows; row++ ) {
    up = pow(10, -7.5);
    down /= base;
    for(col=0; col < cols; col++ ) {
      up *= base;
      trace2("MODEL_BUILT_IN_BTI_INF::attach_rcds ", row, col); 

      if ( mat[ cols * row + col] == .0 ) continue;

      COMMON_BUILT_IN_RCD* RCD1 = new COMMON_BUILT_IN_RCD;
      RCD1->set_modelname( rcd_model_name ); // <= !
      RCD1->attach(this); // ?
      RCD1->Uref = uref;
      RCD1->Recommon0 = double (up);
      double wt = mat[ cols * row + col];

      RCD1->weight = wt;
      RCD1->Rccommon0 = double(down);
      trace6("MODEL_BUILT_IN_BTI_INF::attach_rcds ", row, col, k, up, down, wt); 
      trace1("MODEL_BUILT_IN_BTI_INF::attach_rcds ", rcd_number); 
      assert( rcd_number > k );

      //stringstream a;
      //a << "RCD_" << i << "_" << j;
      //_RCD[k]->set_label(a.str());

      COMMON_COMPONENT::attach_common(RCD1, (COMMON_COMPONENT**)&(_RCD[k]));
      k++;
    }
  }
  trace2(" done attaching rcds ", rcd_number, k );
  assert( (uint_t) rcd_number == k );
}
/*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_BTI_INF_DISPATCHER { 
  static DEV_BUILT_IN_BTI p3d;
  static MODEL_BUILT_IN_BTI_INF p3(&p3d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d3(&model_dispatcher, "bti_inf|bti_matrix", &p3);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
