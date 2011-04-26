/* $Id: BSIMFD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soild.c, FD2.1 $
 * Revision 2.1  99/9/27 Pin Su 
 * BSIMFD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soild.c, FD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: Weidong Liu and Pin Su         Feb 1999
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
Modified by Pin Su, Wei Jin 99/9/27
File: b3soild.c          98/5/01
**********/


#include "spice.h"
#include <stdio.h>
#include <math.h>
#include "util.h"
#include "cktdefs.h"
#include "b3soidef.h"
#include "trandefs.h"
#include "const.h"
#include "sperror.h"
#include "devdefs.h"
#include "suffix.h"

#define MAX_EXP 5.834617425e14
#define MIN_EXP 1.713908431e-15
#define EXP_THRESHOLD 34.0
#define EPSOX 3.453133e-11
#define EPSSI 1.03594e-10
#define Charge_q 1.60219e-19
#define KboQ 8.617087e-5  /*  Kb / q   */
#define Eg300 1.115   /*  energy gap at 300K  */
#define DELTA_1 0.02
#define DELTA_2 0.02
#define DELTA_3 0.02
#define DELTA_4 0.02
#define DELT_Vbs0eff 0.02
#define DELT_Vbsmos  0.005
#define DELT_Vbseff  0.005
#define DELT_Xcsat   0.2
#define DELT_Vbs0dio 1e-7
#define DELTA_VFB  0.02
#define DELTA_Vcscv  0.0004
#define DELT_Vbsdio 0.01
#define CONST_2OV3 0.6666666666
#define OFF_Vbsdio  2e-2
#define OFF_Vbs0_dio 2.02e-2
#define QEX_FACT  20


    /* B3SOISmartVbs(Vbs, Old, here, check)
     *  Smart Vbs guess.
     */

double
B3SOISmartVbs(New, Old, here, ckt, check)
   double New, Old;
   B3SOIinstance *here;
   CKTcircuit *ckt;
   int *check;
{
   double T0, T1, del;

   /* only do it for floating body and DC */
   if (here->B3SOIfloat && (ckt->CKTmode & (MODEDC | MODEDCOP)))
   {
      /* Vbs cannot be negative in DC */
      if (New < 0.0)  New = 0.0;
   }
   return(New);
}


    /* B3SOIlimit(vnew,vold)
     *  limits the per-iteration change of any absolute voltage value
     */

double
B3SOIlimit(vnew, vold, limit, check)
    double vnew;
    double vold;
    double limit;
    int *check;
{
    double T0, T1;

    if (isnan (vnew) || isnan (vold))
    {
	fprintf(stderr, "Alberto says:  YOU TURKEY!  The limiting function received NaN.\n");
	fprintf(stderr, "New prediction returns to 0.0!\n");
        vnew = 0.0;
        *check = 1;
    }
    T0 = vnew - vold;
    T1 = fabs(T0);
    if (T1 > limit) {
        if (T0 > 0.0) 
            vnew = vold + limit;
        else 
            vnew = vold - limit;
	*check = 1;
    }
    return vnew;
}



