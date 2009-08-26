/* $Id: BSIMFD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soitemp.c, FD2.1 $
 * Revision 2.1  99/9/27 Pin Su
 * BSIMFD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soitemp.c, FD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soitemp.c          98/5/01
Modified by Pin Su, Wei Jin 99/9/27
**********/

/* Lmin, Lmax, Wmin, Wmax */

#include "spice.h"
#include <stdio.h>
#include <math.h>
#include "smpdefs.h"
#include "cktdefs.h"
#include "b3soidef.h"
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
B3SOItemp(inModel,ckt)
GENmodel *inModel;
CKTcircuit *ckt;
{
register B3SOImodel *model = (B3SOImodel*) inModel;
register B3SOIinstance *here;
struct b3soiSizeDependParam *pSizeDependParamKnot, *pLastKnot, *pParam;
double tmp, tmp1, tmp2, Eg, Eg0, ni, T0, T1, T2, T3, T4, T5, T6, Ldrn, Wdrn;
double Temp, TRatio, Inv_L, Inv_W, Inv_LW, Dw, Dl, Vtm0, Tnom;
double SDphi, SDgamma;
int Size_Not_Found;

    /*  loop through all the B3SOI device models */
    for (; model != NULL; model = model->B3SOInextModel)
    {    Temp = ckt->CKTtemp;
         if (model->B3SOIGatesidewallJctPotential < 0.1)
             model->B3SOIGatesidewallJctPotential = 0.1;
         model->pSizeDependParamKnot = NULL;
	 pLastKnot = NULL;

	 Tnom = model->B3SOItnom;
	 TRatio = Temp / Tnom;

	 model->B3SOIvcrit = CONSTvt0 * log(CONSTvt0 / (CONSTroot2 * 1.0e-14));
         model->B3SOIfactor1 = sqrt(EPSSI / EPSOX * model->B3SOItox);

         Vtm0 = KboQ * Tnom;
         Eg0 = 1.16 - 7.02e-4 * Tnom * Tnom / (Tnom + 1108.0);
         model->B3SOIeg0 = Eg0; 
         model->B3SOIvtm = KboQ * Temp;

         Eg = 1.16 - 7.02e-4 * Temp * Temp / (Temp + 1108.0);
         /* ni is in cm^-3 */
         ni = 1.45e10 * (Temp / 300.15) * sqrt(Temp / 300.15) 
            * exp(21.5565981 - Eg / (2.0 * model->B3SOIvtm));


         /* loop through all the instances of the model */
	 /* MCJ: Length and Width not initialized */
         for (here = model->B3SOIinstances; here != NULL;
              here = here->B3SOInextInstance) 
	 {    
              here->B3SOIrbodyext = here->B3SOIbodySquares *
                                    model->B3SOIrbsh;
	      pSizeDependParamKnot = model->pSizeDependParamKnot;
              Size_Not_Found = 1;
	      while ((pSizeDependParamKnot != NULL) && Size_Not_Found)
	      {   if ((here->B3SOIl == pSizeDependParamKnot->Length)
		      && (here->B3SOIw == pSizeDependParamKnot->Width)
                      && (here->B3SOIrth0 == pSizeDependParamKnot->Rth0)
                      && (here->B3SOIcth0 == pSizeDependParamKnot->Cth0))
                  {   Size_Not_Found = 0;
		      here->pParam = pSizeDependParamKnot;
		  }
		  else
		  {   pLastKnot = pSizeDependParamKnot;
		      pSizeDependParamKnot = pSizeDependParamKnot->pNext;
		  }
              }

	      if (Size_Not_Found)
	      {   pParam = (struct b3soiSizeDependParam *)malloc(
	                    sizeof(struct b3soiSizeDependParam));
                  if (pLastKnot == NULL)
		      model->pSizeDependParamKnot = pParam;
                  else
		      pLastKnot->pNext = pParam;
                  pParam->pNext = NULL;
                  here->pParam = pParam;

		  Ldrn = here->B3SOIl;
		  Wdrn = here->B3SOIw;
                  pParam->Length = Ldrn;
                  pParam->Width = Wdrn;
                  pParam->Rth0 = here->B3SOIrth0;
                  pParam->Cth0 = here->B3SOIcth0;
		  
                  T0 = pow(Ldrn, model->B3SOILln);
                  T1 = pow(Wdrn, model->B3SOILwn);
                  tmp1 = model->B3SOILl / T0 + model->B3SOILw / T1
                       + model->B3SOILwl / (T0 * T1);
                  pParam->B3SOIdl = model->B3SOILint + tmp1;
                  pParam->B3SOIdlc = model->B3SOIdlc + tmp1;

                  T2 = pow(Ldrn, model->B3SOIWln);
                  T3 = pow(Wdrn, model->B3SOIWwn);
                  tmp2 = model->B3SOIWl / T2 + model->B3SOIWw / T3
                       + model->B3SOIWwl / (T2 * T3);
                  pParam->B3SOIdw = model->B3SOIWint + tmp2;
                  pParam->B3SOIdwc = model->B3SOIdwc + tmp2;

                  pParam->B3SOIleff = here->B3SOIl - 2.0 * pParam->B3SOIdl;
                  if (pParam->B3SOIleff <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "B3SOI: mosfet %s, model %s: Effective channel length <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->B3SOIweff = here->B3SOIw - 2.0 * pParam->B3SOIdw;
                  if (pParam->B3SOIweff <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "B3SOI: mosfet %s, model %s: Effective channel width <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->B3SOIleffCV = here->B3SOIl - 2.0 * pParam->B3SOIdlc;
                  if (pParam->B3SOIleffCV <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "B3SOI: mosfet %s, model %s: Effective channel length for C-V <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->B3SOIweffCV = here->B3SOIw - 2.0 * pParam->B3SOIdwc;
                  if (pParam->B3SOIweffCV <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "B3SOI: mosfet %s, model %s: Effective channel width for C-V <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  /* Not binned - START */
                  pParam->B3SOIat = model->B3SOIat;
                  pParam->B3SOIgamma1 = model->B3SOIgamma1;
                  pParam->B3SOIgamma2 = model->B3SOIgamma2;
                  pParam->B3SOIvbx = model->B3SOIvbx;
                  pParam->B3SOIvbm = model->B3SOIvbm;
                  pParam->B3SOIxt = model->B3SOIxt;
                  pParam->B3SOIkt1 = model->B3SOIkt1;
                  pParam->B3SOIkt1l = model->B3SOIkt1l;
                  pParam->B3SOIkt2 = model->B3SOIkt2;
                  pParam->B3SOIua1 = model->B3SOIua1;
                  pParam->B3SOIub1 = model->B3SOIub1;
                  pParam->B3SOIuc1 = model->B3SOIuc1;
                  pParam->B3SOIute = model->B3SOIute;
                  pParam->B3SOIprt = model->B3SOIprt;
                  /* Not binned - END */

		  /* CV model */
		  pParam->B3SOIcgsl = model->B3SOIcgsl;
		  pParam->B3SOIcgdl = model->B3SOIcgdl;
		  pParam->B3SOIckappa = model->B3SOIckappa;
		  pParam->B3SOIcf = model->B3SOIcf;
		  pParam->B3SOIclc = model->B3SOIclc;
		  pParam->B3SOIcle = model->B3SOIcle;

                  pParam->B3SOIabulkCVfactor = pow(1.0+(pParam->B3SOIclc
					     / pParam->B3SOIleff),
					     pParam->B3SOIcle);

/* Added for binning - START */
		  if (model->B3SOIbinUnit == 1)
		  {   Inv_L = 1.0e-6 / pParam->B3SOIleff;
		      Inv_W = 1.0e-6 / pParam->B3SOIweff;
		      Inv_LW = 1.0e-12 / (pParam->B3SOIleff
			     * pParam->B3SOIweff);
		  }
		  else
		  {   Inv_L = 1.0 / pParam->B3SOIleff;
		      Inv_W = 1.0 / pParam->B3SOIweff;
		      Inv_LW = 1.0 / (pParam->B3SOIleff
			     * pParam->B3SOIweff);
		  }
		  pParam->B3SOInpeak = model->B3SOInpeak
				     + model->B3SOIlnpeak * Inv_L
				     + model->B3SOIwnpeak * Inv_W
				     + model->B3SOIpnpeak * Inv_LW;
		  pParam->B3SOInsub = model->B3SOInsub
				    + model->B3SOIlnsub * Inv_L
				    + model->B3SOIwnsub * Inv_W
				    + model->B3SOIpnsub * Inv_LW;
		  pParam->B3SOIngate = model->B3SOIngate
				     + model->B3SOIlngate * Inv_L
				     + model->B3SOIwngate * Inv_W
				     + model->B3SOIpngate * Inv_LW;
		  pParam->B3SOIvth0 = model->B3SOIvth0
				    + model->B3SOIlvth0 * Inv_L
				    + model->B3SOIwvth0 * Inv_W
				    + model->B3SOIpvth0 * Inv_LW;
		  pParam->B3SOIk1 = model->B3SOIk1
				  + model->B3SOIlk1 * Inv_L
				  + model->B3SOIwk1 * Inv_W
				  + model->B3SOIpk1 * Inv_LW;
		  pParam->B3SOIk2 = model->B3SOIk2
				  + model->B3SOIlk2 * Inv_L
				  + model->B3SOIwk2 * Inv_W
				  + model->B3SOIpk2 * Inv_LW;
		  pParam->B3SOIk3 = model->B3SOIk3
				  + model->B3SOIlk3 * Inv_L
				  + model->B3SOIwk3 * Inv_W
				  + model->B3SOIpk3 * Inv_LW;
		  pParam->B3SOIk3b = model->B3SOIk3b
				   + model->B3SOIlk3b * Inv_L
				   + model->B3SOIwk3b * Inv_W
				   + model->B3SOIpk3b * Inv_LW;
		  pParam->B3SOIvbsa = model->B3SOIvbsa
				   + model->B3SOIlvbsa * Inv_L
				   + model->B3SOIwvbsa * Inv_W
				   + model->B3SOIpvbsa * Inv_LW;
		  pParam->B3SOIdelp = model->B3SOIdelp
				   + model->B3SOIldelp * Inv_L
				   + model->B3SOIwdelp * Inv_W
				   + model->B3SOIpdelp * Inv_LW;
		  pParam->B3SOIkb1 = model->B3SOIkb1
				   + model->B3SOIlkb1 * Inv_L
				   + model->B3SOIwkb1 * Inv_W
				   + model->B3SOIpkb1 * Inv_LW;
		  pParam->B3SOIkb3 = model->B3SOIkb3
				   + model->B3SOIlkb3 * Inv_L
				   + model->B3SOIwkb3 * Inv_W
				   + model->B3SOIpkb3 * Inv_LW;
		  pParam->B3SOIdvbd0 = model->B3SOIdvbd0
				   + model->B3SOIldvbd0 * Inv_L
				   + model->B3SOIwdvbd0 * Inv_W
				   + model->B3SOIpdvbd0 * Inv_LW;
		  pParam->B3SOIdvbd1 = model->B3SOIdvbd1
				   + model->B3SOIldvbd1 * Inv_L
				   + model->B3SOIwdvbd1 * Inv_W
				   + model->B3SOIpdvbd1 * Inv_LW;
		  pParam->B3SOIw0 = model->B3SOIw0
				  + model->B3SOIlw0 * Inv_L
				  + model->B3SOIww0 * Inv_W
				  + model->B3SOIpw0 * Inv_LW;
		  pParam->B3SOInlx = model->B3SOInlx
				   + model->B3SOIlnlx * Inv_L
				   + model->B3SOIwnlx * Inv_W
				   + model->B3SOIpnlx * Inv_LW;
		  pParam->B3SOIdvt0 = model->B3SOIdvt0
				    + model->B3SOIldvt0 * Inv_L
				    + model->B3SOIwdvt0 * Inv_W
				    + model->B3SOIpdvt0 * Inv_LW;
		  pParam->B3SOIdvt1 = model->B3SOIdvt1
				    + model->B3SOIldvt1 * Inv_L
				    + model->B3SOIwdvt1 * Inv_W
				    + model->B3SOIpdvt1 * Inv_LW;
		  pParam->B3SOIdvt2 = model->B3SOIdvt2
				    + model->B3SOIldvt2 * Inv_L
				    + model->B3SOIwdvt2 * Inv_W
				    + model->B3SOIpdvt2 * Inv_LW;
		  pParam->B3SOIdvt0w = model->B3SOIdvt0w
				    + model->B3SOIldvt0w * Inv_L
				    + model->B3SOIwdvt0w * Inv_W
				    + model->B3SOIpdvt0w * Inv_LW;
		  pParam->B3SOIdvt1w = model->B3SOIdvt1w
				    + model->B3SOIldvt1w * Inv_L
				    + model->B3SOIwdvt1w * Inv_W
				    + model->B3SOIpdvt1w * Inv_LW;
		  pParam->B3SOIdvt2w = model->B3SOIdvt2w
				    + model->B3SOIldvt2w * Inv_L
				    + model->B3SOIwdvt2w * Inv_W
				    + model->B3SOIpdvt2w * Inv_LW;
		  pParam->B3SOIu0 = model->B3SOIu0
				  + model->B3SOIlu0 * Inv_L
				  + model->B3SOIwu0 * Inv_W
				  + model->B3SOIpu0 * Inv_LW;
		  pParam->B3SOIua = model->B3SOIua
				  + model->B3SOIlua * Inv_L
				  + model->B3SOIwua * Inv_W
				  + model->B3SOIpua * Inv_LW;
		  pParam->B3SOIub = model->B3SOIub
				  + model->B3SOIlub * Inv_L
				  + model->B3SOIwub * Inv_W
				  + model->B3SOIpub * Inv_LW;
		  pParam->B3SOIuc = model->B3SOIuc
				  + model->B3SOIluc * Inv_L
				  + model->B3SOIwuc * Inv_W
				  + model->B3SOIpuc * Inv_LW;
		  pParam->B3SOIvsat = model->B3SOIvsat
				    + model->B3SOIlvsat * Inv_L
				    + model->B3SOIwvsat * Inv_W
				    + model->B3SOIpvsat * Inv_LW;
		  pParam->B3SOIa0 = model->B3SOIa0
				  + model->B3SOIla0 * Inv_L
				  + model->B3SOIwa0 * Inv_W
				  + model->B3SOIpa0 * Inv_LW; 
		  pParam->B3SOIags = model->B3SOIags
				  + model->B3SOIlags * Inv_L
				  + model->B3SOIwags * Inv_W
				  + model->B3SOIpags * Inv_LW;
		  pParam->B3SOIb0 = model->B3SOIb0
				  + model->B3SOIlb0 * Inv_L
				  + model->B3SOIwb0 * Inv_W
				  + model->B3SOIpb0 * Inv_LW;
		  pParam->B3SOIb1 = model->B3SOIb1
				  + model->B3SOIlb1 * Inv_L
				  + model->B3SOIwb1 * Inv_W
				  + model->B3SOIpb1 * Inv_LW;
		  pParam->B3SOIketa = model->B3SOIketa
				    + model->B3SOIlketa * Inv_L
				    + model->B3SOIwketa * Inv_W
				    + model->B3SOIpketa * Inv_LW;
		  pParam->B3SOIabp = model->B3SOIabp
				  + model->B3SOIlabp * Inv_L
				  + model->B3SOIwabp * Inv_W
				  + model->B3SOIpabp * Inv_LW;
		  pParam->B3SOImxc = model->B3SOImxc
				  + model->B3SOIlmxc * Inv_L
				  + model->B3SOIwmxc * Inv_W
				  + model->B3SOIpmxc * Inv_LW;
		  pParam->B3SOIadice0 = model->B3SOIadice0
				  + model->B3SOIladice0 * Inv_L
				  + model->B3SOIwadice0 * Inv_W
				  + model->B3SOIpadice0 * Inv_LW;
		  pParam->B3SOIa1 = model->B3SOIa1
				  + model->B3SOIla1 * Inv_L
				  + model->B3SOIwa1 * Inv_W
				  + model->B3SOIpa1 * Inv_LW;
		  pParam->B3SOIa2 = model->B3SOIa2
				  + model->B3SOIla2 * Inv_L
				  + model->B3SOIwa2 * Inv_W
				  + model->B3SOIpa2 * Inv_LW;
		  pParam->B3SOIrdsw = model->B3SOIrdsw
				    + model->B3SOIlrdsw * Inv_L
				    + model->B3SOIwrdsw * Inv_W
				    + model->B3SOIprdsw * Inv_LW;
		  pParam->B3SOIprwb = model->B3SOIprwb
				    + model->B3SOIlprwb * Inv_L
				    + model->B3SOIwprwb * Inv_W
				    + model->B3SOIpprwb * Inv_LW;
		  pParam->B3SOIprwg = model->B3SOIprwg
				    + model->B3SOIlprwg * Inv_L
				    + model->B3SOIwprwg * Inv_W
				    + model->B3SOIpprwg * Inv_LW;
		  pParam->B3SOIwr = model->B3SOIwr
				  + model->B3SOIlwr * Inv_L
				  + model->B3SOIwwr * Inv_W
				  + model->B3SOIpwr * Inv_LW;
		  pParam->B3SOInfactor = model->B3SOInfactor
				       + model->B3SOIlnfactor * Inv_L
				       + model->B3SOIwnfactor * Inv_W
				       + model->B3SOIpnfactor * Inv_LW;
		  pParam->B3SOIdwg = model->B3SOIdwg
				   + model->B3SOIldwg * Inv_L
				   + model->B3SOIwdwg * Inv_W
				   + model->B3SOIpdwg * Inv_LW;
		  pParam->B3SOIdwb = model->B3SOIdwb
				   + model->B3SOIldwb * Inv_L
				   + model->B3SOIwdwb * Inv_W
				   + model->B3SOIpdwb * Inv_LW;
		  pParam->B3SOIvoff = model->B3SOIvoff
				    + model->B3SOIlvoff * Inv_L
				    + model->B3SOIwvoff * Inv_W
				    + model->B3SOIpvoff * Inv_LW;
		  pParam->B3SOIeta0 = model->B3SOIeta0
				    + model->B3SOIleta0 * Inv_L
				    + model->B3SOIweta0 * Inv_W
				    + model->B3SOIpeta0 * Inv_LW;
		  pParam->B3SOIetab = model->B3SOIetab
				    + model->B3SOIletab * Inv_L
				    + model->B3SOIwetab * Inv_W
				    + model->B3SOIpetab * Inv_LW;
		  pParam->B3SOIdsub = model->B3SOIdsub
				    + model->B3SOIldsub * Inv_L
				    + model->B3SOIwdsub * Inv_W
				    + model->B3SOIpdsub * Inv_LW;
		  pParam->B3SOIcit = model->B3SOIcit
				   + model->B3SOIlcit * Inv_L
				   + model->B3SOIwcit * Inv_W
				   + model->B3SOIpcit * Inv_LW;
		  pParam->B3SOIcdsc = model->B3SOIcdsc
				    + model->B3SOIlcdsc * Inv_L
				    + model->B3SOIwcdsc * Inv_W
				    + model->B3SOIpcdsc * Inv_LW;
		  pParam->B3SOIcdscb = model->B3SOIcdscb
				     + model->B3SOIlcdscb * Inv_L
				     + model->B3SOIwcdscb * Inv_W
				     + model->B3SOIpcdscb * Inv_LW; 
    		  pParam->B3SOIcdscd = model->B3SOIcdscd
				     + model->B3SOIlcdscd * Inv_L
				     + model->B3SOIwcdscd * Inv_W
				     + model->B3SOIpcdscd * Inv_LW; 
		  pParam->B3SOIpclm = model->B3SOIpclm
				    + model->B3SOIlpclm * Inv_L
				    + model->B3SOIwpclm * Inv_W
				    + model->B3SOIppclm * Inv_LW;
		  pParam->B3SOIpdibl1 = model->B3SOIpdibl1
				      + model->B3SOIlpdibl1 * Inv_L
				      + model->B3SOIwpdibl1 * Inv_W
				      + model->B3SOIppdibl1 * Inv_LW;
		  pParam->B3SOIpdibl2 = model->B3SOIpdibl2
				      + model->B3SOIlpdibl2 * Inv_L
				      + model->B3SOIwpdibl2 * Inv_W
				      + model->B3SOIppdibl2 * Inv_LW;
		  pParam->B3SOIpdiblb = model->B3SOIpdiblb
				      + model->B3SOIlpdiblb * Inv_L
				      + model->B3SOIwpdiblb * Inv_W
				      + model->B3SOIppdiblb * Inv_LW;
		  pParam->B3SOIdrout = model->B3SOIdrout
				     + model->B3SOIldrout * Inv_L
				     + model->B3SOIwdrout * Inv_W
				     + model->B3SOIpdrout * Inv_LW;
		  pParam->B3SOIpvag = model->B3SOIpvag
				    + model->B3SOIlpvag * Inv_L
				    + model->B3SOIwpvag * Inv_W
				    + model->B3SOIppvag * Inv_LW;
		  pParam->B3SOIdelta = model->B3SOIdelta
				     + model->B3SOIldelta * Inv_L
				     + model->B3SOIwdelta * Inv_W
				     + model->B3SOIpdelta * Inv_LW;
		  pParam->B3SOIaii = model->B3SOIaii
				     + model->B3SOIlaii * Inv_L
				     + model->B3SOIwaii * Inv_W
				     + model->B3SOIpaii * Inv_LW;
		  pParam->B3SOIbii = model->B3SOIbii
				     + model->B3SOIlbii * Inv_L
				     + model->B3SOIwbii * Inv_W
				     + model->B3SOIpbii * Inv_LW;
		  pParam->B3SOIcii = model->B3SOIcii
				     + model->B3SOIlcii * Inv_L
				     + model->B3SOIwcii * Inv_W
				     + model->B3SOIpcii * Inv_LW;
		  pParam->B3SOIdii = model->B3SOIdii
				     + model->B3SOIldii * Inv_L
				     + model->B3SOIwdii * Inv_W
				     + model->B3SOIpdii * Inv_LW;
		  pParam->B3SOIalpha0 = model->B3SOIalpha0
				      + model->B3SOIlalpha0 * Inv_L
				      + model->B3SOIwalpha0 * Inv_W
				      + model->B3SOIpalpha0 * Inv_LW;
		  pParam->B3SOIalpha1 = model->B3SOIalpha1
				      + model->B3SOIlalpha1 * Inv_L
				      + model->B3SOIwalpha1 * Inv_W
				      + model->B3SOIpalpha1 * Inv_LW;
		  pParam->B3SOIbeta0 = model->B3SOIbeta0
				     + model->B3SOIlbeta0 * Inv_L
				     + model->B3SOIwbeta0 * Inv_W
				     + model->B3SOIpbeta0 * Inv_LW;
		  pParam->B3SOIagidl = model->B3SOIagidl
				      + model->B3SOIlagidl * Inv_L
				      + model->B3SOIwagidl * Inv_W
				      + model->B3SOIpagidl * Inv_LW;
		  pParam->B3SOIbgidl = model->B3SOIbgidl
				      + model->B3SOIlbgidl * Inv_L
				      + model->B3SOIwbgidl * Inv_W
				      + model->B3SOIpbgidl * Inv_LW;
		  pParam->B3SOIngidl = model->B3SOIngidl
				      + model->B3SOIlngidl * Inv_L
				      + model->B3SOIwngidl * Inv_W
				      + model->B3SOIpngidl * Inv_LW;
		  pParam->B3SOIntun = model->B3SOIntun
				      + model->B3SOIlntun * Inv_L
				      + model->B3SOIwntun * Inv_W
				      + model->B3SOIpntun * Inv_LW;
		  pParam->B3SOIndiode = model->B3SOIndiode
				      + model->B3SOIlndiode * Inv_L
				      + model->B3SOIwndiode * Inv_W
				      + model->B3SOIpndiode * Inv_LW;
		  pParam->B3SOIisbjt = model->B3SOIisbjt
				  + model->B3SOIlisbjt * Inv_L
				  + model->B3SOIwisbjt * Inv_W
				  + model->B3SOIpisbjt * Inv_LW;
		  pParam->B3SOIisdif = model->B3SOIisdif
				  + model->B3SOIlisdif * Inv_L
				  + model->B3SOIwisdif * Inv_W
				  + model->B3SOIpisdif * Inv_LW;
		  pParam->B3SOIisrec = model->B3SOIisrec
				  + model->B3SOIlisrec * Inv_L
				  + model->B3SOIwisrec * Inv_W
				  + model->B3SOIpisrec * Inv_LW;
		  pParam->B3SOIistun = model->B3SOIistun
				  + model->B3SOIlistun * Inv_L
				  + model->B3SOIwistun * Inv_W
				  + model->B3SOIpistun * Inv_LW;
		  pParam->B3SOIedl = model->B3SOIedl
				  + model->B3SOIledl * Inv_L
				  + model->B3SOIwedl * Inv_W
				  + model->B3SOIpedl * Inv_LW;
		  pParam->B3SOIkbjt1 = model->B3SOIkbjt1
				  + model->B3SOIlkbjt1 * Inv_L
				  + model->B3SOIwkbjt1 * Inv_W
				  + model->B3SOIpkbjt1 * Inv_LW;
		  /* CV model */
		  pParam->B3SOIvsdfb = model->B3SOIvsdfb
				  + model->B3SOIlvsdfb * Inv_L
				  + model->B3SOIwvsdfb * Inv_W
				  + model->B3SOIpvsdfb * Inv_LW;
		  pParam->B3SOIvsdth = model->B3SOIvsdth
				  + model->B3SOIlvsdth * Inv_L
				  + model->B3SOIwvsdth * Inv_W
				  + model->B3SOIpvsdth * Inv_LW;
/* Added for binning - END */

	          T0 = (TRatio - 1.0);

                  pParam->B3SOIuatemp = pParam->B3SOIua;  /*  save ua, ub, and uc for b3soild.c */
                  pParam->B3SOIubtemp = pParam->B3SOIub;
                  pParam->B3SOIuctemp = pParam->B3SOIuc;
                  pParam->B3SOIrds0denom = pow(pParam->B3SOIweff * 1E6, pParam->B3SOIwr);
                  pParam->B3SOIrth = here->B3SOIrth0 * sqrt(model->B3SOItbox
                      / model->B3SOItsi) / pParam->B3SOIweff;
                  pParam->B3SOIcth = here->B3SOIcth0 * model->B3SOItsi;
                  pParam->B3SOIrbody = model->B3SOIrbody *
                                     pParam->B3SOIweff / pParam->B3SOIleff;
	          pParam->B3SOIua = pParam->B3SOIua + pParam->B3SOIua1 * T0;
	          pParam->B3SOIub = pParam->B3SOIub + pParam->B3SOIub1 * T0;
	          pParam->B3SOIuc = pParam->B3SOIuc + pParam->B3SOIuc1 * T0;
                  if (pParam->B3SOIu0 > 1.0) 
                      pParam->B3SOIu0 = pParam->B3SOIu0 / 1.0e4;

                  pParam->B3SOIu0temp = pParam->B3SOIu0
				      * pow(TRatio, pParam->B3SOIute); 
                  pParam->B3SOIvsattemp = pParam->B3SOIvsat - pParam->B3SOIat 
			                * T0;
	          pParam->B3SOIrds0 = (pParam->B3SOIrdsw + pParam->B3SOIprt * T0)
                                    / pow(pParam->B3SOIweff * 1E6, pParam->B3SOIwr);

		  if (B3SOIcheckModel(model, here, ckt))
		  {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror)) (ERR_FATAL, "Fatal error(s) detected during B3SOIV3 parameter checking for %s in model %s", namarray);
                      return(E_BADPARM);   
		  }


                  pParam->B3SOIcgdo = (model->B3SOIcgdo + pParam->B3SOIcf)
				    * pParam->B3SOIweffCV;
                  pParam->B3SOIcgso = (model->B3SOIcgso + pParam->B3SOIcf)
				    * pParam->B3SOIweffCV;


                  pParam->B3SOIcgeo = model->B3SOIcgeo * pParam->B3SOIleffCV;


                  if (!model->B3SOInpeakGiven && model->B3SOIgamma1Given)
                  {   T0 = pParam->B3SOIgamma1 * model->B3SOIcox;
                      pParam->B3SOInpeak = 3.021E22 * T0 * T0;
                  }

                  T0 = pow(TRatio, model->B3SOIxbjt / pParam->B3SOIndiode);
                  T1 = pow(TRatio, model->B3SOIxdif / pParam->B3SOIndiode);
                  T2 = pow(TRatio, model->B3SOIxrec / pParam->B3SOIndiode / 2);
                  T4 = -Eg0 / pParam->B3SOIndiode / model->B3SOIvtm * (1 - TRatio);
                  T5 = exp(T4);
                  T6 = sqrt(T5);
                  pParam->B3SOIjbjt = pParam->B3SOIisbjt * T0 * T5;
                  pParam->B3SOIjdif = pParam->B3SOIisdif * T1 * T5;
                  pParam->B3SOIjrec = pParam->B3SOIisrec * T2 * T6;
                  T0 = pow(TRatio, model->B3SOIxtun / pParam->B3SOIntun);
                  pParam->B3SOIjtun = pParam->B3SOIistun * T0 ;

                  if (pParam->B3SOInsub > 0)
                     pParam->B3SOIvfbb = -model->B3SOItype * model->B3SOIvtm *
                                log(pParam->B3SOInpeak/ pParam->B3SOInsub);
                  else
                     pParam->B3SOIvfbb = -model->B3SOItype * model->B3SOIvtm *
                                log(-pParam->B3SOInpeak* pParam->B3SOInsub/ni/ni);

                  if (!model->B3SOIvsdfbGiven)
                  {
                     if (pParam->B3SOInsub > 0)
                        pParam->B3SOIvsdfb = -model->B3SOItype * (model->B3SOIvtm*log(1e20 *
                                            pParam->B3SOInsub / ni /ni) - 0.3);
                     else if (pParam->B3SOInsub < 0)
                        pParam->B3SOIvsdfb = -model->B3SOItype * (model->B3SOIvtm*log(-1e20 /
                                            pParam->B3SOInsub) + 0.3);
                  }
                 
                  /* Phi  & Gamma */
                  SDphi = 2.0*model->B3SOIvtm*log(fabs(pParam->B3SOInsub) / ni);
                  SDgamma = 5.753e-12 * sqrt(fabs(pParam->B3SOInsub)) / model->B3SOIcbox;

                  if (!model->B3SOIvsdthGiven)
                  {
                     if ( ((pParam->B3SOInsub > 0) && (model->B3SOItype > 0)) ||
                          ((pParam->B3SOInsub < 0) && (model->B3SOItype < 0)) )
                        pParam->B3SOIvsdth = pParam->B3SOIvsdfb + SDphi +
                                            SDgamma * sqrt(SDphi);
                     else 
                        pParam->B3SOIvsdth = pParam->B3SOIvsdfb - SDphi -
                                            SDgamma * sqrt(SDphi);
                  }
                  if (!model->B3SOIcsdminGiven)
                  {
                     /* Cdmin */
                     tmp = sqrt(2.0 * EPSSI * SDphi / (Charge_q * 
                                FABS(pParam->B3SOInsub) * 1.0e6));
                     tmp1 = EPSSI / tmp;
                     model->B3SOIcsdmin = tmp1 * model->B3SOIcbox /
                                          (tmp1 + model->B3SOIcbox);
                  }

                  T0 = model->B3SOIcsdesw * log(1 + model->B3SOItsi /
                       model->B3SOItbox);
                  T1 = here->B3SOIsourcePerimeter - pParam->B3SOIweff;
                  if (T1 > 0.0)
                     pParam->B3SOIcsesw = T0 * T1; 
                  else
                     pParam->B3SOIcsesw = 0.0;
                  T1 = here->B3SOIdrainPerimeter - pParam->B3SOIweff;
                  if (T1 > 0.0)
                     pParam->B3SOIcdesw = T0 * T1;
                  else
                     pParam->B3SOIcdesw = 0.0;

		  pParam->B3SOIphi = 2.0 * model->B3SOIvtm 
			           * log(pParam->B3SOInpeak / ni);

	          pParam->B3SOIsqrtPhi = sqrt(pParam->B3SOIphi);
	          pParam->B3SOIphis3 = pParam->B3SOIsqrtPhi * pParam->B3SOIphi;

                  pParam->B3SOIXdep0 = sqrt(2.0 * EPSSI / (Charge_q
				     * pParam->B3SOInpeak * 1.0e6))
                                     * pParam->B3SOIsqrtPhi; 
                  pParam->B3SOIsqrtXdep0 = sqrt(pParam->B3SOIXdep0);
                  pParam->B3SOIlitl = sqrt(3.0 * model->B3SOIxj
				    * model->B3SOItox);
                  pParam->B3SOIvbi = model->B3SOIvtm * log(1.0e20
			           * pParam->B3SOInpeak / (ni * ni));
                  pParam->B3SOIcdep0 = sqrt(Charge_q * EPSSI
				     * pParam->B3SOInpeak * 1.0e6 / 2.0
				     / pParam->B3SOIphi);
        
                  if (model->B3SOIk1Given || model->B3SOIk2Given)
	          {   if (!model->B3SOIk1Given)
	              {   fprintf(stdout, "Warning: k1 should be specified with k2.\n");
                          pParam->B3SOIk1 = 0.53;
                      }
                      if (!model->B3SOIk2Given)
	              {   fprintf(stdout, "Warning: k2 should be specified with k1.\n");
                          pParam->B3SOIk2 = -0.0186;
                      }
                      if (model->B3SOIxtGiven)
                          fprintf(stdout, "Warning: xt is ignored because k1 or k2 is given.\n");
                      if (model->B3SOIvbxGiven)
                          fprintf(stdout, "Warning: vbx is ignored because k1 or k2 is given.\n");
                      if (model->B3SOIvbmGiven)
                          fprintf(stdout, "Warning: vbm is ignored because k1 or k2 is given.\n");
                      if (model->B3SOIgamma1Given)
                          fprintf(stdout, "Warning: gamma1 is ignored because k1 or k2 is given.\n");
                      if (model->B3SOIgamma2Given)
                          fprintf(stdout, "Warning: gamma2 is ignored because k1 or k2 is given.\n");
                  }
                  else
	          {   if (!model->B3SOIvbxGiven)
                          pParam->B3SOIvbx = pParam->B3SOIphi - 7.7348e-4 
                                           * pParam->B3SOInpeak
					   * pParam->B3SOIxt * pParam->B3SOIxt;
	              if (pParam->B3SOIvbx > 0.0)
		          pParam->B3SOIvbx = -pParam->B3SOIvbx;
	              if (pParam->B3SOIvbm > 0.0)
                          pParam->B3SOIvbm = -pParam->B3SOIvbm;
           
                      if (!model->B3SOIgamma1Given)
                          pParam->B3SOIgamma1 = 5.753e-12
					      * sqrt(pParam->B3SOInpeak)
                                              / model->B3SOIcox;
                      if (!model->B3SOIgamma2Given)
                          pParam->B3SOIgamma2 = 5.753e-12
					      * sqrt(pParam->B3SOInsub)
                                              / model->B3SOIcox;

                      T0 = pParam->B3SOIgamma1 - pParam->B3SOIgamma2;
                      T1 = sqrt(pParam->B3SOIphi - pParam->B3SOIvbx)
			 - pParam->B3SOIsqrtPhi;
                      T2 = sqrt(pParam->B3SOIphi * (pParam->B3SOIphi
			 - pParam->B3SOIvbm)) - pParam->B3SOIphi;
                      pParam->B3SOIk2 = T0 * T1 / (2.0 * T2 + pParam->B3SOIvbm);
                      pParam->B3SOIk1 = pParam->B3SOIgamma2 - 2.0
				      * pParam->B3SOIk2 * sqrt(pParam->B3SOIphi
				      - pParam->B3SOIvbm);
                  }
 
		  if (pParam->B3SOIk2 < 0.0)
		  {   T0 = 0.5 * pParam->B3SOIk1 / pParam->B3SOIk2;
                      pParam->B3SOIvbsc = 0.9 * (pParam->B3SOIphi - T0 * T0);
		      if (pParam->B3SOIvbsc > -3.0)
		          pParam->B3SOIvbsc = -3.0;
		      else if (pParam->B3SOIvbsc < -30.0)
		          pParam->B3SOIvbsc = -30.0;
		  }
		  else
		  {   pParam->B3SOIvbsc = -30.0;
		  }
		  if (pParam->B3SOIvbsc > pParam->B3SOIvbm)
		      pParam->B3SOIvbsc = pParam->B3SOIvbm;

	          if (model->B3SOIvth0Given)
		  {   pParam->B3SOIvfb = model->B3SOItype * pParam->B3SOIvth0 
                                       - pParam->B3SOIphi - pParam->B3SOIk1 
                                       * pParam->B3SOIsqrtPhi;
		  }
		  else
		  {   pParam->B3SOIvfb = -1.0;
		      pParam->B3SOIvth0 = model->B3SOItype * (pParam->B3SOIvfb
                                        + pParam->B3SOIphi + pParam->B3SOIk1 
                                        * pParam->B3SOIsqrtPhi);
		  }
                  T1 = sqrt(EPSSI / EPSOX * model->B3SOItox
		     * pParam->B3SOIXdep0);
                  T0 = exp(-0.5 * pParam->B3SOIdsub * pParam->B3SOIleff / T1);
                  pParam->B3SOItheta0vb0 = (T0 + 2.0 * T0 * T0);

                  T0 = exp(-0.5 * pParam->B3SOIdrout * pParam->B3SOIleff / T1);
                  T2 = (T0 + 2.0 * T0 * T0);
                  pParam->B3SOIthetaRout = pParam->B3SOIpdibl1 * T2
				         + pParam->B3SOIpdibl2;

                  here->B3SOIminIsub = 5.0e-2 * pParam->B3SOIweff * model->B3SOItsi
                                     * MAX(pParam->B3SOIisdif, pParam->B3SOIisrec);
	      }

              here->B3SOIcsbox = model->B3SOIcbox*here->B3SOIsourceArea;
              here->B3SOIcsmin = model->B3SOIcsdmin*here->B3SOIsourceArea;
              here->B3SOIcdbox = model->B3SOIcbox*here->B3SOIdrainArea;
              here->B3SOIcdmin = model->B3SOIcsdmin*here->B3SOIdrainArea;

              if ( ((pParam->B3SOInsub > 0) && (model->B3SOItype > 0)) ||
	           ((pParam->B3SOInsub < 0) && (model->B3SOItype < 0)) )
              {
                 T0 = pParam->B3SOIvsdth - pParam->B3SOIvsdfb;
                 pParam->B3SOIsdt1 = pParam->B3SOIvsdfb + model->B3SOIasd * T0;
                 T1 = here->B3SOIcsbox - here->B3SOIcsmin;
                 T2 = T1 / T0 / T0;
                 pParam->B3SOIst2 = T2 / model->B3SOIasd;
                 pParam->B3SOIst3 = T2 /( 1 - model->B3SOIasd);
                 here->B3SOIst4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                  - here->B3SOIcsmin * pParam->B3SOIvsdfb;
 
                 T1 = here->B3SOIcdbox - here->B3SOIcdmin;
                 T2 = T1 / T0 / T0;
                 pParam->B3SOIdt2 = T2 / model->B3SOIasd;
                 pParam->B3SOIdt3 = T2 /( 1 - model->B3SOIasd);
                 here->B3SOIdt4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                  - here->B3SOIcdmin * pParam->B3SOIvsdfb;
	      } else
              {
                 T0 = pParam->B3SOIvsdfb - pParam->B3SOIvsdth;
                 pParam->B3SOIsdt1 = pParam->B3SOIvsdth + model->B3SOIasd * T0;
                 T1 = here->B3SOIcsmin - here->B3SOIcsbox;
                 T2 = T1 / T0 / T0;
                 pParam->B3SOIst2 = T2 / model->B3SOIasd;
                 pParam->B3SOIst3 = T2 /( 1 - model->B3SOIasd);
                 here->B3SOIst4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                  - here->B3SOIcsbox * pParam->B3SOIvsdth;
  
                 T1 = here->B3SOIcdmin - here->B3SOIcdbox;
                 T2 = T1 / T0 / T0;
                 pParam->B3SOIdt2 = T2 / model->B3SOIasd;
                 pParam->B3SOIdt3 = T2 /( 1 - model->B3SOIasd);
                 here->B3SOIdt4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                  - here->B3SOIcdbox * pParam->B3SOIvsdth;
              } 

	      here->B3SOIphi = pParam->B3SOIphi;
              /* process source/drain series resistance */
              here->B3SOIdrainConductance = model->B3SOIsheetResistance 
		                              * here->B3SOIdrainSquares;
              if (here->B3SOIdrainConductance > 0.0)
                  here->B3SOIdrainConductance = 1.0
					      / here->B3SOIdrainConductance;
	      else
                  here->B3SOIdrainConductance = 0.0;
                  
              here->B3SOIsourceConductance = model->B3SOIsheetResistance 
		                           * here->B3SOIsourceSquares;
              if (here->B3SOIsourceConductance > 0.0) 
                  here->B3SOIsourceConductance = 1.0
					       / here->B3SOIsourceConductance;
	      else
                  here->B3SOIsourceConductance = 0.0;
	      here->B3SOIcgso = pParam->B3SOIcgso;
	      here->B3SOIcgdo = pParam->B3SOIcgdo;

         }
    }
    return(OK);
}

