/***********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3temp.c
**********/
/* Lmin, Lmax, Wmin, Wmax */

#include "spice.h"
#include <stdio.h>
#include <math.h>
#include "smpdefs.h"
#include "cktdefs.h"
#include "bsim3def.h"
#include "util.h"
#include "const.h"
#include "sperror.h"
#include "suffix.h"

#define Kb 1.3806226e-23
#define KboQ 8.617087e-5  /* Kb / q  where q = 1.60219e-19 */
#define EPSOX 3.453133e-11
#define EPSSI 1.03594e-10
#define PI 3.141592654
#define MAX_EXP 5.834617425e14
#define MIN_EXP 1.713908431e-15
#define EXP_THRESHOLD 34.0
#define Charge_q 1.60219e-19


/* ARGSUSED */
int
BSIM3temp(inModel,ckt)
GENmodel *inModel;
CKTcircuit *ckt;
{
register BSIM3model *model = (BSIM3model*) inModel;
register BSIM3instance *here;
struct bsim3SizeDependParam *pSizeDependParamKnot, *pLastKnot, *pParam;
double tmp, tmp1, tmp2, Eg, ni, T0, T1, T2, T3, T4, T5, Ldrn, Wdrn;
double Temp, TRatio, Inv_L, Inv_W, Inv_LW, Dw, Dl, Vtm0, Tnom;
int Size_Not_Found;

    /*  loop through all the BSIM3 device models */
    for (; model != NULL; model = model->BSIM3nextModel)
    {    Temp = ckt->CKTtemp;
         if (model->BSIM3bulkJctPotential < 0.1)  
	     model->BSIM3bulkJctPotential = 0.1;
         if (model->BSIM3sidewallJctPotential < 0.1)
             model->BSIM3sidewallJctPotential = 0.1;
         model->pSizeDependParamKnot = NULL;
	 pLastKnot = NULL;

	 Tnom = model->BSIM3tnom;
	 TRatio = Temp / Tnom;
         /* loop through all the instances of the model */
         for (here = model->BSIM3instances; here != NULL;
              here=here->BSIM3nextInstance) 
	 {    pSizeDependParamKnot = model->pSizeDependParamKnot;
	      Size_Not_Found = 1;
	      while ((pSizeDependParamKnot != NULL) && Size_Not_Found)
	      {   if ((here->BSIM3l == pSizeDependParamKnot->Length)
		      && (here->BSIM3w == pSizeDependParamKnot->Width))
                  {   Size_Not_Found = 0;
		      here->pParam = pSizeDependParamKnot;
		  }
		  else
		  {   pLastKnot = pSizeDependParamKnot;
		      pSizeDependParamKnot = pSizeDependParamKnot->pNext;
		  }
              }

	      if (Size_Not_Found)
	      {   pParam = (struct bsim3SizeDependParam *)malloc(
	                    sizeof(struct bsim3SizeDependParam));
                  if (pLastKnot == NULL)
		      model->pSizeDependParamKnot = pParam;
                  else
		      pLastKnot->pNext = pParam;
                  pParam->pNext = NULL;
                  here->pParam = pParam;

		     Ldrn = here->BSIM3l;
		     Wdrn = here->BSIM3w;
		  
                  T0 = pow(Ldrn, model->BSIM3Lln);
                  T1 = pow(Wdrn, model->BSIM3Lwn);
                  tmp1 = model->BSIM3Ll / T0 + model->BSIM3Lw / T1
                       + model->BSIM3Lwl / (T0 * T1);
                  pParam->BSIM3dl = model->BSIM3Lint + tmp1;
                  pParam->BSIM3dlc = model->BSIM3dlc + tmp1;

                  T2 = pow(Ldrn, model->BSIM3Wln);
                  T3 = pow(Wdrn, model->BSIM3Wwn);
                  tmp2 = model->BSIM3Wl / T2 + model->BSIM3Ww / T3
                       + model->BSIM3Wwl / (T2 * T3);
                  pParam->BSIM3dw = model->BSIM3Wint + tmp2;
                  pParam->BSIM3dwc = model->BSIM3dwc + tmp2;

                  pParam->BSIM3leff = here->BSIM3l - 2.0 * pParam->BSIM3dl;
                  if (pParam->BSIM3leff <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->BSIM3modName;
                      namarray[1] = here->BSIM3name;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "BSIM3: mosfet %s, model %s: Effective channel length <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->BSIM3weff = here->BSIM3w - 2.0 * pParam->BSIM3dw;
                  if (pParam->BSIM3weff <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->BSIM3modName;
                      namarray[1] = here->BSIM3name;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "BSIM3: mosfet %s, model %s: Effective channel width <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->BSIM3leffCV = here->BSIM3l - 2.0 * pParam->BSIM3dlc;
                  if (pParam->BSIM3leffCV <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->BSIM3modName;
                      namarray[1] = here->BSIM3name;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "BSIM3: mosfet %s, model %s: Effective channel length for C-V <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->BSIM3weffCV = here->BSIM3w - 2.0 * pParam->BSIM3dwc;
                  if (pParam->BSIM3weffCV <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->BSIM3modName;
                      namarray[1] = here->BSIM3name;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "BSIM3: mosfet %s, model %s: Effective channel width for C-V <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

	          model->BSIM3vcrit = CONSTvt0 * log(CONSTvt0
				    / (CONSTroot2 * 1.0e-14));
                  model->BSIM3factor1 = sqrt(EPSSI / EPSOX * model->BSIM3tox);


		  if (model->BSIM3binUnit == 1)
		  {   Inv_L = 1.0e-6 / pParam->BSIM3leff;
		      Inv_W = 1.0e-6 / pParam->BSIM3weff;
		      Inv_LW = 1.0e-12 / (pParam->BSIM3leff
			     * pParam->BSIM3weff);
		  }
		  else
		  {   Inv_L = 1.0 / pParam->BSIM3leff;
		      Inv_W = 1.0 / pParam->BSIM3weff;
		      Inv_LW = 1.0 / (pParam->BSIM3leff
			     * pParam->BSIM3weff);
		  }
		  pParam->BSIM3cdsc = model->BSIM3cdsc
				    + model->BSIM3lcdsc * Inv_L
				    + model->BSIM3wcdsc * Inv_W
				    + model->BSIM3pcdsc * Inv_LW;
		  pParam->BSIM3cdscb = model->BSIM3cdscb
				     + model->BSIM3lcdscb * Inv_L
				     + model->BSIM3wcdscb * Inv_W
				     + model->BSIM3pcdscb * Inv_LW; 
				     
    		  pParam->BSIM3cdscd = model->BSIM3cdscd
				     + model->BSIM3lcdscd * Inv_L
				     + model->BSIM3wcdscd * Inv_W
				     + model->BSIM3pcdscd * Inv_LW; 
				     
		  pParam->BSIM3cit = model->BSIM3cit
				   + model->BSIM3lcit * Inv_L
				   + model->BSIM3wcit * Inv_W
				   + model->BSIM3pcit * Inv_LW;
		  pParam->BSIM3nfactor = model->BSIM3nfactor
				       + model->BSIM3lnfactor * Inv_L
				       + model->BSIM3wnfactor * Inv_W
				       + model->BSIM3pnfactor * Inv_LW;
		  pParam->BSIM3xj = model->BSIM3xj
				  + model->BSIM3lxj * Inv_L
				  + model->BSIM3wxj * Inv_W
				  + model->BSIM3pxj * Inv_LW;
		  pParam->BSIM3vsat = model->BSIM3vsat
				    + model->BSIM3lvsat * Inv_L
				    + model->BSIM3wvsat * Inv_W
				    + model->BSIM3pvsat * Inv_LW;
		  pParam->BSIM3at = model->BSIM3at
				  + model->BSIM3lat * Inv_L
				  + model->BSIM3wat * Inv_W
				  + model->BSIM3pat * Inv_LW;
		  pParam->BSIM3a0 = model->BSIM3a0
				  + model->BSIM3la0 * Inv_L
				  + model->BSIM3wa0 * Inv_W
				  + model->BSIM3pa0 * Inv_LW; 
				  
		  pParam->BSIM3ags = model->BSIM3ags
				  + model->BSIM3lags * Inv_L
				  + model->BSIM3wags * Inv_W
				  + model->BSIM3pags * Inv_LW;
				  
		  pParam->BSIM3a1 = model->BSIM3a1
				  + model->BSIM3la1 * Inv_L
				  + model->BSIM3wa1 * Inv_W
				  + model->BSIM3pa1 * Inv_LW;
		  pParam->BSIM3a2 = model->BSIM3a2
				  + model->BSIM3la2 * Inv_L
				  + model->BSIM3wa2 * Inv_W
				  + model->BSIM3pa2 * Inv_LW;
		  pParam->BSIM3keta = model->BSIM3keta
				    + model->BSIM3lketa * Inv_L
				    + model->BSIM3wketa * Inv_W
				    + model->BSIM3pketa * Inv_LW;
		  pParam->BSIM3nsub = model->BSIM3nsub
				    + model->BSIM3lnsub * Inv_L
				    + model->BSIM3wnsub * Inv_W
				    + model->BSIM3pnsub * Inv_LW;
		  pParam->BSIM3npeak = model->BSIM3npeak
				     + model->BSIM3lnpeak * Inv_L
				     + model->BSIM3wnpeak * Inv_W
				     + model->BSIM3pnpeak * Inv_LW;
		  pParam->BSIM3ngate = model->BSIM3ngate
				     + model->BSIM3lngate * Inv_L
				     + model->BSIM3wngate * Inv_W
				     + model->BSIM3pngate * Inv_LW;
		  pParam->BSIM3gamma1 = model->BSIM3gamma1
				      + model->BSIM3lgamma1 * Inv_L
				      + model->BSIM3wgamma1 * Inv_W
				      + model->BSIM3pgamma1 * Inv_LW;
		  pParam->BSIM3gamma2 = model->BSIM3gamma2
				      + model->BSIM3lgamma2 * Inv_L
				      + model->BSIM3wgamma2 * Inv_W
				      + model->BSIM3pgamma2 * Inv_LW;
		  pParam->BSIM3vbx = model->BSIM3vbx
				   + model->BSIM3lvbx * Inv_L
				   + model->BSIM3wvbx * Inv_W
				   + model->BSIM3pvbx * Inv_LW;
		  pParam->BSIM3vbm = model->BSIM3vbm
				   + model->BSIM3lvbm * Inv_L
				   + model->BSIM3wvbm * Inv_W
				   + model->BSIM3pvbm * Inv_LW;
		  pParam->BSIM3xt = model->BSIM3xt
				   + model->BSIM3lxt * Inv_L
				   + model->BSIM3wxt * Inv_W
				   + model->BSIM3pxt * Inv_LW;
		  pParam->BSIM3k1 = model->BSIM3k1
				  + model->BSIM3lk1 * Inv_L
				  + model->BSIM3wk1 * Inv_W
				  + model->BSIM3pk1 * Inv_LW;
		  pParam->BSIM3kt1 = model->BSIM3kt1
				   + model->BSIM3lkt1 * Inv_L
				   + model->BSIM3wkt1 * Inv_W
				   + model->BSIM3pkt1 * Inv_LW;
		  pParam->BSIM3kt1l = model->BSIM3kt1l
				    + model->BSIM3lkt1l * Inv_L
				    + model->BSIM3wkt1l * Inv_W
				    + model->BSIM3pkt1l * Inv_LW;
		  pParam->BSIM3k2 = model->BSIM3k2
				  + model->BSIM3lk2 * Inv_L
				  + model->BSIM3wk2 * Inv_W
				  + model->BSIM3pk2 * Inv_LW;
		  pParam->BSIM3kt2 = model->BSIM3kt2
				   + model->BSIM3lkt2 * Inv_L
				   + model->BSIM3wkt2 * Inv_W
				   + model->BSIM3pkt2 * Inv_LW;
		  pParam->BSIM3k3 = model->BSIM3k3
				  + model->BSIM3lk3 * Inv_L
				  + model->BSIM3wk3 * Inv_W
				  + model->BSIM3pk3 * Inv_LW;
		  pParam->BSIM3k3b = model->BSIM3k3b
				   + model->BSIM3lk3b * Inv_L
				   + model->BSIM3wk3b * Inv_W
				   + model->BSIM3pk3b * Inv_LW;
		  pParam->BSIM3w0 = model->BSIM3w0
				  + model->BSIM3lw0 * Inv_L
				  + model->BSIM3ww0 * Inv_W
				  + model->BSIM3pw0 * Inv_LW;
		  pParam->BSIM3nlx = model->BSIM3nlx
				   + model->BSIM3lnlx * Inv_L
				   + model->BSIM3wnlx * Inv_W
				   + model->BSIM3pnlx * Inv_LW;
		  pParam->BSIM3dvt0 = model->BSIM3dvt0
				    + model->BSIM3ldvt0 * Inv_L
				    + model->BSIM3wdvt0 * Inv_W
				    + model->BSIM3pdvt0 * Inv_LW;
		  pParam->BSIM3dvt1 = model->BSIM3dvt1
				    + model->BSIM3ldvt1 * Inv_L
				    + model->BSIM3wdvt1 * Inv_W
				    + model->BSIM3pdvt1 * Inv_LW;
		  pParam->BSIM3dvt2 = model->BSIM3dvt2
				    + model->BSIM3ldvt2 * Inv_L
				    + model->BSIM3wdvt2 * Inv_W
				    + model->BSIM3pdvt2 * Inv_LW;
		  pParam->BSIM3dvt0w = model->BSIM3dvt0w
				    + model->BSIM3ldvt0w * Inv_L
				    + model->BSIM3wdvt0w * Inv_W
				    + model->BSIM3pdvt0w * Inv_LW;
		  pParam->BSIM3dvt1w = model->BSIM3dvt1w
				    + model->BSIM3ldvt1w * Inv_L
				    + model->BSIM3wdvt1w * Inv_W
				    + model->BSIM3pdvt1w * Inv_LW;
		  pParam->BSIM3dvt2w = model->BSIM3dvt2w
				    + model->BSIM3ldvt2w * Inv_L
				    + model->BSIM3wdvt2w * Inv_W
				    + model->BSIM3pdvt2w * Inv_LW;
		  pParam->BSIM3drout = model->BSIM3drout
				     + model->BSIM3ldrout * Inv_L
				     + model->BSIM3wdrout * Inv_W
				     + model->BSIM3pdrout * Inv_LW;
		  pParam->BSIM3dsub = model->BSIM3dsub
				    + model->BSIM3ldsub * Inv_L
				    + model->BSIM3wdsub * Inv_W
				    + model->BSIM3pdsub * Inv_LW;
		  pParam->BSIM3vth0 = model->BSIM3vth0
				    + model->BSIM3lvth0 * Inv_L
				    + model->BSIM3wvth0 * Inv_W
				    + model->BSIM3pvth0 * Inv_LW;
		  pParam->BSIM3ua = model->BSIM3ua
				  + model->BSIM3lua * Inv_L
				  + model->BSIM3wua * Inv_W
				  + model->BSIM3pua * Inv_LW;
		  pParam->BSIM3ua1 = model->BSIM3ua1
				   + model->BSIM3lua1 * Inv_L
				   + model->BSIM3wua1 * Inv_W
				   + model->BSIM3pua1 * Inv_LW;
		  pParam->BSIM3ub = model->BSIM3ub
				  + model->BSIM3lub * Inv_L
				  + model->BSIM3wub * Inv_W
				  + model->BSIM3pub * Inv_LW;
		  pParam->BSIM3ub1 = model->BSIM3ub1
				   + model->BSIM3lub1 * Inv_L
				   + model->BSIM3wub1 * Inv_W
				   + model->BSIM3pub1 * Inv_LW;
		  pParam->BSIM3uc = model->BSIM3uc
				  + model->BSIM3luc * Inv_L
				  + model->BSIM3wuc * Inv_W
				  + model->BSIM3puc * Inv_LW;
		  pParam->BSIM3uc1 = model->BSIM3uc1
				   + model->BSIM3luc1 * Inv_L
				   + model->BSIM3wuc1 * Inv_W
				   + model->BSIM3puc1 * Inv_LW;
		  pParam->BSIM3u0 = model->BSIM3u0
				  + model->BSIM3lu0 * Inv_L
				  + model->BSIM3wu0 * Inv_W
				  + model->BSIM3pu0 * Inv_LW;
		  pParam->BSIM3ute = model->BSIM3ute
				   + model->BSIM3lute * Inv_L
				   + model->BSIM3wute * Inv_W
				   + model->BSIM3pute * Inv_LW;
		  pParam->BSIM3voff = model->BSIM3voff
				    + model->BSIM3lvoff * Inv_L
				    + model->BSIM3wvoff * Inv_W
				    + model->BSIM3pvoff * Inv_LW;
		  pParam->BSIM3delta = model->BSIM3delta
				     + model->BSIM3ldelta * Inv_L
				     + model->BSIM3wdelta * Inv_W
				     + model->BSIM3pdelta * Inv_LW;
		  pParam->BSIM3rdsw = model->BSIM3rdsw
				    + model->BSIM3lrdsw * Inv_L
				    + model->BSIM3wrdsw * Inv_W
				    + model->BSIM3prdsw * Inv_LW;
		  pParam->BSIM3prwg = model->BSIM3prwg
				    + model->BSIM3lprwg * Inv_L
				    + model->BSIM3wprwg * Inv_W
				    + model->BSIM3pprwg * Inv_LW;
		  pParam->BSIM3prwb = model->BSIM3prwb
				    + model->BSIM3lprwb * Inv_L
				    + model->BSIM3wprwb * Inv_W
				    + model->BSIM3pprwb * Inv_LW;
		  pParam->BSIM3prt = model->BSIM3prt
				    + model->BSIM3lprt * Inv_L
				    + model->BSIM3wprt * Inv_W
				    + model->BSIM3pprt * Inv_LW;
		  pParam->BSIM3eta0 = model->BSIM3eta0
				    + model->BSIM3leta0 * Inv_L
				    + model->BSIM3weta0 * Inv_W
				    + model->BSIM3peta0 * Inv_LW;
		  pParam->BSIM3etab = model->BSIM3etab
				    + model->BSIM3letab * Inv_L
				    + model->BSIM3wetab * Inv_W
				    + model->BSIM3petab * Inv_LW;
		  pParam->BSIM3pclm = model->BSIM3pclm
				    + model->BSIM3lpclm * Inv_L
				    + model->BSIM3wpclm * Inv_W
				    + model->BSIM3ppclm * Inv_LW;
		  pParam->BSIM3pdibl1 = model->BSIM3pdibl1
				      + model->BSIM3lpdibl1 * Inv_L
				      + model->BSIM3wpdibl1 * Inv_W
				      + model->BSIM3ppdibl1 * Inv_LW;
		  pParam->BSIM3pdibl2 = model->BSIM3pdibl2
				      + model->BSIM3lpdibl2 * Inv_L
				      + model->BSIM3wpdibl2 * Inv_W
				      + model->BSIM3ppdibl2 * Inv_LW;
		  pParam->BSIM3pdiblb = model->BSIM3pdiblb
				      + model->BSIM3lpdiblb * Inv_L
				      + model->BSIM3wpdiblb * Inv_W
				      + model->BSIM3ppdiblb * Inv_LW;
		  pParam->BSIM3pscbe1 = model->BSIM3pscbe1
				      + model->BSIM3lpscbe1 * Inv_L
				      + model->BSIM3wpscbe1 * Inv_W
				      + model->BSIM3ppscbe1 * Inv_LW;
		  pParam->BSIM3pscbe2 = model->BSIM3pscbe2
				      + model->BSIM3lpscbe2 * Inv_L
				      + model->BSIM3wpscbe2 * Inv_W
				      + model->BSIM3ppscbe2 * Inv_LW;
		  pParam->BSIM3pvag = model->BSIM3pvag
				    + model->BSIM3lpvag * Inv_L
				    + model->BSIM3wpvag * Inv_W
				    + model->BSIM3ppvag * Inv_LW;
		  pParam->BSIM3wr = model->BSIM3wr
				  + model->BSIM3lwr * Inv_L
				  + model->BSIM3wwr * Inv_W
				  + model->BSIM3pwr * Inv_LW;
		  pParam->BSIM3dwg = model->BSIM3dwg
				   + model->BSIM3ldwg * Inv_L
				   + model->BSIM3wdwg * Inv_W
				   + model->BSIM3pdwg * Inv_LW;
		  pParam->BSIM3dwb = model->BSIM3dwb
				   + model->BSIM3ldwb * Inv_L
				   + model->BSIM3wdwb * Inv_W
				   + model->BSIM3pdwb * Inv_LW;
		  pParam->BSIM3b0 = model->BSIM3b0
				  + model->BSIM3lb0 * Inv_L
				  + model->BSIM3wb0 * Inv_W
				  + model->BSIM3pb0 * Inv_LW;
		  pParam->BSIM3b1 = model->BSIM3b1
				  + model->BSIM3lb1 * Inv_L
				  + model->BSIM3wb1 * Inv_W
				  + model->BSIM3pb1 * Inv_LW;
		  pParam->BSIM3alpha0 = model->BSIM3alpha0
				      + model->BSIM3lalpha0 * Inv_L
				      + model->BSIM3walpha0 * Inv_W
				      + model->BSIM3palpha0 * Inv_LW;
		  pParam->BSIM3beta0 = model->BSIM3beta0
				     + model->BSIM3lbeta0 * Inv_L
				     + model->BSIM3wbeta0 * Inv_W
				     + model->BSIM3pbeta0 * Inv_LW;
		  /* CV model */
		  pParam->BSIM3elm = model->BSIM3elm
				  + model->BSIM3lelm * Inv_L
				  + model->BSIM3welm * Inv_W
				  + model->BSIM3pelm * Inv_LW;
		  pParam->BSIM3cgsl = model->BSIM3cgsl
				    + model->BSIM3lcgsl * Inv_L
				    + model->BSIM3wcgsl * Inv_W
				    + model->BSIM3pcgsl * Inv_LW;
		  pParam->BSIM3cgdl = model->BSIM3cgdl
				    + model->BSIM3lcgdl * Inv_L
				    + model->BSIM3wcgdl * Inv_W
				    + model->BSIM3pcgdl * Inv_LW;
		  pParam->BSIM3ckappa = model->BSIM3ckappa
				      + model->BSIM3lckappa * Inv_L
				      + model->BSIM3wckappa * Inv_W
				      + model->BSIM3pckappa * Inv_LW;
		  pParam->BSIM3cf = model->BSIM3cf
				  + model->BSIM3lcf * Inv_L
				  + model->BSIM3wcf * Inv_W
				  + model->BSIM3pcf * Inv_LW;
		  pParam->BSIM3clc = model->BSIM3clc
				   + model->BSIM3lclc * Inv_L
				   + model->BSIM3wclc * Inv_W
				   + model->BSIM3pclc * Inv_LW;
		  pParam->BSIM3cle = model->BSIM3cle
				   + model->BSIM3lcle * Inv_L
				   + model->BSIM3wcle * Inv_W
				   + model->BSIM3pcle * Inv_LW;
                  pParam->BSIM3abulkCVfactor = 1.0 + pow((pParam->BSIM3clc
					     / pParam->BSIM3leff),
					     pParam->BSIM3cle);

                  pParam->BSIM3cgdo = (model->BSIM3cgdo + pParam->BSIM3cf)
				    * pParam->BSIM3weffCV;
                  pParam->BSIM3cgso = (model->BSIM3cgso + pParam->BSIM3cf)
				    * pParam->BSIM3weffCV;
                  pParam->BSIM3cgbo = model->BSIM3cgbo * pParam->BSIM3leffCV;

	          T0 = (TRatio - 1.0);
	          pParam->BSIM3ua = pParam->BSIM3ua + pParam->BSIM3ua1 * T0;
	          pParam->BSIM3ub = pParam->BSIM3ub + pParam->BSIM3ub1 * T0;
	          pParam->BSIM3uc = pParam->BSIM3uc + pParam->BSIM3uc1 * T0;

                  pParam->BSIM3u0temp = pParam->BSIM3u0
				      * pow(TRatio, pParam->BSIM3ute); 
                  pParam->BSIM3vsattemp = pParam->BSIM3vsat - pParam->BSIM3at 
			                * T0;
	          pParam->BSIM3rds0 = (pParam->BSIM3rdsw + pParam->BSIM3prt * T0)
                                    / pow(pParam->BSIM3weff * 1E6, pParam->BSIM3wr);

                  if (!model->BSIM3npeakGiven && model->BSIM3gamma1Given)
                  {   T0 = pParam->BSIM3gamma1 * model->BSIM3cox;
                      pParam->BSIM3npeak = 3.021E22 * T0 * T0;
                  }

	          Vtm0 = KboQ * Tnom;
	          Eg = 1.16 - 7.02e-4 * Tnom * Tnom / (Tnom + 1108.0);
	          ni = 1.45e10 * (Tnom / 300.15) * sqrt(Tnom / 300.15) 
                     * exp(21.5565981 - Eg / (2.0 * Vtm0));

		  pParam->BSIM3phi = 2.0 * Vtm0 
			           * log(pParam->BSIM3npeak / ni);

	          pParam->BSIM3sqrtPhi = sqrt(pParam->BSIM3phi);
	          pParam->BSIM3phis3 = pParam->BSIM3sqrtPhi * pParam->BSIM3phi;

                  pParam->BSIM3Xdep0 = sqrt(2.0 * EPSSI / (Charge_q
				     * pParam->BSIM3npeak * 1.0e6))
                                     * pParam->BSIM3sqrtPhi; 
                  pParam->BSIM3sqrtXdep0 = sqrt(pParam->BSIM3Xdep0);
                  pParam->BSIM3litl = sqrt(3.0 * pParam->BSIM3xj
				    * model->BSIM3tox);
                  pParam->BSIM3vbi = Vtm0 * log(1.0e20
			           * pParam->BSIM3npeak / (ni * ni));
                  pParam->BSIM3cdep0 = sqrt(Charge_q * EPSSI
				     * pParam->BSIM3npeak * 1.0e6 / 2.0
				     / pParam->BSIM3phi);
        
                  if (model->BSIM3k1Given || model->BSIM3k2Given)
	          {   if (!model->BSIM3k1Given)
	              {   fprintf(stdout, "Warning: k1 should be specified with k2.\n");
                          pParam->BSIM3k1 = 0.53;
                      }
                      if (!model->BSIM3k2Given)
	              {   fprintf(stdout, "Warning: k2 should be specified with k1.\n");
                          pParam->BSIM3k2 = -0.0186;
                      }
                      if (model->BSIM3nsubGiven)
                          fprintf(stdout, "Warning: nsub is ignored because k1 or k2 is given.\n");
                      if (model->BSIM3xtGiven)
                          fprintf(stdout, "Warning: xt is ignored because k1 or k2 is given.\n");
                      if (model->BSIM3vbxGiven)
                          fprintf(stdout, "Warning: vbx is ignored because k1 or k2 is given.\n");
                      if (model->BSIM3vbmGiven)
                          fprintf(stdout, "Warning: vbm is ignored because k1 or k2 is given.\n");
                      if (model->BSIM3gamma1Given)
                          fprintf(stdout, "Warning: gamma1 is ignored because k1 or k2 is given.\n");
                      if (model->BSIM3gamma2Given)
                          fprintf(stdout, "Warning: gamma2 is ignored because k1 or k2 is given.\n");
                  }
                  else
	          {   if (!model->BSIM3vbxGiven)
                          pParam->BSIM3vbx = pParam->BSIM3phi - 7.7348e-4 
                                           * pParam->BSIM3npeak
					   * pParam->BSIM3xt * pParam->BSIM3xt;
	              if (pParam->BSIM3vbx > 0.0)
		          pParam->BSIM3vbx = -pParam->BSIM3vbx;
	              if (pParam->BSIM3vbm > 0.0)
                          pParam->BSIM3vbm = -pParam->BSIM3vbm;
           
                      if (!model->BSIM3gamma1Given)
                          pParam->BSIM3gamma1 = 5.753e-12
					      * sqrt(pParam->BSIM3npeak)
                                              / model->BSIM3cox;
                      if (!model->BSIM3gamma2Given)
                          pParam->BSIM3gamma2 = 5.753e-12
					      * sqrt(pParam->BSIM3nsub)
                                              / model->BSIM3cox;

                      T0 = pParam->BSIM3gamma1 - pParam->BSIM3gamma2;
                      T1 = sqrt(pParam->BSIM3phi - pParam->BSIM3vbx)
			 - pParam->BSIM3sqrtPhi;
                      T2 = sqrt(pParam->BSIM3phi * (pParam->BSIM3phi
			 - pParam->BSIM3vbm)) - pParam->BSIM3phi;
                      pParam->BSIM3k2 = T0 * T1 / (2.0 * T2 + pParam->BSIM3vbm);
                      pParam->BSIM3k1 = pParam->BSIM3gamma2 - 2.0
				      * pParam->BSIM3k2 * sqrt(pParam->BSIM3phi
				      - pParam->BSIM3vbm);
                  }
 
		  if (pParam->BSIM3k2 > 0.0)
		  {   T0 = 0.5 * pParam->BSIM3k1 / pParam->BSIM3k2;
                      pParam->BSIM3vbsc = 0.9 * (pParam->BSIM3phi - T0 * T0);
		      if (pParam->BSIM3vbsc > -3.0)
		          pParam->BSIM3vbsc = -3.0;
		      else if (pParam->BSIM3vbsc < -30.0)
		          pParam->BSIM3vbsc = -30.0;
		  }
		  else
		  {   pParam->BSIM3vbsc = -10.0;
		  }

	          model->BSIM3vtm = KboQ * Temp;

	          if (model->BSIM3vth0Given)
                      pParam->BSIM3vfb = model->BSIM3type * pParam->BSIM3vth0 
                                       - pParam->BSIM3phi - pParam->BSIM3k1 
                                       * pParam->BSIM3sqrtPhi;
                  else
                      pParam->BSIM3vth0 = model->BSIM3type * (-1.0
                                        + pParam->BSIM3phi + pParam->BSIM3k1 
                                        * pParam->BSIM3sqrtPhi);

                  T1 = sqrt(EPSSI / EPSOX * model->BSIM3tox
		     * pParam->BSIM3Xdep0);
                  T0 = exp(-0.5 * pParam->BSIM3dsub * pParam->BSIM3leff / T1);
                  pParam->BSIM3theta0vb0 = (T0 + 2.0 * T0 * T0);

                  T0 = exp(-0.5 * pParam->BSIM3drout * pParam->BSIM3leff / T1);
                  T2 = (T0 + 2.0 * T0 * T0);
                  pParam->BSIM3thetaRout = pParam->BSIM3pdibl1 * T2
				         + pParam->BSIM3pdibl2;

                  /* process source/drain series resistance */
                  here->BSIM3drainConductance = model->BSIM3sheetResistance 
		                              * here->BSIM3drainSquares;
                  if (here->BSIM3drainConductance > 0.0)
                      here->BSIM3drainConductance = 1.0
						  / here->BSIM3drainConductance;
	          else
                      here->BSIM3drainConductance = 0.0;
                   
                  here->BSIM3sourceConductance = model->BSIM3sheetResistance 
		                               * here->BSIM3sourceSquares;
                  if (here->BSIM3sourceConductance > 0.0) 
                      here->BSIM3sourceConductance = 1.0
						   / here->BSIM3sourceConductance;
	          else
                      here->BSIM3sourceConductance = 0.0;
	      }
	      here->BSIM3cgso = pParam->BSIM3cgso;
	      here->BSIM3cgdo = pParam->BSIM3cgdo;
         }
    }
    return(OK);
}