int
B3SOIload(inModel,ckt)
GENmodel *inModel;
register CKTcircuit *ckt;
{
register B3SOImodel *model = (B3SOImodel*)inModel;
register B3SOIinstance *here;
register int selfheat;

double SourceSatCurrent, DrainSatCurrent, Gmin;
double ag0, qgd, qgs, qgb, von, cbhat, VgstNVt, ExpVgst;
double cdhat, cdreq, ceqbd, ceqbs, ceqqb, ceqqd, ceqqg, ceq, geq;
double evbd, evbs, arg, sarg;
double delvbd, delvbs, delvds, delvgd, delvgs;
double Vfbeff, dVfbeff_dVg, dVfbeff_dVd, dVfbeff_dVb, V3, V4;
double tol, PhiB, PhiBSW, MJ, MJSW, PhiBSWG, MJSWG;
double gcgdb, gcggb, gcgsb, gcgeb, gcgT;
double gcsdb, gcsgb, gcssb, gcseb, gcsT;
double gcddb, gcdgb, gcdsb, gcdeb, gcdT;
double gcbdb, gcbgb, gcbsb, gcbeb, gcbT;
double gcedb, gcegb, gcesb, gceeb, gceT;
double gcTt, gTtg, gTtb, gTte, gTtdp, gTtt, gTtsp;
double vbd, vbs, vds, vgb, vgd, vgs, vgdo, xfact;
double vg, vd, vs, vp, ve, vb;
double Vds, Vgs, Vbs, Gmbs, FwdSum, RevSum;
double Vgs_eff, Vfb, dVfb_dVb, dVfb_dVd, dVfb_dT;
double Phis, dPhis_dVb, sqrtPhis, dsqrtPhis_dVb, Vth, dVth_dVb, dVth_dVd, dVth_dT;
double Vgst, dVgst_dVg, dVgst_dVb, dVgs_eff_dVg, Nvtm;
double Vgdt, Vgsaddvth, Vgsaddvth2, Vgsaddvth1o3, n, dn_dVb, Vtm;
double ExpArg, V0;
double ueff, dueff_dVg, dueff_dVd, dueff_dVb, dueff_dT;
double Esat, dEsat_dVg, dEsat_dVd, dEsat_dVb, Vdsat, Vdsat0;
double EsatL, dEsatL_dVg, dEsatL_dVd, dEsatL_dVb, dEsatL_dT;
double dVdsat_dVg, dVdsat_dVb, dVdsat_dVd, dVdsat_dT, Vasat, dAlphaz_dVg, dAlphaz_dVb;
double dVasat_dVg, dVasat_dVb, dVasat_dVd, dVasat_dT;
double Va, Va2, dVa_dVd, dVa_dVg, dVa_dVb, dVa_dT;
double Vbseff, dVbseff_dVb, VbseffCV, dVbseffCV_dVb;
double One_Third_CoxWL, Two_Third_CoxWL, Alphaz, CoxWL;
double dVgdt_dVg, dVgdt_dVd, dVgdt_dVb;
double T0, dT0_dVg, dT0_dVd, dT0_dVb, dT0_dVc, dT0_dVe, dT0_dVrg, dT0_dT;
double T1, dT1_dVg, dT1_dVd, dT1_dVb, dT1_dVc, dT1_dVe, dT1_dT;
double T2, dT2_dVg, dT2_dVd, dT2_dVb, dT2_dVc, dT2_dVe, dT2_dT;
double T3, dT3_dVg, dT3_dVd, dT3_dVb, dT3_dVc, dT3_dVe, dT3_dT;
double T4, dT4_dVg, dT4_dVd, dT4_dVb, dT4_dVc, dT4_dVe, dT4_dT;
double T5, dT5_dVg, dT5_dVd, dT5_dVb, dT5_dVc, dT5_dVe, dT5_dT;
double T6, dT6_dVg, dT6_dVd, dT6_dVb, dT6_dVc, dT6_dVe, dT6_dT;
double T7, dT7_dVg, dT7_dVd, dT7_dVb;
double T8, dT8_dVg, dT8_dVd, dT8_dVb, dT8_dVc, dT8_dVe, dT8_dVrg;
double T9, dT9_dVg, dT9_dVd, dT9_dVb, dT9_dVc, dT9_dVe, dT9_dVrg;
double T10, dT10_dVg, dT10_dVb, dT10_dVd;
double T11, T12;
double tmp, Abulk, dAbulk_dVb, Abulk0, dAbulk0_dVb;
double T100, T101;
double VACLM, dVACLM_dVg, dVACLM_dVd, dVACLM_dVb, dVACLM_dT;
double VADIBL, dVADIBL_dVg, dVADIBL_dVd, dVADIBL_dVb, dVADIBL_dT;
double VAHCE,  dVAHCE_dVg, dVAHCE_dVd, dVAHCE_dVb;
double Xdep, dXdep_dVb, lt1, dlt1_dVb, ltw, dltw_dVb;
double Delt_vth, dDelt_vth_dVb, dDelt_vth_dT;
double Theta0, dTheta0_dVb, Theta1, dTheta1_dVb;
double Thetarout, dThetarout_dVb, TempRatio, tmp1, tmp2, tmp3, tmp4;
double DIBL_Sft, dDIBL_Sft_dVd, DIBL_fact, Lambda, dLambda_dVg;
double Rout_Vgs_factor, dRout_Vgs_factor_dVg, dRout_Vgs_factor_dVb;
double dRout_Vgs_factor_dVd;
double tempv, a1;
 
double Vgsteff, dVgsteff_dVg, dVgsteff_dVd, dVgsteff_dVb, dVgsteff_dVe, dVgsteff_dT;
double Vdseff, dVdseff_dVg, dVdseff_dVd, dVdseff_dVb, dVdseff_dT;
double VdseffCV, dVdseffCV_dVg, dVdseffCV_dVd, dVdseffCV_dVb;
double diffVds, diffVdsCV;
double dAbulk_dVg, dn_dVd ;
double beta, dbeta_dVg, dbeta_dVd, dbeta_dVb, dbeta_dT;
double gche, dgche_dVg, dgche_dVd, dgche_dVb, dgche_dT;
double fgche1, dfgche1_dVg, dfgche1_dVd, dfgche1_dVb, dfgche1_dT;
double fgche2, dfgche2_dVg, dfgche2_dVd, dfgche2_dVb, dfgche2_dT;
double Idl, dIdl_dVg, dIdl_dVd, dIdl_dVb, dIdl_dT;
double Ids, Gm, Gds, Gmb;
double CoxWovL;
double Rds, dRds_dVg, dRds_dVb, dRds_dT, WVCox, WVCoxRds;
double Vgst2Vtm, dVgst2Vtm_dT, VdsatCV, dVdsatCV_dVd, dVdsatCV_dVg, dVdsatCV_dVb;
double Leff, Weff, dWeff_dVg, dWeff_dVb;
double AbulkCV, dAbulkCV_dVb;
double qgdo, qgso, cgdo, cgso;
 
double dxpart, sxpart;
 
struct b3soiSizeDependParam *pParam;
int ByPass, Check, ChargeComputationNeeded, J, error, I;
double junk[50];
 
double gbbsp, gbbdp, gbbg, gbbb, gbbe, gbbp, gbbT;
double gddpsp, gddpdp, gddpg, gddpb, gddpe, gddpT;
double gsspsp, gsspdp, gsspg, gsspb, gsspe, gsspT;
double Gbpbs, Gbpgs, Gbpds, Gbpes, Gbpps, GbpT;
double vse, vde, ves, ved, veb, vge, delves, vedo, delved;
double vps, vpd, Vps, delvps;
double Vbd, Ves, Vesfb, sqrtXdep, DeltVthtemp, dDeltVthtemp_dT;
double Vbp, dVbp_dVp, dVbp_dVb, dVbp_dVg, dVbp_dVd, dVbp_dVe, dVbp_dT;
double Vpsdio, dVpsdio_dVg, dVpsdio_dVd, dVpsdio_dVe, dVpsdio_dVp, dVpsdio_dT;
double DeltVthw, dDeltVthw_dVb, dDeltVthw_dT;
double dVbseff_dVd, dVbseff_dVe, dVbseff_dT;
double dVdsat_dVc, dVasat_dVc, dVACLM_dVc, dVADIBL_dVc, dVa_dVc;
double dfgche1_dVc, dfgche2_dVc, dgche_dVc, dVdseff_dVc, dIdl_dVc;
double Gm0, Gds0, Gmb0, GmT0, Gmc, Gme, GmT, dVbseff_dVg;
double dDIBL_Sft_dVb, BjtA, dBjtA_dVd;
double diffVdsii  ;
double Idgidl, Gdgidld, Gdgidlg, Isgidl, Gsgidlg;
double Gjsd, Gjss, Gjsb, GjsT, Gjdd, Gjdb, GjdT;
double Ibp, Iii, Giid, Giig, Giib, Giie, GiiT, Gcd, Gcb, GcT, ceqbody, ceqbodcon;
double gppg, gppdp, gppb, gppe, gppp, gppsp, gppT;
double delTemp, deldelTemp, Temp;
double ceqth, ceqqth;
double K1, WL;
double qjs, gcjsbs, gcjsT;
double qjd, gcjdbs, gcjdds, gcjdT;
double qge;
double ceqqe;
double ni, Eg, Cbox, Nfb, CboxWL;
double cjsbs;
double Qbf0, Qsicv, dVfbeff_dVrg, Cbe ;
double qinv, qgate, qbody, qdrn, qsrc, qsub, cqgate, cqbody, cqdrn, cqsub, cqtemp;
double Cgg, Cgd, Cgs, Cgb, Cge, Cdg, Cdd, Cds, Cdb, Qg, Qd;
double Csg, Csd, Css, Csb, Cse, Cbg, Cbd, Cbs, Cbb, Qs, Qb;
double Cgg1, Cgb1, Cgd1, Cbg1, Cbb1, Cbd1, Csg1, Csd1, Csb1;
double Vbs0t, dVbs0t_dT ;
double Vbs0 ,dVbs0_dVe, dVbs0_dT;
double Vbs0eff ,dVbs0eff_dVg ,dVbs0eff_dVd ,dVbs0eff_dVe, dVbs0eff_dT;
double Vbs0teff,dVbs0teff_dVg ,dVbs0teff_dVd, dVbs0teff_dVe, dVbs0teff_dT;
double Vbsdio, dVbsdio_dVg, dVbsdio_dVd, dVbsdio_dVe, dVbsdio_dVb, dVbsdio_dT;
double Vbseff0;
double Vthfd ,dVthfd_dVd ,dVthfd_dVe, dVthfd_dT;
double Vbs0mos ,dVbs0mos_dVe, dVbs0mos_dT;
double Vbsmos ,dVbsmos_dVg ,dVbsmos_dVb ,dVbsmos_dVd, dVbsmos_dVe, dVbsmos_dT;
double Abeff ,dAbeff_dVg ,dAbeff_dVb, dAbeff_dVc;
double Vcs ,dVcs_dVg ,dVcs_dVb ,dVcs_dVd ,dVcs_dVe, dVcs_dT;
double Xcsat ,dXcsat_dVg ,dXcsat_dVb, dXcsat_dVc;
double Vdsatii ,dVdsatii_dVg ,dVdsatii_dVd, dVdsatii_dVb, dVdsatii_dT;
double Vdseffii ,dVdseffii_dVg ,dVdseffii_dVd, dVdseffii_dVb, dVdseffii_dT;
double VcsCV ,dVcsCV_dVg ,dVcsCV_dVb ,dVcsCV_dVd  ,dVcsCV_dVc ,dVcsCV_dVe;
double VdsCV ,dVdsCV_dVg ,dVdsCV_dVb ,dVdsCV_dVd  ,dVdsCV_dVc;
double Phisc ,dPhisc_dVg ,dPhisc_dVb ,dPhisc_dVd,  dPhisc_dVc;
double Phisd ,dPhisd_dVg ,dPhisd_dVb ,dPhisd_dVd,  dPhisd_dVc;
double sqrtPhisc ,dsqrtPhisc_dVg  ,dsqrtPhisc_dVb;
double sqrtPhisd ,dsqrtPhisd_dVg  ,dsqrtPhisd_dVb;
double Xc ,dXc_dVg ,dXc_dVb ,dXc_dVd ,dXc_dVc;
double Ibjt ,dIbjt_dVb ,dIbjt_dVd ,dIbjt_dT;
double Ibs1 ,dIbs1_dVb ,dIbs1_dT;
double Ibs2 ,dIbs2_dVb ,dIbs2_dT;
double Ibs3 ,dIbs3_dVb ,dIbs3_dVd, dIbs3_dT;
double Ibs4 ,dIbs4_dVb ,dIbs4_dT;
double Ibd1 ,dIbd1_dVb ,dIbd1_dVd ,dIbd1_dT;
double Ibd2 ,dIbd2_dVb ,dIbd2_dVd ,dIbd2_dT;
double Ibd3 ,dIbd3_dVb ,dIbd3_dVd ,dIbd3_dT;
double Ibd4 ,dIbd4_dVb ,dIbd4_dVd ,dIbd4_dT;
double ExpVbs1, dExpVbs1_dVb, dExpVbs1_dT;
double ExpVbs2, dExpVbs2_dVb, dExpVbs2_dT;
double ExpVbs4, dExpVbs4_dVb, dExpVbs4_dT;
double ExpVbd1, dExpVbd1_dVb, dExpVbd1_dT;
double ExpVbd2, dExpVbd2_dVb, dExpVbd2_dT;
double ExpVbd4, dExpVbd4_dVb, dExpVbd4_dT;
double WTsi, NVtm1, NVtm2;
double Ic  ,dIc_dVb ,dIc_dVd;
double Ibs ,dIbs_dVb ,dIbs_dVd ,dIbs_dVe;
double Ibd ,dIbd_dVb;
double Nomi ,dNomi_dVg ,dNomi_dVb ,dNomi_dVd ,dNomi_dVc;
double Denomi ,dDenomi_dVg ,dDenomi_dVd ,dDenomi_dVb ,dDenomi_dVc, dDenomi_dT;
double Qbf ,dQbf_dVg ,dQbf_dVb ,dQbf_dVd ,dQbf_dVc ,dQbf_dVe;
double Qsubs1 ,dQsubs1_dVg  ,dQsubs1_dVb ,dQsubs1_dVd ,dQsubs1_dVc ,dQsubs1_dVe;
double Qsubs2 ,dQsubs2_dVg  ,dQsubs2_dVb ,dQsubs2_dVd ,dQsubs2_dVc ,dQsubs2_dVe;
double Qsub0  ,dQsub0_dVg   ,dQsub0_dVb  ,dQsub0_dVd ;
double Qac0 ,dQac0_dVb   ,dQac0_dVd;
double Qdep0 ,dQdep0_dVb;
double Qe1 , dQe1_dVg ,dQe1_dVb, dQe1_dVd, dQe1_dVe, dQe1_dT;
double Ce1g ,Ce1b ,Ce1d ,Ce1e, Ce1T;
double Ce2g ,Ce2b ,Ce2d ,Ce2e, Ce2T;
double Qe2 , dQe2_dVg ,dQe2_dVb, dQe2_dVd, dQe2_dVe, dQe2_dT;
double dQbf_dVrg, dQac0_dVrg, dQsub0_dVrg; 
double dQsubs1_dVrg, dQsubs2_dVrg, dQbf0_dVe, dQbf0_dT;

/*  for self-heating  */
double vbi, vfbb, phi, sqrtPhi, Xdep0, jbjt, jdif, jrec, jtun, u0temp, vsattemp;
double rds0, ua, ub, uc;
double dvbi_dT, dvfbb_dT, djbjt_dT, djdif_dT, djrec_dT, djtun_dT, du0temp_dT;
double dvsattemp_dT, drds0_dT, dua_dT, dub_dT, duc_dT, dni_dT, dVtm_dT;
double dVfbeff_dT, dQac0_dT, dQsub0_dT, dQbf_dT, dVdsCV_dT, dPhisd_dT;
double dNomi_dT,dXc_dT,dQsubs1_dT,dQsubs2_dT, dVcsCV_dT, dPhisc_dT, dQsicv_dT;
double CbT, CsT, CgT, CeT;

double Qex, dQex_dVg, dQex_dVb, dQex_dVd, dQex_dVe, dQex_dT;

/* clean up last */
FILE *fpdebug;
/* end clean up */
int nandetect;
static int nanfound = 0;
char nanmessage [12];



for (; model != NULL; model = model->B3SOInextModel)
{    for (here = model->B3SOIinstances; here != NULL; 
          here = here->B3SOInextInstance)
     {    Check = 0;
          ByPass = 0;
          selfheat = (model->B3SOIshMod == 1) && (here->B3SOIrth0 != 0.0);
	  pParam = here->pParam;

          if (here->B3SOIdebugMod > 3)
          {
             if (model->B3SOItype > 0)
                fpdebug = fopen("b3soin.log", "a");
             else
                fpdebug = fopen("b3soip.log", "a");

             fprintf(fpdebug, "******* Time : %.5e ******* Device:  %s  Iteration:  %d\n",
                     ckt->CKTtime, here->B3SOIname, here->B3SOIiterations);
          }

          if ((ckt->CKTmode & MODEINITSMSIG))
	  {   vbs = *(ckt->CKTstate0 + here->B3SOIvbs);
              vgs = *(ckt->CKTstate0 + here->B3SOIvgs);
              ves = *(ckt->CKTstate0 + here->B3SOIves);
              vps = *(ckt->CKTstate0 + here->B3SOIvps);
              vds = *(ckt->CKTstate0 + here->B3SOIvds);
              delTemp = *(ckt->CKTstate0 + here->B3SOIdeltemp);

              vg = *(ckt->CKTrhsOld + here->B3SOIgNode);
              vd = *(ckt->CKTrhsOld + here->B3SOIdNodePrime);
              vs = *(ckt->CKTrhsOld + here->B3SOIsNodePrime);
              vp = *(ckt->CKTrhsOld + here->B3SOIpNode);
              ve = *(ckt->CKTrhsOld + here->B3SOIeNode);
              vb = *(ckt->CKTrhsOld + here->B3SOIbNode);

              if (here->B3SOIdebugMod > 2)
              {
                  fprintf(fpdebug, "... INIT SMSIG ...\n");
              }
              if (here->B3SOIdebugMod > 0)
              {
                 fprintf(stderr,"DC op. point converge with %d iterations\n");
              }
          }
	  else if ((ckt->CKTmode & MODEINITTRAN))
	  {   vbs = *(ckt->CKTstate1 + here->B3SOIvbs);
              vgs = *(ckt->CKTstate1 + here->B3SOIvgs);
              ves = *(ckt->CKTstate1 + here->B3SOIves);
              vps = *(ckt->CKTstate1 + here->B3SOIvps);
              vds = *(ckt->CKTstate1 + here->B3SOIvds);
              delTemp = *(ckt->CKTstate1 + here->B3SOIdeltemp);

              vg = *(ckt->CKTrhsOld + here->B3SOIgNode);
              vd = *(ckt->CKTrhsOld + here->B3SOIdNodePrime);
              vs = *(ckt->CKTrhsOld + here->B3SOIsNodePrime);
              vp = *(ckt->CKTrhsOld + here->B3SOIpNode);
              ve = *(ckt->CKTrhsOld + here->B3SOIeNode);
              vb = *(ckt->CKTrhsOld + here->B3SOIbNode);

              if (here->B3SOIdebugMod > 2)
              {
                 fprintf(fpdebug, "... Init Transient ....\n");
              }
              if (here->B3SOIdebugMod > 0)
              {
                 fprintf(stderr, "Transient operation point converge with %d iterations\n",
here->B3SOIiterations);  
              }
              here->B3SOIiterations = 0;
          }
	  else if ((ckt->CKTmode & MODEINITJCT) && !here->B3SOIoff)
	  {   vds = model->B3SOItype * here->B3SOIicVDS;
              vgs = model->B3SOItype * here->B3SOIicVGS;
              ves = model->B3SOItype * here->B3SOIicVES;
              vbs = model->B3SOItype * here->B3SOIicVBS;
              vps = model->B3SOItype * here->B3SOIicVPS;

	      vg = vd = vs = vp = ve = 0.0;

              here->B3SOIiterations = 0;  /*  initialize iteration number  */

              delTemp = 0.0;
	      here->B3SOIphi = pParam->B3SOIphi;


              if (here->B3SOIdebugMod > 2)
		fprintf(fpdebug, "... INIT JCT ...\n");

	      if ((vds == 0.0) && (vgs == 0.0) && (vbs == 0.0) && 
	         ((ckt->CKTmode & (MODETRAN | MODEAC|MODEDCOP |
		   MODEDCTRANCURVE)) || (!(ckt->CKTmode & MODEUIC))))
	      {   vbs = 0.0;
		  vgs = model->B3SOItype*0.1 + pParam->B3SOIvth0;
		  vds = 0.0;
		  ves = 0.0;
		  vps = 0.0;
	      }
	  }
	  else if ((ckt->CKTmode & (MODEINITJCT | MODEINITFIX)) && 
		  (here->B3SOIoff)) 
	  {    delTemp = vps = vbs = vgs = vds = ves = 0.0;
               vg = vd = vs = vp = ve = 0.0;
               here->B3SOIiterations = 0;  /*  initialize iteration number  */
	  }
	  else
	  {
#ifndef PREDICTOR
	       if ((ckt->CKTmode & MODEINITPRED))
	       {   xfact = ckt->CKTdelta / ckt->CKTdeltaOld[1];
		   *(ckt->CKTstate0 + here->B3SOIvbs) = 
			 *(ckt->CKTstate1 + here->B3SOIvbs);
		   vbs = (1.0 + xfact)* (*(ckt->CKTstate1 + here->B3SOIvbs))
			 - (xfact * (*(ckt->CKTstate2 + here->B3SOIvbs)));
		   *(ckt->CKTstate0 + here->B3SOIvgs) = 
			 *(ckt->CKTstate1 + here->B3SOIvgs);
		   vgs = (1.0 + xfact)* (*(ckt->CKTstate1 + here->B3SOIvgs))
			 - (xfact * (*(ckt->CKTstate2 + here->B3SOIvgs)));
		   *(ckt->CKTstate0 + here->B3SOIves) = 
			 *(ckt->CKTstate1 + here->B3SOIves);
		   ves = (1.0 + xfact)* (*(ckt->CKTstate1 + here->B3SOIves))
			 - (xfact * (*(ckt->CKTstate2 + here->B3SOIves)));
		   *(ckt->CKTstate0 + here->B3SOIvps) = 
			 *(ckt->CKTstate1 + here->B3SOIvps);
		   vps = (1.0 + xfact)* (*(ckt->CKTstate1 + here->B3SOIvps))
			 - (xfact * (*(ckt->CKTstate2 + here->B3SOIvps)));
		   *(ckt->CKTstate0 + here->B3SOIvds) = 
			 *(ckt->CKTstate1 + here->B3SOIvds);
		   vds = (1.0 + xfact)* (*(ckt->CKTstate1 + here->B3SOIvds))
			 - (xfact * (*(ckt->CKTstate2 + here->B3SOIvds)));
		   *(ckt->CKTstate0 + here->B3SOIvbd) = 
			 *(ckt->CKTstate0 + here->B3SOIvbs)
			 - *(ckt->CKTstate0 + here->B3SOIvds);

                   *(ckt->CKTstate0 + here->B3SOIvg) = *(ckt->CKTstate1 + here->B3SOIvg);
                   *(ckt->CKTstate0 + here->B3SOIvd) = *(ckt->CKTstate1 + here->B3SOIvd);
                   *(ckt->CKTstate0 + here->B3SOIvs) = *(ckt->CKTstate1 + here->B3SOIvs);
                   *(ckt->CKTstate0 + here->B3SOIvp) = *(ckt->CKTstate1 + here->B3SOIvp);
                   *(ckt->CKTstate0 + here->B3SOIve) = *(ckt->CKTstate1 + here->B3SOIve);

                   /* Only predict ve */
                   ve = (1.0 + xfact)* (*(ckt->CKTstate1 + here->B3SOIve))

                        - (xfact * (*(ckt->CKTstate2 + here->B3SOIve)));
                   /* Then update vg, vs, vb, vd, vp base on ve */
                   vs = ve - model->B3SOItype * ves;
                   vg = model->B3SOItype * vgs + vs;
                   vd = model->B3SOItype * vds + vs;
                   vb = model->B3SOItype * vbs + vs;
                   vp = model->B3SOItype * vps + vs;

		   delTemp = (1.0 + xfact)* (*(ckt->CKTstate1 +
			 here->B3SOIdeltemp))-(xfact * (*(ckt->CKTstate2 +
			 here->B3SOIdeltemp)));

		   if (selfheat)
		   {
		       here->B3SOIphi = 2.0 * here->B3SOIvtm
					* log (pParam->B3SOInpeak /
					       here->B3SOIni); 
		   }

		   if (here->B3SOIdebugMod > 0)
		   {
                      fprintf(stderr, "Time = %.6e converge with %d iterations\n", ckt->CKTtime, here->B3SOIiterations);  
                   }
		   if (here->B3SOIdebugMod > 2)
		   {
		      fprintf(fpdebug, "... PREDICTOR calculation ....\n");
		   }
                   here->B3SOIiterations = 0;
	       }
	       else
	       {
#endif /* PREDICTOR */

                   vg = B3SOIlimit(*(ckt->CKTrhsOld + here->B3SOIgNode),
                                 *(ckt->CKTstate0 + here->B3SOIvg), 3.0, &Check);
                   vd = B3SOIlimit(*(ckt->CKTrhsOld + here->B3SOIdNodePrime),
                                 *(ckt->CKTstate0 + here->B3SOIvd), 3.0, &Check);
                   vs = B3SOIlimit(*(ckt->CKTrhsOld + here->B3SOIsNodePrime),
                                 *(ckt->CKTstate0 + here->B3SOIvs), 3.0, &Check);
                   vp = B3SOIlimit(*(ckt->CKTrhsOld + here->B3SOIpNode),
                                 *(ckt->CKTstate0 + here->B3SOIvp), 3.0, &Check);
                   ve = B3SOIlimit(*(ckt->CKTrhsOld + here->B3SOIeNode),
                                 *(ckt->CKTstate0 + here->B3SOIve), 3.0, &Check);
                   delTemp = *(ckt->CKTrhsOld + here->B3SOItempNode);

		   vbs = model->B3SOItype * (*(ckt->CKTrhsOld+here->B3SOIbNode)
                                - *(ckt->CKTrhsOld+here->B3SOIsNodePrime));

		   vps = model->B3SOItype * (vp - vs);
		   vgs = model->B3SOItype * (vg - vs);
		   ves = model->B3SOItype * (ve - vs);
		   vds = model->B3SOItype * (vd - vs);

		   if (here->B3SOIdebugMod > 2)
		   {
		      fprintf(fpdebug, "... DC calculation ....\n");
fprintf(fpdebug, "Vg = %.10f; Vb = %.10f; Vs = %.10f\n",
			 *(ckt->CKTrhsOld + here->B3SOIgNode),
			 *(ckt->CKTrhsOld + here->B3SOIbNode),
			 *(ckt->CKTrhsOld + here->B3SOIsNode));
fprintf(fpdebug, "Vd = %.10f; Vsp = %.10f; Vdp = %.10f\n",
			 *(ckt->CKTrhsOld + here->B3SOIdNode),
			 *(ckt->CKTrhsOld + here->B3SOIsNodePrime),
			 *(ckt->CKTrhsOld + here->B3SOIdNodePrime));
fprintf(fpdebug, "Ve = %.10f; Vp = %.10f; delTemp = %.10f\n",
			 *(ckt->CKTrhsOld + here->B3SOIeNode),
			 *(ckt->CKTrhsOld + here->B3SOIpNode),
			 *(ckt->CKTrhsOld + here->B3SOItempNode));
                     
		   }

#ifndef PREDICTOR
	       }
#endif /* PREDICTOR */

	       vbd = vbs - vds;
	       vgd = vgs - vds;
               ved = ves - vds;
	       vgdo = *(ckt->CKTstate0 + here->B3SOIvgs)
		    - *(ckt->CKTstate0 + here->B3SOIvds);
	       vedo = *(ckt->CKTstate0 + here->B3SOIves)
		    - *(ckt->CKTstate0 + here->B3SOIvds);
	       delvbs = vbs - *(ckt->CKTstate0 + here->B3SOIvbs);
	       delvbd = vbd - *(ckt->CKTstate0 + here->B3SOIvbd);
	       delvgs = vgs - *(ckt->CKTstate0 + here->B3SOIvgs);
	       delves = ves - *(ckt->CKTstate0 + here->B3SOIves);
	       delvps = vps - *(ckt->CKTstate0 + here->B3SOIvps);
	       deldelTemp = delTemp - *(ckt->CKTstate0 + here->B3SOIdeltemp);
	       delvds = vds - *(ckt->CKTstate0 + here->B3SOIvds);
	       delvgd = vgd - vgdo;
               delved = ved - vedo;

	       if (here->B3SOImode >= 0) 
	       {   
                   cdhat = here->B3SOIcd + (here->B3SOIgm-here->B3SOIgjdg) * delvgs
                         + (here->B3SOIgds - here->B3SOIgjdd) * delvds
                         + (here->B3SOIgmbs - here->B3SOIgjdb) * delvbs
                         + (here->B3SOIgme - here->B3SOIgjde) * delves
			 + (here->B3SOIgmT - here->B3SOIgjdT) * deldelTemp;
	       }
	       else
	       {   
                   cdhat = here->B3SOIcd + (here->B3SOIgm-here->B3SOIgjdg) * delvgd
                         - (here->B3SOIgds - here->B3SOIgjdd) * delvds
                         + (here->B3SOIgmbs - here->B3SOIgjdb) * delvbd
                         + (here->B3SOIgme - here->B3SOIgjde) * delved
                         + (here->B3SOIgmT - here->B3SOIgjdT) * deldelTemp;

	       }
	       cbhat = here->B3SOIcb + here->B3SOIgbgs * delvgs
		     + here->B3SOIgbbs * delvbs + here->B3SOIgbds * delvds
                     + here->B3SOIgbes * delves + here->B3SOIgbps * delvps
		     + here->B3SOIgbT * deldelTemp;

#ifndef NOBYPASS
	   /* following should be one big if connected by && all over
	    * the place, but some C compilers can't handle that, so
	    * we split it up here to let them digest it in stages
	    */

	       if (here->B3SOIdebugMod > 3)
               {
fprintf(fpdebug, "Convergent Criteria : vbs %d  vds %d  vgs %d  ves %d  vps %d  temp %d\n",
	((FABS(delvbs) < (ckt->CKTreltol * MAX(FABS(vbs),
	FABS(*(ckt->CKTstate0+here->B3SOIvbs))) + ckt->CKTvoltTol))) ? 1 : 0,
	((FABS(delvds) < (ckt->CKTreltol * MAX(FABS(vds),
	FABS(*(ckt->CKTstate0+here->B3SOIvds))) + ckt->CKTvoltTol))) ? 1 : 0,
	((FABS(delvgs) < (ckt->CKTreltol * MAX(FABS(vgs),
	FABS(*(ckt->CKTstate0+here->B3SOIvgs))) + ckt->CKTvoltTol))) ? 1 : 0,
	((FABS(delves) < (ckt->CKTreltol * MAX(FABS(ves),
	FABS(*(ckt->CKTstate0+here->B3SOIves))) + ckt->CKTvoltTol))) ? 1 : 0,
	((FABS(delvps) < (ckt->CKTreltol * MAX(FABS(vps),
	FABS(*(ckt->CKTstate0+here->B3SOIvps))) + ckt->CKTvoltTol))) ? 1 : 0,
	((FABS(deldelTemp) < (ckt->CKTreltol * MAX(FABS(delTemp),
	FABS(*(ckt->CKTstate0+here->B3SOIdeltemp))) + ckt->CKTvoltTol*1e4))) ? 1 : 0);
fprintf(fpdebug, "delCd %.4e, delCb %.4e\n",  FABS(cdhat - here->B3SOIcd) ,
        FABS(cbhat - here->B3SOIcb));

               }
	       if ((!(ckt->CKTmode & MODEINITPRED)) && (ckt->CKTbypass) && Check == 0)
	       if ((FABS(delvbs) < (ckt->CKTreltol * MAX(FABS(vbs),
		   FABS(*(ckt->CKTstate0+here->B3SOIvbs))) + ckt->CKTvoltTol))  )
	       if ((FABS(delvbd) < (ckt->CKTreltol * MAX(FABS(vbd),
		   FABS(*(ckt->CKTstate0+here->B3SOIvbd))) + ckt->CKTvoltTol))  )
	       if ((FABS(delvgs) < (ckt->CKTreltol * MAX(FABS(vgs),
		   FABS(*(ckt->CKTstate0+here->B3SOIvgs))) + ckt->CKTvoltTol)))
	       if ((FABS(delves) < (ckt->CKTreltol * MAX(FABS(ves),
		   FABS(*(ckt->CKTstate0+here->B3SOIves))) + ckt->CKTvoltTol)))
	       if ( (here->B3SOIbodyMod == 0) || (here->B3SOIbodyMod == 2) ||
                  (FABS(delvps) < (ckt->CKTreltol * MAX(FABS(vps),
		   FABS(*(ckt->CKTstate0+here->B3SOIvps))) + ckt->CKTvoltTol)) )
	       if ( (here->B3SOItempNode == 0)  ||
                  (FABS(deldelTemp) < (ckt->CKTreltol * MAX(FABS(delTemp),
		   FABS(*(ckt->CKTstate0+here->B3SOIdeltemp)))
		   + ckt->CKTvoltTol*1e4)))
	       if ((FABS(delvds) < (ckt->CKTreltol * MAX(FABS(vds),
		   FABS(*(ckt->CKTstate0+here->B3SOIvds))) + ckt->CKTvoltTol)))
	       if ((FABS(cdhat - here->B3SOIcd) < ckt->CKTreltol 
		   * MAX(FABS(cdhat),FABS(here->B3SOIcd)) + ckt->CKTabstol)) 
	       if ((FABS(cbhat - here->B3SOIcb) < ckt->CKTreltol 
		   * MAX(FABS(cbhat),FABS(here->B3SOIcb)) + ckt->CKTabstol) )
	       {   /* bypass code */
	           vbs = *(ckt->CKTstate0 + here->B3SOIvbs);
	           vbd = *(ckt->CKTstate0 + here->B3SOIvbd);
	           vgs = *(ckt->CKTstate0 + here->B3SOIvgs);
	           ves = *(ckt->CKTstate0 + here->B3SOIves);
	           vps = *(ckt->CKTstate0 + here->B3SOIvps);
	           vds = *(ckt->CKTstate0 + here->B3SOIvds);
	           delTemp = *(ckt->CKTstate0 + here->B3SOIdeltemp);

		   /*  calculate Vds for temperature conductance calculation
		       in bypass (used later when filling Temp node matrix)  */
		   Vds = here->B3SOImode > 0 ? vds : -vds;

	           vgd = vgs - vds;
	           vgb = vgs - vbs;
                   veb = ves - vbs;

	           if (here->B3SOIdebugMod > 2)
	           {
fprintf(stderr, "Bypass for %s...\n", here->B3SOIname);
	    	      fprintf(fpdebug, "... By pass  ....\n");
		      fprintf(fpdebug, "vgs=%.4f, vds=%.4f, vbs=%.4f, ",
			   vgs, vds, vbs);
	    	      fprintf(fpdebug, "ves=%.4f, vps=%.4f\n", ves, vps);
		   }
		   if ((ckt->CKTmode & (MODETRAN | MODEAC)) || 
		      ((ckt->CKTmode & MODETRANOP) && (ckt->CKTmode & MODEUIC)))
		   {   ByPass = 1;
		       goto line755;
		   }
		   else
		   {   goto line850;
		   }
	       }


#endif /*NOBYPASS*/
		       von = here->B3SOIvon;

			if ((here->B3SOIdebugMod > 1) || (here->B3SOIdebugMod == -1))
		       {
			   here->B3SOIdum1 = here->B3SOIdum2 = here->B3SOIdum3 = 0.0;
                           here->B3SOIdum4 = here->B3SOIdum5 = 0.0;
			   Qac0 = Qsub0 = Qsubs1 = Qsubs2 = Qbf = Qe1 = Qe2 = 0.0;
			   qjs = qjd = Cbg = Cbb = Cbd = Cbe = Xc = qdrn = qgate = 0.0;
			   qbody = qsub = 0.0;
		       }

		       if (here->B3SOIdebugMod > 2) {
			      fprintf(fpdebug, "Limited : vgs = %.8f\n", vgs);
			      fprintf(fpdebug, "Limited : vds = %.8f\n", vds);
		       }

                       if (*(ckt->CKTstate0 + here->B3SOIvds) >= 0.0)
                          T0 = *(ckt->CKTstate0 + here->B3SOIvbs);
                       else
                          T0 = *(ckt->CKTstate0 + here->B3SOIvbd);

		       if (here->B3SOIdebugMod > 2)
			  fprintf(fpdebug, "Before lim : vbs = %.8f, after = ", T0);

		       if (vds >= 0.0) 
		       {   
		           vbs = B3SOIlimit(vbs, T0, 0.2, &Check);
                           vbs = B3SOISmartVbs(vbs, T0, here, ckt, &Check);
			   vbd = vbs - vds;
                           vb = model->B3SOItype * vbs + vs;
		           if (here->B3SOIdebugMod > 2)
			      fprintf(fpdebug, "%.8f\n", vbs);
		       } else 
		       {   
		           vbd = B3SOIlimit(vbd, T0, 0.2, &Check);
                           vbd = B3SOISmartVbs(vbd, T0, here, ckt, &Check);
			   vbs = vbd + vds;
                           vb = model->B3SOItype * vbs + vd;
		           if (here->B3SOIdebugMod > 2)
			      fprintf(fpdebug, "%.8f\n", vbd);
		       }

		       delTemp =B3SOIlimit(delTemp, *(ckt->CKTstate0 + here->B3SOIdeltemp),5.0,&Check);

                  }

/*  Calculate temperature dependent values for self-heating effect  */
		  Temp = delTemp + ckt->CKTtemp;
/* for debugging  
  Temp = ckt->CKTtemp;
  selfheat = 1;
  if (here->B3SOIname[1] == '2')
  {
     Temp += 0.01; 
  } */
		  TempRatio = Temp / model->B3SOItnom;

		  if (selfheat) {
		      Vtm = KboQ * Temp;

                      T0 = 1108.0 + Temp;
		      T5 = Temp * Temp;
		      Eg = 1.16 - 7.02e-4 * T5 / T0;
		      T1 = ((7.02e-4 * T5) - T0 * (14.04e-4 * Temp)) / T0 / T0;
                      /*  T1 = dEg / dT  */

                      T2 = 1.9230584e-4;  /*  T2 = 1 / 300.15^(3/2)  */
		      T5 = sqrt(Temp);
                      T3 = 1.45e10 * Temp * T5 * T2;
                      T4 = exp(21.5565981 - Eg / (2.0 * Vtm));
		      ni = T3 * T4;
                      dni_dT = 2.175e10 * T2 * T5 * T4 + T3 * T4 *
                               (-Vtm * T1 + Eg * KboQ) / (2.0 * Vtm * Vtm);

                      T0 = log(1.0e20 * pParam->B3SOInpeak / (ni * ni));
		      vbi = Vtm * T0;
                      dvbi_dT = KboQ * T0 + Vtm * (-2.0 * dni_dT / ni);

		      if (pParam->B3SOInsub > 0) {
                         T0 = log(pParam->B3SOInpeak / pParam->B3SOInsub);
		         vfbb = -model->B3SOItype * Vtm*T0;
                         dvfbb_dT = -model->B3SOItype * KboQ*T0;
                      } 
		      else {
                         T0 = log(-pParam->B3SOInpeak*pParam->B3SOInsub/ni/ni);
		         vfbb = -model->B3SOItype * Vtm*T0;
                         dvfbb_dT = -model->B3SOItype *
                                   (KboQ * T0 + Vtm * 2.0 * dni_dT / ni);
                      }

/*		      phi = 2.0 * Vtm * log(pParam->B3SOInpeak / ni);  */
		      phi = here->B3SOIphi;
		      sqrtPhi = sqrt(phi);
		      Xdep0 = sqrt(2.0 * EPSSI / (Charge_q
				         * pParam->B3SOInpeak * 1.0e6))
				         * sqrtPhi;
		      /*  Save the values below for phi calculation in B3SOIaccept()  */
		      here->B3SOIvtm = Vtm;
		      here->B3SOIni = ni;

                      /*  Use dTx_dVe variables to act as dTx_dT variables  */

                      T8 = 1 / model->B3SOItnom;
                      T7 = model->B3SOIxbjt / pParam->B3SOIndiode;
		      T0 = pow(TempRatio, T7);
                      dT0_dVe = T7 * pow(TempRatio, T7 - 1.0) * T8;

                      T7 = model->B3SOIxdif / pParam->B3SOIndiode;
		      T1 = pow(TempRatio, T7);
                      dT1_dVe = T7 * pow(TempRatio, T7 - 1.0) * T8;

                      T7 = model->B3SOIxrec / pParam->B3SOIndiode / 2.0;
		      T2 = pow(TempRatio, T7);
                      dT2_dVe = T7 * pow(TempRatio, T7 - 1.0) * T8;

		      T3 = TempRatio - 1.0;
		      T4 = Eg300 / pParam->B3SOIndiode / Vtm * T3;
                      dT4_dVe = Eg300 / pParam->B3SOIndiode / Vtm / Vtm *
                                (Vtm * T8 - T3 * KboQ);
		      T5 = exp(T4);
                      dT5_dVe = dT4_dVe * T5;
		      T6 = sqrt(T5);
                      dT6_dVe = 0.5 / T6 * dT5_dVe;

		      jbjt = pParam->B3SOIisbjt * T0 * T5;
		      jdif = pParam->B3SOIisdif * T1 * T5;
		      jrec = pParam->B3SOIisrec * T2 * T6;
                      djbjt_dT = pParam->B3SOIisbjt * (T0 * dT5_dVe + T5 * dT0_dVe);
                      djdif_dT = pParam->B3SOIisdif * (T1 * dT5_dVe + T5 * dT1_dVe);
                      djrec_dT = pParam->B3SOIisrec * (T2 * dT6_dVe + T6 * dT2_dVe);

                      T7 = model->B3SOIxtun / pParam->B3SOIntun;
		      T0 = pow(TempRatio, T7);
		      jtun = pParam->B3SOIistun * T0;
                      djtun_dT = pParam->B3SOIistun * T7 * pow(TempRatio, T7 - 1.0) * T8;

		      u0temp = pParam->B3SOIu0 * pow(TempRatio, pParam->B3SOIute);
                      du0temp_dT = pParam->B3SOIu0 * pParam->B3SOIute *
                                   pow(TempRatio, pParam->B3SOIute - 1.0) * T8;

		      vsattemp = pParam->B3SOIvsat - pParam->B3SOIat * T3;
                      dvsattemp_dT = -pParam->B3SOIat * T8;

		      rds0 = (pParam->B3SOIrdsw + pParam->B3SOIprt
		          * T3) / pParam->B3SOIrds0denom;
                      drds0_dT = pParam->B3SOIprt / pParam->B3SOIrds0denom * T8;

		      ua = pParam->B3SOIuatemp + pParam->B3SOIua1 * T3;
		      ub = pParam->B3SOIubtemp + pParam->B3SOIub1 * T3;
		      uc = pParam->B3SOIuctemp + pParam->B3SOIuc1 * T3;
                      dua_dT = pParam->B3SOIua1 * T8;
                      dub_dT = pParam->B3SOIub1 * T8;
                      duc_dT = pParam->B3SOIuc1 * T8;
		  }
		  else {
                      vbi = pParam->B3SOIvbi;
                      vfbb = pParam->B3SOIvfbb;
                      phi = pParam->B3SOIphi;
                      sqrtPhi = pParam->B3SOIsqrtPhi;
                      Xdep0 = pParam->B3SOIXdep0;
                      jbjt = pParam->B3SOIjbjt;
                      jdif = pParam->B3SOIjdif;
                      jrec = pParam->B3SOIjrec;
                      jtun = pParam->B3SOIjtun;
                      u0temp = pParam->B3SOIu0temp;
                      vsattemp = pParam->B3SOIvsattemp;
                      rds0 = pParam->B3SOIrds0;
                      ua = pParam->B3SOIua;
                      ub = pParam->B3SOIub;
                      uc = pParam->B3SOIuc;
                      dni_dT = dvbi_dT = dvfbb_dT = djbjt_dT = djdif_dT = 0.0;
                      djrec_dT = djtun_dT = du0temp_dT = dvsattemp_dT = 0.0;
                      drds0_dT = dua_dT = dub_dT = duc_dT = 0.0;
		  }
		  
		  /* TempRatio used for Vth and mobility */
		  if (selfheat) {
		      TempRatio = Temp / model->B3SOItnom - 1.0;
		  }
		  else {
		      TempRatio =  ckt->CKTtemp / model->B3SOItnom - 1.0;
		  }

		  /* determine DC current and derivatives */
		  vbd = vbs - vds;
		  vgd = vgs - vds;
		  vgb = vgs - vbs;
		  ved = ves - vds;
		  veb = ves - vbs;
		  vge = vgs - ves;
		  vpd = vps - vds;


		  if (vds >= 0.0)
		  {   /* normal mode */
		      here->B3SOImode = 1;
		      Vds = vds;
		      Vgs = vgs;
		      Vbs = vbs;
		      Vbd = vbd;
		      Ves = ves;
		      Vps = vps;
		  }
		  else
		  {   /* inverse mode */
		      here->B3SOImode = -1;
		      Vds = -vds;
		      Vgs = vgd;
		      Vbs = vbd;
		      Vbd = vbs;
		      Ves = ved;
		      Vps = vpd;
		  }


                  if (here->B3SOIdebugMod > 2)
		  {
		     fprintf(fpdebug, "Vgs=%.4f, Vds=%.4f, Vbs=%.4f, ",
			   Vgs, Vds, Vbs);
		     fprintf(fpdebug, "Ves=%.4f, Vps=%.4f, Temp=%.1f\n", 
			   Ves, Vps, Temp);
		  }

		  Vesfb = Ves - vfbb;
		  Cbox = model->B3SOIcbox;
		  K1 = pParam->B3SOIk1;

		  ChargeComputationNeeded =  
			 ((ckt->CKTmode & (MODEAC | MODETRAN | MODEINITSMSIG)) ||
			 ((ckt->CKTmode & MODETRANOP) && (ckt->CKTmode & MODEUIC)))
			 ? 1 : 0;

                  if (here->B3SOIdebugMod == -1)
                     ChargeComputationNeeded = 1;
                  



/* Poly Gate Si Depletion Effect */
		  T0 = pParam->B3SOIvfb + phi;
		  if ((pParam->B3SOIngate > 1.e18) && (pParam->B3SOIngate < 1.e25) 
		       && (Vgs > T0))
		  /* added to avoid the problem caused by ngate */
		  {   T1 = 1.0e6 * Charge_q * EPSSI * pParam->B3SOIngate
			 / (model->B3SOIcox * model->B3SOIcox);
		      T4 = sqrt(1.0 + 2.0 * (Vgs - T0) / T1);
		      T2 = T1 * (T4 - 1.0);
		      T3 = 0.5 * T2 * T2 / T1; /* T3 = Vpoly */
		      T7 = 1.12 - T3 - 0.05;
		      T6 = sqrt(T7 * T7 + 0.224);
		      T5 = 1.12 - 0.5 * (T7 + T6);
		      Vgs_eff = Vgs - T5;
		      dVgs_eff_dVg = 1.0 - (0.5 - 0.5 / T4) * (1.0 + T7 / T6); 
		  }
		  else
		  {   Vgs_eff = Vgs;
		      dVgs_eff_dVg = 1.0;
		  }


		  Leff = pParam->B3SOIleff;

		  if (selfheat) {
		      Vtm = KboQ * Temp;
                      dVtm_dT = KboQ;
		  }
		  else {
		      Vtm = model->B3SOIvtm;
                      dVtm_dT = 0.0;
		  }

		  V0 = vbi - phi;

/* Prepare Vbs0t */
		      T0 = -pParam->B3SOIdvbd1 * pParam->B3SOIleff / pParam->B3SOIlitl;
		      T1 = pParam->B3SOIdvbd0 * (exp(0.5*T0) + 2*exp(T0));
		      T2 = T1 * (vbi - phi);
		      T3 = 0.5 * model->B3SOIqsi / model->B3SOIcsi;
		      Vbs0t = phi - T3 + pParam->B3SOIvbsa + T2;
                      if (selfheat)
                         dVbs0t_dT = T1 * dvbi_dT;
                      else
                         dVbs0t_dT = 0.0;

/* Prepare Vbs0 */
			  T0 = 1 + model->B3SOIcsieff / Cbox;
                          T1 = pParam->B3SOIkb1 / T0;
			  T2 = T1 * (Vbs0t - Vesfb);

                          /* T6 is Vbs0 before limiting */
                          T6 = Vbs0t - T2;
                          dT6_dVe = T1;
                          if (selfheat)
                             dT6_dT = dVbs0t_dT - T1 * (dVbs0t_dT + dvfbb_dT);
                          else
                             dT6_dT = 0.0;
 
                          /* limit Vbs0 to below phi */
                          T1 = phi - pParam->B3SOIdelp;
                          T2 = T1 - T6 - DELT_Vbseff;
                          T3 = sqrt(T2 * T2 + 4.0 * DELT_Vbseff);
                          Vbs0 = T1 - 0.5 * (T2 + T3);
                          T4 = 0.5 * (1 + T2/T3);
                          dVbs0_dVe = T4 * dT6_dVe;
                          if (selfheat)  dVbs0_dT = T4 * dT6_dT;
                          else  dVbs0_dT = 0.0;

			  T1 = Vbs0t - Vbs0 - DELT_Vbsmos;
			  T2 = sqrt(T1 * T1 + DELT_Vbsmos * DELT_Vbsmos);
			  T3 = 0.5 * (T1 + T2);
			  T4 = T3 * model->B3SOIcsieff / model->B3SOIqsieff;
			  Vbs0mos = Vbs0 - 0.5 * T3 * T4;
                          T5 = 0.5 * T4 * (1 + T1 / T2);
			  dVbs0mos_dVe = dVbs0_dVe * (1 + T5);
                          if (selfheat)
			     dVbs0mos_dT = dVbs0_dT - (dVbs0t_dT - dVbs0_dT) * T5;
                          else
			     dVbs0mos_dT = 0.0;

/* Prepare Vthfd - treat Vbs0mos as if it were independent variable Vb */
		     Phis = phi - Vbs0mos;
		     dPhis_dVb = -1;
		     sqrtPhis = sqrt(Phis);
		     dsqrtPhis_dVb = -0.5 / sqrtPhis;
		     Xdep = Xdep0 * sqrtPhis / sqrtPhi;
		     dXdep_dVb = (Xdep0 / sqrtPhi)
				 * dsqrtPhis_dVb;
		     sqrtXdep = sqrt(Xdep);

		     T0 = pParam->B3SOIdvt2 * Vbs0mos;
		     if (T0 >= - 0.5)
		     {   T1 = 1.0 + T0;
			 T2 = pParam->B3SOIdvt2;
		     }
		     else /* Added to avoid any discontinuity problems caused by dvt2*/ 
		     {   T4 = 1.0 / (3.0 + 8.0 * T0);
			 T1 = (1.0 + 3.0 * T0) * T4; 
			 T2 = pParam->B3SOIdvt2 * T4 * T4;
		     }
		     lt1 = model->B3SOIfactor1 * sqrtXdep * T1;
		     dlt1_dVb = model->B3SOIfactor1 * (0.5 / sqrtXdep * T1 * dXdep_dVb
			      + sqrtXdep * T2);

		     T0 = pParam->B3SOIdvt2w * Vbs0mos;
		     if (T0 >= - 0.5)
		     {   T1 = 1.0 + T0;
			 T2 = pParam->B3SOIdvt2w;
		     }
		     else /* Added to avoid any discontinuity problems caused by
			     dvt2w */
		     {   T4 = 1.0 / (3.0 + 8.0 * T0);
			 T1 = (1.0 + 3.0 * T0) * T4;
			 T2 = pParam->B3SOIdvt2w * T4 * T4;
		     }
		     ltw= model->B3SOIfactor1 * sqrtXdep * T1;
		     dltw_dVb = model->B3SOIfactor1 * (0.5 / sqrtXdep * T1 * dXdep_dVb 
			      + sqrtXdep * T2);

		     T0 = -0.5 * pParam->B3SOIdvt1 * Leff / lt1;
		     if (T0 > -EXP_THRESHOLD)
		     {   T1 = exp(T0);
			 dT1_dVb = -T0 / lt1 * T1 * dlt1_dVb;
			 Theta0 = T1 * (1.0 + 2.0 * T1);
			 dTheta0_dVb = (1.0 + 4.0 * T1) * dT1_dVb;
		     }
		     else
		     {   T1 = MIN_EXP;
			 Theta0 = T1 * (1.0 + 2.0 * T1);
			 dTheta0_dVb = 0.0;
		     }
		     here->B3SOIthetavth = pParam->B3SOIdvt0 * Theta0;
		     Delt_vth = here->B3SOIthetavth * V0;
		     dDelt_vth_dVb = pParam->B3SOIdvt0 * dTheta0_dVb * V0;
                     if (selfheat) dDelt_vth_dT = here->B3SOIthetavth * dvbi_dT;
                     else dDelt_vth_dT = 0.0;

		     T0 = -0.5*pParam->B3SOIdvt1w * pParam->B3SOIweff*Leff/ltw;
		     if (T0 > -EXP_THRESHOLD)
		     {   T1 = exp(T0);
			 T2 = T1 * (1.0 + 2.0 * T1);
			 dT1_dVb = -T0 / ltw * T1 * dltw_dVb;
			 dT2_dVb = (1.0 + 4.0 * T1) * dT1_dVb;
		     }
		     else
		     {   T1 = MIN_EXP;
			 T2 = T1 * (1.0 + 2.0 * T1);
			 dT2_dVb = 0.0;
		     }
		     T0 = pParam->B3SOIdvt0w * T2;
		     DeltVthw = T0 * V0;
		     dDeltVthw_dVb = pParam->B3SOIdvt0w * dT2_dVb * V0;
                     if (selfheat)  dDeltVthw_dT = T0 * dvbi_dT;
                     else  dDeltVthw_dT = 0.0;

		     T0 = sqrt(1.0 + pParam->B3SOInlx / Leff);
                     T1 = (pParam->B3SOIkt1 + pParam->B3SOIkt1l / Leff
                           + pParam->B3SOIkt2 * Vbs0mos);
		     DeltVthtemp = pParam->B3SOIk1 * (T0 - 1.0) * sqrtPhi + T1 * TempRatio;
                     if (selfheat)   dDeltVthtemp_dT = T1 / model->B3SOItnom;
                     else   dDeltVthtemp_dT = 0.0;

		     tmp2 = model->B3SOItox * phi
			  / (pParam->B3SOIweff + pParam->B3SOIw0);

		     T3 = pParam->B3SOIeta0 + pParam->B3SOIetab * Vbs0mos;
		     if (T3 < 1.0e-4) /* avoid discontinuity problems caused by etab */
		     {   T9 = 1.0 / (3.0 - 2.0e4 * T3);
			 T3 = (2.0e-4 - T3) * T9;
			 T4 = T9 * T9 * pParam->B3SOIetab;
			 dT3_dVb = T4;
		     }
		     else
		     {
			 dT3_dVb = pParam->B3SOIetab;
		     }
		     DIBL_Sft = T3 * pParam->B3SOItheta0vb0 * Vds;
		     dDIBL_Sft_dVd = T3 * pParam->B3SOItheta0vb0;
		     dDIBL_Sft_dVb = pParam->B3SOItheta0vb0 * Vds * dT3_dVb;

		     Vthfd = model->B3SOItype * pParam->B3SOIvth0 + pParam->B3SOIk1
			 * (sqrtPhis - sqrtPhi) - pParam->B3SOIk2
			 * Vbs0mos-Delt_vth-DeltVthw +(pParam->B3SOIk3 +pParam->B3SOIk3b
			 * Vbs0mos) * tmp2 + DeltVthtemp - DIBL_Sft;

		     T6 = pParam->B3SOIk3b * tmp2 - pParam->B3SOIk2 +
			  pParam->B3SOIkt2 * TempRatio;
		     dVthfd_dVd = -dDIBL_Sft_dVd;
		     T7 = pParam->B3SOIk1 * dsqrtPhis_dVb
			  - dDelt_vth_dVb - dDeltVthw_dVb
			  + T6 - dDIBL_Sft_dVb;
		     dVthfd_dVe = T7 * dVbs0mos_dVe;
                     if (selfheat)
                        dVthfd_dT = dDeltVthtemp_dT - dDelt_vth_dT - dDeltVthw_dT
                                  + T7 * dVbs0mos_dT;
                     else
                        dVthfd_dT = 0.0;

/* Effective Vbs0 and Vbs0t for all Vgs */
		     T1 = Vthfd - Vgs_eff - DELT_Vbs0eff;
		     T2 = sqrt(T1 * T1 + DELT_Vbs0eff * DELT_Vbs0eff );
	   
		     Vbs0teff = Vbs0t - 0.5 * (T1 + T2);
		     dVbs0teff_dVg = 0.5  * (1 + T1/T2) * dVgs_eff_dVg;
		     dVbs0teff_dVd = - 0.5 * (1 + T1 / T2) * dVthfd_dVd;
		     dVbs0teff_dVe = - 0.5 * (1 + T1 / T2) * dVthfd_dVe;
                     if (selfheat)
                        dVbs0teff_dT = dVbs0t_dT - 0.5 * (1 + T1 / T2) * dVthfd_dT;
                     else
                        dVbs0teff_dT = 0.0;

			/* Calculate nfb */
			T3 = 1 / (K1 * K1);
			T4 = pParam->B3SOIkb3 * Cbox / model->B3SOIcox;
			T8 = sqrt(phi - Vbs0mos);
			T5 = sqrt(1 + 4 * T3 * (phi + K1 * T8 - Vbs0mos));
			T6 = 1 + T4 * T5;
			Nfb = model->B3SOInfb = 1 / T6;
			T7 = 2 * T3 * T4 * Nfb * Nfb / T5 * (0.5 * K1 / T8 + 1);
			Vbs0eff = Vbs0 - Nfb * 0.5 * (T1 + T2);
			dVbs0eff_dVg = Nfb * 0.5  * (1 + T1/T2) * dVgs_eff_dVg;
			dVbs0eff_dVd = - Nfb * 0.5 * (1 + T1 / T2) * dVthfd_dVd;
			dVbs0eff_dVe = dVbs0_dVe - Nfb * 0.5 * (1 + T1 / T2) 
				     * dVthfd_dVe - T7 * 0.5 * (T1 + T2) * dVbs0mos_dVe;
                        if (selfheat)
                           dVbs0eff_dT = dVbs0_dT - Nfb * 0.5 * (1 + T1 / T2)
                                       * dVthfd_dT - T7 * 0.5 * (T1 + T2) * dVbs0mos_dT;
                        else
                           dVbs0eff_dT = 0.0;

/* Simple check of Vbs */
/* Prepare Vbsdio */
                        Vbs = Vbsdio = Vbs0eff;
                        dVbsdio_dVg = dVbs0eff_dVg;
                        dVbsdio_dVd = dVbs0eff_dVd;
                        dVbsdio_dVe = dVbs0eff_dVe;
                        dVbsdio_dT = dVbs0eff_dT;
                        dVbsdio_dVb = 0.0;

/* Prepare Vbseff */
			T1 = Vbs0teff - Vbsdio - DELT_Vbsmos;
			T2 = sqrt(T1 * T1 + DELT_Vbsmos * DELT_Vbsmos);
			T3 = 0.5 * (T1 + T2);
			T5 = 0.5 * (1 + T1/T2);
			dT3_dVg = T5 * (dVbs0teff_dVg - dVbsdio_dVg);
			dT3_dVd = T5 * (dVbs0teff_dVd - dVbsdio_dVd);
			dT3_dVb = - T5 * dVbsdio_dVb;
			dT3_dVe = T5 * (dVbs0teff_dVe - dVbsdio_dVe);
                        if (selfheat)  dT3_dT = T5 * (dVbs0teff_dT - dVbsdio_dT);
                        else  dT3_dT = 0.0;
			T4 = T3 * model->B3SOIcsieff / model->B3SOIqsieff;
	   
			Vbsmos = Vbsdio - 0.5 * T3 * T4;
			dVbsmos_dVg = dVbsdio_dVg - T4 * dT3_dVg;
			dVbsmos_dVd = dVbsdio_dVd - T4 * dT3_dVd;
			dVbsmos_dVb = dVbsdio_dVb - T4 * dT3_dVb;
			dVbsmos_dVe = dVbsdio_dVe - T4 * dT3_dVe;
                        if (selfheat)  dVbsmos_dT = dVbsdio_dT - T4 * dT3_dT;
                        else  dVbsmos_dT = 0.0;

/* Prepare Vcs */
		     Vcs = Vbsdio - Vbs0eff;
		     dVcs_dVb = dVbsdio_dVb;
		     dVcs_dVg = dVbsdio_dVg - dVbs0eff_dVg;
		     dVcs_dVd = dVbsdio_dVd - dVbs0eff_dVd;
		     dVcs_dVe = dVbsdio_dVe - dVbs0eff_dVe;
                     dVcs_dT = dVbsdio_dT - dVbs0eff_dT;

/* Check Vps */
                     /* Note : if Vps is less Vbs0eff => non-physical */
                     T1 = Vps - Vbs0eff + DELT_Vbs0dio;
                     T2 = sqrt(T1 * T1 + DELT_Vbs0dio * DELT_Vbs0dio);
                     T3 = 0.5 * (1 + T1/T2);
                     Vpsdio = Vbs0eff + 0.5 * (T1 + T2);
                     dVpsdio_dVg = (1 - T3) * dVbs0eff_dVg;
                     dVpsdio_dVd = (1 - T3) * dVbs0eff_dVd;
                     dVpsdio_dVe = (1 - T3) * dVbs0eff_dVe;
                     if (selfheat)  dVpsdio_dT = (1 - T3) * dVbs0eff_dT;
                     else  dVpsdio_dT = 0.0;
                     dVpsdio_dVp = T3;
                     Vbp = Vbsdio - Vpsdio;
                     dVbp_dVb = dVbsdio_dVb;
                     dVbp_dVg = dVbsdio_dVg - dVpsdio_dVg;
                     dVbp_dVd = dVbsdio_dVd - dVpsdio_dVd;
                     dVbp_dVe = dVbsdio_dVe - dVpsdio_dVe;
                     dVbp_dT = dVbsdio_dT - dVpsdio_dT;
                     dVbp_dVp = - dVpsdio_dVp;

                  here->B3SOIvbsdio = Vbsdio;
                  here->B3SOIvbs0eff = Vbs0eff;

		  T1 = phi - pParam->B3SOIdelp;
		  T2 = T1 - Vbsmos - DELT_Vbseff;
		  T3 = sqrt(T2 * T2 + 4.0 * DELT_Vbseff * T1);
		  Vbseff = T1 - 0.5 * (T2 + T3);
		  T4 = 0.5 * (1 + T2/T3);
		  dVbseff_dVg = T4 * dVbsmos_dVg;
		  dVbseff_dVd = T4 * dVbsmos_dVd;
		  dVbseff_dVb = T4 * dVbsmos_dVb;
		  dVbseff_dVe = T4 * dVbsmos_dVe;
                  if (selfheat)  dVbseff_dT = T4 * dVbsmos_dT;
                  else  dVbseff_dT = 0.0;

                  here->B3SOIvbseff = Vbseff;

		  Phis = phi - Vbseff;
		  dPhis_dVb = -1;
		  sqrtPhis = sqrt(Phis);
		  dsqrtPhis_dVb = -0.5 / sqrtPhis ;

		  Xdep = Xdep0 * sqrtPhis / sqrtPhi;
		  dXdep_dVb = (Xdep0 / sqrtPhi)
			    * dsqrtPhis_dVb;

/* Vth Calculation */
		  T3 = sqrt(Xdep);

		  T0 = pParam->B3SOIdvt2 * Vbseff;
		  if (T0 >= - 0.5)
		  {   T1 = 1.0 + T0;
		      T2 = pParam->B3SOIdvt2 ;
		  }
		  else /* Added to avoid any discontinuity problems caused by dvt2 */ 
		  {   T4 = 1.0 / (3.0 + 8.0 * T0);
		      T1 = (1.0 + 3.0 * T0) * T4; 
		      T2 = pParam->B3SOIdvt2 * T4 * T4 ;
		  }
		  lt1 = model->B3SOIfactor1 * T3 * T1;
		  dlt1_dVb =model->B3SOIfactor1 * (0.5 / T3 * T1 * dXdep_dVb + T3 * T2);

		  T0 = pParam->B3SOIdvt2w * Vbseff;
		  if (T0 >= - 0.5)
		  {   T1 = 1.0 + T0;
		      T2 = pParam->B3SOIdvt2w ;
		  }
		  else /* Added to avoid any discontinuity problems caused by dvt2w */ 
		  {   T4 = 1.0 / (3.0 + 8.0 * T0);
		      T1 = (1.0 + 3.0 * T0) * T4; 
		      T2 = pParam->B3SOIdvt2w * T4 * T4 ;
		  }
		  ltw= model->B3SOIfactor1 * T3 * T1;
		  dltw_dVb=model->B3SOIfactor1*(0.5 / T3 * T1 * dXdep_dVb + T3 * T2);

		  T0 = -0.5 * pParam->B3SOIdvt1 * Leff / lt1;
		  if (T0 > -EXP_THRESHOLD)
		  {   T1 = exp(T0);
		      Theta0 = T1 * (1.0 + 2.0 * T1);
		      dT1_dVb = -T0 / lt1 * T1 * dlt1_dVb;
		      dTheta0_dVb = (1.0 + 4.0 * T1) * dT1_dVb;
		  }
		  else
		  {   T1 = MIN_EXP;
		      Theta0 = T1 * (1.0 + 2.0 * T1);
		      dTheta0_dVb = 0.0;
		  }

		  here->B3SOIthetavth = pParam->B3SOIdvt0 * Theta0;
		  Delt_vth = here->B3SOIthetavth * V0;
		  dDelt_vth_dVb = pParam->B3SOIdvt0 * dTheta0_dVb * V0;
                  if (selfheat)  dDelt_vth_dT = here->B3SOIthetavth * dvbi_dT;
                  else  dDelt_vth_dT = 0.0;

		  T0 = -0.5 * pParam->B3SOIdvt1w * pParam->B3SOIweff * Leff / ltw;
		  if (T0 > -EXP_THRESHOLD)
		  {   T1 = exp(T0);
		      T2 = T1 * (1.0 + 2.0 * T1);
		      dT1_dVb = -T0 / ltw * T1 * dltw_dVb;
		      dT2_dVb = (1.0 + 4.0 * T1) * dT1_dVb;
		  }
		  else
		  {   T1 = MIN_EXP;
		      T2 = T1 * (1.0 + 2.0 * T1);
		      dT2_dVb = 0.0;
		  }

		  T0 = pParam->B3SOIdvt0w * T2;
		  DeltVthw = T0 * V0;
		  dDeltVthw_dVb = pParam->B3SOIdvt0w * dT2_dVb * V0;
                  if (selfheat)   dDeltVthw_dT = T0 * dvbi_dT;
                  else   dDeltVthw_dT = 0.0;

		  T0 = sqrt(1.0 + pParam->B3SOInlx / Leff);
                  T1 = (pParam->B3SOIkt1 + pParam->B3SOIkt1l / Leff
                        + pParam->B3SOIkt2 * Vbseff);
		  DeltVthtemp = pParam->B3SOIk1 * (T0 - 1.0) * sqrtPhi + T1 * TempRatio;
                  if (selfheat)
                     dDeltVthtemp_dT = T1 / model->B3SOItnom;
                  else
                     dDeltVthtemp_dT = 0.0;

		  tmp2 = model->B3SOItox * phi
		       / (pParam->B3SOIweff + pParam->B3SOIw0);

		  T3 = pParam->B3SOIeta0 + pParam->B3SOIetab * Vbseff;
		  if (T3 < 1.0e-4) /* avoid  discontinuity problems caused by etab */ 
		  {   T9 = 1.0 / (3.0 - 2.0e4 * T3);
		      T3 = (2.0e-4 - T3) * T9;
		      T4 = T9 * T9 * pParam->B3SOIetab;
		      dT3_dVb = T4 ;
		  }
		  else
		  {   
		      dT3_dVb = pParam->B3SOIetab ;
		  }
		  DIBL_Sft = T3 * pParam->B3SOItheta0vb0 * Vds;
		  dDIBL_Sft_dVd = pParam->B3SOItheta0vb0 * T3;
		  dDIBL_Sft_dVb = pParam->B3SOItheta0vb0 * Vds * dT3_dVb;

		  Vth = model->B3SOItype * pParam->B3SOIvth0 + pParam->B3SOIk1 
		      * (sqrtPhis - sqrtPhi) - pParam->B3SOIk2 
		      * Vbseff- Delt_vth - DeltVthw +(pParam->B3SOIk3 + pParam->B3SOIk3b
		      * Vbseff) * tmp2 + DeltVthtemp - DIBL_Sft;

		  here->B3SOIvon = Vth; 

		  T6 = pParam->B3SOIk3b * tmp2 - pParam->B3SOIk2 
		       + pParam->B3SOIkt2 * TempRatio;          
		  dVth_dVb = pParam->B3SOIk1 * dsqrtPhis_dVb 
			   - dDelt_vth_dVb - dDeltVthw_dVb
			   + T6 - dDIBL_Sft_dVb;  /*  this is actually dVth_dVbseff  */
		  dVth_dVd = -dDIBL_Sft_dVd;
                  if (selfheat)  dVth_dT = dDeltVthtemp_dT - dDelt_vth_dT - dDeltVthw_dT;
                  else  dVth_dT = 0.0;

/* Calculate n */
		  T2 = pParam->B3SOInfactor * EPSSI / Xdep;
		  dT2_dVb = - T2 / Xdep * dXdep_dVb;

		  T3 = pParam->B3SOIcdsc + pParam->B3SOIcdscb * Vbseff
		       + pParam->B3SOIcdscd * Vds;
		  dT3_dVb = pParam->B3SOIcdscb;
		  dT3_dVd = pParam->B3SOIcdscd;

		  T4 = (T2 + T3 * Theta0 + pParam->B3SOIcit) / model->B3SOIcox;
		  dT4_dVb = (dT2_dVb + Theta0 * dT3_dVb + dTheta0_dVb * T3)
                            / model->B3SOIcox;
		  dT4_dVd = Theta0 * dT3_dVd / model->B3SOIcox;

		  if (T4 >= -0.5)
		  {   n = 1.0 + T4;
		      dn_dVb = dT4_dVb;
		      dn_dVd = dT4_dVd;
		  }
		  else
		   /* avoid  discontinuity problems caused by T4 */ 
		  {   T0 = 1.0 / (3.0 + 8.0 * T4);
		      n = (1.0 + 3.0 * T4) * T0;
		      T0 *= T0;
		      dn_dVb = T0 * dT4_dVb;
		      dn_dVd = T0 * dT4_dVd;
		  }

/* Effective Vgst (Vgsteff) Calculation */

		  Vgst = Vgs_eff - Vth;

		  T10 = 2.0 * n * Vtm;
		  VgstNVt = Vgst / T10;
		  ExpArg = (2.0 * pParam->B3SOIvoff - Vgst) / T10;

		  /* MCJ: Very small Vgst */
		  if (VgstNVt > EXP_THRESHOLD)
		  {   Vgsteff = Vgst;
                      /* T0 is dVgsteff_dVbseff */
                      T0 = -dVth_dVb;
		      dVgsteff_dVg = dVgs_eff_dVg + T0 * dVbseff_dVg;
		      dVgsteff_dVd = -dVth_dVd + T0 * dVbseff_dVd;
		      dVgsteff_dVb = T0 * dVbseff_dVb;
                      dVgsteff_dVe = T0 * dVbseff_dVe;
                      if (selfheat)
                         dVgsteff_dT  = -dVth_dT + T0 * dVbseff_dT;
                      else
                         dVgsteff_dT = 0.0;
		  }
		  else if (ExpArg > EXP_THRESHOLD)
		  {   T0 = (Vgst - pParam->B3SOIvoff) / (n * Vtm);
		      ExpVgst = exp(T0);
		      Vgsteff = Vtm * pParam->B3SOIcdep0 / model->B3SOIcox * ExpVgst;
		      T3 = Vgsteff / (n * Vtm) ;
                      /* T1 is dVgsteff_dVbseff */
		      T1  = -T3 * (dVth_dVb + T0 * Vtm * dn_dVb);
		      dVgsteff_dVg = T3 * dVgs_eff_dVg + T1 * dVbseff_dVg;
		      dVgsteff_dVd = -T3 * (dVth_dVd + T0 * Vtm * dn_dVd) + T1 * dVbseff_dVd;
                      dVgsteff_dVe = T1 * dVbseff_dVe;
                      dVgsteff_dVb = T1 * dVbseff_dVb;
                      if (selfheat)
                         dVgsteff_dT = -T3 * (dVth_dT + T0 * dVtm_dT * n)
                                     + Vgsteff / Temp + T1 * dVbseff_dT;
                      else
                         dVgsteff_dT = 0.0;
		  }
		  else
		  {   ExpVgst = exp(VgstNVt);
		      T1 = T10 * log(1.0 + ExpVgst);
		      dT1_dVg = ExpVgst / (1.0 + ExpVgst);
		      dT1_dVb = -dT1_dVg * (dVth_dVb + Vgst / n * dn_dVb)
			      + T1 / n * dn_dVb; 
		      dT1_dVd = -dT1_dVg * (dVth_dVd + Vgst / n * dn_dVd)
			      + T1 / n * dn_dVd;
                      T3 = (1.0 / Temp);
                      if (selfheat)
                         dT1_dT = -dT1_dVg * (dVth_dT + Vgst * T3) + T1 * T3;
                      else
                         dT1_dT = 0.0;

		      dT2_dVg = -model->B3SOIcox / (Vtm * pParam->B3SOIcdep0)
			      * exp(ExpArg);
		      T2 = 1.0 - T10 * dT2_dVg;
		      dT2_dVd = -dT2_dVg * (dVth_dVd - 2.0 * Vtm * ExpArg * dn_dVd)
			      + (T2 - 1.0) / n * dn_dVd;
		      dT2_dVb = -dT2_dVg * (dVth_dVb - 2.0 * Vtm * ExpArg * dn_dVb)
			      + (T2 - 1.0) / n * dn_dVb;
                      if (selfheat)
                         dT2_dT = -dT2_dVg * (dVth_dT - ExpArg * T10 * T3);
                      else
                         dT2_dT = 0.0;

		      Vgsteff = T1 / T2;
		      T3 = T2 * T2;
                      /*  T4 is dVgsteff_dVbseff  */
		      T4 = (T2 * dT1_dVb - T1 * dT2_dVb) / T3;
                      dVgsteff_dVb = T4 * dVbseff_dVb;
                      dVgsteff_dVe = T4 * dVbseff_dVe;
		      dVgsteff_dVg = (T2 * dT1_dVg - T1 * dT2_dVg) / T3 * dVgs_eff_dVg
                                     + T4 * dVbseff_dVg;
		      dVgsteff_dVd = (T2 * dT1_dVd - T1 * dT2_dVd) / T3 + T4 * dVbseff_dVd;
                      if (selfheat)
                         dVgsteff_dT = (T2 * dT1_dT - T1 * dT2_dT) / T3 + T4 * dVbseff_dT;
                      else
                         dVgsteff_dT = 0.0;
		  }
		  Vgst2Vtm = Vgsteff + 2.0 * Vtm;
                  if (selfheat)  dVgst2Vtm_dT = 2.0 * dVtm_dT;  
                  else  dVgst2Vtm_dT = 0.0;

/* Calculate Effective Channel Geometry */
		  T9 = sqrtPhis - sqrtPhi;
		  Weff = pParam->B3SOIweff - 2.0 * (pParam->B3SOIdwg * Vgsteff 
		       + pParam->B3SOIdwb * T9); 
		  dWeff_dVg = -2.0 * pParam->B3SOIdwg;
		  dWeff_dVb = -2.0 * pParam->B3SOIdwb * dsqrtPhis_dVb;

		  if (Weff < 2.0e-8) /* to avoid the discontinuity problem due to Weff*/
		  {   T0 = 1.0 / (6.0e-8 - 2.0 * Weff);
		      Weff = 2.0e-8 * (4.0e-8 - Weff) * T0;
		      T0 *= T0 * 4.0e-16;
		      dWeff_dVg *= T0;
		      dWeff_dVb *= T0;
		  }

		  T0 = pParam->B3SOIprwg * Vgsteff + pParam->B3SOIprwb * T9;
		  if (T0 >= -0.9)
		  {   Rds = rds0 * (1.0 + T0);
		      dRds_dVg = rds0 * pParam->B3SOIprwg;
		      dRds_dVb = rds0 * pParam->B3SOIprwb * dsqrtPhis_dVb;
                      if (selfheat)  dRds_dT = (1.0 + T0) * drds0_dT;
                      else  dRds_dT = 0.0;
		  }
		  else
		   /* to avoid the discontinuity problem due to prwg and prwb*/
		  {   T1 = 1.0 / (17.0 + 20.0 * T0);
		      Rds = rds0 * (0.8 + T0) * T1;
		      T1 *= T1;
		      dRds_dVg = rds0 * pParam->B3SOIprwg * T1;
		      dRds_dVb = rds0 * pParam->B3SOIprwb * dsqrtPhis_dVb
			       * T1;
                      if (selfheat)  dRds_dT = (0.8 + T0) * T1 * drds0_dT;
                      else  dRds_dT = 0.0;
		  }

/* Calculate Abulk */
                  if (pParam->B3SOIa0 == 0.0)
                  {
                     Abulk0 = Abulk = dAbulk0_dVb = dAbulk_dVg = dAbulk_dVb = 0.0;
                  }
                  else
                  {
		     T1 = 0.5 * pParam->B3SOIk1 / sqrtPhi;
		     T9 = sqrt(model->B3SOIxj * Xdep);
		     tmp1 = Leff + 2.0 * T9;
		     T5 = Leff / tmp1; 
		     tmp2 = pParam->B3SOIa0 * T5;
		     tmp3 = pParam->B3SOIweff + pParam->B3SOIb1; 
		     tmp4 = pParam->B3SOIb0 / tmp3;
		     T2 = tmp2 + tmp4;
		     dT2_dVb = -T9 * tmp2 / tmp1 / Xdep * dXdep_dVb;
		     T6 = T5 * T5;
		     T7 = T5 * T6;

		     Abulk0 = T1 * T2; 
		     dAbulk0_dVb = T1 * dT2_dVb;

		     T8 = pParam->B3SOIags * pParam->B3SOIa0 * T7;
		     dAbulk_dVg = -T1 * T8;
		     Abulk = Abulk0 + dAbulk_dVg * Vgsteff; 

		     dAbulk_dVb = dAbulk0_dVb - T8 * Vgsteff * 3.0 * T1 * dT2_dVb
                                             / tmp2;
                  }

                  if (Abulk0 < 0.01)
                  {
                     T9 = 1.0 / (3.0 - 200.0 * Abulk0);
                     Abulk0 = (0.02 - Abulk0) * T9;
                     dAbulk0_dVb *= T9 * T9;
                  }

                  if (Abulk < 0.01)
                  {
                     T9 = 1.0 / (3.0 - 200.0 * Abulk);
                     Abulk = (0.02 - Abulk) * T9;
                     dAbulk_dVb *= T9 * T9;
                  }

		  T2 = pParam->B3SOIketa * Vbseff;
		  if (T2 >= -0.9)
		  {   T0 = 1.0 / (1.0 + T2);
		      dT0_dVb = -pParam->B3SOIketa * T0 * T0 ;
		  }
		  else
		  /* added to avoid the problems caused by Keta */
		  {   T1 = 1.0 / (0.8 + T2);
		      T0 = (17.0 + 20.0 * T2) * T1;
		      dT0_dVb = -pParam->B3SOIketa * T1 * T1 ;
		  }
		  dAbulk_dVg *= T0;
		  dAbulk_dVb = dAbulk_dVb * T0 + Abulk * dT0_dVb;
		  dAbulk0_dVb = dAbulk0_dVb * T0 + Abulk0 * dT0_dVb;
		  Abulk *= T0;
		  Abulk0 *= T0;

		  Abulk += 1;
		  Abulk0 += 1;

/* Prepare Abeff */
		      T0 = pParam->B3SOIabp * Vgst2Vtm;
		      T1 = 1 - Vcs / T0 - DELT_Xcsat;
		      T2 = sqrt(T1 * T1 + DELT_Xcsat * DELT_Xcsat);
		      T3 = 1 - 0.5 * (T1 + T2);
		      T5 = -0.5 * (1 + T1 / T2);
		      dT1_dVg = Vcs / Vgst2Vtm / T0;
		      dT3_dVg = T5 * dT1_dVg;
		      dT1_dVc = - 1 / T0;
		      dT3_dVc = T5 * dT1_dVc;

		      Xcsat = pParam->B3SOImxc * T3 * T3 + (1 - pParam->B3SOImxc)*T3;
		      T4 = 2 * pParam->B3SOImxc * T3 + (1 - pParam->B3SOImxc);
		      dXcsat_dVg = T4 * dT3_dVg;
		      dXcsat_dVc = T4 * dT3_dVc;

		      Abeff = Xcsat * Abulk + (1 - Xcsat) * model->B3SOIadice;
		      T0 = Xcsat * dAbulk_dVg + Abulk * dXcsat_dVg;
		      dAbeff_dVg = T0 - model->B3SOIadice * dXcsat_dVg;
		      dAbeff_dVb = Xcsat * dAbulk_dVb;
		      dAbeff_dVc = (Abulk - model->B3SOIadice) * dXcsat_dVc;
                 here->B3SOIabeff = Abeff;

/* Mobility calculation */
		  if (model->B3SOImobMod == 1)
		  {   T0 = Vgsteff + Vth + Vth;
		      T2 = ua + uc * Vbseff;
		      T3 = T0 / model->B3SOItox;
		      T5 = T3 * (T2 + ub * T3);
		      dDenomi_dVg = (T2 + 2.0 * ub * T3) / model->B3SOItox;
		      dDenomi_dVd = dDenomi_dVg * 2 * dVth_dVd;
		      dDenomi_dVb = dDenomi_dVg * 2 * dVth_dVb + uc * T3 ;
                      if (selfheat)
                         dDenomi_dT = dDenomi_dVg * 2 * dVth_dT 
                                    + (dua_dT + Vbseff * duc_dT
                                    + dub_dT * T3 ) * T3;
                      else
                         dDenomi_dT = 0.0;
		  }
		  else if (model->B3SOImobMod == 2)
		  {   T5 = Vgsteff / model->B3SOItox * (ua
			 + uc * Vbseff + ub * Vgsteff
			 / model->B3SOItox);
		      dDenomi_dVg = (ua + uc * Vbseff
				  + 2.0 * ub * Vgsteff / model->B3SOItox)
				  / model->B3SOItox;
		      dDenomi_dVd = 0.0;
		      dDenomi_dVb = Vgsteff * uc / model->B3SOItox ;
                      if (selfheat)
                         dDenomi_dT = Vgsteff / model->B3SOItox
                                    * (dua_dT + Vbseff * duc_dT + dub_dT
                                    * Vgsteff / model->B3SOItox);
                      else
                         dDenomi_dT = 0.0;
		  }
		  else  /*  mobMod == 3  */
		  {   T0 = Vgsteff + Vth + Vth;
		      T2 = 1.0 + uc * Vbseff;
		      T3 = T0 / model->B3SOItox;
		      T4 = T3 * (ua + ub * T3);
		      T5 = T4 * T2;
		      dDenomi_dVg = (ua + 2.0 * ub * T3) * T2
				  / model->B3SOItox;
		      dDenomi_dVd = dDenomi_dVg * 2.0 * dVth_dVd;
		      dDenomi_dVb = dDenomi_dVg * 2.0 * dVth_dVb 
				  + uc * T4 ;
                      if (selfheat)
                         dDenomi_dT = dDenomi_dVg * 2.0 * dVth_dT
                                    + (dua_dT + dub_dT * T3) * T3 * T2
                                    + T4 * Vbseff * duc_dT;
                      else
                         dDenomi_dT = 0.0;
		  }

		  if (T5 >= -0.8)
		  {   Denomi = 1.0 + T5;
		  }
		  else /* Added to avoid the discontinuity problem caused by ua and ub*/ 
		  {   T9 = 1.0 / (7.0 + 10.0 * T5);
		      Denomi = (0.6 + T5) * T9;
		      T9 *= T9;
		      dDenomi_dVg *= T9;
		      dDenomi_dVd *= T9;
		      dDenomi_dVb *= T9;
                      if (selfheat)  dDenomi_dT *= T9;
                      else   dDenomi_dT = 0.0;
		  }

		  here->B3SOIueff = ueff = u0temp / Denomi;
		  T9 = -ueff / Denomi;
		  dueff_dVg = T9 * dDenomi_dVg;
		  dueff_dVd = T9 * dDenomi_dVd;
		  dueff_dVb = T9 * dDenomi_dVb;
                  if (selfheat)  dueff_dT = T9 * dDenomi_dT + du0temp_dT / Denomi;
                  else  dueff_dT = 0.0;

/* Saturation Drain Voltage  Vdsat */
		  WVCox = Weff * vsattemp * model->B3SOIcox;
		  WVCoxRds = WVCox * Rds; 

/*                  dWVCoxRds_dT = WVCox * dRds_dT
                                 + Weff * model->B3SOIcox * Rds * dvsattemp_dT; */

		  Esat = 2.0 * vsattemp / ueff;
		  EsatL = Esat * Leff;
		  T0 = -EsatL /ueff;
		  dEsatL_dVg = T0 * dueff_dVg;
		  dEsatL_dVd = T0 * dueff_dVd;
		  dEsatL_dVb = T0 * dueff_dVb;
                  if (selfheat)
                     dEsatL_dT = T0 * dueff_dT + EsatL / vsattemp * dvsattemp_dT;
                  else
                     dEsatL_dT = 0.0;
	  
		  /* Sqrt() */
		  a1 = pParam->B3SOIa1;
		  if (a1 == 0.0)
		  {   Lambda = pParam->B3SOIa2;
		      dLambda_dVg = 0.0;
		  }
		  else if (a1 > 0.0)
/* Added to avoid the discontinuity problem caused by a1 and a2 (Lambda) */
		  {   T0 = 1.0 - pParam->B3SOIa2;
		      T1 = T0 - pParam->B3SOIa1 * Vgsteff - 0.0001;
		      T2 = sqrt(T1 * T1 + 0.0004 * T0);
		      Lambda = pParam->B3SOIa2 + T0 - 0.5 * (T1 + T2);
		      dLambda_dVg = 0.5 * pParam->B3SOIa1 * (1.0 + T1 / T2);
		  }
		  else
		  {   T1 = pParam->B3SOIa2 + pParam->B3SOIa1 * Vgsteff - 0.0001;
		      T2 = sqrt(T1 * T1 + 0.0004 * pParam->B3SOIa2);
		      Lambda = 0.5 * (T1 + T2);
		      dLambda_dVg = 0.5 * pParam->B3SOIa1 * (1.0 + T1 / T2);
		  }

		  if (Rds > 0)
		  {   tmp2 = dRds_dVg / Rds + dWeff_dVg / Weff;
		      tmp3 = dRds_dVb / Rds + dWeff_dVb / Weff;
		  }
		  else
		  {   tmp2 = dWeff_dVg / Weff;
		      tmp3 = dWeff_dVb / Weff;
		  }
		  if ((Rds == 0.0) && (Lambda == 1.0))
		  {   T0 = 1.0 / (Abeff * EsatL + Vgst2Vtm);
		      tmp1 = 0.0;
		      T1 = T0 * T0;
		      T2 = Vgst2Vtm * T0;
		      T3 = EsatL * Vgst2Vtm;
		      Vdsat = T3 * T0;
				   
		      dT0_dVg = -(Abeff * dEsatL_dVg + EsatL * dAbeff_dVg + 1.0) * T1;
		      dT0_dVd = -(Abeff * dEsatL_dVd) * T1; 
		      dT0_dVb = -(Abeff * dEsatL_dVb + EsatL * dAbeff_dVb) * T1;
                      dT0_dVc = 0.0;
                      if (selfheat)
		         dT0_dT  = -(Abeff * dEsatL_dT + dVgst2Vtm_dT) * T1;
                      else dT0_dT  = 0.0;

		      dVdsat_dVg = T3 * dT0_dVg + T2 * dEsatL_dVg + EsatL * T0;
		      dVdsat_dVd = T3 * dT0_dVd + T2 * dEsatL_dVd;
		      dVdsat_dVb = T3 * dT0_dVb + T2 * dEsatL_dVb;
		      dVdsat_dVc = 0.0;
                      if (selfheat)
		         dVdsat_dT  = T3 * dT0_dT  + T2 * dEsatL_dT
				    + EsatL * T0 * dVgst2Vtm_dT;
                      else dVdsat_dT  = 0.0;
		  }
		  else
		  {   tmp1 = dLambda_dVg / (Lambda * Lambda);
		      T9 = Abeff * WVCoxRds;
		      T8 = Abeff * T9;
		      T7 = Vgst2Vtm * T9;
		      T6 = Vgst2Vtm * WVCoxRds;
		      T0 = 2.0 * Abeff * (T9 - 1.0 + 1.0 / Lambda); 
		      dT0_dVg = 2.0 * (T8 * tmp2 - Abeff * tmp1
			      + (2.0 * T9 + 1.0 / Lambda - 1.0) * dAbeff_dVg);
/*		      dT0_dVb = 2.0 * (T8 * tmp3  this is equivalent to one below, but simpler
			      + (2.0 * T9 + 1.0 / Lambda - 1.0) * dAbeff_dVg);  */
		      dT0_dVb = 2.0 * (T8 * (2.0 / Abeff * dAbeff_dVb + tmp3)
			      + (1.0 / Lambda - 1.0) * dAbeff_dVb);
		      dT0_dVd = 0.0; 
		      dT0_dVc = 0.0;

                      if (selfheat)
                      {
		         tmp4 = dRds_dT / Rds + dvsattemp_dT / vsattemp;
		         dT0_dT  = 2.0 * T8 * tmp4;
                      } else tmp4 = dT0_dT = 0.0;

		      T1 = Vgst2Vtm * (2.0 / Lambda - 1.0) + Abeff * EsatL + 3.0 * T7;
		     
		      dT1_dVg = (2.0 / Lambda - 1.0) - 2.0 * Vgst2Vtm * tmp1
			      + Abeff * dEsatL_dVg + EsatL * dAbeff_dVg + 3.0 * (T9
			      + T7 * tmp2 + T6 * dAbeff_dVg);
		      dT1_dVb = Abeff * dEsatL_dVb + EsatL * dAbeff_dVb
			      + 3.0 * (T6 * dAbeff_dVb + T7 * tmp3);
		      dT1_dVd = Abeff * dEsatL_dVd;
		      dT1_dVc = 0.0;


                      if (selfheat)
                      {
		         tmp4 += dVgst2Vtm_dT / Vgst2Vtm;
		         dT1_dT  = (2.0 / Lambda - 1.0) * dVgst2Vtm_dT
				 + Abeff * dEsatL_dT + 3.0 * T7 * tmp4;
                      } else dT1_dT = 0.0;

		      T2 = Vgst2Vtm * (EsatL + 2.0 * T6);
		      dT2_dVg = EsatL + Vgst2Vtm * dEsatL_dVg
			      + T6 * (4.0 + 2.0 * Vgst2Vtm * tmp2);
		      dT2_dVb = Vgst2Vtm * (dEsatL_dVb + 2.0 * T6 * tmp3);
		      dT2_dVd = Vgst2Vtm * dEsatL_dVd;
                      if (selfheat)
		         dT2_dT  = Vgst2Vtm * dEsatL_dT + EsatL * dVgst2Vtm_dT
				 + 2.0 * T6 * (dVgst2Vtm_dT + Vgst2Vtm * tmp4);
                      else
		         dT2_dT  = 0.0;

		      T3 = sqrt(T1 * T1 - 2.0 * T0 * T2);
		      Vdsat = (T1 - T3) / T0;

		      dVdsat_dVg = (dT1_dVg - (T1 * dT1_dVg - dT0_dVg * T2
				 - T0 * dT2_dVg) / T3 - Vdsat * dT0_dVg) / T0;
		      dVdsat_dVb = (dT1_dVb - (T1 * dT1_dVb - dT0_dVb * T2
				 - T0 * dT2_dVb) / T3 - Vdsat * dT0_dVb) / T0;
		      dVdsat_dVd = (dT1_dVd - (T1 * dT1_dVd - T0 * dT2_dVd) / T3) / T0;
                      dVdsat_dVc  = 0.0;
                      if (selfheat)
		         dVdsat_dT  = (dT1_dT - (T1 * dT1_dT - dT0_dT * T2
				    - T0 * dT2_dT) / T3 - Vdsat * dT0_dT) / T0;
                      else dVdsat_dT  = 0.0;
		  }
		  here->B3SOIvdsat = Vdsat;

/* Vdsatii for impact ionization */
                  if (pParam->B3SOIaii > 0.0) 
                  {
                     if (pParam->B3SOIcii != 0.0)
                     {
                        T0 = pParam->B3SOIcii / sqrt(3.0) + pParam->B3SOIdii;
                        /* Hard limit Vds to T0 => T4  i.e. limit T0 to 3.0 */
                        T1 = Vds - T0 - 0.1;
                        T2 = sqrt(T1 * T1 + 0.4);
                        T3 = T0 + 0.5 * (T1 + T2);
                        dT3_dVd = 0.5 * (1.0 + T1/T2);

                        T4 = T3 - pParam->B3SOIdii;
                        T5 = pParam->B3SOIcii / T4;
                        T0 = T5 * T5;
                        dT0_dVd = - 2 * T0 / T4 * dT3_dVd;
                     } else
                     {
                        T0 = dT0_dVd = 0.0;
                     }
                     T0 += 1.0;

                     T3 = pParam->B3SOIaii + pParam->B3SOIbii / Leff;
                     T4 = 1.0 / (T0 * Vgsteff + T3 * EsatL);
                     T5 = -T4 * T4;
                     T6 = Vgsteff * T4;
                     T7 = EsatL * Vgsteff;
                     Vdsatii = T7 * T4;
   
                     dT4_dVg = T5 * (T0 + T3 * dEsatL_dVg);
                     dT4_dVb = T5 * T3 * dEsatL_dVb;
                     dT4_dVd = T5 * (Vgsteff * dT0_dVd + T3 * dEsatL_dVd);

                     if (selfheat) dT4_dT = T5 * (T3 * dEsatL_dT);
                     else          dT4_dT = 0.0;
   
                     T8 = T4 * Vgsteff;
                     dVdsatii_dVg = T7 * dT4_dVg + T4 * (EsatL + Vgsteff * dEsatL_dVg);
                     dVdsatii_dVb = T7 * dT4_dVb + T8 * dEsatL_dVb;
                     dVdsatii_dVd = T7 * dT4_dVd + T8 * dEsatL_dVd;
                     if (selfheat) dVdsatii_dT  = T7 * dT4_dT  + T8 * dEsatL_dT;
                     else          dVdsatii_dT  = 0.0;
                  } else
                  {
                    Vdsatii = Vdsat;
                    dVdsatii_dVg = dVdsat_dVg;
                    dVdsatii_dVb = dVdsat_dVb;
                    dVdsatii_dVd = dVdsat_dVd;
                    dVdsatii_dT  = dVdsat_dT;
                  }

/* Effective Vds (Vdseff) Calculation */
		  T1 = Vdsat - Vds - pParam->B3SOIdelta;
		  dT1_dVg = dVdsat_dVg;
		  dT1_dVd = dVdsat_dVd - 1.0;
		  dT1_dVb = dVdsat_dVb;
		  dT1_dVc = dVdsat_dVc;
		  dT1_dT  = dVdsat_dT;

		  T2 = sqrt(T1 * T1 + 4.0 * pParam->B3SOIdelta * Vdsat);
		  T0 = T1 / T2;
		  T3 = 2.0 * pParam->B3SOIdelta / T2;
		  dT2_dVg = T0 * dT1_dVg + T3 * dVdsat_dVg;
		  dT2_dVd = T0 * dT1_dVd + T3 * dVdsat_dVd;
		  dT2_dVb = T0 * dT1_dVb + T3 * dVdsat_dVb;
                  dT2_dVc = 0.0;
                  if (selfheat)
		     dT2_dT  = T0 * dT1_dT  + T3 * dVdsat_dT;
                  else dT2_dT  = 0.0;

		  Vdseff = Vdsat - 0.5 * (T1 + T2);
		  dVdseff_dVg = dVdsat_dVg - 0.5 * (dT1_dVg + dT2_dVg); 
		  dVdseff_dVd = dVdsat_dVd - 0.5 * (dT1_dVd + dT2_dVd); 
		  dVdseff_dVb = dVdsat_dVb - 0.5 * (dT1_dVb + dT2_dVb); 
                  dVdseff_dVc = 0.0;
                  if (selfheat)
		     dVdseff_dT  = dVdsat_dT  - 0.5 * (dT1_dT  + dT2_dT);
                  else dVdseff_dT  = 0.0;

		  if (Vdseff > Vds)
		      Vdseff = Vds; /* This code is added to fixed the problem
				       caused by computer precision when
				       Vds is very close to Vdseff. */
		  diffVds = Vds - Vdseff;

/* Effective Vdsii for Iii calculation */
		  T1 = Vdsatii - Vds - pParam->B3SOIdelta;

		  T2 = sqrt(T1 * T1 + 4.0 * pParam->B3SOIdelta * Vdsatii);
		  T0 = T1 / T2;
		  T3 = 2.0 * pParam->B3SOIdelta / T2;
                  T4 = T0 + T3;
		  dT2_dVg = T4 * dVdsatii_dVg;
		  dT2_dVd = T4 * dVdsatii_dVd - T0;
                  dT2_dVb = T4 * dVdsatii_dVb;
                  if (selfheat) dT2_dT = T4*dVdsatii_dT;
                  else dT2_dT  = 0.0;

		  Vdseffii = Vdsatii - 0.5 * (T1 + T2);
		  dVdseffii_dVg = 0.5 * (dVdsatii_dVg - dT2_dVg);
		  dVdseffii_dVd = 0.5 * (dVdsatii_dVd - dT2_dVd + 1.0);
		  dVdseffii_dVb = 0.5 * (dVdsatii_dVb - dT2_dVb);
                  if (selfheat)
		     dVdseffii_dT  = 0.5 * (dVdsatii_dT - dT2_dT);
                  else dVdseffii_dT  = 0.0;
		  diffVdsii = Vds - Vdseffii;

/* Calculate VAsat */
		  tmp4 = 1.0 - 0.5 * Abeff * Vdsat / Vgst2Vtm;
		  T9 = WVCoxRds * Vgsteff;
		  T8 = T9 / Vgst2Vtm;
		  T0 = EsatL + Vdsat + 2.0 * T9 * tmp4;
		 
		  T7 = 2.0 * WVCoxRds * tmp4;
		  dT0_dVg = dEsatL_dVg + dVdsat_dVg + T7 * (1.0 + tmp2 * Vgsteff) 
                          - T8 * (Abeff * dVdsat_dVg - Abeff * Vdsat / Vgst2Vtm
			  + Vdsat * dAbeff_dVg);   
			  
		  dT0_dVb = dEsatL_dVb + dVdsat_dVb + T7 * tmp3 * Vgsteff
			  - T8 * (dAbeff_dVb * Vdsat + Abeff * dVdsat_dVb);
		  dT0_dVd = dEsatL_dVd + dVdsat_dVd - T8 * Abeff * dVdsat_dVd;
                  dT0_dVc = 0.0;

                  if (selfheat)
                  {
		     tmp4 = dRds_dT / Rds + dvsattemp_dT / vsattemp;
		     dT0_dT  = dEsatL_dT + dVdsat_dT + T7 * tmp4 * Vgsteff
			     - T8 * (Abeff * dVdsat_dT - Abeff * Vdsat * dVgst2Vtm_dT
			     / Vgst2Vtm);
                  } else
                     dT0_dT = 0.0;

		  T9 = WVCoxRds * Abeff; 
		  T1 = 2.0 / Lambda - 1.0 + T9; 
		  dT1_dVg = -2.0 * tmp1 +  WVCoxRds * (Abeff * tmp2 + dAbeff_dVg);
		  dT1_dVb = dAbeff_dVb * WVCoxRds + T9 * tmp3;
                  dT1_dVc = 0.0;
                  if (selfheat)
		     dT1_dT  = T9 * tmp4;
                  else
		     dT1_dT  = 0.0;

		  Vasat = T0 / T1;
		  dVasat_dVg = (dT0_dVg - Vasat * dT1_dVg) / T1;
		  dVasat_dVb = (dT0_dVb - Vasat * dT1_dVb) / T1;
		  dVasat_dVd = dT0_dVd / T1;
                  dVasat_dVc = 0.0;
                  if (selfheat) dVasat_dT  = (dT0_dT  - Vasat * dT1_dT)  / T1;
                  else dVasat_dT  = 0.0;

/* Calculate VACLM */
		  if ((pParam->B3SOIpclm > 0.0) && (diffVds > 1.0e-10))
		  {   T0 = 1.0 / (pParam->B3SOIpclm * Abeff * pParam->B3SOIlitl);
		      dT0_dVb = -T0 / Abeff * dAbeff_dVb;
		      dT0_dVg = -T0 / Abeff * dAbeff_dVg; 
                      dT0_dVc = 0.0;
		      
		      T2 = Vgsteff / EsatL;
		      T1 = Leff * (Abeff + T2); 
		      dT1_dVg = Leff * ((1.0 - T2 * dEsatL_dVg) / EsatL + dAbeff_dVg);
		      dT1_dVb = Leff * (dAbeff_dVb - T2 * dEsatL_dVb / EsatL);
		      dT1_dVd = -T2 * dEsatL_dVd / Esat;
		      dT1_dVc = 0.0;
                      if (selfheat) dT1_dT  = -T2 * dEsatL_dT / Esat;
                      else dT1_dT  = 0.0;

		      T9 = T0 * T1;
		      VACLM = T9 * diffVds;
		      dVACLM_dVg = T0 * dT1_dVg * diffVds - T9 * dVdseff_dVg
				 + T1 * diffVds * dT0_dVg;
		      dVACLM_dVb = (dT0_dVb * T1 + T0 * dT1_dVb) * diffVds
				 - T9 * dVdseff_dVb;
		      dVACLM_dVd = T0 * dT1_dVd * diffVds + T9 * (1.0 - dVdseff_dVd);
                      dVACLM_dVc = 0.0;
                      if (selfheat)
		         dVACLM_dT  = T0 * dT1_dT * diffVds - T9 * dVdseff_dT;
                      else dVACLM_dT  = 0.0;

		  }
		  else
		  {   VACLM = MAX_EXP;
		      dVACLM_dVd = dVACLM_dVg = dVACLM_dVb = dVACLM_dVc = dVACLM_dT = 0.0;
		  }


/* Calculate VADIBL */
		  if (pParam->B3SOIthetaRout > 0.0)
		  {   T8 = Abeff * Vdsat;
		      T0 = Vgst2Vtm * T8;
		      T1 = Vgst2Vtm + T8;
		      dT0_dVg = Vgst2Vtm * Abeff * dVdsat_dVg + T8
			      + Vgst2Vtm * Vdsat * dAbeff_dVg;
		      dT1_dVg = 1.0 + Abeff * dVdsat_dVg + Vdsat * dAbeff_dVg;
		      dT1_dVb = dAbeff_dVb * Vdsat + Abeff * dVdsat_dVb;
		      dT0_dVb = Vgst2Vtm * dT1_dVb;
		      dT1_dVd = Abeff * dVdsat_dVd;
		      dT0_dVd = Vgst2Vtm * dT1_dVd;
                      dT0_dVc = dT1_dVc = 0.0;
                      if (selfheat)
                      {
		         dT0_dT  = dVgst2Vtm_dT * T8 + Abeff * Vgst2Vtm * dVdsat_dT;
		         dT1_dT  = dVgst2Vtm_dT + Abeff * dVdsat_dT;
                      } else
                         dT0_dT = dT1_dT = 0.0;

		      T9 = T1 * T1;
		      T2 = pParam->B3SOIthetaRout;
		      VADIBL = (Vgst2Vtm - T0 / T1) / T2;
		      dVADIBL_dVg = (1.0 - dT0_dVg / T1 + T0 * dT1_dVg / T9) / T2;
		      dVADIBL_dVb = (-dT0_dVb / T1 + T0 * dT1_dVb / T9) / T2;
		      dVADIBL_dVd = (-dT0_dVd / T1 + T0 * dT1_dVd / T9) / T2;
                      dVADIBL_dVc = 0.0;
                      if (selfheat)
		         dVADIBL_dT = (dVgst2Vtm_dT - dT0_dT/T1 + T0*dT1_dT/T9) / T2;
                      else dVADIBL_dT = 0.0;

		      T7 = pParam->B3SOIpdiblb * Vbseff;
		      if (T7 >= -0.9)
		      {   T3 = 1.0 / (1.0 + T7);
			  VADIBL *= T3;
			  dVADIBL_dVg *= T3;
			  dVADIBL_dVb = (dVADIBL_dVb - VADIBL * pParam->B3SOIpdiblb)
				      * T3;
			  dVADIBL_dVd *= T3;
			  dVADIBL_dVc *= T3;
			  if (selfheat)  dVADIBL_dT  *= T3;
			  else  dVADIBL_dT  = 0.0;
		      }
		      else
/* Added to avoid the discontinuity problem caused by pdiblcb */
		      {   T4 = 1.0 / (0.8 + T7);
			  T3 = (17.0 + 20.0 * T7) * T4;
			  dVADIBL_dVg *= T3;
			  dVADIBL_dVb = dVADIBL_dVb * T3
				      - VADIBL * pParam->B3SOIpdiblb * T4 * T4;
			  dVADIBL_dVd *= T3;
			  dVADIBL_dVc *= T3;
			  if (selfheat)  dVADIBL_dT  *= T3;
			  else  dVADIBL_dT  = 0.0;
			  VADIBL *= T3;
		      }
		  }
		  else
		  {   VADIBL = MAX_EXP;
		      dVADIBL_dVd = dVADIBL_dVg = dVADIBL_dVb = dVADIBL_dVc 
			= dVADIBL_dT = 0.0;
		  }

/* Calculate VA */
		  
		  T8 = pParam->B3SOIpvag / EsatL;
		  T9 = T8 * Vgsteff;
		  if (T9 > -0.9)
		  {   T0 = 1.0 + T9;
		      dT0_dVg = T8 * (1.0 - Vgsteff * dEsatL_dVg / EsatL);
		      dT0_dVb = -T9 * dEsatL_dVb / EsatL;
		      dT0_dVd = -T9 * dEsatL_dVd / EsatL;
                      if (selfheat)
		         dT0_dT  = -T9 * dEsatL_dT / EsatL;
                      else
		         dT0_dT  = 0.0;
		  }
		  else /* Added to avoid the discontinuity problems caused by pvag */
		  {   T1 = 1.0 / (17.0 + 20.0 * T9);
		      T0 = (0.8 + T9) * T1;
		      T1 *= T1;
		      dT0_dVg = T8 * (1.0 - Vgsteff * dEsatL_dVg / EsatL) * T1;

		      T9 *= T1 / EsatL;
		      dT0_dVb = -T9 * dEsatL_dVb;
		      dT0_dVd = -T9 * dEsatL_dVd;
                      if (selfheat)
		         dT0_dT  = -T9 * dEsatL_dT;
                      else
		         dT0_dT  = 0.0;
		  }
		
		  tmp1 = VACLM * VACLM;
		  tmp2 = VADIBL * VADIBL;
		  tmp3 = VACLM + VADIBL;

		  T1 = VACLM * VADIBL / tmp3;
		  tmp3 *= tmp3;
		  dT1_dVg = (tmp1 * dVADIBL_dVg + tmp2 * dVACLM_dVg) / tmp3;
		  dT1_dVd = (tmp1 * dVADIBL_dVd + tmp2 * dVACLM_dVd) / tmp3;
		  dT1_dVb = (tmp1 * dVADIBL_dVb + tmp2 * dVACLM_dVb) / tmp3;
                  dT1_dVc = 0.0;
                  if (selfheat)
		     dT1_dT  = (tmp1 * dVADIBL_dT  + tmp2 * dVACLM_dT ) / tmp3;
                  else dT1_dT  = 0.0;

		  Va = Vasat + T0 * T1;
		  dVa_dVg = dVasat_dVg + T1 * dT0_dVg + T0 * dT1_dVg;
		  dVa_dVd = dVasat_dVd + T1 * dT0_dVd + T0 * dT1_dVd;
		  dVa_dVb = dVasat_dVb + T1 * dT0_dVb + T0 * dT1_dVb;
                  dVa_dVc = 0.0;
                  if (selfheat)
		     dVa_dT  = dVasat_dT  + T1 * dT0_dT  + T0 * dT1_dT;
                  else dVa_dT  = 0.0;

/* Calculate Ids */
		  CoxWovL = model->B3SOIcox * Weff / Leff;
		  beta = ueff * CoxWovL;
		  dbeta_dVg = CoxWovL * dueff_dVg + beta * dWeff_dVg / Weff;
		  dbeta_dVd = CoxWovL * dueff_dVd;
		  dbeta_dVb = CoxWovL * dueff_dVb + beta * dWeff_dVb / Weff;
		  if (selfheat)  dbeta_dT  = CoxWovL * dueff_dT;
		  else  dbeta_dT  = 0.0;

		  T0 = 1.0 - 0.5 * Abeff * Vdseff / Vgst2Vtm;
		  dT0_dVg = -0.5 * (Abeff * dVdseff_dVg 
			  - Abeff * Vdseff / Vgst2Vtm + Vdseff * dAbeff_dVg) / Vgst2Vtm;
		  dT0_dVd = -0.5 * Abeff * dVdseff_dVd / Vgst2Vtm;
		  dT0_dVb = -0.5 * (Abeff * dVdseff_dVb + dAbeff_dVb * Vdseff)
			  / Vgst2Vtm;
		  dT0_dVc = 0.0;
		  if (selfheat)  
                     dT0_dT  = -0.5 * (Abeff * dVdseff_dT
                             - Abeff * Vdseff / Vgst2Vtm * dVgst2Vtm_dT)
			     / Vgst2Vtm;
		  else dT0_dT = 0.0;

		  fgche1 = Vgsteff * T0;
		  dfgche1_dVg = Vgsteff * dT0_dVg + T0; 
		  dfgche1_dVd = Vgsteff * dT0_dVd;
		  dfgche1_dVb = Vgsteff * dT0_dVb;
		  dfgche1_dVc = 0.0;
		  if (selfheat)  dfgche1_dT  = Vgsteff * dT0_dT;
		  else  dfgche1_dT  = 0.0;

		  T9 = Vdseff / EsatL;
		  fgche2 = 1.0 + T9;
		  dfgche2_dVg = (dVdseff_dVg - T9 * dEsatL_dVg) / EsatL;
		  dfgche2_dVd = (dVdseff_dVd - T9 * dEsatL_dVd) / EsatL;
		  dfgche2_dVb = (dVdseff_dVb - T9 * dEsatL_dVb) / EsatL;
		  dfgche2_dVc = 0.0;
		  if (selfheat)  dfgche2_dT  = (dVdseff_dT  - T9 * dEsatL_dT)  / EsatL;
		  else  dfgche2_dT  = 0.0;

		  gche = beta * fgche1 / fgche2;
		  dgche_dVg = (beta * dfgche1_dVg + fgche1 * dbeta_dVg
			    - gche * dfgche2_dVg) / fgche2;
		  dgche_dVd = (beta * dfgche1_dVd + fgche1 * dbeta_dVd
			    - gche * dfgche2_dVd) / fgche2;
		  dgche_dVb = (beta * dfgche1_dVb + fgche1 * dbeta_dVb
			    - gche * dfgche2_dVb) / fgche2;
		  dgche_dVc = 0.0;
		  if (selfheat)
		     dgche_dT  = (beta * dfgche1_dT  + fgche1 * dbeta_dT
			       - gche * dfgche2_dT)  / fgche2;
		  else dgche_dT  = 0.0;

		  T0 = 1.0 + gche * Rds;
		  T9 = Vdseff / T0;
		  Idl = gche * T9;

/*  Whoa, these formulas for the derivatives of Idl are convoluted, but I
    verified them to be correct  */

		  dIdl_dVg = (gche * dVdseff_dVg + T9 * dgche_dVg) / T0
			   - Idl * gche / T0 * dRds_dVg ; 
		  dIdl_dVd = (gche * dVdseff_dVd + T9 * dgche_dVd) / T0; 
		  dIdl_dVb = (gche * dVdseff_dVb + T9 * dgche_dVb 
			   - Idl * dRds_dVb * gche) / T0; 
		  dIdl_dVc  = 0.0;
		  if (selfheat)
		     dIdl_dT  = (gche * dVdseff_dT + T9 * dgche_dT
			      - Idl * dRds_dT * gche) / T0;
		  else dIdl_dT  = 0.0;

		  T9 =  diffVds / Va;
		  T0 =  1.0 + T9;
		  here->B3SOIids = Ids = Idl * T0;

		  Gm0 = T0 * dIdl_dVg - Idl * (dVdseff_dVg + T9 * dVa_dVg) / Va;
		  Gds0 = T0 * dIdl_dVd + Idl * (1.0 - dVdseff_dVd
			    - T9 * dVa_dVd) / Va;
		  Gmb0 = T0 * dIdl_dVb - Idl * (dVdseff_dVb + T9 * dVa_dVb) / Va;
                  Gmc = 0.0;
                  if (selfheat) 
		     GmT0 = T0 * dIdl_dT - Idl * (dVdseff_dT + T9 * dVa_dT) / Va;
                  else GmT0 = 0.0;

/* This includes all dependencies from Vgsteff, Vbseff, Vcs */

		  Gm = Gm0 * dVgsteff_dVg + Gmb0 * dVbseff_dVg + Gmc * dVcs_dVg;
		  Gmb = Gm0 * dVgsteff_dVb + Gmb0 * dVbseff_dVb + Gmc * dVcs_dVb;
		  Gds = Gm0 * dVgsteff_dVd + Gmb0 * dVbseff_dVd + Gmc * dVcs_dVd + Gds0;
		  Gme = Gm0 * dVgsteff_dVe + Gmb0 * dVbseff_dVe + Gmc * dVcs_dVe;
		  if (selfheat)
		     GmT = Gm0 * dVgsteff_dT + Gmb0 * dVbseff_dT  + Gmc * dVcs_dT + GmT0;
		  else GmT = 0.0;


/* calculate substrate current Iii */

		  Giig = Giib = Giid = Giie = GiiT = 0.0;
		  here->B3SOIiii = Iii = 0.0;

		  Idgidl = Gdgidld = Gdgidlg = 0.0;
		  Isgidl = Gsgidlg = 0;

		  here->B3SOIibs = Ibs = 0.0;
		  here->B3SOIibd = Ibd = 0.0;
		  here->B3SOIic = Ic = 0.0;

                  Gjsb = Gjsd = GjsT = 0.0;
                  Gjdb = Gjdd = GjdT = 0.0;
                  Gcd = Gcb = GcT = 0.0;

		  here->B3SOIibp = Ibp = 0.0;
		  here->B3SOIgbpbs = here->B3SOIgbpgs = here->B3SOIgbpds = 0.0;
		  here->B3SOIgbpes = here->B3SOIgbpps = here->B3SOIgbpT = here->B3SOIcbodcon = 0.0;
                  Gbpbs = Gbpgs = Gbpds = Gbpes = Gbpps = GbpT = 0.0;

		  /*  Current going out of drainprime node into the drain of device  */
		  /*  "node" means the SPICE circuit node  */

                  here->B3SOIcdrain = Ids + Ic;
                  here->B3SOIcd = Ids + Ic - Ibd + Iii + Idgidl;
                  here->B3SOIcb = Ibs + Ibd + Ibp - Iii - Idgidl - Isgidl;

   		  here->B3SOIgds = Gds + Gcd;
   		  here->B3SOIgm = Gm;
   		  here->B3SOIgmbs = Gmb + Gcb;
		  here->B3SOIgme = Gme;
		  if (selfheat)
		      here->B3SOIgmT = GmT + GcT;
		  else
		      here->B3SOIgmT = 0.0;

                  /*  note that sign is switched because power flows out 
                      of device into the temperature node.  
                      Currently ommit self-heating due to bipolar current
                      because it can cause convergence problem*/

                  here->B3SOIgtempg = -Gm * Vds;
                  here->B3SOIgtempb = -Gmb * Vds;
                  here->B3SOIgtempe = -Gme * Vds;
                  here->B3SOIgtempT = -GmT * Vds;
                  here->B3SOIgtempd = -Gds * Vds - Ids;
		  here->B3SOIcth = - Ids * Vds - model->B3SOItype * 
                                  (here->B3SOIgtempg * Vgs + here->B3SOIgtempb * Vbs 
                                 + here->B3SOIgtempe * Ves + here->B3SOIgtempd * Vds)
                                 - here->B3SOIgtempT * delTemp;

                  /*  Body current which flows into drainprime node from the drain of device  */

		  here->B3SOIgjdb = Gjdb - Giib;
		  here->B3SOIgjdd = Gjdd - (Giid + Gdgidld);
		  here->B3SOIgjdg = - (Giig + Gdgidlg);
		  here->B3SOIgjde = - Giie;
		  if (selfheat) here->B3SOIgjdT = GjdT - GiiT;
		  else here->B3SOIgjdT = 0.0;
		  here->B3SOIcjd = Ibd - Iii - Idgidl - here->B3SOIminIsub/2
				 - (here->B3SOIgjdb * Vbs + here->B3SOIgjdd * Vds
				 +  here->B3SOIgjdg * Vgs + here->B3SOIgjde * Ves
				 +  here->B3SOIgjdT * delTemp);

		  /*  Body current which flows into sourceprime node from the source of device  */

   		  here->B3SOIgjsb = Gjsb;
   		  here->B3SOIgjsd = Gjsd;
		  here->B3SOIgjsg = - Gsgidlg;
		  if (selfheat) here->B3SOIgjsT = GjsT;
		  else here->B3SOIgjsT = 0.0;
		  here->B3SOIcjs = Ibs - Isgidl - here->B3SOIminIsub/2
				  - (here->B3SOIgjsb * Vbs + here->B3SOIgjsd * Vds
				  +  here->B3SOIgjsg * Vgs + here->B3SOIgjsT * delTemp);

		  /*  Current flowing into body node  */

		  here->B3SOIgbbs = Giib - Gjsb - Gjdb - Gbpbs;
		  here->B3SOIgbgs = Giig + Gdgidlg + Gsgidlg - Gbpgs;
   		  here->B3SOIgbds = Giid + Gdgidld - Gjsd - Gjdd - Gbpds;
		  here->B3SOIgbes = Giie - Gbpes;
		  here->B3SOIgbps = - Gbpps;
		  if (selfheat) here->B3SOIgbT = GiiT - GjsT - GjdT - GbpT;
		  else here->B3SOIgbT = 0.0;
		  here->B3SOIcbody = Iii + Idgidl + Isgidl - Ibs - Ibd - Ibp + here->B3SOIminIsub
				   - (here->B3SOIgbbs * Vbs + here->B3SOIgbgs * Vgs
				   + here->B3SOIgbds * Vds + here->B3SOIgbps * Vps
				   + here->B3SOIgbes * Ves + here->B3SOIgbT * delTemp);

	/* Calculate Qinv for Noise analysis */

		  T1 = Vgsteff * (1.0 - 0.5 * Abeff * Vdseff / Vgst2Vtm);
		  here->B3SOIqinv = -model->B3SOIcox * pParam->B3SOIweff * Leff * T1;

	/*  Begin CV (charge) model  */

		  if ((model->B3SOIxpart < 0) || (!ChargeComputationNeeded))
		  {   qgate  = qdrn = qsrc = qbody = 0.0;
		      here->B3SOIcggb = here->B3SOIcgsb = here->B3SOIcgdb = 0.0;
		      here->B3SOIcdgb = here->B3SOIcdsb = here->B3SOIcddb = 0.0;
		      here->B3SOIcbgb = here->B3SOIcbsb = here->B3SOIcbdb = 0.0;
		      goto finished;
		  }
		  else 
		  {
		       CoxWL  = model->B3SOIcox * pParam->B3SOIweffCV
			      * pParam->B3SOIleffCV;

                       /* By using this Vgsteff,cv, discontinuity in moderate
                          inversion charges can be avoid.  However, in capMod=3,
                          Vdsat from IV is used.  The dVdsat_dVg is referred to
                          the IV Vgsteff and therefore induces error in the charges
                          derivatives.  Fortunately, Vgsteff,iv and Vgsteff,cv are
                          different only in subthreshold where Qsubs is neglectible.
                          So the errors in derivatives is not a serious problem */

		       if ((VgstNVt > -EXP_THRESHOLD) && (VgstNVt < EXP_THRESHOLD))
		       {   ExpVgst *= ExpVgst;
			   Vgsteff = n * Vtm * log(1.0 + ExpVgst);
			   T0 = ExpVgst / (1.0 + ExpVgst);
			   T1 = -T0 * (dVth_dVb + Vgst / n * dn_dVb) + Vgsteff / n * dn_dVb;
			   dVgsteff_dVd = -T0 * (dVth_dVd + Vgst / n * dn_dVd)
					  + Vgsteff / n * dn_dVd + T1 * dVbseff_dVd;
			   dVgsteff_dVg = T0 * dVgs_eff_dVg + T1 * dVbseff_dVg;
                           dVgsteff_dVb = T1 * dVbseff_dVb;
                           dVgsteff_dVe = T1 * dVbseff_dVe;
                           if (selfheat)
			      dVgsteff_dT  = -T0 * (dVth_dT + Vgst / Temp) + Vgsteff / Temp
					  + T1 * dVbseff_dT;
                           else dVgsteff_dT  = 0.0;
		       } 

		       Vfb = Vth - phi - pParam->B3SOIk1 * sqrtPhis;

		       dVfb_dVb = dVth_dVb - pParam->B3SOIk1 * dsqrtPhis_dVb;
		       dVfb_dVd = dVth_dVd;
		       dVfb_dT  = dVth_dT;

		      if ((model->B3SOIcapMod == 2) || (model->B3SOIcapMod == 3))
		      {   
			  /* Necessary because charge behaviour very strange at 
			     Vgsteff = 0 */
			  Vgsteff += 1e-4;

			  /* Something common in capMod 2 and 3 */
			  V3 = Vfb - Vgs_eff + Vbseff - DELTA_3;
			  if (Vfb <= 0.0)
			  {   T0 = sqrt(V3 * V3 - 4.0 * DELTA_3 * Vfb);
			      T2 = -DELTA_3 / T0;
			  }
			  else
			  {   T0 = sqrt(V3 * V3 + 4.0 * DELTA_3 * Vfb);
			      T2 = DELTA_3 / T0;
			  }

			  T1 = 0.5 * (1.0 + V3 / T0);
			  Vfbeff = Vfb - 0.5 * (V3 + T0);
			  dVfbeff_dVd = (1.0 - T1 - T2) * dVfb_dVd;
			  dVfbeff_dVb = (1.0 - T1 - T2) * dVfb_dVb - T1;
			  dVfbeff_dVrg = T1 * dVgs_eff_dVg;
			  if (selfheat) dVfbeff_dT  = (1.0 - T1 - T2) * dVfb_dT;
                          else  dVfbeff_dT = 0.0;

			  Qac0 = -CoxWL * (Vfbeff - Vfb);
			  dQac0_dVrg = -CoxWL * dVfbeff_dVrg;
			  dQac0_dVd = -CoxWL * (dVfbeff_dVd - dVfb_dVd);
			  dQac0_dVb = -CoxWL * (dVfbeff_dVb - dVfb_dVb);
			  if (selfheat) dQac0_dT  = -CoxWL * (dVfbeff_dT - dVfb_dT);
                          else  dQac0_dT = 0.0;

			  T0 = 0.5 * K1;
			  T3 = Vgs_eff - Vfbeff - Vbseff - Vgsteff;
			  if (pParam->B3SOIk1 == 0.0)
			  {   T1 = 0.0;
			      T2 = 0.0;
			  }
			  else if (T3 < 0.0)
			  {   T1 = T0 + T3 / pParam->B3SOIk1;
			      T2 = CoxWL;
			  }
			  else
			  {   T1 = sqrt(T0 * T0 + T3);
			      T2 = CoxWL * T0 / T1;
			  }

			  Qsub0 = CoxWL * K1 * (T0 - T1);

			  dQsub0_dVrg = T2 * (dVfbeff_dVrg - dVgs_eff_dVg);
			  dQsub0_dVg = T2;
			  dQsub0_dVd = T2 * dVfbeff_dVd;
			  dQsub0_dVb = T2 * (dVfbeff_dVb + 1);
			  if (selfheat) dQsub0_dT  = T2 * dVfbeff_dT;
                          else  dQsub0_dT = 0.0;

			  One_Third_CoxWL = CoxWL / 3.0;
			  Two_Third_CoxWL = 2.0 * One_Third_CoxWL;
			  AbulkCV = Abulk0 * pParam->B3SOIabulkCVfactor;
			  dAbulkCV_dVb = pParam->B3SOIabulkCVfactor * dAbulk0_dVb;

			  /*  This is actually capMod=2 calculation  */
			  VdsatCV = Vgsteff / AbulkCV;
			  dVdsatCV_dVg = 1.0 / AbulkCV;
			  dVdsatCV_dVb = -VdsatCV * dAbulkCV_dVb / AbulkCV;
                          VdsatCV += 1e-5;

			  V4 = VdsatCV - Vds - DELTA_4;
			  T0 = sqrt(V4 * V4 + 4.0 * DELTA_4 * VdsatCV);
			  VdseffCV = VdsatCV - 0.5 * (V4 + T0);
			  T1 = 0.5 * (1.0 + V4 / T0);
			  T2 = DELTA_4 / T0;
			  T3 = (1.0 - T1 - T2) / AbulkCV;
			  dVdseffCV_dVg = T3;
			  dVdseffCV_dVd = T1;
			  dVdseffCV_dVb = -T3 * VdsatCV * dAbulkCV_dVb;

			  if (model->B3SOIcapMod == 2)
			  {
			     /* VdsCV Make it compatible with capMod 3 */
			     VdsCV = VdseffCV;
			     dVdsCV_dVg = dVdseffCV_dVg;
			     dVdsCV_dVd = dVdseffCV_dVd;
			     dVdsCV_dVb = dVdseffCV_dVb;
			 }
			 else if (model->B3SOIcapMod == 3)
			 {   
			    /* Front gate strong inversion depletion charge */
			    /* VdssatCV calculation */
	   
			     T1 = Vgsteff + K1*sqrtPhis + 0.5*K1*K1;
			     T2 = Vgsteff + K1*sqrtPhis + Phis + 0.25*K1*K1;
	   
			     dT1_dVb = K1*dsqrtPhis_dVb;
			     dT2_dVb = dT1_dVb + dPhis_dVb;
			     dT1_dVg = dT2_dVg = 1;
	   
                             /* Note VdsatCV is redefined in capMod = 3 */
			     VdsatCV = T1 - K1*sqrt(T2);
	   
			     dVdsatCV_dVb = dT1_dVb - K1/2/sqrt(T2)*dT2_dVb;
			     dVdsatCV_dVg = dT1_dVg - K1/2/sqrt(T2)*dT2_dVg;
	   
			     T1 = VdsatCV - Vdsat;
			     dT1_dVg = dVdsatCV_dVg - dVdsat_dVg;
			     dT1_dVb = dVdsatCV_dVb - dVdsat_dVb;
			     dT1_dVd = - dVdsat_dVd;
			     dT1_dVc = - dVdsat_dVc;
			     dT1_dT  = - dVdsat_dT;
	   
			     if (!(T1 == 0.0)) 
			     {  T3 = -0.5 * Vdsat / T1;  /* Vdsmax */
				T2 = T3 * Vdsat; 
				T4 = T2 + T1 * T3 * T3;     /* fmax */
				if ((Vdseff > T2) && (T1 < 0)) 
				{  
				   VdsCV = T4;
				   T5 = -0.5 / (T1 * T1);
				   dT3_dVg = T5 * (T1 * dVdsat_dVg - Vdsat * dT1_dVg);
				   dT3_dVb = T5 * (T1 * dVdsat_dVb - Vdsat * dT1_dVb);
				   dT3_dVd = T5 * (T1 * dVdsat_dVd - Vdsat * dT1_dVd);
                                   dT3_dVc=0.0;
                                   if (selfheat)
				      dT3_dT=T5 * (T1 * dVdsat_dT  - Vdsat * dT1_dT);
                                   else  dT3_dT=0.0;
	   
				   dVdsCV_dVd = T3 * dVdsat_dVd + Vdsat * dT3_dVd
					      + T3 * (2 * T1 * dT3_dVd + T3 * dT1_dVd);
				   dVdsCV_dVg = T3 * dVdsat_dVg + Vdsat * dT3_dVg
					      + T3 * (2 * T1 * dT3_dVg + T3 * dT1_dVg);
				   dVdsCV_dVb = T3 * dVdsat_dVb + Vdsat * dT3_dVb
					      + T3 * (2 * T1 * dT3_dVb + T3 * dT1_dVb);
                                   dVdsCV_dVc = 0.0;
                                   if (selfheat)
				      dVdsCV_dT  = T3 * dVdsat_dT  + Vdsat * dT3_dT 
				   	         + T3 * (2 * T1 * dT3_dT  + T3 * dT1_dT );
                                   else  dVdsCV_dT = 0.0;
				} else 
				{  T5 = Vdseff / Vdsat;
				   T6 = T5 * T5;
				   T7 = 2 * T1 * T5 / Vdsat;
				   T8 = T7 / Vdsat;
				   VdsCV = Vdseff + T1 * T6;
				   dVdsCV_dVd = dVdseff_dVd + T8 * 
					      ( Vdsat * dVdseff_dVd 
					      - Vdseff * dVdsat_dVd)
					      + T6 * dT1_dVd;
				   dVdsCV_dVb = dVdseff_dVb + T8 * ( Vdsat * 
						dVdseff_dVb - Vdseff * dVdsat_dVb)
						+ T6 * dT1_dVb;
				   dVdsCV_dVg = dVdseff_dVg + T8 * 
					      ( Vdsat * dVdseff_dVg 
					      - Vdseff * dVdsat_dVg)
					      + T6 * dT1_dVg;
                                   dVdsCV_dVc = 0.0;
				   if (selfheat)
				      dVdsCV_dT  = dVdseff_dT  + T8 *
					         ( Vdsat * dVdseff_dT  
					         - Vdseff * dVdsat_dT )
					         + T6 * dT1_dT ;
				   else dVdsCV_dT  = 0.0;
				}
			     } else 
			     {  VdsCV = Vdseff;
				dVdsCV_dVb = dVdseff_dVb;
				dVdsCV_dVd = dVdseff_dVd;
				dVdsCV_dVg = dVdseff_dVg;
				dVdsCV_dVc = dVdseff_dVc;
				dVdsCV_dT  = dVdseff_dT;
			     }
			     if (VdsCV < 0.0) VdsCV = 0.0;
			     VdsCV += 1e-4;
	   
			     if (VdsCV > (VdsatCV - 1e-7))
			     {
				VdsCV = VdsatCV - 1e-7;
			     }
			     Phisd = Phis + VdsCV;
			     dPhisd_dVb = dPhis_dVb + dVdsCV_dVb;
			     dPhisd_dVd = dVdsCV_dVd;
			     dPhisd_dVg = dVdsCV_dVg;
			     dPhisd_dVc = dVdsCV_dVc;
			     dPhisd_dT  = dVdsCV_dT;
			     sqrtPhisd = sqrt(Phisd);
	    
			     /* Qdep0 - Depletion charge at Vgs=Vth */
			     T10 = CoxWL * K1;
			     Qdep0 = T10 * sqrtPhis;
			     dQdep0_dVb = T10 * dsqrtPhis_dVb; 
			  }    /* End of if capMod == 3 */

		         /* Something common in both capMod 2 or 3 */

		         /* Backgate charge */
		         CboxWL = pParam->B3SOIkb3 * Cbox * pParam->B3SOIweffCV 
                                * pParam->B3SOIleffCV;
			 Cbg = Cbb = Cbd = Cbe = CbT = 0.0;
			 Ce2g = Ce2b = Ce2d = Ce2e = Ce2T = 0.0;
                         Qbf = Qe2 = 0.0;

			 T2 = - 0.5 * model->B3SOIcboxt * pParam->B3SOIweffCV
			    * pParam->B3SOIleffCV;
			 Qe1 = T2 * VdsCV - CboxWL * (Vbs0eff - Vesfb);
			 Ce1g = T2 * (dVdsCV_dVg * dVgsteff_dVg + dVdsCV_dVb * dVbseff_dVg
                                + dVdsCV_dVc * dVcs_dVg) - CboxWL * dVbs0eff_dVg;
			 Ce1d = T2 * (dVdsCV_dVg * dVgsteff_dVd + dVdsCV_dVb * dVbseff_dVd
                                + dVdsCV_dVc * dVcs_dVd + dVdsCV_dVd) 
                                - CboxWL * dVbs0eff_dVd;
                         Ce1b = 0.0;
			 Ce1e = T2 * (dVdsCV_dVg * dVgsteff_dVe + dVdsCV_dVb * dVbseff_dVe
                                + dVdsCV_dVc * dVcs_dVe) 
                                - CboxWL * (dVbs0eff_dVe - 1.0);
                         if (selfheat)
                            Ce1T = T2 * (dVdsCV_dVg  * dVgsteff_dT  + dVdsCV_dVb * dVbseff_dT
                                   + dVdsCV_dVc * dVcs_dT + dVdsCV_dT)
                                   - CboxWL * (dVbs0eff_dT + dvfbb_dT);
                         else Ce1T = 0.0;

			  /* Total inversion charge */
			  T0 = AbulkCV * VdseffCV;
			  T1 = 12.0 * (Vgsteff - 0.5 * T0 + 1e-20);
			  T2 = VdseffCV / T1;
			  T3 = T0 * T2;

			  T4 = (1.0 - 12.0 * T2 * T2 * AbulkCV);
			  T5 = (6.0 * T0 * (4.0 * Vgsteff - T0) / (T1 * T1) - 0.5);
			  T6 = 12.0 * T2 * T2 * Vgsteff;

			  qinv = CoxWL * (Vgsteff - 0.5 * VdseffCV + T3);
			  Cgg1 = CoxWL * (T4 + T5 * dVdseffCV_dVg);
			  Cgd1 = CoxWL * T5 * dVdseffCV_dVd;
			  Cgb1 = CoxWL * (T5 * dVdseffCV_dVb + T6 * dAbulkCV_dVb);
	     
			  /* Inversion charge partitioning into S / D */
			  if (model->B3SOIxpart > 0.5)
			  {   /* 0/100 Charge partition model */
			     T1 = T1 + T1;
			     qsrc = -CoxWL * (0.5 * Vgsteff + 0.25 * T0
				  - T0 * T0 / T1);
			     T7 = (4.0 * Vgsteff - T0) / (T1 * T1);
			     T4 = -(0.5 + 24.0 * T0 * T0 / (T1 * T1));
			     T5 = -(0.25 * AbulkCV - 12.0 * AbulkCV * T0 * T7);
			     T6 = -(0.25 * VdseffCV - 12.0 * T0 * VdseffCV * T7);
			     Csg1 = CoxWL * (T4 + T5 * dVdseffCV_dVg);
			     Csd1 = CoxWL * T5 * dVdseffCV_dVd;
			     Csb1 = CoxWL * (T5 * dVdseffCV_dVb + T6 * dAbulkCV_dVb);
				 
			  }
			  else if (model->B3SOIxpart < 0.5)
			  {   /* 40/60 Charge partition model */
			     T1 = T1 / 12.0;
			     T2 = 0.5 * CoxWL / (T1 * T1);
			     T3 = Vgsteff * (2.0 * T0 * T0 / 3.0 + Vgsteff
				* (Vgsteff - 4.0 * T0 / 3.0))
				- 2.0 * T0 * T0 * T0 / 15.0;
			     qsrc = -T2 * T3;
			     T7 = 4.0 / 3.0 * Vgsteff * (Vgsteff - T0)
				+ 0.4 * T0 * T0;
			     T4 = -2.0 * qsrc / T1 - T2 * (Vgsteff * (3.0
				* Vgsteff - 8.0 * T0 / 3.0)
				   + 2.0 * T0 * T0 / 3.0);
			     T5 = (qsrc / T1 + T2 * T7) * AbulkCV;
			     T6 = (qsrc / T1 * VdseffCV + T2 * T7 * VdseffCV);
			     Csg1 = T4 + T5 * dVdseffCV_dVg;
			     Csd1 = T5 * dVdseffCV_dVd;
			     Csb1 = T5 * dVdseffCV_dVb + T6 * dAbulkCV_dVb;
			  }
			  else
			  {   /* 50/50 Charge partition model */
			     qsrc = - 0.5 * qinv;
			     Csg1 = - 0.5 * Cgg1;
			     Csb1 = - 0.5 * Cgb1;
			     Csd1 = - 0.5 * Cgd1;
			  }

			  Csg = Csg1 * dVgsteff_dVg + Csb1 * dVbseff_dVg;
			  Csd = Csd1 + Csg1 * dVgsteff_dVd + Csb1 * dVbseff_dVd;
   			  Csb = Csg1 * dVgsteff_dVb + Csb1 * dVbseff_dVb;
			  Cse = Csg1 * dVgsteff_dVe + Csb1 * dVbseff_dVe;
                          if (selfheat)
                             CsT = Csg1 * dVgsteff_dT  + Csb1 * dVbseff_dT;
                          else  CsT = 0.0;
 
                          Qex=dQex_dVg=dQex_dVb=dQex_dVd=dQex_dVe=dQex_dT=0.0;

			  qgate = qinv - (Qbf + Qe2);
   			  qbody = (Qbf - Qe1 + Qex);
			  qsub = Qe1 + Qe2 - Qex;
			  qdrn = -(qinv + qsrc);

			  Cgg = (Cgg1 * dVgsteff_dVg + Cgb1 * dVbseff_dVg) - Cbg ;
			  Cgd = (Cgd1  + Cgg1 * dVgsteff_dVd + Cgb1 * dVbseff_dVd)-Cbd;
			  Cgb = (Cgb1 * dVbseff_dVb + Cgg1 * dVgsteff_dVb) - Cbb;
			  Cge = (Cgg1 * dVgsteff_dVe + Cgb1 * dVbseff_dVe) - Cbe;
                          if (selfheat)
                             CgT = (Cgg1 * dVgsteff_dT  + Cgb1 * dVbseff_dT ) - CbT;
                          else  CgT = 0.0;

			  here->B3SOIcggb = Cgg - Ce2g;
			  here->B3SOIcgsb = - (Cgg  + Cgd  + Cgb  + Cge)
					  + (Ce2g + Ce2d + Ce2b + Ce2e);
			  here->B3SOIcgdb = Cgd - Ce2d;
			  here->B3SOIcgeb = Cge - Ce2e;
                          here->B3SOIcgT = CgT - Ce2T;

			  here->B3SOIcbgb = Cbg - Ce1g + dQex_dVg;
			  here->B3SOIcbsb = -(Cbg  + Cbd  + Cbb  + Cbe)
					  + (Ce1g + Ce1d + Ce1b + Ce1e)
                                          - (dQex_dVg + dQex_dVd + dQex_dVb + dQex_dVe);
			  here->B3SOIcbdb = Cbd - Ce1d + dQex_dVd;
			  here->B3SOIcbeb = Cbe - Ce1e + dQex_dVe;
                          here->B3SOIcbT = CbT - Ce1T + dQex_dT;

			  here->B3SOIcegb = Ce1g + Ce2g - dQex_dVg;
			  here->B3SOIcesb = -(Ce1g + Ce1d + Ce1b + Ce1e)
					  -(Ce2g + Ce2d + Ce2b + Ce2e)
                                          +(dQex_dVg + dQex_dVd + dQex_dVb + dQex_dVe);
			  here->B3SOIcedb = Ce1d + Ce2d - dQex_dVd;
			  here->B3SOIceeb = Ce1e + Ce2e - dQex_dVe;
                          here->B3SOIceT = Ce1T + Ce2T - dQex_dT;
 
			  here->B3SOIcdgb = -(Cgg + Cbg + Csg);
			  here->B3SOIcddb = -(Cgd + Cbd + Csd);
			  here->B3SOIcdeb = -(Cge + Cbe + Cse);
                          here->B3SOIcdT = -(CgT + CbT + CsT);
			  here->B3SOIcdsb = (Cgg + Cgd + Cgb + Cge 
					  + Cbg + Cbd + Cbb + Cbe 
					  + Csg + Csd + Csb + Cse); 

		      } /* End of if capMod == 2 or capMod ==3 */
		  }

	finished: /* returning Values to Calling Routine */
		  /*
		   *  COMPUTE EQUIVALENT DRAIN CURRENT SOURCE
		   */
		  if (ChargeComputationNeeded)
		  {   
                      qjs = qjd = 0.0;
                      gcjdds = gcjdbs = gcjdT = 0.0;
                      gcjsbs = gcjsT = 0.0;

		      qdrn -= qjd;
		      qbody += (qjs + qjd);
		      qsrc = -(qgate + qbody + qdrn + qsub);

		      /* Update the conductance */
		      here->B3SOIcddb -= gcjdds;
                      here->B3SOIcdT -= gcjdT;
		      here->B3SOIcdsb += gcjdds + gcjdbs;

		      here->B3SOIcbdb += (gcjdds);
                      here->B3SOIcbT += (gcjdT + gcjsT);
		      here->B3SOIcbsb -= (gcjdds + gcjdbs + gcjsbs);

		      /* Extrinsic Bottom S/D to substrate charge */
		      T10 = -model->B3SOItype * ves;
		      /* T10 is vse without type conversion */
		      if ( ((pParam->B3SOInsub > 0) && (model->B3SOItype > 0)) ||
		           ((pParam->B3SOInsub < 0) && (model->B3SOItype < 0)) )
		      {
		         if (T10 < pParam->B3SOIvsdfb)
		         {  here->B3SOIqse = here->B3SOIcsbox * (T10 - pParam->B3SOIvsdfb);
			    here->B3SOIgcse = here->B3SOIcsbox;
		         }
		         else if (T10 < pParam->B3SOIsdt1)
		         {  T0 = T10 - pParam->B3SOIvsdfb;
			    T1 = T0 * T0;
			    here->B3SOIqse = T0 * (here->B3SOIcsbox - 
                                             pParam->B3SOIst2 / 3 * T1) ;
			    here->B3SOIgcse = here->B3SOIcsbox - pParam->B3SOIst2 * T1;
		         }
		         else if (T10 < pParam->B3SOIvsdth)
		         {  T0 = T10 - pParam->B3SOIvsdth;
			    T1 = T0 * T0;
			    here->B3SOIqse = here->B3SOIcsmin * T10 + here->B3SOIst4 + 
                                             pParam->B3SOIst3 / 3 * T0 * T1;
   			 here->B3SOIgcse = here->B3SOIcsmin + pParam->B3SOIst3 * T1;
		         }
		         else 
		         {  here->B3SOIqse = here->B3SOIcsmin * T10 + here->B3SOIst4;
			    here->B3SOIgcse = here->B3SOIcsmin;
		         }
		      } else
		      {
		         if (T10 < pParam->B3SOIvsdth)
		         {  here->B3SOIqse = here->B3SOIcsmin * (T10 - pParam->B3SOIvsdth);
			    here->B3SOIgcse = here->B3SOIcsmin;
		         }
		         else if (T10 < pParam->B3SOIsdt1)
		         {  T0 = T10 - pParam->B3SOIvsdth;
   			    T1 = T0 * T0;
   			    here->B3SOIqse = T0 * (here->B3SOIcsmin - pParam->B3SOIst2 / 3 * T1) ;
			    here->B3SOIgcse = here->B3SOIcsmin - pParam->B3SOIst2 * T1;
		         }
		         else if (T10 < pParam->B3SOIvsdfb)
		         {  T0 = T10 - pParam->B3SOIvsdfb;
			    T1 = T0 * T0;
			    here->B3SOIqse = here->B3SOIcsbox * T10 + here->B3SOIst4 + 
                                             pParam->B3SOIst3 / 3 * T0 * T1;
			    here->B3SOIgcse = here->B3SOIcsbox + pParam->B3SOIst3 * T1;
		         }
		         else 
		         {  here->B3SOIqse = here->B3SOIcsbox * T10 + here->B3SOIst4;
			    here->B3SOIgcse = here->B3SOIcsbox;
		         }
		      }

		      /* T11 is vde without type conversion */
		      T11 = model->B3SOItype * (vds - ves);
		      if ( ((pParam->B3SOInsub > 0) && (model->B3SOItype > 0)) ||
		           ((pParam->B3SOInsub < 0) && (model->B3SOItype < 0)) )
		      {
		         if (T11 < pParam->B3SOIvsdfb)
		         {  here->B3SOIqde = here->B3SOIcdbox * (T11 - pParam->B3SOIvsdfb);
			    here->B3SOIgcde = here->B3SOIcdbox;
		         }
		         else if (T11 < pParam->B3SOIsdt1)
		         {  T0 = T11 - pParam->B3SOIvsdfb;
   			    T1 = T0 * T0;
   			    here->B3SOIqde = T0 * (here->B3SOIcdbox - pParam->B3SOIdt2 / 3 * T1) ;
			    here->B3SOIgcde = here->B3SOIcdbox - pParam->B3SOIdt2 * T1;
		         }
		         else if (T11 < pParam->B3SOIvsdth)
		         {  T0 = T11 - pParam->B3SOIvsdth;
			    T1 = T0 * T0;
			    here->B3SOIqde = here->B3SOIcdmin * T11 + here->B3SOIdt4 + 
                                             pParam->B3SOIdt3 / 3 * T0 * T1;
			    here->B3SOIgcde = here->B3SOIcdmin + pParam->B3SOIdt3 * T1;
		         }
		         else 
		         {  here->B3SOIqde = here->B3SOIcdmin * T11 + here->B3SOIdt4;
			    here->B3SOIgcde = here->B3SOIcdmin;
		         }
		      } else
		      {
		         if (T11 < pParam->B3SOIvsdth)
		         {  here->B3SOIqde = here->B3SOIcdmin * (T11 - pParam->B3SOIvsdth);
			    here->B3SOIgcde = here->B3SOIcdmin;
		         }
		         else if (T11 < pParam->B3SOIsdt1)
		         {  T0 = T11 - pParam->B3SOIvsdth;
   			    T1 = T0 * T0;
   			    here->B3SOIqde = T0 * (here->B3SOIcdmin - pParam->B3SOIdt2 / 3 * T1) ;
			    here->B3SOIgcde = here->B3SOIcdmin - pParam->B3SOIdt2 * T1;
		         }
		         else if (T11 < pParam->B3SOIvsdfb)
		         {  T0 = T11 - pParam->B3SOIvsdfb;
			    T1 = T0 * T0;
			    here->B3SOIqde = here->B3SOIcdbox * T11 + here->B3SOIdt4 + 
                                             pParam->B3SOIdt3 / 3 * T0 * T1;
			    here->B3SOIgcde = here->B3SOIcdbox + pParam->B3SOIdt3 * T1;
		         }
		         else 
		         {  here->B3SOIqde = here->B3SOIcdbox * T11 + here->B3SOIdt4;
			    here->B3SOIgcde = here->B3SOIcdbox;
		         }
		      } 

		      /* Extrinsic : Sidewall fringing S/D charge */
		      here->B3SOIqse += pParam->B3SOIcsesw * T10;
		      here->B3SOIgcse += pParam->B3SOIcsesw;
		      here->B3SOIqde += pParam->B3SOIcdesw * T11;
		      here->B3SOIgcde += pParam->B3SOIcdesw;

		      /* All charge are mutliplied with type at the end, but qse and qde
			 have true polarity => so pre-mutliplied with type */
		      here->B3SOIqse *= model->B3SOItype;
		      here->B3SOIqde *= model->B3SOItype;
		  }

		  here->B3SOIxc = Xc;
		  here->B3SOIcbb = Cbb;
		  here->B3SOIcbd = Cbd;
		  here->B3SOIcbg = Cbg;
		  here->B3SOIqbf = Qbf;
		  here->B3SOIqjs = qjs;
		  here->B3SOIqjd = qjd;

                  if (here->B3SOIdebugMod == -1)
                     ChargeComputationNeeded = 0;

		  /*
		   *  check convergence
		   */
		  if ((here->B3SOIoff == 0) || (!(ckt->CKTmode & MODEINITFIX)))
		  {   if (Check == 1)
		      {   ckt->CKTnoncon++;
if (here->B3SOIdebugMod > 2)
   fprintf(fpdebug, "Check is on, noncon=%d\n", ckt->CKTnoncon++);
#ifndef NEWCONV
		      } 
		      else
		      {   tol = ckt->CKTreltol * MAX(FABS(cdhat), FABS(here->B3SOIcd))
			      + ckt->CKTabstol;
			  if (FABS(cdhat - here->B3SOIcd) >= tol)
			  {   ckt->CKTnoncon++;
			  }
			  else
			  {   tol = ckt->CKTreltol * MAX(FABS(cbhat), 
				    FABS(here->B3SOIcbs + here->B3SOIcbd)) 
				    + ckt->CKTabstol;
			      if (FABS(cbhat - (here->B3SOIcbs + here->B3SOIcbd)) 
				  > tol)
			      {   ckt->CKTnoncon++;
			      }
			  }
#endif /* NEWCONV */
		      }
		  }

                  *(ckt->CKTstate0 + here->B3SOIvg) = vg;
                  *(ckt->CKTstate0 + here->B3SOIvd) = vd;
                  *(ckt->CKTstate0 + here->B3SOIvs) = vs;
                  *(ckt->CKTstate0 + here->B3SOIvp) = vp;
                  *(ckt->CKTstate0 + here->B3SOIve) = ve;

		  *(ckt->CKTstate0 + here->B3SOIvbs) = vbs;
		  *(ckt->CKTstate0 + here->B3SOIvbd) = vbd;
		  *(ckt->CKTstate0 + here->B3SOIvgs) = vgs;
		  *(ckt->CKTstate0 + here->B3SOIvds) = vds;
		  *(ckt->CKTstate0 + here->B3SOIves) = ves;
		  *(ckt->CKTstate0 + here->B3SOIvps) = vps;
		  *(ckt->CKTstate0 + here->B3SOIdeltemp) = delTemp;

		  /* bulk and channel charge plus overlaps */

		  if (!ChargeComputationNeeded)
		      goto line850; 
		 
	line755:
		  ag0 = ckt->CKTag[0];

		  T0 = vgd + DELTA_1;
		  T1 = sqrt(T0 * T0 + 4.0 * DELTA_1);
		  T2 = 0.5 * (T0 - T1);

		  T3 = pParam->B3SOIweffCV * pParam->B3SOIcgdl;
		  T4 = sqrt(1.0 - 4.0 * T2 / pParam->B3SOIckappa);
		  cgdo = pParam->B3SOIcgdo + T3 - T3 * (1.0 - 1.0 / T4)
			 * (0.5 - 0.5 * T0 / T1);
		  qgdo = (pParam->B3SOIcgdo + T3) * vgd - T3 * (T2
			 + 0.5 * pParam->B3SOIckappa * (T4 - 1.0));

		  T0 = vgs + DELTA_1;
		  T1 = sqrt(T0 * T0 + 4.0 * DELTA_1);
		  T2 = 0.5 * (T0 - T1);
		  T3 = pParam->B3SOIweffCV * pParam->B3SOIcgsl;
		  T4 = sqrt(1.0 - 4.0 * T2 / pParam->B3SOIckappa);
		  cgso = pParam->B3SOIcgso + T3 - T3 * (1.0 - 1.0 / T4)
			 * (0.5 - 0.5 * T0 / T1);
		  qgso = (pParam->B3SOIcgso + T3) * vgs - T3 * (T2
			 + 0.5 * pParam->B3SOIckappa * (T4 - 1.0));


		  if (here->B3SOImode > 0)
		  {   gcdgb = (here->B3SOIcdgb - cgdo) * ag0;
		      gcddb = (here->B3SOIcddb + cgdo + here->B3SOIgcde) * ag0;
		      gcdsb = here->B3SOIcdsb * ag0;
		      gcdeb = (here->B3SOIcdeb - here->B3SOIgcde) * ag0;
                      gcdT = model->B3SOItype * here->B3SOIcdT * ag0;

		      gcsgb = -(here->B3SOIcggb + here->B3SOIcbgb + here->B3SOIcdgb
			    + here->B3SOIcegb + cgso) * ag0;
		      gcsdb = -(here->B3SOIcgdb + here->B3SOIcbdb + here->B3SOIcddb
			    + here->B3SOIcedb) * ag0;
		      gcssb = (cgso + here->B3SOIgcse - (here->B3SOIcgsb + here->B3SOIcbsb
			    + here->B3SOIcdsb + here->B3SOIcesb)) * ag0;
		      gcseb = -(here->B3SOIgcse + here->B3SOIcgeb + here->B3SOIcbeb + here->B3SOIcdeb
			    + here->B3SOIceeb) * ag0;
                      gcsT = - model->B3SOItype * (here->B3SOIcgT + here->B3SOIcbT + here->B3SOIcdT
                            + here->B3SOIceT) * ag0;

		      gcggb = (here->B3SOIcggb + cgdo + cgso + pParam->B3SOIcgeo) * ag0;
		      gcgdb = (here->B3SOIcgdb - cgdo) * ag0;
		      gcgsb = (here->B3SOIcgsb - cgso) * ag0;
		      gcgeb = (here->B3SOIcgeb - pParam->B3SOIcgeo) * ag0;
                      gcgT = model->B3SOItype * here->B3SOIcgT * ag0;

		      gcbgb = here->B3SOIcbgb * ag0;
		      gcbdb = here->B3SOIcbdb * ag0;
		      gcbsb = here->B3SOIcbsb * ag0;
   		      gcbeb = here->B3SOIcbeb * ag0;
                      gcbT = model->B3SOItype * here->B3SOIcbT * ag0;

		      gcegb = (here->B3SOIcegb - pParam->B3SOIcgeo) * ag0;
		      gcedb = (here->B3SOIcedb - here->B3SOIgcde) * ag0;
		      gcesb = (here->B3SOIcesb - here->B3SOIgcse) * ag0;
		      gceeb = (here->B3SOIgcse + here->B3SOIgcde +
                               here->B3SOIceeb + pParam->B3SOIcgeo) * ag0;
 
                      gceT = model->B3SOItype * here->B3SOIceT * ag0;

                      gcTt = pParam->B3SOIcth * ag0;

		      sxpart = 0.6;
		      dxpart = 0.4;

		      /* Lump the overlap capacitance and S/D parasitics */
		      qgd = qgdo;
		      qgs = qgso;
		      qge = pParam->B3SOIcgeo * vge;
		      qgate += qgd + qgs + qge;
		      qdrn += here->B3SOIqde - qgd;
		      qsub -= qge + here->B3SOIqse + here->B3SOIqde; 
		      qsrc = -(qgate + qbody + qdrn + qsub);
		  }
		  else
		  {   gcsgb = (here->B3SOIcdgb - cgso) * ag0;
		      gcssb = (here->B3SOIcddb + cgso + here->B3SOIgcse) * ag0;
		      gcsdb = here->B3SOIcdsb * ag0;
		      gcseb = (here->B3SOIcdeb - here->B3SOIgcse) * ag0;
                      gcsT = model->B3SOItype * here->B3SOIcdT * ag0;

		      gcdgb = -(here->B3SOIcggb + here->B3SOIcbgb + here->B3SOIcdgb
			    + here->B3SOIcegb + cgdo) * ag0;
		      gcdsb = -(here->B3SOIcgdb + here->B3SOIcbdb + here->B3SOIcddb
			    + here->B3SOIcedb) * ag0;
		      gcddb = (cgdo + here->B3SOIgcde - (here->B3SOIcgsb + here->B3SOIcbsb 
			    + here->B3SOIcdsb + here->B3SOIcesb)) * ag0;
		      gcdeb = -(here->B3SOIgcde + here->B3SOIcgeb + here->B3SOIcbeb + here->B3SOIcdeb
			    + here->B3SOIceeb) * ag0;
                      gcdT = - model->B3SOItype * (here->B3SOIcgT + here->B3SOIcbT 
                             + here->B3SOIcdT + here->B3SOIceT) * ag0;

		      gcggb = (here->B3SOIcggb + cgdo + cgso + pParam->B3SOIcgeo) * ag0;
		      gcgsb = (here->B3SOIcgdb - cgso) * ag0;
		      gcgdb = (here->B3SOIcgsb - cgdo) * ag0;
		      gcgeb = (here->B3SOIcgeb - pParam->B3SOIcgeo) * ag0;
                      gcgT = model->B3SOItype * here->B3SOIcgT * ag0;

		      gcbgb = here->B3SOIcbgb * ag0;
		      gcbsb = here->B3SOIcbdb * ag0;
		      gcbdb = here->B3SOIcbsb * ag0;
		      gcbeb = here->B3SOIcbeb * ag0;
                      gcbT = model->B3SOItype * here->B3SOIcbT * ag0;

		      gcegb = (here->B3SOIcegb - pParam->B3SOIcgeo) * ag0;
		      gcesb = (here->B3SOIcedb - here->B3SOIgcse) * ag0;
		      gcedb = (here->B3SOIcesb - here->B3SOIgcde) * ag0;
		      gceeb = (here->B3SOIceeb + pParam->B3SOIcgeo +
			       here->B3SOIgcse + here->B3SOIgcde) * ag0;
                      gceT = model->B3SOItype * here->B3SOIceT * ag0;
		     
                      gcTt = pParam->B3SOIcth * ag0;

		      dxpart = 0.6;
		      sxpart = 0.4;

		      /* Lump the overlap capacitance */
		      qgd = qgdo;
		      qgs = qgso;
		      qge = pParam->B3SOIcgeo * vge;
		      qgate += qgd + qgs + qge;
		      qsrc = qdrn - qgs + here->B3SOIqse;
		      qsub -= qge + here->B3SOIqse + here->B3SOIqde; 
		      qdrn = -(qgate + qbody + qsrc + qsub);
		  }

		  here->B3SOIcgdo = cgdo;
		  here->B3SOIcgso = cgso;

                  if (ByPass) goto line860;

		  *(ckt->CKTstate0 + here->B3SOIqe) = qsub;
		  *(ckt->CKTstate0 + here->B3SOIqg) = qgate;
		  *(ckt->CKTstate0 + here->B3SOIqd) = qdrn;
                  *(ckt->CKTstate0 + here->B3SOIqb) = qbody;
		  if ((model->B3SOIshMod == 1) && (here->B3SOIrth0!=0.0)) 
                     *(ckt->CKTstate0 + here->B3SOIqth) = pParam->B3SOIcth * delTemp;


		  /* store small signal parameters */
		  if (ckt->CKTmode & MODEINITSMSIG)
		  {   goto line1000;
		  }
		  if (!ChargeComputationNeeded)
		      goto line850;
	       

		  if (ckt->CKTmode & MODEINITTRAN)
		  {   *(ckt->CKTstate1 + here->B3SOIqb) =
			    *(ckt->CKTstate0 + here->B3SOIqb);
		      *(ckt->CKTstate1 + here->B3SOIqg) =
			    *(ckt->CKTstate0 + here->B3SOIqg);
		      *(ckt->CKTstate1 + here->B3SOIqd) =
			    *(ckt->CKTstate0 + here->B3SOIqd);
		      *(ckt->CKTstate1 + here->B3SOIqe) =
			    *(ckt->CKTstate0 + here->B3SOIqe);
                      *(ckt->CKTstate1 + here->B3SOIqth) =
                            *(ckt->CKTstate0 + here->B3SOIqth);
		  }
	       
                  error = NIintegrate(ckt, &geq, &ceq,0.0,here->B3SOIqb);
                  if (error) return(error);
                  error = NIintegrate(ckt, &geq, &ceq, 0.0, here->B3SOIqg);
                  if (error) return(error);
                  error = NIintegrate(ckt,&geq, &ceq, 0.0, here->B3SOIqd);
                  if (error) return(error);
                  error = NIintegrate(ckt,&geq, &ceq, 0.0, here->B3SOIqe);
                  if (error) return(error);
		  if ((model->B3SOIshMod == 1) && (here->B3SOIrth0!=0.0)) 
                  {
                      error = NIintegrate(ckt, &geq, &ceq, 0.0, here->B3SOIqth);
                      if (error) return (error);
                  }
		 
		  goto line860;

	line850:
		  /* initialize to zero charge conductance and current */
		  ceqqe = ceqqg = ceqqb = ceqqd = ceqqth= 0.0;

		  gcdgb = gcddb = gcdsb = gcdeb = gcdT = 0.0;
		  gcsgb = gcsdb = gcssb = gcseb = gcsT = 0.0;
		  gcggb = gcgdb = gcgsb = gcgeb = gcgT = 0.0;
		  gcbgb = gcbdb = gcbsb = gcbeb = gcbT = 0.0;
		  gcegb = gcedb = gceeb = gcesb = gceT = 0.0;
                  gcTt = 0.0;

		  sxpart = (1.0 - (dxpart = (here->B3SOImode > 0) ? 0.4 : 0.6));

		  goto line900;
		    
	line860:
		  /* evaluate equivalent charge current */

                  cqgate = *(ckt->CKTstate0 + here->B3SOIcqg);
                  cqbody = *(ckt->CKTstate0 + here->B3SOIcqb);
                  cqdrn = *(ckt->CKTstate0 + here->B3SOIcqd);
                  cqsub = *(ckt->CKTstate0 + here->B3SOIcqe);
                  cqtemp = *(ckt->CKTstate0 + here->B3SOIcqth);

                  here->B3SOIcb += cqbody;
                  here->B3SOIcd += cqdrn;

		  ceqqg = cqgate - gcggb * vgb + gcgdb * vbd + gcgsb * vbs 
                          - gcgeb * veb - gcgT * delTemp;
		  ceqqb = cqbody - gcbgb * vgb + gcbdb * vbd + gcbsb * vbs
			  - gcbeb * veb - gcbT * delTemp;
		  ceqqd = cqdrn - gcdgb * vgb + gcddb * vbd + gcdsb * vbs 
                          - gcdeb * veb - gcdT * delTemp;
		  ceqqe = cqsub - gcegb * vgb + gcedb * vbd + gcesb * vbs
			  - gceeb * veb - gceT * delTemp;;
                  ceqqth = cqtemp - gcTt * delTemp;
	 
		  if (ckt->CKTmode & MODEINITTRAN)
		  {   *(ckt->CKTstate1 + here->B3SOIcqe) =  
			    *(ckt->CKTstate0 + here->B3SOIcqe);
		      *(ckt->CKTstate1 + here->B3SOIcqb) =  
			    *(ckt->CKTstate0 + here->B3SOIcqb);
		      *(ckt->CKTstate1 + here->B3SOIcqg) =  
			    *(ckt->CKTstate0 + here->B3SOIcqg);
		      *(ckt->CKTstate1 + here->B3SOIcqd) =  
			    *(ckt->CKTstate0 + here->B3SOIcqd);
		      *(ckt->CKTstate1 + here->B3SOIcqth) =  
			    *(ckt->CKTstate0 + here->B3SOIcqth);
		  }

		  /*
		   *  load current vector
		   */
	line900:

		  if (here->B3SOImode >= 0)
		  {   Gm = here->B3SOIgm;
		      Gmbs = here->B3SOIgmbs;
		      Gme = here->B3SOIgme;
		      GmT = model->B3SOItype * here->B3SOIgmT;
		      FwdSum = Gm + Gmbs + Gme;
		      RevSum = 0.0;
		      cdreq = model->B3SOItype * (here->B3SOIcdrain - here->B3SOIgds * vds
			    - Gm * vgs - Gmbs * vbs - Gme * ves - GmT * delTemp);
		      /* ceqbs now is compatible with cdreq, ie. going in is +ve */
		      /* Equivalent current source from the diode */
		      ceqbs = here->B3SOIcjs;
		      ceqbd = here->B3SOIcjd;
		      /* Current going in is +ve */
		      ceqbody = -here->B3SOIcbody;
		      ceqth = here->B3SOIcth;
		      ceqbodcon = here->B3SOIcbodcon;

		      gbbg  = -here->B3SOIgbgs;
		      gbbdp = -here->B3SOIgbds;
		      gbbb  = -here->B3SOIgbbs;
		      gbbe  = -here->B3SOIgbes;
		      gbbp  = -here->B3SOIgbps;
		      gbbT  = -model->B3SOItype * here->B3SOIgbT;
		      gbbsp = - ( gbbg + gbbdp + gbbb + gbbe + gbbp);

		      gddpg  = -here->B3SOIgjdg;
		      gddpdp = -here->B3SOIgjdd;
		      gddpb  = -here->B3SOIgjdb;
		      gddpe  = -here->B3SOIgjde;
		      gddpT  = -model->B3SOItype * here->B3SOIgjdT;
		      gddpsp = - ( gddpg + gddpdp + gddpb + gddpe);

		      gsspg  = -here->B3SOIgjsg;
		      gsspdp = -here->B3SOIgjsd;
		      gsspb  = -here->B3SOIgjsb;
		      gsspe  = 0.0;
		      gsspT  = -model->B3SOItype * here->B3SOIgjsT;
		      gsspsp = - (gsspg + gsspdp + gsspb + gsspe);

		      gppg = -here->B3SOIgbpgs;
		      gppdp = -here->B3SOIgbpds;
		      gppb = -here->B3SOIgbpbs;
		      gppe = -here->B3SOIgbpes;
		      gppp = -here->B3SOIgbpps;
		      gppT = -model->B3SOItype * here->B3SOIgbpT;
		      gppsp = - (gppg + gppdp + gppb + gppe + gppp);

                      gTtg  = here->B3SOIgtempg;
                      gTtb  = here->B3SOIgtempb;
                      gTte  = here->B3SOIgtempe;
                      gTtdp = here->B3SOIgtempd;
                      gTtt  = here->B3SOIgtempT;
                      gTtsp = - (gTtg + gTtb + gTte + gTtdp);
		  }
		  else
		  {   Gm = -here->B3SOIgm;
		      Gmbs = -here->B3SOIgmbs;
		      Gme = -here->B3SOIgme;
		      GmT = -model->B3SOItype * here->B3SOIgmT;
		      FwdSum = 0.0;
		      RevSum = -(Gm + Gmbs + Gme);
		      cdreq = -model->B3SOItype * (here->B3SOIcdrain + here->B3SOIgds*vds
			    + Gm * vgd + Gmbs * vbd + Gme * (ves - vds) + GmT * delTemp);
		      ceqbs = here->B3SOIcjd;
		      ceqbd = here->B3SOIcjs;
		      /* Current going in is +ve */
		      ceqbody = -here->B3SOIcbody;
		      ceqth = here->B3SOIcth;
		      ceqbodcon = here->B3SOIcbodcon;

		      gbbg  = -here->B3SOIgbgs;
		      gbbb  = -here->B3SOIgbbs;
		      gbbe  = -here->B3SOIgbes;
		      gbbp  = -here->B3SOIgbps;
		      gbbsp = -here->B3SOIgbds;
		      gbbT  = -model->B3SOItype * here->B3SOIgbT;
		      gbbdp = - ( gbbg + gbbsp + gbbb + gbbe + gbbp);

		      gddpg  = -here->B3SOIgjsg;
		      gddpsp = -here->B3SOIgjsd;
		      gddpb  = -here->B3SOIgjsb;
		      gddpe  = 0.0;
		      gddpT  = -model->B3SOItype * here->B3SOIgjsT;
		      gddpdp = - (gddpg + gddpsp + gddpb + gddpe);

		      gsspg  = -here->B3SOIgjdg;
		      gsspsp = -here->B3SOIgjdd;
		      gsspb  = -here->B3SOIgjdb;
		      gsspe  = -here->B3SOIgjde;
		      gsspT  = -model->B3SOItype * here->B3SOIgjdT;
		      gsspdp = - ( gsspg + gsspsp + gsspb + gsspe);

		      gppg = -here->B3SOIgbpgs;
		      gppsp = -here->B3SOIgbpds;
		      gppb = -here->B3SOIgbpbs;
		      gppe = -here->B3SOIgbpes;
		      gppp = -here->B3SOIgbpps;
		      gppT = -model->B3SOItype * here->B3SOIgbpT;
		      gppdp = - (gppg + gppsp + gppb + gppe + gppp);

                      gTtg  = here->B3SOIgtempg;
                      gTtb  = here->B3SOIgtempb;
                      gTte  = here->B3SOIgtempe;
                      gTtsp = here->B3SOIgtempd;
                      gTtt  = here->B3SOIgtempT;
                      gTtdp = - (gTtg + gTtb + gTte + gTtsp);
		  }

		   if (model->B3SOItype > 0)
		   {   
		       ceqqg = ceqqg;
		       ceqqb = ceqqb;
		       ceqqe = ceqqe;
		       ceqqd = ceqqd;
		   }
		   else
		   {   
		       ceqbodcon = -ceqbodcon;
		       ceqbody = -ceqbody;
		       ceqbs = -ceqbs;
		       ceqbd = -ceqbd;
		       ceqqg = -ceqqg;
		       ceqqb = -ceqqb;
		       ceqqd = -ceqqd;
		       ceqqe = -ceqqe;
		   }

		   (*(ckt->CKTrhs + here->B3SOIgNode) -= ceqqg);
		   (*(ckt->CKTrhs + here->B3SOIdNodePrime) += (ceqbd - cdreq - ceqqd));
		   (*(ckt->CKTrhs + here->B3SOIsNodePrime) += (cdreq + ceqbs + ceqqg
							  + ceqqb + ceqqd + ceqqe));
		   (*(ckt->CKTrhs + here->B3SOIeNode) -= ceqqe);

                   if (here->B3SOIbodyMod == 1) {
		       (*(ckt->CKTrhs + here->B3SOIpNode) += ceqbodcon);
                   }

		   if (selfheat) {
		       (*(ckt->CKTrhs + here->B3SOItempNode) -= ceqth + ceqqth);
                   }



 if ((here->B3SOIdebugMod > 1) || (here->B3SOIdebugMod == -1))
		   {
	              *(ckt->CKTrhs + here->B3SOIvbsNode) = here->B3SOIvbsdio;
		      *(ckt->CKTrhs + here->B3SOIidsNode) = here->B3SOIids;
		      *(ckt->CKTrhs + here->B3SOIicNode) = here->B3SOIic;
		      *(ckt->CKTrhs + here->B3SOIibsNode) = here->B3SOIibs;
		      *(ckt->CKTrhs + here->B3SOIibdNode) = here->B3SOIibd;
		      *(ckt->CKTrhs + here->B3SOIiiiNode) = here->B3SOIiii; 
		      *(ckt->CKTrhs + here->B3SOIigidlNode) = here->B3SOIigidl;
		      *(ckt->CKTrhs + here->B3SOIitunNode) = here->B3SOIitun;
		      *(ckt->CKTrhs + here->B3SOIibpNode) = here->B3SOIibp;
		      *(ckt->CKTrhs + here->B3SOIabeffNode) = here->B3SOIabeff;
		      *(ckt->CKTrhs + here->B3SOIvbs0effNode) = here->B3SOIvbs0eff;
		      *(ckt->CKTrhs + here->B3SOIvbseffNode) = here->B3SOIvbseff;
		      *(ckt->CKTrhs + here->B3SOIxcNode) = here->B3SOIxc;
		      *(ckt->CKTrhs + here->B3SOIcbbNode) = here->B3SOIcbb;
		      *(ckt->CKTrhs + here->B3SOIcbdNode) = here->B3SOIcbd;
		      *(ckt->CKTrhs + here->B3SOIcbgNode) = here->B3SOIcbg;
		      *(ckt->CKTrhs + here->B3SOIqbfNode) = here->B3SOIqbf;
		      *(ckt->CKTrhs + here->B3SOIqjsNode) = here->B3SOIqjs;
		      *(ckt->CKTrhs + here->B3SOIqjdNode) = here->B3SOIqjd;

                      /* clean up last */
		      *(ckt->CKTrhs + here->B3SOIgmNode) = Gm;
		      *(ckt->CKTrhs + here->B3SOIgmbsNode) = Gmbs;
		      *(ckt->CKTrhs + here->B3SOIgdsNode) = Gds;
		      *(ckt->CKTrhs + here->B3SOIgmeNode) = Gme;
		      *(ckt->CKTrhs + here->B3SOIqdNode) = qdrn;
		      *(ckt->CKTrhs + here->B3SOIcbeNode) = Cbe;
		      *(ckt->CKTrhs + here->B3SOIvbs0teffNode) = Vbs0teff;
		      *(ckt->CKTrhs + here->B3SOIvthNode) = here->B3SOIvon;
		      *(ckt->CKTrhs + here->B3SOIvgsteffNode) = Vgsteff;
		      *(ckt->CKTrhs + here->B3SOIxcsatNode) = Xcsat;
		      *(ckt->CKTrhs + here->B3SOIqaccNode) = -Qac0;
		      *(ckt->CKTrhs + here->B3SOIqsub0Node) = Qsub0;
		      *(ckt->CKTrhs + here->B3SOIqsubs1Node) = Qsubs1;
		      *(ckt->CKTrhs + here->B3SOIqsubs2Node) = Qsubs2;
		      *(ckt->CKTrhs + here->B3SOIvdscvNode) = VdsCV;
		      *(ckt->CKTrhs + here->B3SOIvcscvNode) = VcsCV;
		      *(ckt->CKTrhs + here->B3SOIqgNode) = qgate;
		      *(ckt->CKTrhs + here->B3SOIqbNode) = qbody;
		      *(ckt->CKTrhs + here->B3SOIqeNode) = qsub;
		      *(ckt->CKTrhs + here->B3SOIdum1Node) = here->B3SOIdum1;
		      *(ckt->CKTrhs + here->B3SOIdum2Node) = here->B3SOIdum2;
		      *(ckt->CKTrhs + here->B3SOIdum3Node) = here->B3SOIdum3; 
		      *(ckt->CKTrhs + here->B3SOIdum4Node) = here->B3SOIdum4;
		      *(ckt->CKTrhs + here->B3SOIdum5Node) = here->B3SOIdum5; 
                      /* end clean up last */
		   }


		   /*
		    *  load y matrix
		    */
		      (*(here->B3SOIEgPtr) += gcegb);
   		      (*(here->B3SOIEdpPtr) += gcedb);
   		      (*(here->B3SOIEspPtr) += gcesb);
		      (*(here->B3SOIGePtr) += gcgeb);
		      (*(here->B3SOIDPePtr) += Gme + gddpe + gcdeb);
		      (*(here->B3SOISPePtr) += gsspe - Gme + gcseb);

		   (*(here->B3SOIEePtr) += gceeb);

		   (*(here->B3SOIGgPtr) += gcggb + ckt->CKTgmin);
		   (*(here->B3SOIGdpPtr) += gcgdb - ckt->CKTgmin);
		   (*(here->B3SOIGspPtr) += gcgsb );

		   (*(here->B3SOIDPgPtr) += (Gm + gcdgb) + gddpg - ckt->CKTgmin);
		   (*(here->B3SOIDPdpPtr) += (here->B3SOIdrainConductance
					 + here->B3SOIgds + gddpdp
					 + RevSum + gcddb) + ckt->CKTgmin);
		   (*(here->B3SOIDPspPtr) -= (-gddpsp + here->B3SOIgds + FwdSum - gcdsb));
					 
		   (*(here->B3SOIDPdPtr) -= here->B3SOIdrainConductance);

		   (*(here->B3SOISPgPtr) += gcsgb - Gm + gsspg );
		   (*(here->B3SOISPdpPtr) -= (here->B3SOIgds - gsspdp + RevSum - gcsdb));
		   (*(here->B3SOISPspPtr) += (here->B3SOIsourceConductance
					 + here->B3SOIgds + gsspsp
					 + FwdSum + gcssb));
		   (*(here->B3SOISPsPtr) -= here->B3SOIsourceConductance);


		   (*(here->B3SOIDdPtr) += here->B3SOIdrainConductance);
		   (*(here->B3SOIDdpPtr) -= here->B3SOIdrainConductance);


		   (*(here->B3SOISsPtr) += here->B3SOIsourceConductance);
		   (*(here->B3SOISspPtr) -= here->B3SOIsourceConductance);

		   if (here->B3SOIbodyMod == 1) {
		      (*(here->B3SOIBpPtr) -= gppp);
		      (*(here->B3SOIPbPtr) += gppb);
		      (*(here->B3SOIPpPtr) += gppp);
			(*(here->B3SOIPgPtr) += gppg);
			(*(here->B3SOIPdpPtr) += gppdp);
			(*(here->B3SOIPspPtr) += gppsp);
			(*(here->B3SOIPePtr) += gppe);
		   }

		   if (selfheat) 
                   {
		      (*(here->B3SOIDPtempPtr) += GmT + gddpT + gcdT);
		      (*(here->B3SOISPtempPtr) += -GmT + gsspT + gcsT);
		      (*(here->B3SOIBtempPtr) += gbbT + gcbT);
                      (*(here->B3SOIEtempPtr) += gceT);
                      (*(here->B3SOIGtempPtr) += gcgT);
		      if (here->B3SOIbodyMod == 1) {
			  (*(here->B3SOIPtempPtr) += gppT);
		      }
		      (*(here->B3SOITemptempPtr) += gTtt  + 1/pParam->B3SOIrth + gcTt);
                      (*(here->B3SOITempgPtr) += gTtg);
                      (*(here->B3SOITempbPtr) += gTtb);
                      (*(here->B3SOITempePtr) += gTte);
                      (*(here->B3SOITempdpPtr) += gTtdp);
                      (*(here->B3SOITempspPtr) += gTtsp);
		   }

		   if ((here->B3SOIdebugMod > 1) || (here->B3SOIdebugMod == -1))
		   {
		      *(here->B3SOIVbsPtr) += 1;  
		      *(here->B3SOIIdsPtr) += 1;
		      *(here->B3SOIIcPtr) += 1;
		      *(here->B3SOIIbsPtr) += 1;
		      *(here->B3SOIIbdPtr) += 1;
		      *(here->B3SOIIiiPtr) += 1;
		      *(here->B3SOIIgidlPtr) += 1;
		      *(here->B3SOIItunPtr) += 1;
		      *(here->B3SOIIbpPtr) += 1;
		      *(here->B3SOIAbeffPtr) += 1;
		      *(here->B3SOIVbs0effPtr) += 1;
		      *(here->B3SOIVbseffPtr) += 1;
		      *(here->B3SOIXcPtr) += 1;
		      *(here->B3SOICbgPtr) += 1;
		      *(here->B3SOICbbPtr) += 1;
		      *(here->B3SOICbdPtr) += 1;
		      *(here->B3SOIqbPtr) += 1;
		      *(here->B3SOIQbfPtr) += 1;
		      *(here->B3SOIQjsPtr) += 1;
		      *(here->B3SOIQjdPtr) += 1;

                      /* clean up last */
		      *(here->B3SOIGmPtr) += 1;
		      *(here->B3SOIGmbsPtr) += 1;
		      *(here->B3SOIGdsPtr) += 1;
		      *(here->B3SOIGmePtr) += 1;
		      *(here->B3SOIVbs0teffPtr) += 1;
		      *(here->B3SOIVgsteffPtr) += 1;
		      *(here->B3SOICbePtr) += 1;
		      *(here->B3SOIVthPtr) += 1;
		      *(here->B3SOIXcsatPtr) += 1;
		      *(here->B3SOIVdscvPtr) += 1;
		      *(here->B3SOIVcscvPtr) += 1;
		      *(here->B3SOIQaccPtr) += 1;
		      *(here->B3SOIQsub0Ptr) += 1;
		      *(here->B3SOIQsubs1Ptr) += 1;
		      *(here->B3SOIQsubs2Ptr) += 1;
		      *(here->B3SOIqgPtr) += 1;
		      *(here->B3SOIqdPtr) += 1;
		      *(here->B3SOIqePtr) += 1;
		      *(here->B3SOIDum1Ptr) += 1;
		      *(here->B3SOIDum2Ptr) += 1;
		      *(here->B3SOIDum3Ptr) += 1;
		      *(here->B3SOIDum4Ptr) += 1;
		      *(here->B3SOIDum5Ptr) += 1; 
                      /* end clean up last */
		   }

	line1000:  ;

/*  Here NaN will be detected in any conductance or equivalent current.  Note
    that nandetect is initialized within the "if" statements  */

                   if (nandetect = isnan (*(here->B3SOIGgPtr)))
                   { strcpy (nanmessage, "GgPtr"); }
                   else if (nandetect = isnan (*(here->B3SOIGdpPtr)))
                   { strcpy (nanmessage, "GdpPtr"); }
                   else if (nandetect = isnan (*(here->B3SOIGspPtr)))
                   { strcpy (nanmessage, "GspPtr"); }
                   else if (nandetect = isnan (*(here->B3SOIDPgPtr)))
                   { strcpy (nanmessage, "DPgPtr"); }
                   else if (nandetect = isnan (*(here->B3SOIDPdpPtr)))
                   { strcpy (nanmessage, "DPdpPtr"); }
                   else if (nandetect = isnan (*(here->B3SOIDPspPtr)))
                   { strcpy (nanmessage, "DPspPtr"); }
                   else if (nandetect = isnan (*(here->B3SOISPgPtr)))
                   { strcpy (nanmessage, "SPgPtr"); }
                   else if (nandetect = isnan (*(here->B3SOISPdpPtr)))
                   { strcpy (nanmessage, "SPdpPtr"); }
                   else if (nandetect = isnan (*(here->B3SOISPspPtr)))
                   { strcpy (nanmessage, "SPspPtr"); }
                   else if (nandetect = isnan (*(here->B3SOIEePtr)))
                   { strcpy (nanmessage, "EePtr"); }
 
                   /*  At this point, nandetect = 0 if none of the
                       conductances checked so far are NaN  */
 
                   if (nandetect == 0)
                   {
                      if (nandetect = isnan (*(here->B3SOIEgPtr)))
                      { strcpy (nanmessage, "EgPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIEdpPtr)))
                      { strcpy (nanmessage, "EdpPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIEspPtr)))
                      { strcpy (nanmessage, "EspPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIGePtr)))
                      { strcpy (nanmessage, "GePtr"); }
                      else if (nandetect = isnan (*(here->B3SOIDPePtr)))
                      { strcpy (nanmessage, "DPePtr"); }
                      else if (nandetect = isnan (*(here->B3SOISPePtr)))
                      { strcpy (nanmessage, "SPePtr"); } }
 
                   /*  Now check if self-heating caused NaN if nothing else
                       has so far (check tempnode current also)  */
 
                   if (selfheat && nandetect == 0)
                   {
                      if (nandetect = isnan (*(here->B3SOITemptempPtr)))
                      { strcpy (nanmessage, "TemptempPtr"); }
                      else if (nandetect = isnan (*(here->B3SOITempgPtr)))
                      { strcpy (nanmessage, "TempgPtr"); }
                      else if (nandetect = isnan (*(here->B3SOITempbPtr)))
                      { strcpy (nanmessage, "TempbPtr"); }
                      else if (nandetect = isnan (*(here->B3SOITempePtr)))
                      { strcpy (nanmessage, "TempePtr"); }
                      else if (nandetect = isnan (*(here->B3SOITempdpPtr)))
                      { strcpy (nanmessage, "TempdpPtr"); }
                      else if (nandetect = isnan (*(here->B3SOITempspPtr)))
                      { strcpy (nanmessage, "TempspPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIGtempPtr)))
                      { strcpy (nanmessage, "GtempPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIDPtempPtr)))
                      { strcpy (nanmessage, "DPtempPtr"); }
                      else if (nandetect = isnan (*(here->B3SOISPtempPtr)))
                      { strcpy (nanmessage, "SPtempPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIEtempPtr)))
                      { strcpy (nanmessage, "EtempPtr"); }
                      else if (nandetect = isnan (*(here->B3SOIBtempPtr)))
                      { strcpy (nanmessage, "BtempPtr"); }
                      else if (nandetect = isnan (*(ckt->CKTrhs + here->B3SOItempNode)))
                      { strcpy (nanmessage, "tempNode"); }
                   }
 
                   /*  Lastly, check all equivalent currents (tempnode is
                       checked above  */
 
                   if (nandetect == 0)
                   {
                      if (nandetect = isnan (*(ckt->CKTrhs
                                             + here->B3SOIgNode)))
                      { strcpy (nanmessage, "gNode"); }
                      else if (nandetect = isnan (*(ckt->CKTrhs
                                                  + here->B3SOIbNode)))
                      { strcpy (nanmessage, "bNode"); }
                      else if (nandetect = isnan (*(ckt->CKTrhs
                                                  + here->B3SOIdNodePrime)))
                      { strcpy (nanmessage, "dpNode"); }
                      else if (nandetect = isnan (*(ckt->CKTrhs
                                                  + here->B3SOIsNodePrime)))
                      { strcpy (nanmessage, "spNode"); }
                      else if (nandetect = isnan (*(ckt->CKTrhs
                                                  + here->B3SOIeNode)))
                      { strcpy (nanmessage, "eNode"); } 
                   }

                   /*  Now print error message if NaN detected.  Note that
                       error will only be printed once (the first time it is
                       encountered) each time SPICE is run since nanfound is
                       static variable  */

                   if (nanfound == 0 && nandetect)
                   {
                      fprintf(stderr, "Alberto says:  YOU TURKEY!  %s is NaN for instance %s at time %g!\n", nanmessage, here->B3SOIname, ckt->CKTtime);
                      nanfound = nandetect;
		      fprintf(stderr, " The program exit!\n");
		      exit(-1);
                   }

		   if (here->B3SOIdebugMod > 2)
		   {
      fprintf(fpdebug, "Ids = %.4e, Ic = %.4e, cqdrn = %.4e, gmin=%.3e\n", 
                        Ids, Ic, cqdrn, ckt->CKTgmin);
		      fprintf(fpdebug, "Iii = %.4e, Idgidl = %.4e, Ibs = %.14e\n",
			      Iii, Idgidl, Ibs);
		      fprintf(fpdebug, "Ibd = %.4e, Ibp = %.4e\n", Ibd, Ibp);
		      fprintf(fpdebug, "qbody = %.5e, qbf = %.5e, qbe = %.5e\n",
			      qbody, Qbf, -(Qe1+Qe2));
		      fprintf(fpdebug, "qbs = %.5e, qbd = %.5e\n", qjs, qjd);
		      fprintf(fpdebug, "qdrn = %.5e, qinv = %.5e\n", qdrn, qinv);




/*  I am trying to debug the convergence problems here by printing out
    the entire Jacobian and equivalent current matrix  */

  if (here->B3SOIdebugMod > 4) {
  fprintf(fpdebug, "Ibtot = %.6e;\t Cbtot = %.6e;\n", Ibs+Ibp+Ibd-Iii-Idgidl-Isgidl, cqbody);
  fprintf(fpdebug, "ceqg = %.6e;\t ceqb = %.6e;\t ceqdp = %.6e;\t ceqsp = %.6e;\n",
                    *(ckt->CKTrhs + here->B3SOIgNode),
                    *(ckt->CKTrhs + here->B3SOIbNode),
                    *(ckt->CKTrhs + here->B3SOIdNodePrime),
                    *(ckt->CKTrhs + here->B3SOIsNodePrime));
  fprintf(fpdebug, "ceqe = %.6e;\t ceqp = %.6e;\t ceqth = %.6e;\n",
                    *(ckt->CKTrhs + here->B3SOIeNode),
                    *(ckt->CKTrhs + here->B3SOIpNode), 
                    *(ckt->CKTrhs + here->B3SOItempNode));

  fprintf(fpdebug, "Eg = %.5e;\t Edp = %.5e;\t Esp = %.5e;\t Eb = %.5e;\n",
		   *(here->B3SOIEgPtr),
		   *(here->B3SOIEdpPtr), 
		   *(here->B3SOIEspPtr),
		   *(here->B3SOIEbPtr));
  fprintf(fpdebug, "Ee = %.5e;\t Gg = %.5e;\t Gdp = %.5e;\t Gsp = %.5e;\n",
		   *(here->B3SOIEePtr),
		   *(here->B3SOIGgPtr),
		   *(here->B3SOIGdpPtr),
		   *(here->B3SOIGspPtr));
  fprintf(fpdebug, "Gb = %.5e;\t Ge = %.5e;\t DPg = %.5e;\t DPdp = %.5e;\n",
		   *(here->B3SOIGbPtr),
		   *(here->B3SOIGePtr),
		   *(here->B3SOIDPgPtr),
		   *(here->B3SOIDPdpPtr));
  fprintf(fpdebug, "DPsp = %.5e;\t DPb = %.5e;\t DPe = %.5e;\t\n",
		   *(here->B3SOIDPspPtr),
		   *(here->B3SOIDPbPtr),
		   *(here->B3SOIDPePtr));
  fprintf(fpdebug, "DPd = %.5e;\t SPg = %.5e;\t SPdp = %.5e;\t SPsp = %.5e;\n",
		   *(here->B3SOIDPdPtr),
		   *(here->B3SOISPgPtr),
		   *(here->B3SOISPdpPtr),
		   *(here->B3SOISPspPtr));
  fprintf(fpdebug, "SPb = %.5e;\t SPe = %.5e;\t SPs = %.5e;\n",
		   *(here->B3SOISPbPtr),
		   *(here->B3SOISPePtr),
		   *(here->B3SOISPsPtr));
  fprintf(fpdebug, "Dd = %.5e;\t Ddp = %.5e;\t Ss = %.5e;\t Ssp = %.5e;\n",
		   *(here->B3SOIDdPtr),
		   *(here->B3SOIDdpPtr),
		   *(here->B3SOISsPtr),
		   *(here->B3SOISspPtr));
  fprintf(fpdebug, "Bg = %.5e;\t Bdp = %.5e;\t Bsp = %.5e;\t Bb = %.5e;\n",
		   *(here->B3SOIBgPtr),
		   *(here->B3SOIBdpPtr),
		   *(here->B3SOIBspPtr),
		   *(here->B3SOIBbPtr));
  fprintf(fpdebug, "Be = %.5e;\t Btot = %.5e;\t DPtot = %.5e;\n",
                   *(here->B3SOIBePtr), 
                   *(here->B3SOIBgPtr) + *(here->B3SOIBdpPtr)
                   + *(here->B3SOIBspPtr) + *(here->B3SOIBbPtr)
                   + *(here->B3SOIBePtr),
		   *(here->B3SOIDPePtr)
		   + *(here->B3SOIDPgPtr) + *(here->B3SOIDPdpPtr)
		   + *(here->B3SOIDPspPtr) + *(here->B3SOIDPbPtr));

  if (selfheat) {
    fprintf (fpdebug, "DPtemp = %.5e;\t SPtemp = %.5e;\t Btemp = %.5e;\n",
                      *(here->B3SOIDPtempPtr), *(here->B3SOISPtempPtr),
                      *(here->B3SOIBtempPtr));
    fprintf (fpdebug, "Gtemp = %.5e;\t Etemp = %.5e;\n",
                      *(here->B3SOIGtempPtr), *(here->B3SOIEtempPtr));
    fprintf (fpdebug, "Tempg = %.5e;\t Tempdp = %.5e;\t Tempsp = %.5e;\t Tempb = %.5e;\n",
		      *(here->B3SOITempgPtr), *(here->B3SOITempdpPtr),
		      *(here->B3SOITempspPtr), *(here->B3SOITempbPtr));
    fprintf (fpdebug, "Tempe = %.5e;\t TempT = %.5e;\t Temptot = %.5e;\n",
		      *(here->B3SOITempePtr), *(here->B3SOITemptempPtr),
		      *(here->B3SOITempgPtr) + *(here->B3SOITempdpPtr)
		      + *(here->B3SOITempspPtr)+ *(here->B3SOITempbPtr)
		      + *(here->B3SOITempePtr));
  }

   if (here->B3SOIbodyMod == 1)
   {
      fprintf(fpdebug, "ceqbodcon=%.5e;\t", ceqbodcon);
      fprintf(fpdebug, "Bp = %.5e;\t Pb = %.5e;\t Pp = %.5e;\n", -gppp, gppb, gppp);
      fprintf(fpdebug, "Pg=%.5e;\t Pdp=%.5e;\t Psp=%.5e;\t Pe=%.5e;\n",
                    gppg, gppdp, gppsp, gppe);
   }
}

	if (here->B3SOIdebugMod > 3)
        {
           fprintf(fpdebug, "Vth = %.4f, Vbs0eff = %.8f, Vdsat = %.4f\n",
                   Vth, Vbs0eff, Vdsat);
           fprintf(fpdebug, "ueff = %g, Vgsteff = %.4f, Vdseff = %.4f\n",
                   ueff, Vgsteff, Vdseff);
           fprintf(fpdebug, "Vthfd = %.4f, Vbs0mos = %.4f, Vbs0 = %.4f\n",
                   Vthfd, Vbs0mos, Vbs0);
           fprintf(fpdebug, "Vbs0t = %.4f, Vbsdio = %.8f\n",
                   Vbs0t, Vbsdio);
        }

              fclose(fpdebug);
           }

     here->B3SOIiterations++;  /*  increment the iteration counter  */

     }  /* End of Mosfet Instance */
}   /* End of Model Instance */


return(OK);
}

