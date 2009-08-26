/*
$Log:   b3soimask.c,v $
 * Revision 2.0  99/2/15  Pin Su
 * BSIMPD2.0 release
 *
*/
static char rcsid[] = "$Id: b3soimask.c,v 2.0 99/2/15 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soimask.c          98/5/01
Modified by Pin Su and Jan Feng	99/2/15
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
        case B3SOI_MOD_RTH0:
            value->rValue = model->B3SOIrth0; 
            return(OK);
        case B3SOI_MOD_CTH0:
            value->rValue = model->B3SOIcth0; 
            return(OK);
        case B3SOI_MOD_NDIODE:
            value->rValue = model->B3SOIndiode; 
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


/* v2.0 release */
        case B3SOI_MOD_K1W1:                    
            value->rValue = model->B3SOIk1w1;
            return(OK);
        case B3SOI_MOD_K1W2:
            value->rValue = model->B3SOIk1w2;
            return(OK);
        case B3SOI_MOD_KETAS:
            value->rValue = model->B3SOIketas;
            return(OK);
        case B3SOI_MOD_DWBC:
            value->rValue = model->B3SOIdwbc;
            return(OK);
        case B3SOI_MOD_BETA0:
            value->rValue = model->B3SOIbeta0;
            return(OK);
        case B3SOI_MOD_BETA1:
            value->rValue = model->B3SOIbeta1;
            return(OK);
        case B3SOI_MOD_BETA2:
            value->rValue = model->B3SOIbeta2;
            return(OK);
        case B3SOI_MOD_VDSATII0:
            value->rValue = model->B3SOIvdsatii0;
            return(OK);
        case B3SOI_MOD_TII:
            value->rValue = model->B3SOItii;
            return(OK);
        case B3SOI_MOD_LII:
            value->rValue = model->B3SOIlii;
            return(OK);
        case B3SOI_MOD_SII0:
            value->rValue = model->B3SOIsii0;
            return(OK);
        case B3SOI_MOD_SII1:
            value->rValue = model->B3SOIsii1;
            return(OK);
        case B3SOI_MOD_SII2:
            value->rValue = model->B3SOIsii2;
            return(OK);
        case B3SOI_MOD_SIID:
            value->rValue = model->B3SOIsiid;
            return(OK);
        case B3SOI_MOD_FBJTII:
            value->rValue = model->B3SOIfbjtii;
            return(OK);
        case B3SOI_MOD_ESATII:
            value->rValue = model->B3SOIesatii;
            return(OK);
        case B3SOI_MOD_NTUN:
            value->rValue = model->B3SOIntun;
            return(OK);
        case B3SOI_MOD_NDIO:
            value->rValue = model->B3SOIndio;
            return(OK);
        case B3SOI_MOD_NRECF0:
            value->rValue = model->B3SOInrecf0;
            return(OK);
        case B3SOI_MOD_NRECR0:
            value->rValue = model->B3SOInrecr0;
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
        case B3SOI_MOD_LN:
            value->rValue = model->B3SOIln;
            return(OK);
        case B3SOI_MOD_VREC0:
            value->rValue = model->B3SOIvrec0;
            return(OK);
        case B3SOI_MOD_VTUN0:
            value->rValue = model->B3SOIvtun0;
            return(OK);
        case B3SOI_MOD_NBJT:
            value->rValue = model->B3SOInbjt;
            return(OK);
        case B3SOI_MOD_LBJT0:
            value->rValue = model->B3SOIlbjt0;
            return(OK);
        case B3SOI_MOD_LDIF0:
            value->rValue = model->B3SOIldif0;
            return(OK);
        case B3SOI_MOD_VABJT:
            value->rValue = model->B3SOIvabjt;
            return(OK);
        case B3SOI_MOD_AELY:
            value->rValue = model->B3SOIaely;
            return(OK);
        case B3SOI_MOD_AHLI:
            value->rValue = model->B3SOIahli;
            return(OK);
        case B3SOI_MOD_RBODY:
            value->rValue = model->B3SOIrbody;
            return(OK);
        case B3SOI_MOD_RBSH:
            value->rValue = model->B3SOIrbsh;
            return(OK);
        case B3SOI_MOD_NTRECF:
            value->rValue = model->B3SOIntrecf;
            return(OK);
        case B3SOI_MOD_NTRECR:
            value->rValue = model->B3SOIntrecr;
            return(OK);
        case B3SOI_MOD_NDIF:
            value->rValue = model->B3SOIndif;
            return(OK);
        case B3SOI_MOD_DLCB:
            value->rValue = model->B3SOIdlcb;
            return(OK);
        case B3SOI_MOD_FBODY:
            value->rValue = model->B3SOIfbody;
            return(OK);
        case B3SOI_MOD_TCJSWG:
            value->rValue = model->B3SOItcjswg;
            return(OK);
        case B3SOI_MOD_TPBSWG:
            value->rValue = model->B3SOItpbswg;
            return(OK);
        case B3SOI_MOD_ACDE:
            value->rValue = model->B3SOIacde;
            return(OK);
        case B3SOI_MOD_MOIN:
            value->rValue = model->B3SOImoin;
            return(OK);
        case B3SOI_MOD_DELVT:
            value->rValue = model->B3SOIdelvt;
            return(OK);
        case  B3SOI_MOD_KB1:
            value->rValue = model->B3SOIkb1;
            return(OK);
        case B3SOI_MOD_DLBG:
            value->rValue = model->B3SOIdlbg;
            return(OK);

        case B3SOI_MOD_NGIDL:
            value->rValue = model->B3SOIngidl;
            return(OK);
        case B3SOI_MOD_AGIDL:
            value->rValue = model->B3SOIagidl;
            return(OK);
        case B3SOI_MOD_BGIDL:
            value->rValue = model->B3SOIbgidl;
            return(OK);


        default:
            return(E_BADPARM);
    }
    /* NOTREACHED */
}



