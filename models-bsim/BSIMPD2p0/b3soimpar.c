/* $Id: b3soimpar.c,v 2.0 99/2/15 Pin Su Release $  */
/*
$Log:   b3soimpar.c,v $
 * Revision 2.0  99/2/15  Pin Su
 * BSIMPD2.0 release
 *
*/
static char rcsid[] = "$Id: b3soimpar.c,v 2.0 99/2/15 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soimpar.c          98/5/01
Modified by Pin Su and Jan Feng	99/2/15
**********/


#include "spice.h"
#include <stdio.h>
#include "util.h"
#include "b3soidef.h"
#include "ifsim.h"
#include "sperror.h"
#include "suffix.h"


int
B3SOImParam(param,value,inMod)
int param;
IFvalue *value;
GENmodel *inMod;
{
    B3SOImodel *mod = (B3SOImodel*)inMod;
    switch(param)
    {  
 
	case  B3SOI_MOD_MOBMOD :
            mod->B3SOImobMod = value->iValue;
            mod->B3SOImobModGiven = TRUE;
            break;
        case  B3SOI_MOD_BINUNIT :
            mod->B3SOIbinUnit = value->iValue;
            mod->B3SOIbinUnitGiven = TRUE;
            break;
        case  B3SOI_MOD_PARAMCHK :
            mod->B3SOIparamChk = value->iValue;
            mod->B3SOIparamChkGiven = TRUE;
            break;
        case  B3SOI_MOD_CAPMOD :
            mod->B3SOIcapMod = value->iValue;
            mod->B3SOIcapModGiven = TRUE;
            break;
        case  B3SOI_MOD_SHMOD :
            mod->B3SOIshMod = value->iValue;
            mod->B3SOIshModGiven = TRUE;
            break;
        case  B3SOI_MOD_NOIMOD :
            mod->B3SOInoiMod = value->iValue;
            mod->B3SOInoiModGiven = TRUE;
            break;
        case  B3SOI_MOD_VERSION :
            mod->B3SOIversion = value->rValue;
            mod->B3SOIversionGiven = TRUE;
            break;
        case  B3SOI_MOD_TOX :
            mod->B3SOItox = value->rValue;
            mod->B3SOItoxGiven = TRUE;
            break;

        case  B3SOI_MOD_CDSC :
            mod->B3SOIcdsc = value->rValue;
            mod->B3SOIcdscGiven = TRUE;
            break;
        case  B3SOI_MOD_CDSCB :
            mod->B3SOIcdscb = value->rValue;
            mod->B3SOIcdscbGiven = TRUE;
            break;

        case  B3SOI_MOD_CDSCD :
            mod->B3SOIcdscd = value->rValue;
            mod->B3SOIcdscdGiven = TRUE;
            break;

        case  B3SOI_MOD_CIT :
            mod->B3SOIcit = value->rValue;
            mod->B3SOIcitGiven = TRUE;
            break;
        case  B3SOI_MOD_NFACTOR :
            mod->B3SOInfactor = value->rValue;
            mod->B3SOInfactorGiven = TRUE;
            break;
        case B3SOI_MOD_VSAT:
            mod->B3SOIvsat = value->rValue;
            mod->B3SOIvsatGiven = TRUE;
            break;
        case B3SOI_MOD_A0:
            mod->B3SOIa0 = value->rValue;
            mod->B3SOIa0Given = TRUE;
            break;
        
        case B3SOI_MOD_AGS:
            mod->B3SOIags= value->rValue;
            mod->B3SOIagsGiven = TRUE;
            break;
        
        case B3SOI_MOD_A1:
            mod->B3SOIa1 = value->rValue;
            mod->B3SOIa1Given = TRUE;
            break;
        case B3SOI_MOD_A2:
            mod->B3SOIa2 = value->rValue;
            mod->B3SOIa2Given = TRUE;
            break;
        case B3SOI_MOD_AT:
            mod->B3SOIat = value->rValue;
            mod->B3SOIatGiven = TRUE;
            break;
        case B3SOI_MOD_KETA:
            mod->B3SOIketa = value->rValue;
            mod->B3SOIketaGiven = TRUE;
            break;    
        case B3SOI_MOD_NSUB:
            mod->B3SOInsub = value->rValue;
            mod->B3SOInsubGiven = TRUE;
            break;
        case B3SOI_MOD_NPEAK:
            mod->B3SOInpeak = value->rValue;
            mod->B3SOInpeakGiven = TRUE;
	    if (mod->B3SOInpeak > 1.0e20)
		mod->B3SOInpeak *= 1.0e-6;
            break;
        case B3SOI_MOD_NGATE:
            mod->B3SOIngate = value->rValue;
            mod->B3SOIngateGiven = TRUE;
	    if (mod->B3SOIngate > 1.0e23)
		mod->B3SOIngate *= 1.0e-6;
            break;
        case B3SOI_MOD_GAMMA1:
            mod->B3SOIgamma1 = value->rValue;
            mod->B3SOIgamma1Given = TRUE;
            break;
        case B3SOI_MOD_GAMMA2:
            mod->B3SOIgamma2 = value->rValue;
            mod->B3SOIgamma2Given = TRUE;
            break;
        case B3SOI_MOD_VBX:
            mod->B3SOIvbx = value->rValue;
            mod->B3SOIvbxGiven = TRUE;
            break;
        case B3SOI_MOD_VBM:
            mod->B3SOIvbm = value->rValue;
            mod->B3SOIvbmGiven = TRUE;
            break;
        case B3SOI_MOD_XT:
            mod->B3SOIxt = value->rValue;
            mod->B3SOIxtGiven = TRUE;
            break;
        case  B3SOI_MOD_K1:
            mod->B3SOIk1 = value->rValue;
            mod->B3SOIk1Given = TRUE;
            break;
        case  B3SOI_MOD_KT1:
            mod->B3SOIkt1 = value->rValue;
            mod->B3SOIkt1Given = TRUE;
            break;
        case  B3SOI_MOD_KT1L:
            mod->B3SOIkt1l = value->rValue;
            mod->B3SOIkt1lGiven = TRUE;
            break;
        case  B3SOI_MOD_KT2:
            mod->B3SOIkt2 = value->rValue;
            mod->B3SOIkt2Given = TRUE;
            break;
        case  B3SOI_MOD_K2:
            mod->B3SOIk2 = value->rValue;
            mod->B3SOIk2Given = TRUE;
            break;
        case  B3SOI_MOD_K3:
            mod->B3SOIk3 = value->rValue;
            mod->B3SOIk3Given = TRUE;
            break;
        case  B3SOI_MOD_K3B:
            mod->B3SOIk3b = value->rValue;
            mod->B3SOIk3bGiven = TRUE;
            break;
        case  B3SOI_MOD_NLX:
            mod->B3SOInlx = value->rValue;
            mod->B3SOInlxGiven = TRUE;
            break;
        case  B3SOI_MOD_W0:
            mod->B3SOIw0 = value->rValue;
            mod->B3SOIw0Given = TRUE;
            break;
        case  B3SOI_MOD_DVT0:               
            mod->B3SOIdvt0 = value->rValue;
            mod->B3SOIdvt0Given = TRUE;
            break;
        case  B3SOI_MOD_DVT1:             
            mod->B3SOIdvt1 = value->rValue;
            mod->B3SOIdvt1Given = TRUE;
            break;
        case  B3SOI_MOD_DVT2:             
            mod->B3SOIdvt2 = value->rValue;
            mod->B3SOIdvt2Given = TRUE;
            break;
        case  B3SOI_MOD_DVT0W:               
            mod->B3SOIdvt0w = value->rValue;
            mod->B3SOIdvt0wGiven = TRUE;
            break;
        case  B3SOI_MOD_DVT1W:             
            mod->B3SOIdvt1w = value->rValue;
            mod->B3SOIdvt1wGiven = TRUE;
            break;
        case  B3SOI_MOD_DVT2W:             
            mod->B3SOIdvt2w = value->rValue;
            mod->B3SOIdvt2wGiven = TRUE;
            break;
        case  B3SOI_MOD_DROUT:             
            mod->B3SOIdrout = value->rValue;
            mod->B3SOIdroutGiven = TRUE;
            break;
        case  B3SOI_MOD_DSUB:             
            mod->B3SOIdsub = value->rValue;
            mod->B3SOIdsubGiven = TRUE;
            break;
        case B3SOI_MOD_VTH0:
            mod->B3SOIvth0 = value->rValue;
            mod->B3SOIvth0Given = TRUE;
            break;
        case B3SOI_MOD_UA:
            mod->B3SOIua = value->rValue;
            mod->B3SOIuaGiven = TRUE;
            break;
        case B3SOI_MOD_UA1:
            mod->B3SOIua1 = value->rValue;
            mod->B3SOIua1Given = TRUE;
            break;
        case B3SOI_MOD_UB:
            mod->B3SOIub = value->rValue;
            mod->B3SOIubGiven = TRUE;
            break;
        case B3SOI_MOD_UB1:
            mod->B3SOIub1 = value->rValue;
            mod->B3SOIub1Given = TRUE;
            break;
        case B3SOI_MOD_UC:
            mod->B3SOIuc = value->rValue;
            mod->B3SOIucGiven = TRUE;
            break;
        case B3SOI_MOD_UC1:
            mod->B3SOIuc1 = value->rValue;
            mod->B3SOIuc1Given = TRUE;
            break;
        case  B3SOI_MOD_U0 :
            mod->B3SOIu0 = value->rValue;
            mod->B3SOIu0Given = TRUE;
            break;
        case  B3SOI_MOD_UTE :
            mod->B3SOIute = value->rValue;
            mod->B3SOIuteGiven = TRUE;
            break;
        case B3SOI_MOD_VOFF:
            mod->B3SOIvoff = value->rValue;
            mod->B3SOIvoffGiven = TRUE;
            break;
        case  B3SOI_MOD_DELTA :
            mod->B3SOIdelta = value->rValue;
            mod->B3SOIdeltaGiven = TRUE;
            break;
        case B3SOI_MOD_RDSW:
            mod->B3SOIrdsw = value->rValue;
            mod->B3SOIrdswGiven = TRUE;
            break;                     
        case B3SOI_MOD_PRWG:
            mod->B3SOIprwg = value->rValue;
            mod->B3SOIprwgGiven = TRUE;
            break;                     
        case B3SOI_MOD_PRWB:
            mod->B3SOIprwb = value->rValue;
            mod->B3SOIprwbGiven = TRUE;
            break;                     
        case B3SOI_MOD_PRT:
            mod->B3SOIprt = value->rValue;
            mod->B3SOIprtGiven = TRUE;
            break;                     
        case B3SOI_MOD_ETA0:
            mod->B3SOIeta0 = value->rValue;
            mod->B3SOIeta0Given = TRUE;
            break;                 
        case B3SOI_MOD_ETAB:
            mod->B3SOIetab = value->rValue;
            mod->B3SOIetabGiven = TRUE;
            break;                 
        case B3SOI_MOD_PCLM:
            mod->B3SOIpclm = value->rValue;
            mod->B3SOIpclmGiven = TRUE;
            break;                 
        case B3SOI_MOD_PDIBL1:
            mod->B3SOIpdibl1 = value->rValue;
            mod->B3SOIpdibl1Given = TRUE;
            break;                 
        case B3SOI_MOD_PDIBL2:
            mod->B3SOIpdibl2 = value->rValue;
            mod->B3SOIpdibl2Given = TRUE;
            break;                 
        case B3SOI_MOD_PDIBLB:
            mod->B3SOIpdiblb = value->rValue;
            mod->B3SOIpdiblbGiven = TRUE;
            break;                 
        case B3SOI_MOD_PVAG:
            mod->B3SOIpvag = value->rValue;
            mod->B3SOIpvagGiven = TRUE;
            break;                 
        case  B3SOI_MOD_WR :
            mod->B3SOIwr = value->rValue;
            mod->B3SOIwrGiven = TRUE;
            break;
        case  B3SOI_MOD_DWG :
            mod->B3SOIdwg = value->rValue;
            mod->B3SOIdwgGiven = TRUE;
            break;
        case  B3SOI_MOD_DWB :
            mod->B3SOIdwb = value->rValue;
            mod->B3SOIdwbGiven = TRUE;
            break;
        case  B3SOI_MOD_B0 :
            mod->B3SOIb0 = value->rValue;
            mod->B3SOIb0Given = TRUE;
            break;
        case  B3SOI_MOD_B1 :
            mod->B3SOIb1 = value->rValue;
            mod->B3SOIb1Given = TRUE;
            break;
        case  B3SOI_MOD_ALPHA0 :
            mod->B3SOIalpha0 = value->rValue;
            mod->B3SOIalpha0Given = TRUE;
            break;
        case  B3SOI_MOD_ALPHA1 :
            mod->B3SOIalpha1 = value->rValue;
            mod->B3SOIalpha1Given = TRUE;
            break;

        case  B3SOI_MOD_CGSL :
            mod->B3SOIcgsl = value->rValue;
            mod->B3SOIcgslGiven = TRUE;
            break;
        case  B3SOI_MOD_CGDL :
            mod->B3SOIcgdl = value->rValue;
            mod->B3SOIcgdlGiven = TRUE;
            break;
        case  B3SOI_MOD_CKAPPA :
            mod->B3SOIckappa = value->rValue;
            mod->B3SOIckappaGiven = TRUE;
            break;
        case  B3SOI_MOD_CF :
            mod->B3SOIcf = value->rValue;
            mod->B3SOIcfGiven = TRUE;
            break;
        case  B3SOI_MOD_CLC :
            mod->B3SOIclc = value->rValue;
            mod->B3SOIclcGiven = TRUE;
            break;
        case  B3SOI_MOD_CLE :
            mod->B3SOIcle = value->rValue;
            mod->B3SOIcleGiven = TRUE;
            break;
        case  B3SOI_MOD_DWC :
            mod->B3SOIdwc = value->rValue;
            mod->B3SOIdwcGiven = TRUE;
            break;
        case  B3SOI_MOD_DLC :
            mod->B3SOIdlc = value->rValue;
            mod->B3SOIdlcGiven = TRUE;
            break;
        case  B3SOI_MOD_TBOX :
            mod->B3SOItbox = value->rValue;
            mod->B3SOItboxGiven = TRUE;
            break;
        case  B3SOI_MOD_TSI :
            mod->B3SOItsi = value->rValue;
            mod->B3SOItsiGiven = TRUE;
            break;
        case  B3SOI_MOD_XJ :
            mod->B3SOIxj = value->rValue;
            mod->B3SOIxjGiven = TRUE;
            break;
        case  B3SOI_MOD_RBODY :
            mod->B3SOIrbody = value->rValue;
            mod->B3SOIrbodyGiven = TRUE;
            break;
        case  B3SOI_MOD_RBSH :
            mod->B3SOIrbsh = value->rValue;
            mod->B3SOIrbshGiven = TRUE;
            break;
        case  B3SOI_MOD_RTH0 :
            mod->B3SOIrth0 = value->rValue;
            mod->B3SOIrth0Given = TRUE;
            break;
        case  B3SOI_MOD_CTH0 :
            mod->B3SOIcth0 = value->rValue;
            mod->B3SOIcth0Given = TRUE;
            break;
        case  B3SOI_MOD_NGIDL :
            mod->B3SOIngidl = value->rValue;
            mod->B3SOIngidlGiven = TRUE;
            break;
        case  B3SOI_MOD_AGIDL :
            mod->B3SOIagidl = value->rValue;
            mod->B3SOIagidlGiven = TRUE;
            break;
        case  B3SOI_MOD_BGIDL :
            mod->B3SOIbgidl = value->rValue;
            mod->B3SOIbgidlGiven = TRUE;
            break;
        case  B3SOI_MOD_NDIODE :
            mod->B3SOIndiode = value->rValue;
            mod->B3SOIndiodeGiven = TRUE;
            break;
        case  B3SOI_MOD_XBJT :
            mod->B3SOIxbjt = value->rValue;
            mod->B3SOIxbjtGiven = TRUE;
            break;
        case  B3SOI_MOD_XREC :
            mod->B3SOIxrec = value->rValue;
            mod->B3SOIxrecGiven = TRUE;
            break;
        case  B3SOI_MOD_XTUN :
            mod->B3SOIxtun = value->rValue;
            mod->B3SOIxtunGiven = TRUE;
            break;
        case  B3SOI_MOD_TT :
            mod->B3SOItt = value->rValue;
            mod->B3SOIttGiven = TRUE;
            break;
        case  B3SOI_MOD_VSDTH :
            mod->B3SOIvsdth = value->rValue;
            mod->B3SOIvsdthGiven = TRUE;
            break;
        case  B3SOI_MOD_VSDFB :
            mod->B3SOIvsdfb = value->rValue;
            mod->B3SOIvsdfbGiven = TRUE;
            break;
        case  B3SOI_MOD_CSDMIN :
            mod->B3SOIcsdmin = value->rValue;
            mod->B3SOIcsdminGiven = TRUE;
            break;
        case  B3SOI_MOD_ASD :
            mod->B3SOIasd = value->rValue;
            mod->B3SOIasdGiven = TRUE;
            break;


        case  B3SOI_MOD_TNOM :
            mod->B3SOItnom = value->rValue + 273.15;
            mod->B3SOItnomGiven = TRUE;
            break;
        case  B3SOI_MOD_CGSO :
            mod->B3SOIcgso = value->rValue;
            mod->B3SOIcgsoGiven = TRUE;
            break;
        case  B3SOI_MOD_CGDO :
            mod->B3SOIcgdo = value->rValue;
            mod->B3SOIcgdoGiven = TRUE;
            break;
        case  B3SOI_MOD_CGEO :
            mod->B3SOIcgeo = value->rValue;
            mod->B3SOIcgeoGiven = TRUE;
            break;
        case  B3SOI_MOD_XPART :
            mod->B3SOIxpart = value->rValue;
            mod->B3SOIxpartGiven = TRUE;
            break;
        case  B3SOI_MOD_RSH :
            mod->B3SOIsheetResistance = value->rValue;
            mod->B3SOIsheetResistanceGiven = TRUE;
            break;
        case  B3SOI_MOD_PBSWG :
            mod->B3SOIGatesidewallJctPotential = value->rValue;
            mod->B3SOIGatesidewallJctPotentialGiven = TRUE;
            break;
        case  B3SOI_MOD_MJSWG :
            mod->B3SOIbodyJctGateSideGradingCoeff = value->rValue;
            mod->B3SOIbodyJctGateSideGradingCoeffGiven = TRUE;
            break;
        case  B3SOI_MOD_CJSWG :
            mod->B3SOIunitLengthGateSidewallJctCap = value->rValue;
            mod->B3SOIunitLengthGateSidewallJctCapGiven = TRUE;
            break;
        case  B3SOI_MOD_CSDESW :
            mod->B3SOIcsdesw = value->rValue;
            mod->B3SOIcsdeswGiven = TRUE;
            break;
        case  B3SOI_MOD_LINT :
            mod->B3SOILint = value->rValue;
            mod->B3SOILintGiven = TRUE;
            break;
        case  B3SOI_MOD_LL :
            mod->B3SOILl = value->rValue;
            mod->B3SOILlGiven = TRUE;
            break;
        case  B3SOI_MOD_LLN :
            mod->B3SOILln = value->rValue;
            mod->B3SOILlnGiven = TRUE;
            break;
        case  B3SOI_MOD_LW :
            mod->B3SOILw = value->rValue;
            mod->B3SOILwGiven = TRUE;
            break;
        case  B3SOI_MOD_LWN :
            mod->B3SOILwn = value->rValue;
            mod->B3SOILwnGiven = TRUE;
            break;
        case  B3SOI_MOD_LWL :
            mod->B3SOILwl = value->rValue;
            mod->B3SOILwlGiven = TRUE;
            break;
        case  B3SOI_MOD_WINT :
            mod->B3SOIWint = value->rValue;
            mod->B3SOIWintGiven = TRUE;
            break;
        case  B3SOI_MOD_WL :
            mod->B3SOIWl = value->rValue;
            mod->B3SOIWlGiven = TRUE;
            break;
        case  B3SOI_MOD_WLN :
            mod->B3SOIWln = value->rValue;
            mod->B3SOIWlnGiven = TRUE;
            break;
        case  B3SOI_MOD_WW :
            mod->B3SOIWw = value->rValue;
            mod->B3SOIWwGiven = TRUE;
            break;
        case  B3SOI_MOD_WWN :
            mod->B3SOIWwn = value->rValue;
            mod->B3SOIWwnGiven = TRUE;
            break;
        case  B3SOI_MOD_WWL :
            mod->B3SOIWwl = value->rValue;
            mod->B3SOIWwlGiven = TRUE;
            break;

        case  B3SOI_MOD_NOIA :
            mod->B3SOIoxideTrapDensityA = value->rValue;
            mod->B3SOIoxideTrapDensityAGiven = TRUE;
            break;
        case  B3SOI_MOD_NOIB :
            mod->B3SOIoxideTrapDensityB = value->rValue;
            mod->B3SOIoxideTrapDensityBGiven = TRUE;
            break;
        case  B3SOI_MOD_NOIC :
            mod->B3SOIoxideTrapDensityC = value->rValue;
            mod->B3SOIoxideTrapDensityCGiven = TRUE;
            break;
        case  B3SOI_MOD_NOIF :
            mod->B3SOInoif = value->rValue;
            mod->B3SOInoifGiven = TRUE;
            break;
        case  B3SOI_MOD_EM :
            mod->B3SOIem = value->rValue;
            mod->B3SOIemGiven = TRUE;
            break;
        case  B3SOI_MOD_EF :
            mod->B3SOIef = value->rValue;
            mod->B3SOIefGiven = TRUE;
            break;
        case  B3SOI_MOD_AF :
            mod->B3SOIaf = value->rValue;
            mod->B3SOIafGiven = TRUE;
            break;
        case  B3SOI_MOD_KF :
            mod->B3SOIkf = value->rValue;
            mod->B3SOIkfGiven = TRUE;
            break;


/* v2.0 release */
        case  B3SOI_MOD_K1W1 :         
            mod->B3SOIk1w1 = value->rValue;
            mod->B3SOIk1w1Given = TRUE;
            break;
        case  B3SOI_MOD_K1W2 :
            mod->B3SOIk1w2 = value->rValue;
            mod->B3SOIk1w2Given = TRUE;
            break;
        case  B3SOI_MOD_KETAS :
            mod->B3SOIketas = value->rValue;
            mod->B3SOIketasGiven = TRUE;
            break;
        case  B3SOI_MOD_DWBC :
            mod->B3SOIdwbc = value->rValue;
            mod->B3SOIdwbcGiven = TRUE;
            break;
        case  B3SOI_MOD_BETA0 :
            mod->B3SOIbeta0 = value->rValue;
            mod->B3SOIbeta0Given = TRUE;
            break;
        case  B3SOI_MOD_BETA1 :
            mod->B3SOIbeta1 = value->rValue;
            mod->B3SOIbeta1Given = TRUE;
            break;
        case  B3SOI_MOD_BETA2 :
            mod->B3SOIbeta2 = value->rValue;
            mod->B3SOIbeta2Given = TRUE;
            break;
        case  B3SOI_MOD_VDSATII0 :
            mod->B3SOIvdsatii0 = value->rValue;
            mod->B3SOIvdsatii0Given = TRUE;
            break;
        case  B3SOI_MOD_TII :
            mod->B3SOItii = value->rValue;
            mod->B3SOItiiGiven = TRUE;
            break;
        case  B3SOI_MOD_LII :
            mod->B3SOIlii = value->rValue;
            mod->B3SOIliiGiven = TRUE;
            break;
        case  B3SOI_MOD_SII0 :
            mod->B3SOIsii0 = value->rValue;
            mod->B3SOIsii0Given = TRUE;
            break;
        case  B3SOI_MOD_SII1 :
            mod->B3SOIsii1 = value->rValue;
            mod->B3SOIsii1Given = TRUE;
            break;
        case  B3SOI_MOD_SII2 :
            mod->B3SOIsii2 = value->rValue;
            mod->B3SOIsii2Given = TRUE;
            break;
        case  B3SOI_MOD_SIID :
            mod->B3SOIsiid = value->rValue;
            mod->B3SOIsiidGiven = TRUE;
            break;
        case  B3SOI_MOD_FBJTII :
            mod->B3SOIfbjtii = value->rValue;
            mod->B3SOIfbjtiiGiven = TRUE;
            break;
        case  B3SOI_MOD_ESATII :
            mod->B3SOIesatii = value->rValue;
            mod->B3SOIesatiiGiven = TRUE;
            break;
        case  B3SOI_MOD_NTUN :
            mod->B3SOIntun = value->rValue;
            mod->B3SOIntunGiven = TRUE;
            break;
        case  B3SOI_MOD_NDIO :
            mod->B3SOIndio = value->rValue;
            mod->B3SOIndioGiven = TRUE;
            break;
        case  B3SOI_MOD_NRECF0 :
            mod->B3SOInrecf0 = value->rValue;
            mod->B3SOInrecf0Given = TRUE;
            break;
        case  B3SOI_MOD_NRECR0 :
            mod->B3SOInrecr0 = value->rValue;
            mod->B3SOInrecr0Given = TRUE;
            break;
        case  B3SOI_MOD_ISBJT :
            mod->B3SOIisbjt = value->rValue;
            mod->B3SOIisbjtGiven = TRUE;
            break;
        case  B3SOI_MOD_ISDIF :
            mod->B3SOIisdif = value->rValue;
            mod->B3SOIisdifGiven = TRUE;
            break;
        case  B3SOI_MOD_ISREC :
            mod->B3SOIisrec = value->rValue;
            mod->B3SOIisrecGiven = TRUE;
            break;
        case  B3SOI_MOD_ISTUN :
            mod->B3SOIistun = value->rValue;
            mod->B3SOIistunGiven = TRUE;
            break;
        case  B3SOI_MOD_LN :
            mod->B3SOIln = value->rValue;
            mod->B3SOIlnGiven = TRUE;
            break;
        case  B3SOI_MOD_VREC0 :
            mod->B3SOIvrec0 = value->rValue;
            mod->B3SOIvrec0Given = TRUE;
            break;
        case  B3SOI_MOD_VTUN0 :
            mod->B3SOIvtun0 = value->rValue;
            mod->B3SOIvtun0Given = TRUE;
            break;
        case  B3SOI_MOD_NBJT :
            mod->B3SOInbjt = value->rValue;
            mod->B3SOInbjtGiven = TRUE;
            break;
        case  B3SOI_MOD_LBJT0 :
            mod->B3SOIlbjt0 = value->rValue;
            mod->B3SOIlbjt0Given = TRUE;
            break;
        case  B3SOI_MOD_LDIF0 :
            mod->B3SOIldif0 = value->rValue;
            mod->B3SOIldif0Given = TRUE;
            break;
        case  B3SOI_MOD_VABJT :
            mod->B3SOIvabjt = value->rValue;
            mod->B3SOIvabjtGiven = TRUE;
            break;
        case  B3SOI_MOD_AELY :
            mod->B3SOIaely = value->rValue;
            mod->B3SOIaelyGiven = TRUE;
            break;
        case  B3SOI_MOD_AHLI :
            mod->B3SOIahli = value->rValue;
            mod->B3SOIahliGiven = TRUE;
            break;
        case  B3SOI_MOD_NDIF :
            mod->B3SOIndif = value->rValue;
            mod->B3SOIndifGiven = TRUE;
            break;
        case  B3SOI_MOD_NTRECF :
            mod->B3SOIntrecf = value->rValue;
            mod->B3SOIntrecfGiven = TRUE;
            break;
        case  B3SOI_MOD_NTRECR :
            mod->B3SOIntrecr = value->rValue;
            mod->B3SOIntrecrGiven = TRUE;
            break;
        case  B3SOI_MOD_DLCB :
            mod->B3SOIdlcb = value->rValue;
            mod->B3SOIdlcbGiven = TRUE;
            break;
        case  B3SOI_MOD_FBODY :
            mod->B3SOIfbody = value->rValue;
            mod->B3SOIfbodyGiven = TRUE;
            break;
        case  B3SOI_MOD_TCJSWG :
            mod->B3SOItcjswg = value->rValue;
            mod->B3SOItcjswgGiven = TRUE;
            break;
        case  B3SOI_MOD_TPBSWG :
            mod->B3SOItpbswg = value->rValue;
            mod->B3SOItpbswgGiven = TRUE;
            break;
        case  B3SOI_MOD_ACDE :
            mod->B3SOIacde = value->rValue;
            mod->B3SOIacdeGiven = TRUE;
            break;
        case  B3SOI_MOD_MOIN :
            mod->B3SOImoin = value->rValue;
            mod->B3SOImoinGiven = TRUE;
            break;
        case  B3SOI_MOD_DELVT :
            mod->B3SOIdelvt = value->rValue;
            mod->B3SOIdelvtGiven = TRUE;
            break;
        case  B3SOI_MOD_KB1 :
            mod->B3SOIkb1 = value->rValue;
            mod->B3SOIkb1Given = TRUE;
            break;
        case  B3SOI_MOD_DLBG :
            mod->B3SOIdlbg = value->rValue;
            mod->B3SOIdlbgGiven = TRUE;
            break;


        case  B3SOI_MOD_NMOS  :
            if(value->iValue) {
                mod->B3SOItype = 1;
                mod->B3SOItypeGiven = TRUE;
            }
            break;
        case  B3SOI_MOD_PMOS  :
            if(value->iValue) {
                mod->B3SOItype = - 1;
                mod->B3SOItypeGiven = TRUE;
            }
            break;
        default:
            return(E_BADPARM);
    }
    return(OK);
}


