/* $Id: d_bti.cc,v 1.7 2010-09-07 07:46:19 felix Exp $ -*- C++ -*-
vim:ts=8:sw=2:et:
*/


/* This file is no longer automatically generated. */

#include "d_bti_inf.h"

static double MATRIX_MANY[WS_ROWS][WS_COLS] =
// wrong order   ^^       ^^ due to C++
//  v R_0_0                                      v R_0_9
{ { .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 },
  { .0 , .0 , .1 , .1 , .32, .32, .32, .37, .32, .01, .0 },
  { .0 , .1 , .1 , .1 ,  .1, .26, .32, .32, .20, .14, .0 },
  { .0 , .1 , .29, .29, .32, .47, .46, .25, .10, .10, .0 },
  { .0 , .29, .32, .37, .47, .32, .31, .31, .13, .10, .0 },
  { .0 , .21, .26, .37, .48, .32, .14, .13, .10, .0 , .0 },
  { .0 , .32, .40, .26, .27, .19, .11, .10, .0 , .0 , .0 },
  { .0 , .4 , .32, .25, .25, .1 , .1 , .0 , .0 , .0 , .0 },
  { .0 , .26, .37, .26, .1 , .1 , .0 , .0 , .0 , .0 , .0 },
  { .0 , .11, .1 , .0 , .0 , .0 ,  0 , .0 , .0 , .0 , .0 } };
//  ^ R_9_0

static int COUNT_MANY = 63;

static double MATRIX_FAST[WS_ROWS][WS_COLS] =
// wrong order   ^^       ^^ due to C++
//  v R_0_0                                      v R_0_9
{ { .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 },
  { .0 , .0 , .1 , .1 , .32, .32, .32, .37, .32, .01, .0 },
  { .0 , .1 , .1 , .1 ,  .1, .26, .32, .32, .20, .14, .0 },
  { .0 , .1 , .29, .29, .32, .47, .46, .25, .10, .10, .0 },
  { .0 , .29, .32, .37, .47, .32, .31, .31, .13, .10, .0 },
  { .0 , .21, .26, .37, .48, .32, .14, .13, .10, .0 , .0 },
  { .0 , .32, .40, .26, .27, .19, .11, .10, .0 , .0 , .0 },
  { .0 , .4 , .32, .25, .25, .1 , .1 , .0 , .0 , .0 , .0 },
  { .0 , .0 , .37, .26, .1 , .1 , .0 , .0 , .0 , .0 , .0 },
  { .0 , .0 , .0 , .0 , .0 , .0 ,  0 , .0 , .0 , .0 , .0 } };
//  ^ R_9_0

static int COUNT_FAST = 60;

static double MATRIX_FEW[WS_ROWS][WS_COLS] =
// wrong order   ^^       ^^ due to C++
{ { .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 , .0 },  //<= langsam
  { .0 , .0 , .0 , .00, .50, .00, .00, .00, .00, .00, .0 },
  { .0 , .00, .4 , .00, .00, .00, .00, .00, .00, .30, .0 },
  { .0 , .00, .00, .00, .00, .00, .76, .00, .00, .00, .0 },
  { .0 , .00, .00, .00, .90, .00, .00, .00, .00, .00, .0 },
  { .0 , .00, .00, .00, .00, .00, .00, .00, .00, .0 , .0 },
  { .0 , .00, .80, .00, .00, .00, .00, .00, .0 , .0 , .0 },
  { .0 , .00, .00, .00, .00, .00, .0 , .0 , .0 , .0 , .0 },
  { .0 , .00, .00, .00, .0 , .00, .0 , .0 , .0 , .0 , .0 },
  { .0 , .21, .00, .0 , .0 , .0 ,  0 , .0 , .0 , .0 , .0 } };  //<=schnell
//  ^^schnell                                 langsam ^^

