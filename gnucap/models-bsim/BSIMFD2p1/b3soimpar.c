/* $Id: BSIMFD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soimpar.c, FD2.1 $
 * Revision 2.1  99/9/27 Pin Su 
 * BSIMFD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soimpar.c, FD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: Weidong Liu and Pin Su         Feb 1999
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soimpar.c          98/5/01
Modified by Wei Jin 99/9/27
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
        case  B3SOI_MOD_BETA0 :
            mod->B3SOIbeta0 = value->rValue;
            mod->B3SOIbeta0Given = TRUE;
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
        case  B3SOI_MOD_KB1 :
            mod->B3SOIkb1 = value->rValue;
            mod->B3SOIkb1Given = TRUE;
            break;
        case  B3SOI_MOD_KB3 :
            mod->B3SOIkb3 = value->rValue;
            mod->B3SOIkb3Given = TRUE;
            break;
        case  B3SOI_MOD_DVBD0 :
            mod->B3SOIdvbd0 = value->rValue;
            mod->B3SOIdvbd0Given = TRUE;
            break;
        case  B3SOI_MOD_DVBD1 :
            mod->B3SOIdvbd1 = value->rValue;
            mod->B3SOIdvbd1Given = TRUE;
            break;
        case  B3SOI_MOD_DELP :
            mod->B3SOIdelp = value->rValue;
            mod->B3SOIdelpGiven = TRUE;
            break;
        case  B3SOI_MOD_VBSA :
            mod->B3SOIvbsa = value->rValue;
            mod->B3SOIvbsaGiven = TRUE;
            break;
        case  B3SOI_MOD_RBODY :
            mod->B3SOIrbody = value->rValue;
            mod->B3SOIrbodyGiven = TRUE;
            break;
        case  B3SOI_MOD_RBSH :
            mod->B3SOIrbsh = value->rValue;
            mod->B3SOIrbshGiven = TRUE;
            break;
        case  B3SOI_MOD_ADICE0 :
            mod->B3SOIadice0 = value->rValue;
            mod->B3SOIadice0Given = TRUE;
            break;
        case  B3SOI_MOD_ABP :
            mod->B3SOIabp = value->rValue;
            mod->B3SOIabpGiven = TRUE;
            break;
        case  B3SOI_MOD_MXC :
            mod->B3SOImxc = value->rValue;
            mod->B3SOImxcGiven = TRUE;
            break;
        case  B3SOI_MOD_RTH0 :
            mod->B3SOIrth0 = value->rValue;
            mod->B3SOIrth0Given = TRUE;
            break;
        case  B3SOI_MOD_CTH0 :
            mod->B3SOIcth0 = value->rValue;
            mod->B3SOIcth0Given = TRUE;
            break;
        case  B3SOI_MOD_AII :
            mod->B3SOIaii = value->rValue;
            mod->B3SOIaiiGiven = TRUE;
            break;
        case  B3SOI_MOD_BII :
            mod->B3SOIbii = value->rValue;
            mod->B3SOIbiiGiven = TRUE;
            break;
        case  B3SOI_MOD_CII :
            mod->B3SOIcii = value->rValue;
            mod->B3SOIciiGiven = TRUE;
            break;
        case  B3SOI_MOD_DII :
            mod->B3SOIdii = value->rValue;
            mod->B3SOIdiiGiven = TRUE;
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
        case  B3SOI_MOD_NTUN :
            mod->B3SOIntun = value->rValue;
            mod->B3SOIntunGiven = TRUE;
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
        case  B3SOI_MOD_EDL :
            mod->B3SOIedl = value->rValue;
            mod->B3SOIedlGiven = TRUE;
            break;
        case  B3SOI_MOD_KBJT1 :
            mod->B3SOIkbjt1 = value->rValue;
            mod->B3SOIkbjt1Given = TRUE;
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

/* Added for binning - START */
        /* Length Dependence */
        case B3SOI_MOD_LNPEAK:
            mod->B3SOIlnpeak = value->rValue;
            mod->B3SOIlnpeakGiven = TRUE;
            break;
        case B3SOI_MOD_LNSUB:
            mod->B3SOIlnsub = value->rValue;
            mod->B3SOIlnsubGiven = TRUE;
            break;
        case B3SOI_MOD_LNGATE:
            mod->B3SOIlngate = value->rValue;
            mod->B3SOIlngateGiven = TRUE;
            break;
        case B3SOI_MOD_LVTH0:
            mod->B3SOIlvth0 = value->rValue;
            mod->B3SOIlvth0Given = TRUE;
            break;
        case  B3SOI_MOD_LK1:
            mod->B3SOIlk1 = value->rValue;
            mod->B3SOIlk1Given = TRUE;
            break;
        case  B3SOI_MOD_LK2:
            mod->B3SOIlk2 = value->rValue;
            mod->B3SOIlk2Given = TRUE;
            break;
        case  B3SOI_MOD_LK3:
            mod->B3SOIlk3 = value->rValue;
            mod->B3SOIlk3Given = TRUE;
            break;
        case  B3SOI_MOD_LK3B:
            mod->B3SOIlk3b = value->rValue;
            mod->B3SOIlk3bGiven = TRUE;
            break;
        case  B3SOI_MOD_LVBSA:
            mod->B3SOIlvbsa = value->rValue;
            mod->B3SOIlvbsaGiven = TRUE;
            break;
        case  B3SOI_MOD_LDELP:
            mod->B3SOIldelp = value->rValue;
            mod->B3SOIldelpGiven = TRUE;
            break;
        case  B3SOI_MOD_LKB1 :
            mod->B3SOIlkb1 = value->rValue;
            mod->B3SOIlkb1Given = TRUE;
            break;
        case  B3SOI_MOD_LKB3 :
            mod->B3SOIlkb3 = value->rValue;
            mod->B3SOIlkb3Given = TRUE;
            break;
        case  B3SOI_MOD_LDVBD0 :
            mod->B3SOIldvbd0 = value->rValue;
            mod->B3SOIldvbd0Given = TRUE;
            break;
        case  B3SOI_MOD_LDVBD1 :
            mod->B3SOIldvbd1 = value->rValue;
            mod->B3SOIldvbd1Given = TRUE;
            break;
        case  B3SOI_MOD_LW0:
            mod->B3SOIlw0 = value->rValue;
            mod->B3SOIlw0Given = TRUE;
            break;
        case  B3SOI_MOD_LNLX:
            mod->B3SOIlnlx = value->rValue;
            mod->B3SOIlnlxGiven = TRUE;
            break;
        case  B3SOI_MOD_LDVT0:               
            mod->B3SOIldvt0 = value->rValue;
            mod->B3SOIldvt0Given = TRUE;
            break;
        case  B3SOI_MOD_LDVT1:             
            mod->B3SOIldvt1 = value->rValue;
            mod->B3SOIldvt1Given = TRUE;
            break;
        case  B3SOI_MOD_LDVT2:             
            mod->B3SOIldvt2 = value->rValue;
            mod->B3SOIldvt2Given = TRUE;
            break;
        case  B3SOI_MOD_LDVT0W:               
            mod->B3SOIldvt0w = value->rValue;
            mod->B3SOIldvt0wGiven = TRUE;
            break;
        case  B3SOI_MOD_LDVT1W:             
            mod->B3SOIldvt1w = value->rValue;
            mod->B3SOIldvt1wGiven = TRUE;
            break;
        case  B3SOI_MOD_LDVT2W:             
            mod->B3SOIldvt2w = value->rValue;
            mod->B3SOIldvt2wGiven = TRUE;
            break;
        case  B3SOI_MOD_LU0 :
            mod->B3SOIlu0 = value->rValue;
            mod->B3SOIlu0Given = TRUE;
            break;
        case B3SOI_MOD_LUA:
            mod->B3SOIlua = value->rValue;
            mod->B3SOIluaGiven = TRUE;
            break;
        case B3SOI_MOD_LUB:
            mod->B3SOIlub = value->rValue;
            mod->B3SOIlubGiven = TRUE;
            break;
        case B3SOI_MOD_LUC:
            mod->B3SOIluc = value->rValue;
            mod->B3SOIlucGiven = TRUE;
            break;
        case B3SOI_MOD_LVSAT:
            mod->B3SOIlvsat = value->rValue;
            mod->B3SOIlvsatGiven = TRUE;
            break;
        case B3SOI_MOD_LA0:
            mod->B3SOIla0 = value->rValue;
            mod->B3SOIla0Given = TRUE;
            break;
        case B3SOI_MOD_LAGS:
            mod->B3SOIlags= value->rValue;
            mod->B3SOIlagsGiven = TRUE;
            break;
        case  B3SOI_MOD_LB0 :
            mod->B3SOIlb0 = value->rValue;
            mod->B3SOIlb0Given = TRUE;
            break;
        case  B3SOI_MOD_LB1 :
            mod->B3SOIlb1 = value->rValue;
            mod->B3SOIlb1Given = TRUE;
            break;
        case B3SOI_MOD_LKETA:
            mod->B3SOIlketa = value->rValue;
            mod->B3SOIlketaGiven = TRUE;
            break;    
        case B3SOI_MOD_LABP:
            mod->B3SOIlabp = value->rValue;
            mod->B3SOIlabpGiven = TRUE;
            break;    
        case B3SOI_MOD_LMXC:
            mod->B3SOIlmxc = value->rValue;
            mod->B3SOIlmxcGiven = TRUE;
            break;    
        case B3SOI_MOD_LADICE0:
            mod->B3SOIladice0 = value->rValue;
            mod->B3SOIladice0Given = TRUE;
            break;    
        case B3SOI_MOD_LA1:
            mod->B3SOIla1 = value->rValue;
            mod->B3SOIla1Given = TRUE;
            break;
        case B3SOI_MOD_LA2:
            mod->B3SOIla2 = value->rValue;
            mod->B3SOIla2Given = TRUE;
            break;
        case B3SOI_MOD_LRDSW:
            mod->B3SOIlrdsw = value->rValue;
            mod->B3SOIlrdswGiven = TRUE;
            break;                     
        case B3SOI_MOD_LPRWB:
            mod->B3SOIlprwb = value->rValue;
            mod->B3SOIlprwbGiven = TRUE;
            break;                     
        case B3SOI_MOD_LPRWG:
            mod->B3SOIlprwg = value->rValue;
            mod->B3SOIlprwgGiven = TRUE;
            break;                     
        case  B3SOI_MOD_LWR :
            mod->B3SOIlwr = value->rValue;
            mod->B3SOIlwrGiven = TRUE;
            break;
        case  B3SOI_MOD_LNFACTOR :
            mod->B3SOIlnfactor = value->rValue;
            mod->B3SOIlnfactorGiven = TRUE;
            break;
        case  B3SOI_MOD_LDWG :
            mod->B3SOIldwg = value->rValue;
            mod->B3SOIldwgGiven = TRUE;
            break;
        case  B3SOI_MOD_LDWB :
            mod->B3SOIldwb = value->rValue;
            mod->B3SOIldwbGiven = TRUE;
            break;
        case B3SOI_MOD_LVOFF:
            mod->B3SOIlvoff = value->rValue;
            mod->B3SOIlvoffGiven = TRUE;
            break;
        case B3SOI_MOD_LETA0:
            mod->B3SOIleta0 = value->rValue;
            mod->B3SOIleta0Given = TRUE;
            break;                 
        case B3SOI_MOD_LETAB:
            mod->B3SOIletab = value->rValue;
            mod->B3SOIletabGiven = TRUE;
            break;                 
        case  B3SOI_MOD_LDSUB:             
            mod->B3SOIldsub = value->rValue;
            mod->B3SOIldsubGiven = TRUE;
            break;
        case  B3SOI_MOD_LCIT :
            mod->B3SOIlcit = value->rValue;
            mod->B3SOIlcitGiven = TRUE;
            break;
        case  B3SOI_MOD_LCDSC :
            mod->B3SOIlcdsc = value->rValue;
            mod->B3SOIlcdscGiven = TRUE;
            break;
        case  B3SOI_MOD_LCDSCB :
            mod->B3SOIlcdscb = value->rValue;
            mod->B3SOIlcdscbGiven = TRUE;
            break;
        case  B3SOI_MOD_LCDSCD :
            mod->B3SOIlcdscd = value->rValue;
            mod->B3SOIlcdscdGiven = TRUE;
            break;
        case B3SOI_MOD_LPCLM:
            mod->B3SOIlpclm = value->rValue;
            mod->B3SOIlpclmGiven = TRUE;
            break;                 
        case B3SOI_MOD_LPDIBL1:
            mod->B3SOIlpdibl1 = value->rValue;
            mod->B3SOIlpdibl1Given = TRUE;
            break;                 
        case B3SOI_MOD_LPDIBL2:
            mod->B3SOIlpdibl2 = value->rValue;
            mod->B3SOIlpdibl2Given = TRUE;
            break;                 
        case B3SOI_MOD_LPDIBLB:
            mod->B3SOIlpdiblb = value->rValue;
            mod->B3SOIlpdiblbGiven = TRUE;
            break;                 
        case  B3SOI_MOD_LDROUT:             
            mod->B3SOIldrout = value->rValue;
            mod->B3SOIldroutGiven = TRUE;
            break;
        case B3SOI_MOD_LPVAG:
            mod->B3SOIlpvag = value->rValue;
            mod->B3SOIlpvagGiven = TRUE;
            break;                 
        case  B3SOI_MOD_LDELTA :
            mod->B3SOIldelta = value->rValue;
            mod->B3SOIldeltaGiven = TRUE;
            break;
        case  B3SOI_MOD_LAII :
            mod->B3SOIlaii = value->rValue;
            mod->B3SOIlaiiGiven = TRUE;
            break;
        case  B3SOI_MOD_LBII :
            mod->B3SOIlbii = value->rValue;
            mod->B3SOIlbiiGiven = TRUE;
            break;
        case  B3SOI_MOD_LCII :
            mod->B3SOIlcii = value->rValue;
            mod->B3SOIlciiGiven = TRUE;
            break;
        case  B3SOI_MOD_LDII :
            mod->B3SOIldii = value->rValue;
            mod->B3SOIldiiGiven = TRUE;
            break;
        case  B3SOI_MOD_LALPHA0 :
            mod->B3SOIlalpha0 = value->rValue;
            mod->B3SOIlalpha0Given = TRUE;
            break;
        case  B3SOI_MOD_LALPHA1 :
            mod->B3SOIlalpha1 = value->rValue;
            mod->B3SOIlalpha1Given = TRUE;
            break;
        case  B3SOI_MOD_LBETA0 :
            mod->B3SOIlbeta0 = value->rValue;
            mod->B3SOIlbeta0Given = TRUE;
            break;
        case  B3SOI_MOD_LAGIDL :
            mod->B3SOIlagidl = value->rValue;
            mod->B3SOIlagidlGiven = TRUE;
            break;
        case  B3SOI_MOD_LBGIDL :
            mod->B3SOIlbgidl = value->rValue;
            mod->B3SOIlbgidlGiven = TRUE;
            break;
        case  B3SOI_MOD_LNGIDL :
            mod->B3SOIlngidl = value->rValue;
            mod->B3SOIlngidlGiven = TRUE;
            break;
        case  B3SOI_MOD_LNTUN :
            mod->B3SOIlntun = value->rValue;
            mod->B3SOIlntunGiven = TRUE;
            break;
        case  B3SOI_MOD_LNDIODE :
            mod->B3SOIlndiode = value->rValue;
            mod->B3SOIlndiodeGiven = TRUE;
            break;
        case  B3SOI_MOD_LISBJT :
            mod->B3SOIlisbjt = value->rValue;
            mod->B3SOIlisbjtGiven = TRUE;
            break;
        case  B3SOI_MOD_LISDIF :
            mod->B3SOIlisdif = value->rValue;
            mod->B3SOIlisdifGiven = TRUE;
            break;
        case  B3SOI_MOD_LISREC :
            mod->B3SOIlisrec = value->rValue;
            mod->B3SOIlisrecGiven = TRUE;
            break;
        case  B3SOI_MOD_LISTUN :
            mod->B3SOIlistun = value->rValue;
            mod->B3SOIlistunGiven = TRUE;
            break;
        case  B3SOI_MOD_LEDL :
            mod->B3SOIledl = value->rValue;
            mod->B3SOIledlGiven = TRUE;
            break;
        case  B3SOI_MOD_LKBJT1 :
            mod->B3SOIlkbjt1 = value->rValue;
            mod->B3SOIlkbjt1Given = TRUE;
            break;
	/* CV Model */
        case  B3SOI_MOD_LVSDFB :
            mod->B3SOIlvsdfb = value->rValue;
            mod->B3SOIlvsdfbGiven = TRUE;
            break;
        case  B3SOI_MOD_LVSDTH :
            mod->B3SOIlvsdth = value->rValue;
            mod->B3SOIlvsdthGiven = TRUE;
            break;
        /* Width Dependence */
        case B3SOI_MOD_WNPEAK:
            mod->B3SOIwnpeak = value->rValue;
            mod->B3SOIwnpeakGiven = TRUE;
            break;
        case B3SOI_MOD_WNSUB:
            mod->B3SOIwnsub = value->rValue;
            mod->B3SOIwnsubGiven = TRUE;
            break;
        case B3SOI_MOD_WNGATE:
            mod->B3SOIwngate = value->rValue;
            mod->B3SOIwngateGiven = TRUE;
            break;
        case B3SOI_MOD_WVTH0:
            mod->B3SOIwvth0 = value->rValue;
            mod->B3SOIwvth0Given = TRUE;
            break;
        case  B3SOI_MOD_WK1:
            mod->B3SOIwk1 = value->rValue;
            mod->B3SOIwk1Given = TRUE;
            break;
        case  B3SOI_MOD_WK2:
            mod->B3SOIwk2 = value->rValue;
            mod->B3SOIwk2Given = TRUE;
            break;
        case  B3SOI_MOD_WK3:
            mod->B3SOIwk3 = value->rValue;
            mod->B3SOIwk3Given = TRUE;
            break;
        case  B3SOI_MOD_WK3B:
            mod->B3SOIwk3b = value->rValue;
            mod->B3SOIwk3bGiven = TRUE;
            break;
        case  B3SOI_MOD_WVBSA:
            mod->B3SOIwvbsa = value->rValue;
            mod->B3SOIwvbsaGiven = TRUE;
            break;
        case  B3SOI_MOD_WDELP:
            mod->B3SOIwdelp = value->rValue;
            mod->B3SOIwdelpGiven = TRUE;
            break;
        case  B3SOI_MOD_WKB1 :
            mod->B3SOIwkb1 = value->rValue;
            mod->B3SOIwkb1Given = TRUE;
            break;
        case  B3SOI_MOD_WKB3 :
            mod->B3SOIwkb3 = value->rValue;
            mod->B3SOIwkb3Given = TRUE;
            break;
        case  B3SOI_MOD_WDVBD0 :
            mod->B3SOIwdvbd0 = value->rValue;
            mod->B3SOIwdvbd0Given = TRUE;
            break;
        case  B3SOI_MOD_WDVBD1 :
            mod->B3SOIwdvbd1 = value->rValue;
            mod->B3SOIwdvbd1Given = TRUE;
            break;
        case  B3SOI_MOD_WW0:
            mod->B3SOIww0 = value->rValue;
            mod->B3SOIww0Given = TRUE;
            break;
        case  B3SOI_MOD_WNLX:
            mod->B3SOIwnlx = value->rValue;
            mod->B3SOIwnlxGiven = TRUE;
            break;
        case  B3SOI_MOD_WDVT0:               
            mod->B3SOIwdvt0 = value->rValue;
            mod->B3SOIwdvt0Given = TRUE;
            break;
        case  B3SOI_MOD_WDVT1:             
            mod->B3SOIwdvt1 = value->rValue;
            mod->B3SOIwdvt1Given = TRUE;
            break;
        case  B3SOI_MOD_WDVT2:             
            mod->B3SOIwdvt2 = value->rValue;
            mod->B3SOIwdvt2Given = TRUE;
            break;
        case  B3SOI_MOD_WDVT0W:               
            mod->B3SOIwdvt0w = value->rValue;
            mod->B3SOIwdvt0wGiven = TRUE;
            break;
        case  B3SOI_MOD_WDVT1W:             
            mod->B3SOIwdvt1w = value->rValue;
            mod->B3SOIwdvt1wGiven = TRUE;
            break;
        case  B3SOI_MOD_WDVT2W:             
            mod->B3SOIwdvt2w = value->rValue;
            mod->B3SOIwdvt2wGiven = TRUE;
            break;
        case  B3SOI_MOD_WU0 :
            mod->B3SOIwu0 = value->rValue;
            mod->B3SOIwu0Given = TRUE;
            break;
        case B3SOI_MOD_WUA:
            mod->B3SOIwua = value->rValue;
            mod->B3SOIwuaGiven = TRUE;
            break;
        case B3SOI_MOD_WUB:
            mod->B3SOIwub = value->rValue;
            mod->B3SOIwubGiven = TRUE;
            break;
        case B3SOI_MOD_WUC:
            mod->B3SOIwuc = value->rValue;
            mod->B3SOIwucGiven = TRUE;
            break;
        case B3SOI_MOD_WVSAT:
            mod->B3SOIwvsat = value->rValue;
            mod->B3SOIwvsatGiven = TRUE;
            break;
        case B3SOI_MOD_WA0:
            mod->B3SOIwa0 = value->rValue;
            mod->B3SOIwa0Given = TRUE;
            break;
        case B3SOI_MOD_WAGS:
            mod->B3SOIwags= value->rValue;
            mod->B3SOIwagsGiven = TRUE;
            break;
        case  B3SOI_MOD_WB0 :
            mod->B3SOIwb0 = value->rValue;
            mod->B3SOIwb0Given = TRUE;
            break;
        case  B3SOI_MOD_WB1 :
            mod->B3SOIwb1 = value->rValue;
            mod->B3SOIwb1Given = TRUE;
            break;
        case B3SOI_MOD_WKETA:
            mod->B3SOIwketa = value->rValue;
            mod->B3SOIwketaGiven = TRUE;
            break;    
        case B3SOI_MOD_WABP:
            mod->B3SOIwabp = value->rValue;
            mod->B3SOIwabpGiven = TRUE;
            break;    
        case B3SOI_MOD_WMXC:
            mod->B3SOIwmxc = value->rValue;
            mod->B3SOIwmxcGiven = TRUE;
            break;    
        case B3SOI_MOD_WADICE0:
            mod->B3SOIwadice0 = value->rValue;
            mod->B3SOIwadice0Given = TRUE;
            break;    
        case B3SOI_MOD_WA1:
            mod->B3SOIwa1 = value->rValue;
            mod->B3SOIwa1Given = TRUE;
            break;
        case B3SOI_MOD_WA2:
            mod->B3SOIwa2 = value->rValue;
            mod->B3SOIwa2Given = TRUE;
            break;
        case B3SOI_MOD_WRDSW:
            mod->B3SOIwrdsw = value->rValue;
            mod->B3SOIwrdswGiven = TRUE;
            break;                     
        case B3SOI_MOD_WPRWB:
            mod->B3SOIwprwb = value->rValue;
            mod->B3SOIwprwbGiven = TRUE;
            break;                     
        case B3SOI_MOD_WPRWG:
            mod->B3SOIwprwg = value->rValue;
            mod->B3SOIwprwgGiven = TRUE;
            break;                     
        case  B3SOI_MOD_WWR :
            mod->B3SOIwwr = value->rValue;
            mod->B3SOIwwrGiven = TRUE;
            break;
        case  B3SOI_MOD_WNFACTOR :
            mod->B3SOIwnfactor = value->rValue;
            mod->B3SOIwnfactorGiven = TRUE;
            break;
        case  B3SOI_MOD_WDWG :
            mod->B3SOIwdwg = value->rValue;
            mod->B3SOIwdwgGiven = TRUE;
            break;
        case  B3SOI_MOD_WDWB :
            mod->B3SOIwdwb = value->rValue;
            mod->B3SOIwdwbGiven = TRUE;
            break;
        case B3SOI_MOD_WVOFF:
            mod->B3SOIwvoff = value->rValue;
            mod->B3SOIwvoffGiven = TRUE;
            break;
        case B3SOI_MOD_WETA0:
            mod->B3SOIweta0 = value->rValue;
            mod->B3SOIweta0Given = TRUE;
            break;                 
        case B3SOI_MOD_WETAB:
            mod->B3SOIwetab = value->rValue;
            mod->B3SOIwetabGiven = TRUE;
            break;                 
        case  B3SOI_MOD_WDSUB:             
            mod->B3SOIwdsub = value->rValue;
            mod->B3SOIwdsubGiven = TRUE;
            break;
        case  B3SOI_MOD_WCIT :
            mod->B3SOIwcit = value->rValue;
            mod->B3SOIwcitGiven = TRUE;
            break;
        case  B3SOI_MOD_WCDSC :
            mod->B3SOIwcdsc = value->rValue;
            mod->B3SOIwcdscGiven = TRUE;
            break;
        case  B3SOI_MOD_WCDSCB :
            mod->B3SOIwcdscb = value->rValue;
            mod->B3SOIwcdscbGiven = TRUE;
            break;
        case  B3SOI_MOD_WCDSCD :
            mod->B3SOIwcdscd = value->rValue;
            mod->B3SOIwcdscdGiven = TRUE;
            break;
        case B3SOI_MOD_WPCLM:
            mod->B3SOIwpclm = value->rValue;
            mod->B3SOIwpclmGiven = TRUE;
            break;                 
        case B3SOI_MOD_WPDIBL1:
            mod->B3SOIwpdibl1 = value->rValue;
            mod->B3SOIwpdibl1Given = TRUE;
            break;                 
        case B3SOI_MOD_WPDIBL2:
            mod->B3SOIwpdibl2 = value->rValue;
            mod->B3SOIwpdibl2Given = TRUE;
            break;                 
        case B3SOI_MOD_WPDIBLB:
            mod->B3SOIwpdiblb = value->rValue;
            mod->B3SOIwpdiblbGiven = TRUE;
            break;                 
        case  B3SOI_MOD_WDROUT:             
            mod->B3SOIwdrout = value->rValue;
            mod->B3SOIwdroutGiven = TRUE;
            break;
        case B3SOI_MOD_WPVAG:
            mod->B3SOIwpvag = value->rValue;
            mod->B3SOIwpvagGiven = TRUE;
            break;                 
        case  B3SOI_MOD_WDELTA :
            mod->B3SOIwdelta = value->rValue;
            mod->B3SOIwdeltaGiven = TRUE;
            break;
        case  B3SOI_MOD_WAII :
            mod->B3SOIwaii = value->rValue;
            mod->B3SOIwaiiGiven = TRUE;
            break;
        case  B3SOI_MOD_WBII :
            mod->B3SOIwbii = value->rValue;
            mod->B3SOIwbiiGiven = TRUE;
            break;
        case  B3SOI_MOD_WCII :
            mod->B3SOIwcii = value->rValue;
            mod->B3SOIwciiGiven = TRUE;
            break;
        case  B3SOI_MOD_WDII :
            mod->B3SOIwdii = value->rValue;
            mod->B3SOIwdiiGiven = TRUE;
            break;
        case  B3SOI_MOD_WALPHA0 :
            mod->B3SOIwalpha0 = value->rValue;
            mod->B3SOIwalpha0Given = TRUE;
            break;
        case  B3SOI_MOD_WALPHA1 :
            mod->B3SOIwalpha1 = value->rValue;
            mod->B3SOIwalpha1Given = TRUE;
            break;
        case  B3SOI_MOD_WBETA0 :
            mod->B3SOIwbeta0 = value->rValue;
            mod->B3SOIwbeta0Given = TRUE;
            break;
        case  B3SOI_MOD_WAGIDL :
            mod->B3SOIwagidl = value->rValue;
            mod->B3SOIwagidlGiven = TRUE;
            break;
        case  B3SOI_MOD_WBGIDL :
            mod->B3SOIwbgidl = value->rValue;
            mod->B3SOIwbgidlGiven = TRUE;
            break;
        case  B3SOI_MOD_WNGIDL :
            mod->B3SOIwngidl = value->rValue;
            mod->B3SOIwngidlGiven = TRUE;
            break;
        case  B3SOI_MOD_WNTUN :
            mod->B3SOIwntun = value->rValue;
            mod->B3SOIwntunGiven = TRUE;
            break;
        case  B3SOI_MOD_WNDIODE :
            mod->B3SOIwndiode = value->rValue;
            mod->B3SOIwndiodeGiven = TRUE;
            break;
        case  B3SOI_MOD_WISBJT :
            mod->B3SOIwisbjt = value->rValue;
            mod->B3SOIwisbjtGiven = TRUE;
            break;
        case  B3SOI_MOD_WISDIF :
            mod->B3SOIwisdif = value->rValue;
            mod->B3SOIwisdifGiven = TRUE;
            break;
        case  B3SOI_MOD_WISREC :
            mod->B3SOIwisrec = value->rValue;
            mod->B3SOIwisrecGiven = TRUE;
            break;
        case  B3SOI_MOD_WISTUN :
            mod->B3SOIwistun = value->rValue;
            mod->B3SOIwistunGiven = TRUE;
            break;
        case  B3SOI_MOD_WEDL :
            mod->B3SOIwedl = value->rValue;
            mod->B3SOIwedlGiven = TRUE;
            break;
        case  B3SOI_MOD_WKBJT1 :
            mod->B3SOIwkbjt1 = value->rValue;
            mod->B3SOIwkbjt1Given = TRUE;
            break;
	/* CV Model */
        case  B3SOI_MOD_WVSDFB :
            mod->B3SOIwvsdfb = value->rValue;
            mod->B3SOIwvsdfbGiven = TRUE;
            break;
        case  B3SOI_MOD_WVSDTH :
            mod->B3SOIwvsdth = value->rValue;
            mod->B3SOIwvsdthGiven = TRUE;
            break;
        /* Cross-term Dependence */
        case B3SOI_MOD_PNPEAK:
            mod->B3SOIpnpeak = value->rValue;
            mod->B3SOIpnpeakGiven = TRUE;
            break;
        case B3SOI_MOD_PNSUB:
            mod->B3SOIpnsub = value->rValue;
            mod->B3SOIpnsubGiven = TRUE;
            break;
        case B3SOI_MOD_PNGATE:
            mod->B3SOIpngate = value->rValue;
            mod->B3SOIpngateGiven = TRUE;
            break;
        case B3SOI_MOD_PVTH0:
            mod->B3SOIpvth0 = value->rValue;
            mod->B3SOIpvth0Given = TRUE;
            break;
        case  B3SOI_MOD_PK1:
            mod->B3SOIpk1 = value->rValue;
            mod->B3SOIpk1Given = TRUE;
            break;
        case  B3SOI_MOD_PK2:
            mod->B3SOIpk2 = value->rValue;
            mod->B3SOIpk2Given = TRUE;
            break;
        case  B3SOI_MOD_PK3:
            mod->B3SOIpk3 = value->rValue;
            mod->B3SOIpk3Given = TRUE;
            break;
        case  B3SOI_MOD_PK3B:
            mod->B3SOIpk3b = value->rValue;
            mod->B3SOIpk3bGiven = TRUE;
            break;
        case  B3SOI_MOD_PVBSA:
            mod->B3SOIpvbsa = value->rValue;
            mod->B3SOIpvbsaGiven = TRUE;
            break;
        case  B3SOI_MOD_PDELP:
            mod->B3SOIpdelp = value->rValue;
            mod->B3SOIpdelpGiven = TRUE;
            break;
        case  B3SOI_MOD_PKB1 :
            mod->B3SOIpkb1 = value->rValue;
            mod->B3SOIpkb1Given = TRUE;
            break;
        case  B3SOI_MOD_PKB3 :
            mod->B3SOIpkb3 = value->rValue;
            mod->B3SOIpkb3Given = TRUE;
            break;
        case  B3SOI_MOD_PDVBD0 :
            mod->B3SOIpdvbd0 = value->rValue;
            mod->B3SOIpdvbd0Given = TRUE;
            break;
        case  B3SOI_MOD_PDVBD1 :
            mod->B3SOIpdvbd1 = value->rValue;
            mod->B3SOIpdvbd1Given = TRUE;
            break;
        case  B3SOI_MOD_PW0:
            mod->B3SOIpw0 = value->rValue;
            mod->B3SOIpw0Given = TRUE;
            break;
        case  B3SOI_MOD_PNLX:
            mod->B3SOIpnlx = value->rValue;
            mod->B3SOIpnlxGiven = TRUE;
            break;
        case  B3SOI_MOD_PDVT0:               
            mod->B3SOIpdvt0 = value->rValue;
            mod->B3SOIpdvt0Given = TRUE;
            break;
        case  B3SOI_MOD_PDVT1:             
            mod->B3SOIpdvt1 = value->rValue;
            mod->B3SOIpdvt1Given = TRUE;
            break;
        case  B3SOI_MOD_PDVT2:             
            mod->B3SOIpdvt2 = value->rValue;
            mod->B3SOIpdvt2Given = TRUE;
            break;
        case  B3SOI_MOD_PDVT0W:               
            mod->B3SOIpdvt0w = value->rValue;
            mod->B3SOIpdvt0wGiven = TRUE;
            break;
        case  B3SOI_MOD_PDVT1W:             
            mod->B3SOIpdvt1w = value->rValue;
            mod->B3SOIpdvt1wGiven = TRUE;
            break;
        case  B3SOI_MOD_PDVT2W:             
            mod->B3SOIpdvt2w = value->rValue;
            mod->B3SOIpdvt2wGiven = TRUE;
            break;
        case  B3SOI_MOD_PU0 :
            mod->B3SOIpu0 = value->rValue;
            mod->B3SOIpu0Given = TRUE;
            break;
        case B3SOI_MOD_PUA:
            mod->B3SOIpua = value->rValue;
            mod->B3SOIpuaGiven = TRUE;
            break;
        case B3SOI_MOD_PUB:
            mod->B3SOIpub = value->rValue;
            mod->B3SOIpubGiven = TRUE;
            break;
        case B3SOI_MOD_PUC:
            mod->B3SOIpuc = value->rValue;
            mod->B3SOIpucGiven = TRUE;
            break;
        case B3SOI_MOD_PVSAT:
            mod->B3SOIpvsat = value->rValue;
            mod->B3SOIpvsatGiven = TRUE;
            break;
        case B3SOI_MOD_PA0:
            mod->B3SOIpa0 = value->rValue;
            mod->B3SOIpa0Given = TRUE;
            break;
        case B3SOI_MOD_PAGS:
            mod->B3SOIpags= value->rValue;
            mod->B3SOIpagsGiven = TRUE;
            break;
        case  B3SOI_MOD_PB0 :
            mod->B3SOIpb0 = value->rValue;
            mod->B3SOIpb0Given = TRUE;
            break;
        case  B3SOI_MOD_PB1 :
            mod->B3SOIpb1 = value->rValue;
            mod->B3SOIpb1Given = TRUE;
            break;
        case B3SOI_MOD_PKETA:
            mod->B3SOIpketa = value->rValue;
            mod->B3SOIpketaGiven = TRUE;
            break;    
        case B3SOI_MOD_PABP:
            mod->B3SOIpabp = value->rValue;
            mod->B3SOIpabpGiven = TRUE;
            break;    
        case B3SOI_MOD_PMXC:
            mod->B3SOIpmxc = value->rValue;
            mod->B3SOIpmxcGiven = TRUE;
            break;    
        case B3SOI_MOD_PADICE0:
            mod->B3SOIpadice0 = value->rValue;
            mod->B3SOIpadice0Given = TRUE;
            break;    
        case B3SOI_MOD_PA1:
            mod->B3SOIpa1 = value->rValue;
            mod->B3SOIpa1Given = TRUE;
            break;
        case B3SOI_MOD_PA2:
            mod->B3SOIpa2 = value->rValue;
            mod->B3SOIpa2Given = TRUE;
            break;
        case B3SOI_MOD_PRDSW:
            mod->B3SOIprdsw = value->rValue;
            mod->B3SOIprdswGiven = TRUE;
            break;                     
        case B3SOI_MOD_PPRWB:
            mod->B3SOIpprwb = value->rValue;
            mod->B3SOIpprwbGiven = TRUE;
            break;                     
        case B3SOI_MOD_PPRWG:
            mod->B3SOIpprwg = value->rValue;
            mod->B3SOIpprwgGiven = TRUE;
            break;                     
        case  B3SOI_MOD_PWR :
            mod->B3SOIpwr = value->rValue;
            mod->B3SOIpwrGiven = TRUE;
            break;
        case  B3SOI_MOD_PNFACTOR :
            mod->B3SOIpnfactor = value->rValue;
            mod->B3SOIpnfactorGiven = TRUE;
            break;
        case  B3SOI_MOD_PDWG :
            mod->B3SOIpdwg = value->rValue;
            mod->B3SOIpdwgGiven = TRUE;
            break;
        case  B3SOI_MOD_PDWB :
            mod->B3SOIpdwb = value->rValue;
            mod->B3SOIpdwbGiven = TRUE;
            break;
        case B3SOI_MOD_PVOFF:
            mod->B3SOIpvoff = value->rValue;
            mod->B3SOIpvoffGiven = TRUE;
            break;
        case B3SOI_MOD_PETA0:
            mod->B3SOIpeta0 = value->rValue;
            mod->B3SOIpeta0Given = TRUE;
            break;                 
        case B3SOI_MOD_PETAB:
            mod->B3SOIpetab = value->rValue;
            mod->B3SOIpetabGiven = TRUE;
            break;                 
        case  B3SOI_MOD_PDSUB:             
            mod->B3SOIpdsub = value->rValue;
            mod->B3SOIpdsubGiven = TRUE;
            break;
        case  B3SOI_MOD_PCIT :
            mod->B3SOIpcit = value->rValue;
            mod->B3SOIpcitGiven = TRUE;
            break;
        case  B3SOI_MOD_PCDSC :
            mod->B3SOIpcdsc = value->rValue;
            mod->B3SOIpcdscGiven = TRUE;
            break;
        case  B3SOI_MOD_PCDSCB :
            mod->B3SOIpcdscb = value->rValue;
            mod->B3SOIpcdscbGiven = TRUE;
            break;
        case  B3SOI_MOD_PCDSCD :
            mod->B3SOIpcdscd = value->rValue;
            mod->B3SOIpcdscdGiven = TRUE;
            break;
        case B3SOI_MOD_PPCLM:
            mod->B3SOIppclm = value->rValue;
            mod->B3SOIppclmGiven = TRUE;
            break;                 
        case B3SOI_MOD_PPDIBL1:
            mod->B3SOIppdibl1 = value->rValue;
            mod->B3SOIppdibl1Given = TRUE;
            break;                 
        case B3SOI_MOD_PPDIBL2:
            mod->B3SOIppdibl2 = value->rValue;
            mod->B3SOIppdibl2Given = TRUE;
            break;                 
        case B3SOI_MOD_PPDIBLB:
            mod->B3SOIppdiblb = value->rValue;
            mod->B3SOIppdiblbGiven = TRUE;
            break;                 
        case  B3SOI_MOD_PDROUT:             
            mod->B3SOIpdrout = value->rValue;
            mod->B3SOIpdroutGiven = TRUE;
            break;
        case B3SOI_MOD_PPVAG:
            mod->B3SOIppvag = value->rValue;
            mod->B3SOIppvagGiven = TRUE;
            break;                 
        case  B3SOI_MOD_PDELTA :
            mod->B3SOIpdelta = value->rValue;
            mod->B3SOIpdeltaGiven = TRUE;
            break;
        case  B3SOI_MOD_PAII :
            mod->B3SOIpaii = value->rValue;
            mod->B3SOIpaiiGiven = TRUE;
            break;
        case  B3SOI_MOD_PBII :
            mod->B3SOIpbii = value->rValue;
            mod->B3SOIpbiiGiven = TRUE;
            break;
        case  B3SOI_MOD_PCII :
            mod->B3SOIpcii = value->rValue;
            mod->B3SOIpciiGiven = TRUE;
            break;
        case  B3SOI_MOD_PDII :
            mod->B3SOIpdii = value->rValue;
            mod->B3SOIpdiiGiven = TRUE;
            break;
        case  B3SOI_MOD_PALPHA0 :
            mod->B3SOIpalpha0 = value->rValue;
            mod->B3SOIpalpha0Given = TRUE;
            break;
        case  B3SOI_MOD_PALPHA1 :
            mod->B3SOIpalpha1 = value->rValue;
            mod->B3SOIpalpha1Given = TRUE;
            break;
        case  B3SOI_MOD_PBETA0 :
            mod->B3SOIpbeta0 = value->rValue;
            mod->B3SOIpbeta0Given = TRUE;
            break;
        case  B3SOI_MOD_PAGIDL :
            mod->B3SOIpagidl = value->rValue;
            mod->B3SOIpagidlGiven = TRUE;
            break;
        case  B3SOI_MOD_PBGIDL :
            mod->B3SOIpbgidl = value->rValue;
            mod->B3SOIpbgidlGiven = TRUE;
            break;
        case  B3SOI_MOD_PNGIDL :
            mod->B3SOIpngidl = value->rValue;
            mod->B3SOIpngidlGiven = TRUE;
            break;
        case  B3SOI_MOD_PNTUN :
            mod->B3SOIpntun = value->rValue;
            mod->B3SOIpntunGiven = TRUE;
            break;
        case  B3SOI_MOD_PNDIODE :
            mod->B3SOIpndiode = value->rValue;
            mod->B3SOIpndiodeGiven = TRUE;
            break;
        case  B3SOI_MOD_PISBJT :
            mod->B3SOIpisbjt = value->rValue;
            mod->B3SOIpisbjtGiven = TRUE;
            break;
        case  B3SOI_MOD_PISDIF :
            mod->B3SOIpisdif = value->rValue;
            mod->B3SOIpisdifGiven = TRUE;
            break;
        case  B3SOI_MOD_PISREC :
            mod->B3SOIpisrec = value->rValue;
            mod->B3SOIpisrecGiven = TRUE;
            break;
        case  B3SOI_MOD_PISTUN :
            mod->B3SOIpistun = value->rValue;
            mod->B3SOIpistunGiven = TRUE;
            break;
        case  B3SOI_MOD_PEDL :
            mod->B3SOIpedl = value->rValue;
            mod->B3SOIpedlGiven = TRUE;
            break;
        case  B3SOI_MOD_PKBJT1 :
            mod->B3SOIpkbjt1 = value->rValue;
            mod->B3SOIpkbjt1Given = TRUE;
            break;
	/* CV Model */
        case  B3SOI_MOD_PVSDFB :
            mod->B3SOIpvsdfb = value->rValue;
            mod->B3SOIpvsdfbGiven = TRUE;
            break;
        case  B3SOI_MOD_PVSDTH :
            mod->B3SOIpvsdth = value->rValue;
            mod->B3SOIpvsdthGiven = TRUE;
            break;
/* Added for binning - END */

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


