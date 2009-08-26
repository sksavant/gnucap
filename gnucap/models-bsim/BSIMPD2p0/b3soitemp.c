/* $Id: b3soitemp.c,v 2.0 99/2/15 Pin Su Release $  */
/*
$Log:   b3soitemp.c,v $
 * Revision 2.0  99/2/15  Pin Su
 * BSIMPD2.0 release
 *
*/
static char rcsid[] = "$Id: b3soitemp.c,v 2.0 99/2/15 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soitemp.c          98/5/01
Modified by Pin Su	99/2/15
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
#define MIN_EXP 1.713908432e-15
#define EXP_THRESHOLD 34.0
#define Charge_q 1.60219e-19
#define Eg300 1.115   /*  energy gap at 300K  */
#define DEXP(A,B) {                                                        \
        if (A > EXP_THRESHOLD) {                                              \
            B = MAX_EXP;                                                      \
        } else if (A < -30)  {                                                \
            B = MIN_EXP;                                                      \
        } else   {                                                            \
            B = exp(A);                                                       \
        }                                                                     \
    }



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
double Temp, TempRatio, Inv_L, Inv_W, Inv_LW, Dw, Dl, Vtm0, Tnom;
double SDphi, SDgamma;
int Size_Not_Found;


/* v2.0 release */
double tmp3, T7, T8, T9;


    /*  loop through all the B3SOI device models */
    for (; model != NULL; model = model->B3SOInextModel)
    {    Temp = ckt->CKTtemp;
         if (model->B3SOIGatesidewallJctPotential < 0.1)
             model->B3SOIGatesidewallJctPotential = 0.1;
         model->pSizeDependParamKnot = NULL;
	 pLastKnot = NULL;

	 Tnom = model->B3SOItnom;
	 TempRatio = Temp / Tnom;

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

                  pParam->B3SOIweff = here->B3SOIw - here->B3SOInbc * model->B3SOIdwbc
                     - (2.0 - here->B3SOInbc) * pParam->B3SOIdw;
                  if (pParam->B3SOIweff <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "B3SOI: mosfet %s, model %s: Effective channel width <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->B3SOIwdiod = pParam->B3SOIweff / here->B3SOInseg + here->B3SOIpdbcp;
                  pParam->B3SOIwdios = pParam->B3SOIweff / here->B3SOInseg + here->B3SOIpsbcp;

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

                  pParam->B3SOIweffCV = here->B3SOIw - here->B3SOInbc * model->B3SOIdwbc
                     - (2.0 - here->B3SOInbc) * pParam->B3SOIdwc;
                  if (pParam->B3SOIweffCV <= 0.0)
	          {   IFuid namarray[2];
                      namarray[0] = model->B3SOImodName;
                      namarray[1] = here->B3SOIname;
                      (*(SPfrontEnd->IFerror))(ERR_FATAL,
                      "B3SOI: mosfet %s, model %s: Effective channel width for C-V <= 0",
                       namarray);
                      return(E_BADPARM);
                  }

                  pParam->B3SOIwdiodCV = pParam->B3SOIweffCV / here->B3SOInseg + here->B3SOIpdbcp;
                  pParam->B3SOIwdiosCV = pParam->B3SOIweffCV / here->B3SOInseg + here->B3SOIpsbcp;

                  pParam->B3SOIleffCVb = here->B3SOIl - 2.0 * pParam->B3SOIdlc - model->B3SOIdlcb;
                  if (pParam->B3SOIleffCVb <= 0.0)
                  {  
                     IFuid namarray[2];
                     namarray[0] = model->B3SOImodName;
                     namarray[1] = here->B3SOIname;
                     (*(SPfrontEnd->IFerror))(ERR_FATAL,
                     "B3SOI: mosfet %s, model %s: Effective channel length for C-V (body) <= 0",
                     namarray);
                     return(E_BADPARM);
                  }

                  pParam->B3SOIleffCVbg = pParam->B3SOIleffCVb + 2 * model->B3SOIdlbg;
                  if (pParam->B3SOIleffCVbg <= 0.0)
                  { 
                     IFuid namarray[2];
                     namarray[0] = model->B3SOImodName;
                     namarray[1] = here->B3SOIname;
                     (*(SPfrontEnd->IFerror))(ERR_FATAL,
                     "B3SOI: mosfet %s, model %s: Effective channel length for C-V (backgate) <= 0",
                     namarray);
                     return(E_BADPARM);
                  }


		  pParam->B3SOIcdsc = model->B3SOIcdsc;
		  pParam->B3SOIcdscb = model->B3SOIcdscb;
				     
    		  pParam->B3SOIcdscd = model->B3SOIcdscd;
				     
		  pParam->B3SOIcit = model->B3SOIcit;
		  pParam->B3SOInfactor = model->B3SOInfactor;
		  pParam->B3SOIvsat = model->B3SOIvsat;
		  pParam->B3SOIat = model->B3SOIat;
		  pParam->B3SOIa0 = model->B3SOIa0;
				  
		  pParam->B3SOIags = model->B3SOIags;
				  
		  pParam->B3SOIa1 = model->B3SOIa1;
		  pParam->B3SOIa2 = model->B3SOIa2;
		  pParam->B3SOIketa = model->B3SOIketa;
		  pParam->B3SOInsub = model->B3SOInsub;
		  pParam->B3SOInpeak = model->B3SOInpeak;
		  pParam->B3SOIngate = model->B3SOIngate;
		  pParam->B3SOIgamma1 = model->B3SOIgamma1;
		  pParam->B3SOIgamma2 = model->B3SOIgamma2;
		  pParam->B3SOIvbx = model->B3SOIvbx;
		  pParam->B3SOIvbm = model->B3SOIvbm;
		  pParam->B3SOIxt = model->B3SOIxt;
		  pParam->B3SOIk1 = model->B3SOIk1;
		  pParam->B3SOIkt1 = model->B3SOIkt1;
		  pParam->B3SOIkt1l = model->B3SOIkt1l;
		  pParam->B3SOIk2 = model->B3SOIk2;
		  pParam->B3SOIkt2 = model->B3SOIkt2;
		  pParam->B3SOIk3 = model->B3SOIk3;
		  pParam->B3SOIk3b = model->B3SOIk3b;
		  pParam->B3SOIw0 = model->B3SOIw0;
		  pParam->B3SOInlx = model->B3SOInlx;
		  pParam->B3SOIdvt0 = model->B3SOIdvt0;
		  pParam->B3SOIdvt1 = model->B3SOIdvt1;
		  pParam->B3SOIdvt2 = model->B3SOIdvt2;
		  pParam->B3SOIdvt0w = model->B3SOIdvt0w;
		  pParam->B3SOIdvt1w = model->B3SOIdvt1w;
		  pParam->B3SOIdvt2w = model->B3SOIdvt2w;
		  pParam->B3SOIdrout = model->B3SOIdrout;
		  pParam->B3SOIdsub = model->B3SOIdsub;
		  pParam->B3SOIvth0 = model->B3SOIvth0;
		  pParam->B3SOIua = model->B3SOIua;
		  pParam->B3SOIua1 = model->B3SOIua1;
		  pParam->B3SOIub = model->B3SOIub;
		  pParam->B3SOIub1 = model->B3SOIub1;
		  pParam->B3SOIuc = model->B3SOIuc;
		  pParam->B3SOIuc1 = model->B3SOIuc1;
		  pParam->B3SOIu0 = model->B3SOIu0;
		  pParam->B3SOIute = model->B3SOIute;
		  pParam->B3SOIvoff = model->B3SOIvoff;
		  pParam->B3SOIdelta = model->B3SOIdelta;
		  pParam->B3SOIrdsw = model->B3SOIrdsw;
		  pParam->B3SOIprwg = model->B3SOIprwg;
		  pParam->B3SOIprwb = model->B3SOIprwb;
		  pParam->B3SOIprt = model->B3SOIprt;
		  pParam->B3SOIeta0 = model->B3SOIeta0;
		  pParam->B3SOIetab = model->B3SOIetab;
		  pParam->B3SOIpclm = model->B3SOIpclm;
		  pParam->B3SOIpdibl1 = model->B3SOIpdibl1;
		  pParam->B3SOIpdibl2 = model->B3SOIpdibl2;
		  pParam->B3SOIpdiblb = model->B3SOIpdiblb;
		  pParam->B3SOIpvag = model->B3SOIpvag;
		  pParam->B3SOIwr = model->B3SOIwr;
		  pParam->B3SOIdwg = model->B3SOIdwg;
		  pParam->B3SOIdwb = model->B3SOIdwb;
		  pParam->B3SOIb0 = model->B3SOIb0;
		  pParam->B3SOIb1 = model->B3SOIb1;
		  pParam->B3SOIalpha0 = model->B3SOIalpha0;
		  pParam->B3SOIbeta0 = model->B3SOIbeta0;
		  /* CV model */
		  pParam->B3SOIcgsl = model->B3SOIcgsl;
		  pParam->B3SOIcgdl = model->B3SOIcgdl;
		  pParam->B3SOIckappa = model->B3SOIckappa;
		  pParam->B3SOIcf = model->B3SOIcf;
		  pParam->B3SOIclc = model->B3SOIclc;
		  pParam->B3SOIcle = model->B3SOIcle;

                  pParam->B3SOIabulkCVfactor = 1.0 + pow((pParam->B3SOIclc / pParam->B3SOIleff),
					     pParam->B3SOIcle);

	          T0 = (TempRatio - 1.0);

                  pParam->B3SOIuatemp = pParam->B3SOIua;  /*  save ua, ub, and uc for b3soild.c */
                  pParam->B3SOIubtemp = pParam->B3SOIub;
                  pParam->B3SOIuctemp = pParam->B3SOIuc;
                  pParam->B3SOIrds0denom = pow(pParam->B3SOIweff * 1E6, pParam->B3SOIwr);
                  pParam->B3SOIrth = here->B3SOIrth0 / pParam->B3SOIweff * here->B3SOInseg;
                  pParam->B3SOIcth = here->B3SOIcth0 * pParam->B3SOIweff / here->B3SOInseg;
                  pParam->B3SOIrbody = model->B3SOIrbody *
                                     pParam->B3SOIweff / here->B3SOInseg / pParam->B3SOIleff;
	          pParam->B3SOIua = pParam->B3SOIua + pParam->B3SOIua1 * T0;
	          pParam->B3SOIub = pParam->B3SOIub + pParam->B3SOIub1 * T0;
	          pParam->B3SOIuc = pParam->B3SOIuc + pParam->B3SOIuc1 * T0;
                  if (pParam->B3SOIu0 > 1.0) 
                      pParam->B3SOIu0 = pParam->B3SOIu0 / 1.0e4;

                  pParam->B3SOIu0temp = pParam->B3SOIu0
				      * pow(TempRatio, pParam->B3SOIute); 
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
				    * pParam->B3SOIwdiodCV;
                  pParam->B3SOIcgso = (model->B3SOIcgso + pParam->B3SOIcf)
				    * pParam->B3SOIwdiosCV;

                  if (!model->B3SOInpeakGiven && model->B3SOIgamma1Given)
                  {   T0 = pParam->B3SOIgamma1 * model->B3SOIcox;
                      pParam->B3SOInpeak = 3.021E22 * T0 * T0;
                  }


                  T4 = Eg300 / model->B3SOIvtm * (TempRatio - 1.0);
                  T7 = model->B3SOIxbjt * T4 / model->B3SOIndiode;
                  DEXP(T7, T0);
                  T7 = model->B3SOIxdif * T4 / model->B3SOIndiode;
                  DEXP(T7, T1);
                  T7 = model->B3SOIxrec * T4 / model->B3SOInrecf0;
                  DEXP(T7, T2);
 
                  /* high level injection */
                  pParam->B3SOIahli = model->B3SOIahli * T0;
 
                  pParam->B3SOIjbjt = model->B3SOIisbjt * T0;
                  pParam->B3SOIjdif = model->B3SOIisdif * T1;
                  pParam->B3SOIjrec = model->B3SOIisrec * T2;
 
                  T7 = model->B3SOIxtun * (TempRatio - 1);
                  DEXP(T7, T0);
                  pParam->B3SOIjtun = model->B3SOIistun * T0;
 

                  if (model->B3SOInsub > 0)
                     pParam->B3SOIvfbb = -model->B3SOItype * model->B3SOIvtm *
                                log(model->B3SOInpeak/ model->B3SOInsub);
                  else
                     pParam->B3SOIvfbb = -model->B3SOItype * model->B3SOIvtm *
                                log(-model->B3SOInpeak* model->B3SOInsub/ni/ni);

                  if (!model->B3SOIvsdfbGiven)
                  {
                     if (model->B3SOInsub > 0)
                        model->B3SOIvsdfb = -model->B3SOItype * (model->B3SOIvtm*log(1e20 *
                                            model->B3SOInsub / ni /ni) - 0.3);
                     else if (model->B3SOInsub < 0)
                        model->B3SOIvsdfb = -model->B3SOItype * (model->B3SOIvtm*log(-1e20 /
                                            model->B3SOInsub) + 0.3);
                  }
                 
                  /* Phi  & Gamma */
                  SDphi = 2.0*model->B3SOIvtm*log(fabs(pParam->B3SOInsub) / ni);
                  SDgamma = 5.753e-12 * sqrt(fabs(pParam->B3SOInsub)) / model->B3SOIcbox;

                  if (!model->B3SOIvsdthGiven)
                  {
                     if ( ((model->B3SOInsub > 0) && (model->B3SOItype > 0)) ||
                          ((model->B3SOInsub < 0) && (model->B3SOItype < 0)) )
                        model->B3SOIvsdth = model->B3SOIvsdfb + SDphi +
                                            SDgamma * sqrt(SDphi);
                     else 
                        model->B3SOIvsdth = model->B3SOIvsdfb - SDphi -
                                            SDgamma * sqrt(SDphi);
                  }

                  if (!model->B3SOIcsdminGiven) {
                     /* Cdmin */
                     tmp = sqrt(2.0 * EPSSI * SDphi / (Charge_q * 
                                FABS(pParam->B3SOInsub) * 1.0e6));
                     tmp1 = EPSSI / tmp;
                     model->B3SOIcsdmin = tmp1 * model->B3SOIcbox /
                                          (tmp1 + model->B3SOIcbox);
                  } 

                  pParam->B3SOIcsbox = model->B3SOIcbox*here->B3SOIsourceArea;
                  pParam->B3SOIcsmin = model->B3SOIcsdmin*here->B3SOIsourceArea;
                  pParam->B3SOIcdbox = model->B3SOIcbox*here->B3SOIdrainArea;
                  pParam->B3SOIcdmin = model->B3SOIcsdmin*here->B3SOIdrainArea;

		  if ( ((model->B3SOInsub > 0) && (model->B3SOItype > 0)) ||
		       ((model->B3SOInsub < 0) && (model->B3SOItype < 0)) )
		  {
                     T0 = model->B3SOIvsdth - model->B3SOIvsdfb;
                     pParam->B3SOIsdt1 = model->B3SOIvsdfb + model->B3SOIasd * T0;
                     T1 = pParam->B3SOIcsbox - pParam->B3SOIcsmin;
                     T2 = T1 / T0 / T0;
                     pParam->B3SOIst2 = T2 / model->B3SOIasd;
                     pParam->B3SOIst3 = T2 /( 1 - model->B3SOIasd);
                     pParam->B3SOIst4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                      - pParam->B3SOIcsmin * model->B3SOIvsdfb;
   
                     T1 = pParam->B3SOIcdbox - pParam->B3SOIcdmin;
                     T2 = T1 / T0 / T0;
                     pParam->B3SOIdt2 = T2 / model->B3SOIasd;
                     pParam->B3SOIdt3 = T2 /( 1 - model->B3SOIasd);
                     pParam->B3SOIdt4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                      - pParam->B3SOIcdmin * model->B3SOIvsdfb;
		  } else
		  {
                     T0 = model->B3SOIvsdfb - model->B3SOIvsdth;
                     pParam->B3SOIsdt1 = model->B3SOIvsdth + model->B3SOIasd * T0;
                     T1 = pParam->B3SOIcsmin - pParam->B3SOIcsbox;
                     T2 = T1 / T0 / T0;
                     pParam->B3SOIst2 = T2 / model->B3SOIasd;
                     pParam->B3SOIst3 = T2 /( 1 - model->B3SOIasd);
                     pParam->B3SOIst4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                      - pParam->B3SOIcsbox * model->B3SOIvsdth;
   
                     T1 = pParam->B3SOIcdmin - pParam->B3SOIcdbox;
                     T2 = T1 / T0 / T0;
                     pParam->B3SOIdt2 = T2 / model->B3SOIasd;
                     pParam->B3SOIdt3 = T2 /( 1 - model->B3SOIasd);
                     pParam->B3SOIdt4 =  T0 * T1 * (1 + model->B3SOIasd) / 3
                                      - pParam->B3SOIcdbox * model->B3SOIvsdth;
		  } 

                  T0 = model->B3SOIcsdesw * log(1 + model->B3SOItsi /
                       model->B3SOItbox);
                  T1 = here->B3SOIsourcePerimeter - here->B3SOIw;
                  if (T1 > 0.0)
                     pParam->B3SOIcsesw = T0 * T1; 
                  else
                     pParam->B3SOIcsesw = 0.0;
                  T1 = here->B3SOIdrainPerimeter - here->B3SOIw;
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

                  if ((T0 = pParam->B3SOIweff + model->B3SOIk1w2) < 1e-8)
                     T0 = 1e-8;
                  pParam->B3SOIk1eff = pParam->B3SOIk1 * (1 + model->B3SOIk1w1/T0);

	          if (model->B3SOIvth0Given)
		  {   pParam->B3SOIvfb = model->B3SOItype * pParam->B3SOIvth0 
                                       - pParam->B3SOIphi - pParam->B3SOIk1eff 
                                       * pParam->B3SOIsqrtPhi;
		  }
		  else
		  {   pParam->B3SOIvfb = -1.0;
		      pParam->B3SOIvth0 = model->B3SOItype * (pParam->B3SOIvfb
                                        + pParam->B3SOIphi + pParam->B3SOIk1eff 
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


/* v2.0 release */
              if (model->B3SOIln < 1e-15) model->B3SOIln = 1e-15;
              T0 = -0.5 * pParam->B3SOIleff * pParam->B3SOIleff / model->B3SOIln / model->B3SOIln;
              DEXP(T0,T1);
              pParam->B3SOIarfabjt = T1;

              T0 = model->B3SOIlbjt0 * (1.0 / pParam->B3SOIleff + 1.0 / model->B3SOIln);
              pParam->B3SOIlratio = pow(T0,model->B3SOInbjt);
              pParam->B3SOIlratiodif = 1.0 + model->B3SOIldif0 * pow(T0,model->B3SOIndif);

              if ((pParam->B3SOIvearly = model->B3SOIvabjt + model->B3SOIaely * pParam->B3SOIleff) < 1) 
                 pParam->B3SOIvearly = 1; 

              /* vfbzb calculation for capMod 3 */
              tmp = sqrt(pParam->B3SOIXdep0);
              tmp1 = pParam->B3SOIvbi - pParam->B3SOIphi;
              tmp2 = model->B3SOIfactor1 * tmp;

              T0 = -0.5 * pParam->B3SOIdvt1w * pParam->B3SOIweff
                 * pParam->B3SOIleff / tmp2;
              if (T0 > -EXP_THRESHOLD)
              {   T1 = exp(T0);
                  T2 = T1 * (1.0 + 2.0 * T1);
              }
              else
              {   T1 = MIN_EXP;
                  T2 = T1 * (1.0 + 2.0 * T1);
              }
              T0 = pParam->B3SOIdvt0w * T2;
              T2 = T0 * tmp1;

              T0 = -0.5 * pParam->B3SOIdvt1 * pParam->B3SOIleff / tmp2;
              if (T0 > -EXP_THRESHOLD)
              {   T1 = exp(T0);
                  T3 = T1 * (1.0 + 2.0 * T1);
              }
              else
              {   T1 = MIN_EXP;
                  T3 = T1 * (1.0 + 2.0 * T1);
              }
              T3 = pParam->B3SOIdvt0 * T3 * tmp1;

              T4 = model->B3SOItox * pParam->B3SOIphi
                 / (pParam->B3SOIweff + pParam->B3SOIw0);

              T0 = sqrt(1.0 + pParam->B3SOInlx / pParam->B3SOIleff);
              T5 = pParam->B3SOIk1eff * (T0 - 1.0) * pParam->B3SOIsqrtPhi
                 + (pParam->B3SOIkt1 + pParam->B3SOIkt1l / pParam->B3SOIleff)
                 * (TempRatio - 1.0);

              tmp3 = model->B3SOItype * pParam->B3SOIvth0
                   - T2 - T3 + pParam->B3SOIk3 * T4 + T5;
              pParam->B3SOIvfbzb = tmp3 - pParam->B3SOIphi - pParam->B3SOIk1eff
                                 * pParam->B3SOIsqrtPhi;
              /* End of vfbzb */

              pParam->B3SOIldeb = sqrt(EPSSI * Vtm0 / (Charge_q * pParam->B3SOInpeak * 1.0e6)) / 3.0;
              pParam->B3SOIacde = model->B3SOIacde * pow((pParam->B3SOInpeak / 2.0e16), -0.25);
              pParam->B3SOImoin = model->B3SOImoin;
         }
    }
    return(OK);
}

