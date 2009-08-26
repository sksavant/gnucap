/* $Id: BSIMDD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soimask.c, DD2.1 $
 * Revision 2.1  99/9/27 Pin Su 
 * BSIMDD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soimask.c, DD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: Weidong Liu and Pin Su         Feb 1999
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
Modified by Wei Jin 99/9/27
File: b3soimask.c          98/5/01
**********/


#include "spice.h"
#include <stdio.h>
#include "ifsim.h"
#include "cktdefs.h"
#include "devdefs.h"
#include "b3soidef.h"
#include "sperror.h"
#include "suffix.h"

int
B3SOImAsk(ckt,inst,which,value)
CKTcircuit *ckt;
GENmodel *inst;
int which;
IFvalue *value;
{
    B3SOImodel *model = (B3SOImodel *)inst;
    switch(which) 
    {   case B3SOI_MOD_MOBMOD:
            value->iValue = model->B3SOImobMod; 
            return(OK);
        case B3SOI_MOD_PARAMCHK:
            value->iValue = model->B3SOIparamChk; 
            return(OK);
        case B3SOI_MOD_BINUNIT:
            value->iValue = model->B3SOIbinUnit; 
            return(OK);
        case B3SOI_MOD_CAPMOD:
            value->iValue = model->B3SOIcapMod; 
            return(OK);
        case B3SOI_MOD_SHMOD:
            value->iValue = model->B3SOIshMod; 
            return(OK);
        case B3SOI_MOD_NOIMOD:
            value->iValue = model->B3SOInoiMod; 
            return(OK);
        case  B3SOI_MOD_VERSION :
          value->rValue = model->B3SOIversion;
            return(OK);
        case  B3SOI_MOD_TOX :
          value->rValue = model->B3SOItox;
            return(OK);
        case  B3SOI_MOD_CDSC :
          value->rValue = model->B3SOIcdsc;
            return(OK);
        case  B3SOI_MOD_CDSCB :
          value->rValue = model->B3SOIcdscb;
            return(OK);

        case  B3SOI_MOD_CDSCD :
          value->rValue = model->B3SOIcdscd;
            return(OK);

        case  B3SOI_MOD_CIT :
          value->rValue = model->B3SOIcit;
            return(OK);
        case  B3SOI_MOD_NFACTOR :
          value->rValue = model->B3SOInfactor;
            return(OK);
        case B3SOI_MOD_VSAT:
            value->rValue = model->B3SOIvsat;
            return(OK);
        case B3SOI_MOD_AT:
            value->rValue = model->B3SOIat;
            return(OK);
        case B3SOI_MOD_A0:
            value->rValue = model->B3SOIa0;
            return(OK);

        case B3SOI_MOD_AGS:
            value->rValue = model->B3SOIags;
            return(OK);

        case B3SOI_MOD_A1:
            value->rValue = model->B3SOIa1;
            return(OK);
        case B3SOI_MOD_A2:
            value->rValue = model->B3SOIa2;
            return(OK);
        case B3SOI_MOD_KETA:
            value->rValue = model->B3SOIketa;
            return(OK);   
        case B3SOI_MOD_NSUB:
            value->rValue = model->B3SOInsub;
            return(OK);
        case B3SOI_MOD_NPEAK:
            value->rValue = model->B3SOInpeak;
            return(OK);
        case B3SOI_MOD_NGATE:
            value->rValue = model->B3SOIngate;
            return(OK);
        case B3SOI_MOD_GAMMA1:
            value->rValue = model->B3SOIgamma1;
            return(OK);
        case B3SOI_MOD_GAMMA2:
            value->rValue = model->B3SOIgamma2;
            return(OK);
        case B3SOI_MOD_VBX:
            value->rValue = model->B3SOIvbx;
            return(OK);
        case B3SOI_MOD_VBM:
            value->rValue = model->B3SOIvbm;
            return(OK);
        case B3SOI_MOD_XT:
            value->rValue = model->B3SOIxt;
            return(OK);
        case  B3SOI_MOD_K1:
          value->rValue = model->B3SOIk1;
            return(OK);
        case  B3SOI_MOD_KT1:
          value->rValue = model->B3SOIkt1;
            return(OK);
        case  B3SOI_MOD_KT1L:
          value->rValue = model->B3SOIkt1l;
            return(OK);
        case  B3SOI_MOD_KT2 :
          value->rValue = model->B3SOIkt2;
            return(OK);
        case  B3SOI_MOD_K2 :
          value->rValue = model->B3SOIk2;
            return(OK);
        case  B3SOI_MOD_K3:
          value->rValue = model->B3SOIk3;
            return(OK);
        case  B3SOI_MOD_K3B:
          value->rValue = model->B3SOIk3b;
            return(OK);
        case  B3SOI_MOD_W0:
          value->rValue = model->B3SOIw0;
            return(OK);
        case  B3SOI_MOD_NLX:
          value->rValue = model->B3SOInlx;
            return(OK);
        case  B3SOI_MOD_DVT0 :                
          value->rValue = model->B3SOIdvt0;
            return(OK);
        case  B3SOI_MOD_DVT1 :             
          value->rValue = model->B3SOIdvt1;
            return(OK);
        case  B3SOI_MOD_DVT2 :             
          value->rValue = model->B3SOIdvt2;
            return(OK);
        case  B3SOI_MOD_DVT0W :                
          value->rValue = model->B3SOIdvt0w;
            return(OK);
        case  B3SOI_MOD_DVT1W :             
          value->rValue = model->B3SOIdvt1w;
            return(OK);
        case  B3SOI_MOD_DVT2W :             
          value->rValue = model->B3SOIdvt2w;
            return(OK);
        case  B3SOI_MOD_DROUT :           
          value->rValue = model->B3SOIdrout;
            return(OK);
        case  B3SOI_MOD_DSUB :           
          value->rValue = model->B3SOIdsub;
            return(OK);
        case B3SOI_MOD_VTH0:
            value->rValue = model->B3SOIvth0; 
            return(OK);
        case B3SOI_MOD_UA:
            value->rValue = model->B3SOIua; 
            return(OK);
        case B3SOI_MOD_UA1:
            value->rValue = model->B3SOIua1; 
            return(OK);
        case B3SOI_MOD_UB:
            value->rValue = model->B3SOIub;  
            return(OK);
        case B3SOI_MOD_UB1:
            value->rValue = model->B3SOIub1;  
            return(OK);
        case B3SOI_MOD_UC:
            value->rValue = model->B3SOIuc; 
            return(OK);
        case B3SOI_MOD_UC1:
            value->rValue = model->B3SOIuc1; 
            return(OK);
        case B3SOI_MOD_U0:
            value->rValue = model->B3SOIu0;
            return(OK);
        case B3SOI_MOD_UTE:
            value->rValue = model->B3SOIute;
            return(OK);
        case B3SOI_MOD_VOFF:
            value->rValue = model->B3SOIvoff;
            return(OK);
        case B3SOI_MOD_DELTA:
            value->rValue = model->B3SOIdelta;
            return(OK);
        case B3SOI_MOD_RDSW:
            value->rValue = model->B3SOIrdsw; 
            return(OK);             
        case B3SOI_MOD_PRWG:
            value->rValue = model->B3SOIprwg; 
            return(OK);             
        case B3SOI_MOD_PRWB:
            value->rValue = model->B3SOIprwb; 
            return(OK);             
        case B3SOI_MOD_PRT:
            value->rValue = model->B3SOIprt; 
            return(OK);              
        case B3SOI_MOD_ETA0:
            value->rValue = model->B3SOIeta0; 
            return(OK);               
        case B3SOI_MOD_ETAB:
            value->rValue = model->B3SOIetab; 
            return(OK);               
        case B3SOI_MOD_PCLM:
            value->rValue = model->B3SOIpclm; 
            return(OK);               
        case B3SOI_MOD_PDIBL1:
            value->rValue = model->B3SOIpdibl1; 
            return(OK);               
        case B3SOI_MOD_PDIBL2:
            value->rValue = model->B3SOIpdibl2; 
            return(OK);               
        case B3SOI_MOD_PDIBLB:
            value->rValue = model->B3SOIpdiblb; 
            return(OK);               
        case B3SOI_MOD_PVAG:
            value->rValue = model->B3SOIpvag; 
            return(OK);               
        case B3SOI_MOD_WR:
            value->rValue = model->B3SOIwr;
            return(OK);
        case B3SOI_MOD_DWG:
            value->rValue = model->B3SOIdwg;
            return(OK);
        case B3SOI_MOD_DWB:
            value->rValue = model->B3SOIdwb;
            return(OK);
        case B3SOI_MOD_B0:
            value->rValue = model->B3SOIb0;
            return(OK);
        case B3SOI_MOD_B1:
            value->rValue = model->B3SOIb1;
            return(OK);
        case B3SOI_MOD_ALPHA0:
            value->rValue = model->B3SOIalpha0;
            return(OK);
        case B3SOI_MOD_ALPHA1:
            value->rValue = model->B3SOIalpha1;
            return(OK);
        case B3SOI_MOD_BETA0:
            value->rValue = model->B3SOIbeta0;
            return(OK);

        case B3SOI_MOD_CGSL:
            value->rValue = model->B3SOIcgsl;
            return(OK);
        case B3SOI_MOD_CGDL:
            value->rValue = model->B3SOIcgdl;
            return(OK);
        case B3SOI_MOD_CKAPPA:
            value->rValue = model->B3SOIckappa;
            return(OK);
        case B3SOI_MOD_CF:
            value->rValue = model->B3SOIcf;
            return(OK);
        case B3SOI_MOD_CLC:
            value->rValue = model->B3SOIclc;
            return(OK);
        case B3SOI_MOD_CLE:
            value->rValue = model->B3SOIcle;
            return(OK);
        case B3SOI_MOD_DWC:
            value->rValue = model->B3SOIdwc;
            return(OK);
        case B3SOI_MOD_DLC:
            value->rValue = model->B3SOIdlc;
            return(OK);

        case B3SOI_MOD_TBOX:
            value->rValue = model->B3SOItbox; 
            return(OK);
        case B3SOI_MOD_TSI:
            value->rValue = model->B3SOItsi; 
            return(OK);
        case B3SOI_MOD_KB1:
            value->rValue = model->B3SOIkb1; 
            return(OK);
        case B3SOI_MOD_KB3:
            value->rValue = model->B3SOIkb3; 
            return(OK);
        case B3SOI_MOD_DVBD0:
            value->rValue = model->B3SOIdvbd0; 
            return(OK);
        case B3SOI_MOD_DVBD1:
            value->rValue = model->B3SOIdvbd1; 
            return(OK);
        case B3SOI_MOD_DELP:
            value->rValue = model->B3SOIdelp; 
            return(OK);
        case B3SOI_MOD_VBSA:
            value->rValue = model->B3SOIvbsa; 
            return(OK);
        case B3SOI_MOD_RBODY:
            value->rValue = model->B3SOIrbody; 
            return(OK);
        case B3SOI_MOD_RBSH:
            value->rValue = model->B3SOIrbsh; 
            return(OK);
        case B3SOI_MOD_ADICE0:
            value->rValue = model->B3SOIadice0; 
            return(OK);
        case B3SOI_MOD_ABP:
            value->rValue = model->B3SOIabp; 
            return(OK);
        case B3SOI_MOD_MXC:
            value->rValue = model->B3SOImxc; 
            return(OK);
        case B3SOI_MOD_RTH0:
            value->rValue = model->B3SOIrth0; 
            return(OK);
        case B3SOI_MOD_CTH0:
            value->rValue = model->B3SOIcth0; 
            return(OK);
        case B3SOI_MOD_AII:
            value->rValue = model->B3SOIaii; 
            return(OK);
        case B3SOI_MOD_BII:
            value->rValue = model->B3SOIbii; 
            return(OK);
        case B3SOI_MOD_CII:
            value->rValue = model->B3SOIcii; 
            return(OK);
        case B3SOI_MOD_DII:
            value->rValue = model->B3SOIdii; 
            return(OK);
        case B3SOI_MOD_NDIODE:
            value->rValue = model->B3SOIndiode; 
            return(OK);
        case B3SOI_MOD_NTUN:
            value->rValue = model->B3SOIntun; 
            return(OK);
        case B3SOI_MOD_ISBJT:
            value->rValue = model->B3SOIisbjt; 
            return(OK);
        case B3SOI_MOD_ISDIF:
            value->rValue = model->B3SOIisdif; 
            return(OK);
        case B3SOI_MOD_ISREC:
            value->rValue = model->B3SOIisrec; 
            return(OK);
        case B3SOI_MOD_ISTUN:
            value->rValue = model->B3SOIistun; 
            return(OK);
        case B3SOI_MOD_XBJT:
            value->rValue = model->B3SOIxbjt; 
            return(OK);
        case B3SOI_MOD_XREC:
            value->rValue = model->B3SOIxrec; 
            return(OK);
        case B3SOI_MOD_XTUN:
            value->rValue = model->B3SOIxtun; 
            return(OK);
        case B3SOI_MOD_EDL:
            value->rValue = model->B3SOIedl; 
            return(OK);
        case B3SOI_MOD_KBJT1:
            value->rValue = model->B3SOIkbjt1; 
            return(OK);
        case B3SOI_MOD_TT:
            value->rValue = model->B3SOItt; 
            return(OK);
        case B3SOI_MOD_VSDTH:
            value->rValue = model->B3SOIvsdth; 
            return(OK);
        case B3SOI_MOD_VSDFB:
            value->rValue = model->B3SOIvsdfb; 
            return(OK);
        case B3SOI_MOD_CSDMIN:
            value->rValue = model->B3SOIcsdmin; 
            return(OK);
        case B3SOI_MOD_ASD:
            value->rValue = model->B3SOIasd; 
            return(OK);

        case  B3SOI_MOD_TNOM :
          value->rValue = model->B3SOItnom;
            return(OK);
        case B3SOI_MOD_CGSO:
            value->rValue = model->B3SOIcgso; 
            return(OK);
        case B3SOI_MOD_CGDO:
            value->rValue = model->B3SOIcgdo; 
            return(OK);
        case B3SOI_MOD_CGEO:
            value->rValue = model->B3SOIcgeo; 
            return(OK);
        case B3SOI_MOD_XPART:
            value->rValue = model->B3SOIxpart; 
            return(OK);
        case B3SOI_MOD_RSH:
            value->rValue = model->B3SOIsheetResistance; 
            return(OK);
        case B3SOI_MOD_PBSWG:
            value->rValue = model->B3SOIGatesidewallJctPotential; 
            return(OK);
        case B3SOI_MOD_MJSWG:
            value->rValue = model->B3SOIbodyJctGateSideGradingCoeff; 
            return(OK);
        case B3SOI_MOD_CJSWG:
            value->rValue = model->B3SOIunitLengthGateSidewallJctCap; 
            return(OK);
        case B3SOI_MOD_CSDESW:
            value->rValue = model->B3SOIcsdesw; 
            return(OK);
        case B3SOI_MOD_LINT:
            value->rValue = model->B3SOILint; 
            return(OK);
        case B3SOI_MOD_LL:
            value->rValue = model->B3SOILl;
            return(OK);
        case B3SOI_MOD_LLN:
            value->rValue = model->B3SOILln;
            return(OK);
        case B3SOI_MOD_LW:
            value->rValue = model->B3SOILw;
            return(OK);
        case B3SOI_MOD_LWN:
            value->rValue = model->B3SOILwn;
            return(OK);
        case B3SOI_MOD_LWL:
            value->rValue = model->B3SOILwl;
            return(OK);
        case B3SOI_MOD_WINT:
            value->rValue = model->B3SOIWint;
            return(OK);
        case B3SOI_MOD_WL:
            value->rValue = model->B3SOIWl;
            return(OK);
        case B3SOI_MOD_WLN:
            value->rValue = model->B3SOIWln;
            return(OK);
        case B3SOI_MOD_WW:
            value->rValue = model->B3SOIWw;
            return(OK);
        case B3SOI_MOD_WWN:
            value->rValue = model->B3SOIWwn;
            return(OK);
        case B3SOI_MOD_WWL:
            value->rValue = model->B3SOIWwl;
            return(OK);
        case B3SOI_MOD_NOIA:
            value->rValue = model->B3SOIoxideTrapDensityA;
            return(OK);
        case B3SOI_MOD_NOIB:
            value->rValue = model->B3SOIoxideTrapDensityB;
            return(OK);
        case B3SOI_MOD_NOIC:
            value->rValue = model->B3SOIoxideTrapDensityC;
            return(OK);
        case B3SOI_MOD_NOIF:
            value->rValue = model->B3SOInoif;
            return(OK);
        case B3SOI_MOD_EM:
            value->rValue = model->B3SOIem;
            return(OK);
        case B3SOI_MOD_EF:
            value->rValue = model->B3SOIef;
            return(OK);
        case B3SOI_MOD_AF:
            value->rValue = model->B3SOIaf;
            return(OK);
        case B3SOI_MOD_KF:
            value->rValue = model->B3SOIkf;
            return(OK);

/* Added for binning - START */
        /* Length Dependence */
        case B3SOI_MOD_LNPEAK:
            value->rValue = model->B3SOIlnpeak;
            return(OK);
        case B3SOI_MOD_LNSUB:
            value->rValue = model->B3SOIlnsub;
            return(OK);
        case B3SOI_MOD_LNGATE:
            value->rValue = model->B3SOIlngate;
            return(OK);
        case B3SOI_MOD_LVTH0:
            value->rValue = model->B3SOIlvth0;
            return(OK);
        case  B3SOI_MOD_LK1:
          value->rValue = model->B3SOIlk1;
            return(OK);
        case  B3SOI_MOD_LK2:
          value->rValue = model->B3SOIlk2;
            return(OK);
        case  B3SOI_MOD_LK3:
          value->rValue = model->B3SOIlk3;
            return(OK);
        case  B3SOI_MOD_LK3B:
          value->rValue = model->B3SOIlk3b;
            return(OK);
        case  B3SOI_MOD_LVBSA:
          value->rValue = model->B3SOIlvbsa;
            return(OK);
        case  B3SOI_MOD_LDELP:
          value->rValue = model->B3SOIldelp;
            return(OK);
        case  B3SOI_MOD_LKB1:
            value->rValue = model->B3SOIlkb1;
            return(OK);
        case  B3SOI_MOD_LKB3:
            value->rValue = model->B3SOIlkb3;
            return(OK);
        case  B3SOI_MOD_LDVBD0:
            value->rValue = model->B3SOIdvbd0;
            return(OK);
        case  B3SOI_MOD_LDVBD1:
            value->rValue = model->B3SOIdvbd1;
            return(OK);
        case  B3SOI_MOD_LW0:
          value->rValue = model->B3SOIlw0;
            return(OK);
        case  B3SOI_MOD_LNLX:
          value->rValue = model->B3SOIlnlx;
            return(OK);
        case  B3SOI_MOD_LDVT0 :
          value->rValue = model->B3SOIldvt0;
            return(OK);
        case  B3SOI_MOD_LDVT1 :
          value->rValue = model->B3SOIldvt1;
            return(OK);
        case  B3SOI_MOD_LDVT2 :
          value->rValue = model->B3SOIldvt2;
            return(OK);
        case  B3SOI_MOD_LDVT0W :
          value->rValue = model->B3SOIldvt0w;
            return(OK);
        case  B3SOI_MOD_LDVT1W :
          value->rValue = model->B3SOIldvt1w;
            return(OK);
        case  B3SOI_MOD_LDVT2W :
          value->rValue = model->B3SOIldvt2w;
            return(OK);
        case B3SOI_MOD_LU0:
            value->rValue = model->B3SOIlu0;
            return(OK);
        case B3SOI_MOD_LUA:
            value->rValue = model->B3SOIlua;
            return(OK);
        case B3SOI_MOD_LUB:
            value->rValue = model->B3SOIlub;
            return(OK);
        case B3SOI_MOD_LUC:
            value->rValue = model->B3SOIluc;
            return(OK);
        case B3SOI_MOD_LVSAT:
            value->rValue = model->B3SOIlvsat;
            return(OK);
        case B3SOI_MOD_LA0:
            value->rValue = model->B3SOIla0;
            return(OK);
        case B3SOI_MOD_LAGS:
            value->rValue = model->B3SOIlags;
            return(OK);
        case B3SOI_MOD_LB0:
            value->rValue = model->B3SOIlb0;
            return(OK);
        case B3SOI_MOD_LB1:
            value->rValue = model->B3SOIlb1;
            return(OK);
        case B3SOI_MOD_LKETA:
            value->rValue = model->B3SOIlketa;
            return(OK);
        case B3SOI_MOD_LABP:
            value->rValue = model->B3SOIlabp;
            return(OK);
        case B3SOI_MOD_LMXC:
            value->rValue = model->B3SOIlmxc;
            return(OK);
        case B3SOI_MOD_LADICE0:
            value->rValue = model->B3SOIladice0;
            return(OK);
        case B3SOI_MOD_LA1:
            value->rValue = model->B3SOIla1;
            return(OK);
        case B3SOI_MOD_LA2:
            value->rValue = model->B3SOIla2;
            return(OK);
        case B3SOI_MOD_LRDSW:
            value->rValue = model->B3SOIlrdsw;
            return(OK);
        case B3SOI_MOD_LPRWB:
            value->rValue = model->B3SOIlprwb;
            return(OK);
        case B3SOI_MOD_LPRWG:
            value->rValue = model->B3SOIlprwg;
            return(OK);
        case B3SOI_MOD_LWR:
            value->rValue = model->B3SOIlwr;
            return(OK);
        case  B3SOI_MOD_LNFACTOR :
          value->rValue = model->B3SOIlnfactor;
            return(OK);
        case B3SOI_MOD_LDWG:
            value->rValue = model->B3SOIldwg;
            return(OK);
        case B3SOI_MOD_LDWB:
            value->rValue = model->B3SOIldwb;
            return(OK);
        case B3SOI_MOD_LVOFF:
            value->rValue = model->B3SOIlvoff;
            return(OK);
        case B3SOI_MOD_LETA0:
            value->rValue = model->B3SOIleta0;
            return(OK);
        case B3SOI_MOD_LETAB:
            value->rValue = model->B3SOIletab;
            return(OK);
        case  B3SOI_MOD_LDSUB :
          value->rValue = model->B3SOIldsub;
            return(OK);
        case  B3SOI_MOD_LCIT :
          value->rValue = model->B3SOIlcit;
            return(OK);
        case  B3SOI_MOD_LCDSC :
          value->rValue = model->B3SOIlcdsc;
            return(OK);
        case  B3SOI_MOD_LCDSCB :
          value->rValue = model->B3SOIlcdscb;
            return(OK);
        case  B3SOI_MOD_LCDSCD :
          value->rValue = model->B3SOIlcdscd;
            return(OK);
        case B3SOI_MOD_LPCLM:
            value->rValue = model->B3SOIlpclm;
            return(OK);
        case B3SOI_MOD_LPDIBL1:
            value->rValue = model->B3SOIlpdibl1;
            return(OK);
        case B3SOI_MOD_LPDIBL2:
            value->rValue = model->B3SOIlpdibl2;
            return(OK);
        case B3SOI_MOD_LPDIBLB:
            value->rValue = model->B3SOIlpdiblb;
            return(OK);
        case  B3SOI_MOD_LDROUT :
          value->rValue = model->B3SOIldrout;
            return(OK);
        case B3SOI_MOD_LPVAG:
            value->rValue = model->B3SOIlpvag;
            return(OK);
        case B3SOI_MOD_LDELTA:
            value->rValue = model->B3SOIldelta;
            return(OK);
        case B3SOI_MOD_LAII:
            value->rValue = model->B3SOIlaii;
            return(OK);
        case B3SOI_MOD_LBII:
            value->rValue = model->B3SOIlbii;
            return(OK);
        case B3SOI_MOD_LCII:
            value->rValue = model->B3SOIlcii;
            return(OK);
        case B3SOI_MOD_LDII:
            value->rValue = model->B3SOIldii;
            return(OK);
        case B3SOI_MOD_LALPHA0:
            value->rValue = model->B3SOIlalpha0;
            return(OK);
        case B3SOI_MOD_LALPHA1:
            value->rValue = model->B3SOIlalpha1;
            return(OK);
        case B3SOI_MOD_LBETA0:
            value->rValue = model->B3SOIlbeta0;
            return(OK);
        case B3SOI_MOD_LAGIDL:
            value->rValue = model->B3SOIlagidl;
            return(OK);
        case B3SOI_MOD_LBGIDL:
            value->rValue = model->B3SOIlbgidl;
            return(OK);
        case B3SOI_MOD_LNGIDL:
            value->rValue = model->B3SOIlngidl;
            return(OK);
        case B3SOI_MOD_LNTUN:
            value->rValue = model->B3SOIlntun;
            return(OK);
        case B3SOI_MOD_LNDIODE:
            value->rValue = model->B3SOIlndiode;
            return(OK);
        case B3SOI_MOD_LISBJT:
            value->rValue = model->B3SOIlisbjt;
            return(OK);
        case B3SOI_MOD_LISDIF:
            value->rValue = model->B3SOIlisdif;
            return(OK);
        case B3SOI_MOD_LISREC:
            value->rValue = model->B3SOIlisrec;
            return(OK);
        case B3SOI_MOD_LISTUN:
            value->rValue = model->B3SOIlistun;
            return(OK);
        case B3SOI_MOD_LEDL:
            value->rValue = model->B3SOIledl;
            return(OK);
        case B3SOI_MOD_LKBJT1:
            value->rValue = model->B3SOIlkbjt1;
            return(OK);
	/* CV Model */
        case B3SOI_MOD_LVSDFB:
            value->rValue = model->B3SOIlvsdfb;
            return(OK);
        case B3SOI_MOD_LVSDTH:
            value->rValue = model->B3SOIlvsdth;
            return(OK);
        /* Width Dependence */
        case B3SOI_MOD_WNPEAK:
            value->rValue = model->B3SOIwnpeak;
            return(OK);
        case B3SOI_MOD_WNSUB:
            value->rValue = model->B3SOIwnsub;
            return(OK);
        case B3SOI_MOD_WNGATE:
            value->rValue = model->B3SOIwngate;
            return(OK);
        case B3SOI_MOD_WVTH0:
            value->rValue = model->B3SOIwvth0;
            return(OK);
        case  B3SOI_MOD_WK1:
          value->rValue = model->B3SOIwk1;
            return(OK);
        case  B3SOI_MOD_WK2:
          value->rValue = model->B3SOIwk2;
            return(OK);
        case  B3SOI_MOD_WK3:
          value->rValue = model->B3SOIwk3;
            return(OK);
        case  B3SOI_MOD_WK3B:
          value->rValue = model->B3SOIwk3b;
            return(OK);
        case  B3SOI_MOD_WVBSA:
          value->rValue = model->B3SOIwvbsa;
            return(OK);
        case  B3SOI_MOD_WDELP:
          value->rValue = model->B3SOIwdelp;
            return(OK);
        case  B3SOI_MOD_WKB1:
            value->rValue = model->B3SOIwkb1;
            return(OK);
        case  B3SOI_MOD_WKB3:
            value->rValue = model->B3SOIwkb3;
            return(OK);
        case  B3SOI_MOD_WDVBD0:
            value->rValue = model->B3SOIdvbd0;
            return(OK);
        case  B3SOI_MOD_WDVBD1:
            value->rValue = model->B3SOIdvbd1;
            return(OK);
        case  B3SOI_MOD_WW0:
          value->rValue = model->B3SOIww0;
            return(OK);
        case  B3SOI_MOD_WNLX:
          value->rValue = model->B3SOIwnlx;
            return(OK);
        case  B3SOI_MOD_WDVT0 :
          value->rValue = model->B3SOIwdvt0;
            return(OK);
        case  B3SOI_MOD_WDVT1 :
          value->rValue = model->B3SOIwdvt1;
            return(OK);
        case  B3SOI_MOD_WDVT2 :
          value->rValue = model->B3SOIwdvt2;
            return(OK);
        case  B3SOI_MOD_WDVT0W :
          value->rValue = model->B3SOIwdvt0w;
            return(OK);
        case  B3SOI_MOD_WDVT1W :
          value->rValue = model->B3SOIwdvt1w;
            return(OK);
        case  B3SOI_MOD_WDVT2W :
          value->rValue = model->B3SOIwdvt2w;
            return(OK);
        case B3SOI_MOD_WU0:
            value->rValue = model->B3SOIwu0;
            return(OK);
        case B3SOI_MOD_WUA:
            value->rValue = model->B3SOIwua;
            return(OK);
        case B3SOI_MOD_WUB:
            value->rValue = model->B3SOIwub;
            return(OK);
        case B3SOI_MOD_WUC:
            value->rValue = model->B3SOIwuc;
            return(OK);
        case B3SOI_MOD_WVSAT:
            value->rValue = model->B3SOIwvsat;
            return(OK);
        case B3SOI_MOD_WA0:
            value->rValue = model->B3SOIwa0;
            return(OK);
        case B3SOI_MOD_WAGS:
            value->rValue = model->B3SOIwags;
            return(OK);
        case B3SOI_MOD_WB0:
            value->rValue = model->B3SOIwb0;
            return(OK);
        case B3SOI_MOD_WB1:
            value->rValue = model->B3SOIwb1;
            return(OK);
        case B3SOI_MOD_WKETA:
            value->rValue = model->B3SOIwketa;
            return(OK);
        case B3SOI_MOD_WABP:
            value->rValue = model->B3SOIwabp;
            return(OK);
        case B3SOI_MOD_WMXC:
            value->rValue = model->B3SOIwmxc;
            return(OK);
        case B3SOI_MOD_WADICE0:
            value->rValue = model->B3SOIwadice0;
            return(OK);
        case B3SOI_MOD_WA1:
            value->rValue = model->B3SOIwa1;
            return(OK);
        case B3SOI_MOD_WA2:
            value->rValue = model->B3SOIwa2;
            return(OK);
        case B3SOI_MOD_WRDSW:
            value->rValue = model->B3SOIwrdsw;
            return(OK);
        case B3SOI_MOD_WPRWB:
            value->rValue = model->B3SOIwprwb;
            return(OK);
        case B3SOI_MOD_WPRWG:
            value->rValue = model->B3SOIwprwg;
            return(OK);
        case B3SOI_MOD_WWR:
            value->rValue = model->B3SOIwwr;
            return(OK);
        case  B3SOI_MOD_WNFACTOR :
          value->rValue = model->B3SOIwnfactor;
            return(OK);
        case B3SOI_MOD_WDWG:
            value->rValue = model->B3SOIwdwg;
            return(OK);
        case B3SOI_MOD_WDWB:
            value->rValue = model->B3SOIwdwb;
            return(OK);
        case B3SOI_MOD_WVOFF:
            value->rValue = model->B3SOIwvoff;
            return(OK);
        case B3SOI_MOD_WETA0:
            value->rValue = model->B3SOIweta0;
            return(OK);
        case B3SOI_MOD_WETAB:
            value->rValue = model->B3SOIwetab;
            return(OK);
        case  B3SOI_MOD_WDSUB :
          value->rValue = model->B3SOIwdsub;
            return(OK);
        case  B3SOI_MOD_WCIT :
          value->rValue = model->B3SOIwcit;
            return(OK);
        case  B3SOI_MOD_WCDSC :
          value->rValue = model->B3SOIwcdsc;
            return(OK);
        case  B3SOI_MOD_WCDSCB :
          value->rValue = model->B3SOIwcdscb;
            return(OK);
        case  B3SOI_MOD_WCDSCD :
          value->rValue = model->B3SOIwcdscd;
            return(OK);
        case B3SOI_MOD_WPCLM:
            value->rValue = model->B3SOIwpclm;
            return(OK);
        case B3SOI_MOD_WPDIBL1:
            value->rValue = model->B3SOIwpdibl1;
            return(OK);
        case B3SOI_MOD_WPDIBL2:
            value->rValue = model->B3SOIwpdibl2;
            return(OK);
        case B3SOI_MOD_WPDIBLB:
            value->rValue = model->B3SOIwpdiblb;
            return(OK);
        case  B3SOI_MOD_WDROUT :
          value->rValue = model->B3SOIwdrout;
            return(OK);
        case B3SOI_MOD_WPVAG:
            value->rValue = model->B3SOIwpvag;
            return(OK);
        case B3SOI_MOD_WDELTA:
            value->rValue = model->B3SOIwdelta;
            return(OK);
        case B3SOI_MOD_WAII:
            value->rValue = model->B3SOIwaii;
            return(OK);
        case B3SOI_MOD_WBII:
            value->rValue = model->B3SOIwbii;
            return(OK);
        case B3SOI_MOD_WCII:
            value->rValue = model->B3SOIwcii;
            return(OK);
        case B3SOI_MOD_WDII:
            value->rValue = model->B3SOIwdii;
            return(OK);
        case B3SOI_MOD_WALPHA0:
            value->rValue = model->B3SOIwalpha0;
            return(OK);
        case B3SOI_MOD_WALPHA1:
            value->rValue = model->B3SOIwalpha1;
            return(OK);
        case B3SOI_MOD_WBETA0:
            value->rValue = model->B3SOIwbeta0;
            return(OK);
        case B3SOI_MOD_WAGIDL:
            value->rValue = model->B3SOIwagidl;
            return(OK);
        case B3SOI_MOD_WBGIDL:
            value->rValue = model->B3SOIwbgidl;
            return(OK);
        case B3SOI_MOD_WNGIDL:
            value->rValue = model->B3SOIwngidl;
            return(OK);
        case B3SOI_MOD_WNTUN:
            value->rValue = model->B3SOIwntun;
            return(OK);
        case B3SOI_MOD_WNDIODE:
            value->rValue = model->B3SOIwndiode;
            return(OK);
        case B3SOI_MOD_WISBJT:
            value->rValue = model->B3SOIwisbjt;
            return(OK);
        case B3SOI_MOD_WISDIF:
            value->rValue = model->B3SOIwisdif;
            return(OK);
        case B3SOI_MOD_WISREC:
            value->rValue = model->B3SOIwisrec;
            return(OK);
        case B3SOI_MOD_WISTUN:
            value->rValue = model->B3SOIwistun;
            return(OK);
        case B3SOI_MOD_WEDL:
            value->rValue = model->B3SOIwedl;
            return(OK);
        case B3SOI_MOD_WKBJT1:
            value->rValue = model->B3SOIwkbjt1;
            return(OK);
	/* CV Model */
        case B3SOI_MOD_WVSDFB:
            value->rValue = model->B3SOIwvsdfb;
            return(OK);
        case B3SOI_MOD_WVSDTH:
            value->rValue = model->B3SOIwvsdth;
            return(OK);
        /* Cross-term Dependence */
        case B3SOI_MOD_PNPEAK:
            value->rValue = model->B3SOIpnpeak;
            return(OK);
        case B3SOI_MOD_PNSUB:
            value->rValue = model->B3SOIpnsub;
            return(OK);
        case B3SOI_MOD_PNGATE:
            value->rValue = model->B3SOIpngate;
            return(OK);
        case B3SOI_MOD_PVTH0:
            value->rValue = model->B3SOIpvth0;
            return(OK);
        case  B3SOI_MOD_PK1:
          value->rValue = model->B3SOIpk1;
            return(OK);
        case  B3SOI_MOD_PK2:
          value->rValue = model->B3SOIpk2;
            return(OK);
        case  B3SOI_MOD_PK3:
          value->rValue = model->B3SOIpk3;
            return(OK);
        case  B3SOI_MOD_PK3B:
          value->rValue = model->B3SOIpk3b;
            return(OK);
        case  B3SOI_MOD_PVBSA:
          value->rValue = model->B3SOIpvbsa;
            return(OK);
        case  B3SOI_MOD_PDELP:
          value->rValue = model->B3SOIpdelp;
            return(OK);
        case  B3SOI_MOD_PKB1:
            value->rValue = model->B3SOIpkb1;
            return(OK);
        case  B3SOI_MOD_PKB3:
            value->rValue = model->B3SOIpkb3;
            return(OK);
        case  B3SOI_MOD_PDVBD0:
            value->rValue = model->B3SOIdvbd0;
            return(OK);
        case  B3SOI_MOD_PDVBD1:
            value->rValue = model->B3SOIdvbd1;
            return(OK);
        case  B3SOI_MOD_PW0:
          value->rValue = model->B3SOIpw0;
            return(OK);
        case  B3SOI_MOD_PNLX:
          value->rValue = model->B3SOIpnlx;
            return(OK);
        case  B3SOI_MOD_PDVT0 :
          value->rValue = model->B3SOIpdvt0;
            return(OK);
        case  B3SOI_MOD_PDVT1 :
          value->rValue = model->B3SOIpdvt1;
            return(OK);
        case  B3SOI_MOD_PDVT2 :
          value->rValue = model->B3SOIpdvt2;
            return(OK);
        case  B3SOI_MOD_PDVT0W :
          value->rValue = model->B3SOIpdvt0w;
            return(OK);
        case  B3SOI_MOD_PDVT1W :
          value->rValue = model->B3SOIpdvt1w;
            return(OK);
        case  B3SOI_MOD_PDVT2W :
          value->rValue = model->B3SOIpdvt2w;
            return(OK);
        case B3SOI_MOD_PU0:
            value->rValue = model->B3SOIpu0;
            return(OK);
        case B3SOI_MOD_PUA:
            value->rValue = model->B3SOIpua;
            return(OK);
        case B3SOI_MOD_PUB:
            value->rValue = model->B3SOIpub;
            return(OK);
        case B3SOI_MOD_PUC:
            value->rValue = model->B3SOIpuc;
            return(OK);
        case B3SOI_MOD_PVSAT:
            value->rValue = model->B3SOIpvsat;
            return(OK);
        case B3SOI_MOD_PA0:
            value->rValue = model->B3SOIpa0;
            return(OK);
        case B3SOI_MOD_PAGS:
            value->rValue = model->B3SOIpags;
            return(OK);
        case B3SOI_MOD_PB0:
            value->rValue = model->B3SOIpb0;
            return(OK);
        case B3SOI_MOD_PB1:
            value->rValue = model->B3SOIpb1;
            return(OK);
        case B3SOI_MOD_PKETA:
            value->rValue = model->B3SOIpketa;
            return(OK);
        case B3SOI_MOD_PABP:
            value->rValue = model->B3SOIpabp;
            return(OK);
        case B3SOI_MOD_PMXC:
            value->rValue = model->B3SOIpmxc;
            return(OK);
        case B3SOI_MOD_PADICE0:
            value->rValue = model->B3SOIpadice0;
            return(OK);
        case B3SOI_MOD_PA1:
            value->rValue = model->B3SOIpa1;
            return(OK);
        case B3SOI_MOD_PA2:
            value->rValue = model->B3SOIpa2;
            return(OK);
        case B3SOI_MOD_PRDSW:
            value->rValue = model->B3SOIprdsw;
            return(OK);
        case B3SOI_MOD_PPRWB:
            value->rValue = model->B3SOIpprwb;
            return(OK);
        case B3SOI_MOD_PPRWG:
            value->rValue = model->B3SOIpprwg;
            return(OK);
        case B3SOI_MOD_PWR:
            value->rValue = model->B3SOIpwr;
            return(OK);
        case  B3SOI_MOD_PNFACTOR :
          value->rValue = model->B3SOIpnfactor;
            return(OK);
        case B3SOI_MOD_PDWG:
            value->rValue = model->B3SOIpdwg;
            return(OK);
        case B3SOI_MOD_PDWB:
            value->rValue = model->B3SOIpdwb;
            return(OK);
        case B3SOI_MOD_PVOFF:
            value->rValue = model->B3SOIpvoff;
            return(OK);
        case B3SOI_MOD_PETA0:
            value->rValue = model->B3SOIpeta0;
            return(OK);
        case B3SOI_MOD_PETAB:
            value->rValue = model->B3SOIpetab;
            return(OK);
        case  B3SOI_MOD_PDSUB :
          value->rValue = model->B3SOIpdsub;
            return(OK);
        case  B3SOI_MOD_PCIT :
          value->rValue = model->B3SOIpcit;
            return(OK);
        case  B3SOI_MOD_PCDSC :
          value->rValue = model->B3SOIpcdsc;
            return(OK);
        case  B3SOI_MOD_PCDSCB :
          value->rValue = model->B3SOIpcdscb;
            return(OK);
        case  B3SOI_MOD_PCDSCD :
          value->rValue = model->B3SOIpcdscd;
            return(OK);
        case B3SOI_MOD_PPCLM:
            value->rValue = model->B3SOIppclm;
            return(OK);
        case B3SOI_MOD_PPDIBL1:
            value->rValue = model->B3SOIppdibl1;
            return(OK);
        case B3SOI_MOD_PPDIBL2:
            value->rValue = model->B3SOIppdibl2;
            return(OK);
        case B3SOI_MOD_PPDIBLB:
            value->rValue = model->B3SOIppdiblb;
            return(OK);
        case  B3SOI_MOD_PDROUT :
          value->rValue = model->B3SOIpdrout;
            return(OK);
        case B3SOI_MOD_PPVAG:
            value->rValue = model->B3SOIppvag;
            return(OK);
        case B3SOI_MOD_PDELTA:
            value->rValue = model->B3SOIpdelta;
            return(OK);
        case B3SOI_MOD_PAII:
            value->rValue = model->B3SOIpaii;
            return(OK);
        case B3SOI_MOD_PBII:
            value->rValue = model->B3SOIpbii;
            return(OK);
        case B3SOI_MOD_PCII:
            value->rValue = model->B3SOIpcii;
            return(OK);
        case B3SOI_MOD_PDII:
            value->rValue = model->B3SOIpdii;
            return(OK);
        case B3SOI_MOD_PALPHA0:
            value->rValue = model->B3SOIpalpha0;
            return(OK);
        case B3SOI_MOD_PALPHA1:
            value->rValue = model->B3SOIpalpha1;
            return(OK);
        case B3SOI_MOD_PBETA0:
            value->rValue = model->B3SOIpbeta0;
            return(OK);
        case B3SOI_MOD_PAGIDL:
            value->rValue = model->B3SOIpagidl;
            return(OK);
        case B3SOI_MOD_PBGIDL:
            value->rValue = model->B3SOIpbgidl;
            return(OK);
        case B3SOI_MOD_PNGIDL:
            value->rValue = model->B3SOIpngidl;
            return(OK);
        case B3SOI_MOD_PNTUN:
            value->rValue = model->B3SOIpntun;
            return(OK);
        case B3SOI_MOD_PNDIODE:
            value->rValue = model->B3SOIpndiode;
            return(OK);
        case B3SOI_MOD_PISBJT:
            value->rValue = model->B3SOIpisbjt;
            return(OK);
        case B3SOI_MOD_PISDIF:
            value->rValue = model->B3SOIpisdif;
            return(OK);
        case B3SOI_MOD_PISREC:
            value->rValue = model->B3SOIpisrec;
            return(OK);
        case B3SOI_MOD_PISTUN:
            value->rValue = model->B3SOIpistun;
            return(OK);
        case B3SOI_MOD_PEDL:
            value->rValue = model->B3SOIpedl;
            return(OK);
        case B3SOI_MOD_PKBJT1:
            value->rValue = model->B3SOIpkbjt1;
            return(OK);
	/* CV Model */
        case B3SOI_MOD_PVSDFB:
            value->rValue = model->B3SOIpvsdfb;
            return(OK);
        case B3SOI_MOD_PVSDTH:
            value->rValue = model->B3SOIpvsdth;
            return(OK);
/* Added for binning - END */

        default:
            return(E_BADPARM);
    }
    /* NOTREACHED */
}