static int COUNT_FEW=7;


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_INF::MODEL_BUILT_IN_BTI_INF(const
    MODEL_BUILT_IN_BTI_INF& p)
  :MODEL_BUILT_IN_BTI(p),
  matrix_number(p.matrix_number)
{
  trace0("MODEL_BUILT_IN_BTI_INF::MODEL_BUILT_IN_BTI_INF");

  _w_matrix=p._w_matrix;
  rcd_number=p.rcd_number;
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_INF::precalc_first()
{
  const CARD_LIST* par_scope = scope();
  assert(par_scope);
  MODEL_BUILT_IN_BTI::precalc_first();

  trace0("MODEL_BUILT_IN_BTI::precalc_first");

  e_val(&(this->matrix_number), 0, par_scope);


  std::cout << "* " <<  matrix_number << "\n";
  switch(matrix_number){
    case 0:
      rcd_number = COUNT_MANY;
      _w_matrix = MATRIX_MANY;
      break;
    case 1:
      rcd_number = COUNT_FEW;
      _w_matrix = MATRIX_FEW;
      break;
    case 2:
      rcd_number = COUNT_FAST;
      _w_matrix = MATRIX_FAST;
      break;
  }

}
/*--------------------------------------------------------------------------*/
MODEL_BUILT_IN_BTI_INF::MODEL_BUILT_IN_BTI_INF(const BASE_SUBCKT* p)
  :MODEL_BUILT_IN_BTI(p),
  matrix_number(0)
{
  _w_matrix=(0);
}
/*--------------------------------------------------------------------------*/
int MODEL_BUILT_IN_BTI_INF::param_count()const{return MODEL_BUILT_IN_BTI::param_count() + 1;}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_INF::param_value(int i)const
{
  switch (param_count() - 1 - i) {
    case 0: return "...";
    case 1: return to_string(matrix_number);
  }
  return MODEL_BUILT_IN_BTI::param_value(i);
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_INF::param_name(int i)const
{
  switch (param_count() - 1 - i ){
        case 0: return "======";
        case 1: return "mn";
  }
  return MODEL_BUILT_IN_BTI::param_name(i );

}
/*--------------------------------------------------------------------------*/
bool MODEL_BUILT_IN_BTI_INF::param_is_printable(int i)const
{
  switch (param_count() - 1 - i) {
  case 0:  return (false);
  case 1:  return (true);
  default: return MODEL_BUILT_IN_BTI::param_is_printable( i );
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_INF::set_param_by_index(int i, std::string& value, int offset)
{
  switch (param_count() - 1 - i) {
  case 0: untested(); break;
  case 1: matrix_number = value; break;
  default: MODEL_BUILT_IN_BTI::set_param_by_index(i,value,offset);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_BUILT_IN_BTI_INF::RCD_name(int i) const{

  int row=0;
  int col=0;
  int pos=0;
  int find=0;

  while( true  ){
    row = pos/WS_COLS;
    col = pos%WS_COLS;
    if(_w_matrix[row][col]==.0){
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

  trace3("MODEL_BUILT_IN_BTI_INF::RCD_name()", i, row, col);

  stringstream a;
  a << "R_" << row << "_" << col ;
  return a.str();

}
/*--------------------------------------------------------------------------*/
void MODEL_BUILT_IN_BTI_INF::attach_rcds(COMMON_BUILT_IN_RCD** _RCD) const
{
  trace0("MODEL_BUILT_IN_BTI_INF::attach_rcds()");
  trace0(rcd_model_name.string().c_str());
  int row, col;
  int cols = WS_COLS;
  int rows = 10;


  assert(_w_matrix);

  long double up = pow(10, -7.5);
  long double uref = 1;
  // double lambda=1;
  double base=10;
  //double mu=1;
  int k=0;
  trace1("MODEL_BUILT_IN_BTI_INF::attach_rcds",_w_matrix[1][4]); 
  // k
  // 1 2 3
  // 4 5 6
  // 7 8 9
  long double down = pow(base,4.5);

  for(row=0; row < rows; row++ ) {
    up = pow(10, -7.5);
    down /= base;
    for(col=0; col < cols; col++ ) {
      up *= base;
      trace2("MODEL_BUILT_IN_BTI_INF::attach_rcds ", row, col); 
      if ( _w_matrix[row][col] == .0 ) continue;

      COMMON_BUILT_IN_RCD* RCD1 = new COMMON_BUILT_IN_RCD;
      RCD1->set_modelname( rcd_model_name ); // <= !
      RCD1->attach(this); // ?
      RCD1->Uref = double( uref );
      RCD1->Recommon = double (up);
      double wt = _w_matrix[row][col];
      RCD1->weight = wt;
      RCD1->Rccommon0 = double(down);
      trace6("MODEL_BUILT_IN_BTI_INF::attach_rcds ", row, col, k, up, down, wt); 
      trace2("MODEL_BUILT_IN_BTI_INF::attach_rcds ", matrix_number, rcd_number); 

      //double _rr = _rr_.subs(runter=runter, u_gate_=uref)

      assert( rcd_number > k );

      //stringstream a;
      //a << "RCD_" << i << "_" << j;
      //_RCD[k]->set_label(a.str());

      COMMON_COMPONENT::attach_common(RCD1, (COMMON_COMPONENT**)&(_RCD[k]));
      k++;
      
    }

  }
  
  trace2(" done attaching rcds ", rcd_number, k );
  assert( rcd_number == k );

}
/*--------------------------------------------------------------------------*/
namespace MODEL_BUILT_IN_BTI_INF_DISPATCHER { 
  static DEV_BUILT_IN_BTI p3d;
  static MODEL_BUILT_IN_BTI_INF p3(&p3d);
  static DISPATCHER<MODEL_CARD>::INSTALL
    d3(&model_dispatcher, "bti_inf", &p3);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
