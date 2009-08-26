/* $Id: BSIMFD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soiset.c, FD2.1 $
 * Revision 2.1  99/9/27 Pin Su 
 * BSIMFD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soiset.c, FD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: Weidong Liu and Pin Su         Feb 1999
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soiset.c          98/5/01
Modified by Pin Su, Wei Jin 99/9/27
**********/


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

#define MAX_EXP 5.834617425e14
#define MIN_EXP 1.713908431e-15
#define EXP_THRESHOLD 34.0
#define SMOOTHFACTOR 0.1
#define EPSOX 3.453133e-11
#define EPSSI 1.03594e-10
#define PI 3.141592654
#define Charge_q 1.60219e-19
#define Meter2Micron 1.0e6

int
B3SOIsetup(matrix,inModel,ckt,states)
register SMPmatrix *matrix;
register GENmodel *inModel;
register CKTcircuit *ckt;
int *states;
{
register B3SOImodel *model = (B3SOImodel*)inModel;
register B3SOIinstance *here;
int error;
CKTnode *tmp;

double tmp1, tmp2;
double nfb0, Cboxt;
int    itmp1;

    /*  loop through all the B3SOI device models */
    for( ; model != NULL; model = model->B3SOInextModel )
    {
/* Default value Processing for B3SOI MOSFET Models */

        if (!model->B3SOItypeGiven)
            model->B3SOItype = NMOS;     
        if (!model->B3SOImobModGiven) 
            model->B3SOImobMod = 1;
        if (!model->B3SOIbinUnitGiven) 
            model->B3SOIbinUnit = 1;
        if (!model->B3SOIparamChkGiven) 
            model->B3SOIparamChk = 0;
        if (!model->B3SOIcapModGiven) 
            model->B3SOIcapMod = 2;
        if (!model->B3SOInoiModGiven) 
            model->B3SOInoiMod = 1;
        if (!model->B3SOIshModGiven) 
            model->B3SOIshMod = 0;
        if (!model->B3SOIversionGiven) 
            model->B3SOIversion = 2.0;
        if (!model->B3SOItoxGiven)
            model->B3SOItox = 100.0e-10;
        model->B3SOIcox = 3.453133e-11 / model->B3SOItox;

        if (!model->B3SOIcdscGiven)
	    model->B3SOIcdsc = 2.4e-4;   /* unit Q/V/m^2  */
        if (!model->B3SOIcdscbGiven)
	    model->B3SOIcdscb = 0.0;   /* unit Q/V/m^2  */    
	    if (!model->B3SOIcdscdGiven)
	    model->B3SOIcdscd = 0.0;   /* unit Q/V/m^2  */
        if (!model->B3SOIcitGiven)
	    model->B3SOIcit = 0.0;   /* unit Q/V/m^2  */
        if (!model->B3SOInfactorGiven)
	    model->B3SOInfactor = 1;
        if (!model->B3SOIvsatGiven)
            model->B3SOIvsat = 8.0e4;    /* unit m/s */ 
        if (!model->B3SOIatGiven)
            model->B3SOIat = 3.3e4;    /* unit m/s */ 
        if (!model->B3SOIa0Given)
            model->B3SOIa0 = 1.0;  
        if (!model->B3SOIagsGiven)
            model->B3SOIags = 0.0;
        if (!model->B3SOIa1Given)
            model->B3SOIa1 = 0.0;
        if (!model->B3SOIa2Given)
            model->B3SOIa2 = 1.0;
        if (!model->B3SOIketaGiven)
            model->B3SOIketa = -0.6;    /* unit  / V */
        if (!model->B3SOInsubGiven)
            model->B3SOInsub = 6.0e16;   /* unit 1/cm3 */
        if (!model->B3SOInpeakGiven)
            model->B3SOInpeak = 1.7e17;   /* unit 1/cm3 */
        if (!model->B3SOIngateGiven)
            model->B3SOIngate = 0;   /* unit 1/cm3 */
        if (!model->B3SOIvbmGiven)
	    model->B3SOIvbm = -3.0;
        if (!model->B3SOIxtGiven)
	    model->B3SOIxt = 1.55e-7;
        if (!model->B3SOIkt1Given)
            model->B3SOIkt1 = -0.11;      /* unit V */
        if (!model->B3SOIkt1lGiven)
            model->B3SOIkt1l = 0.0;      /* unit V*m */
        if (!model->B3SOIkt2Given)
            model->B3SOIkt2 = 0.022;      /* No unit */
        if (!model->B3SOIk3Given)
            model->B3SOIk3 = 0.0;      
        if (!model->B3SOIk3bGiven)
            model->B3SOIk3b = 0.0;      
        if (!model->B3SOIw0Given)
            model->B3SOIw0 = 2.5e-6;    
        if (!model->B3SOInlxGiven)
            model->B3SOInlx = 1.74e-7;     
        if (!model->B3SOIdvt0Given)
            model->B3SOIdvt0 = 2.2;    
        if (!model->B3SOIdvt1Given)
            model->B3SOIdvt1 = 0.53;      
        if (!model->B3SOIdvt2Given)
            model->B3SOIdvt2 = -0.032;   /* unit 1 / V */     

        if (!model->B3SOIdvt0wGiven)
            model->B3SOIdvt0w = 0.0;    
        if (!model->B3SOIdvt1wGiven)
            model->B3SOIdvt1w = 5.3e6;    
        if (!model->B3SOIdvt2wGiven)
            model->B3SOIdvt2w = -0.032;   

        if (!model->B3SOIdroutGiven)
            model->B3SOIdrout = 0.56;     
        if (!model->B3SOIdsubGiven)
            model->B3SOIdsub = model->B3SOIdrout;     
        if (!model->B3SOIvth0Given)
            model->B3SOIvth0 = (model->B3SOItype == NMOS) ? 0.7 : -0.7;
        if (!model->B3SOIuaGiven)
            model->B3SOIua = 2.25e-9;      /* unit m/V */
        if (!model->B3SOIua1Given)
            model->B3SOIua1 = 4.31e-9;      /* unit m/V */
        if (!model->B3SOIubGiven)
            model->B3SOIub = 5.87e-19;     /* unit (m/V)**2 */
        if (!model->B3SOIub1Given)
            model->B3SOIub1 = -7.61e-18;     /* unit (m/V)**2 */
        if (!model->B3SOIucGiven)
            model->B3SOIuc = (model->B3SOImobMod == 3) ? -0.0465 : -0.0465e-9;   
        if (!model->B3SOIuc1Given)
            model->B3SOIuc1 = (model->B3SOImobMod == 3) ? -0.056 : -0.056e-9;   
        if (!model->B3SOIu0Given)
            model->B3SOIu0 = (model->B3SOItype == NMOS) ? 0.067 : 0.025;
        if (!model->B3SOIuteGiven)
	    model->B3SOIute = -1.5;    
        if (!model->B3SOIvoffGiven)
	    model->B3SOIvoff = -0.08;
        if (!model->B3SOIdeltaGiven)  
           model->B3SOIdelta = 0.01;
        if (!model->B3SOIrdswGiven)
            model->B3SOIrdsw = 100;
        if (!model->B3SOIprwgGiven)
            model->B3SOIprwg = 0.0;      /* unit 1/V */
        if (!model->B3SOIprwbGiven)
            model->B3SOIprwb = 0.0;      
        if (!model->B3SOIprtGiven)
        if (!model->B3SOIprtGiven)
            model->B3SOIprt = 0.0;      
        if (!model->B3SOIeta0Given)
            model->B3SOIeta0 = 0.08;      /* no unit  */ 
        if (!model->B3SOIetabGiven)
            model->B3SOIetab = -0.07;      /* unit  1/V */ 
        if (!model->B3SOIpclmGiven)
            model->B3SOIpclm = 1.3;      /* no unit  */ 
        if (!model->B3SOIpdibl1Given)
            model->B3SOIpdibl1 = .39;    /* no unit  */
        if (!model->B3SOIpdibl2Given)
            model->B3SOIpdibl2 = 0.0086;    /* no unit  */ 
        if (!model->B3SOIpdiblbGiven)
            model->B3SOIpdiblb = 0.0;    /* 1/V  */ 
        if (!model->B3SOIpvagGiven)
            model->B3SOIpvag = 0.0;     
        if (!model->B3SOIwrGiven)  
            model->B3SOIwr = 1.0;
        if (!model->B3SOIdwgGiven)  
            model->B3SOIdwg = 0.0;
        if (!model->B3SOIdwbGiven)  
            model->B3SOIdwb = 0.0;
        if (!model->B3SOIb0Given)
            model->B3SOIb0 = 0.0;
        if (!model->B3SOIb1Given)  
            model->B3SOIb1 = 0.0;
        if (!model->B3SOIalpha0Given)  
            model->B3SOIalpha0 = 0.0;
        if (!model->B3SOIalpha1Given)  
            model->B3SOIalpha1 = 1.0;
        if (!model->B3SOIbeta0Given)  
            model->B3SOIbeta0 = 30.0;

        if (!model->B3SOIcgslGiven)  
            model->B3SOIcgsl = 0.0;
        if (!model->B3SOIcgdlGiven)  
            model->B3SOIcgdl = 0.0;
        if (!model->B3SOIckappaGiven)  
            model->B3SOIckappa = 0.6;
        if (!model->B3SOIclcGiven)  
            model->B3SOIclc = 0.1e-7;
        if (!model->B3SOIcleGiven)  
            model->B3SOIcle = 0.0;
        if (!model->B3SOItboxGiven)  
            model->B3SOItbox = 3e-7;
        if (!model->B3SOItsiGiven)  
            model->B3SOItsi = 1e-7;
        if (!model->B3SOIxjGiven)  
            model->B3SOIxj = model->B3SOItsi;
        if (!model->B3SOIkb1Given)  
            model->B3SOIkb1 = 1;
        if (!model->B3SOIkb3Given)  
            model->B3SOIkb3 = 1;
        if (!model->B3SOIdvbd0Given)  
            model->B3SOIdvbd0 = 0.0;
        if (!model->B3SOIdvbd1Given)  
            model->B3SOIdvbd1 = 0.0;
        if (!model->B3SOIvbsaGiven)  
            model->B3SOIvbsa = 0.0;
        if (!model->B3SOIdelpGiven)  
            model->B3SOIdelp = 0.02;
        if (!model->B3SOIrbodyGiven)  
            model->B3SOIrbody = 0.0;
        if (!model->B3SOIrbshGiven)  
            model->B3SOIrbsh = 0.0;
        if (!model->B3SOIadice0Given)  
            model->B3SOIadice0 = 1;
        if (!model->B3SOIabpGiven)  
            model->B3SOIabp = 1;
        if (!model->B3SOImxcGiven)  
            model->B3SOImxc = -0.9;
        if (!model->B3SOIrth0Given)  
            model->B3SOIrth0 = 0;

        if (!model->B3SOIcth0Given)
            model->B3SOIcth0 =0;

        if (!model->B3SOIaiiGiven)  
            model->B3SOIaii = 0.0;
        if (!model->B3SOIbiiGiven)  
            model->B3SOIbii = 0.0;
        if (!model->B3SOIciiGiven)  
            model->B3SOIcii = 0.0;
        if (!model->B3SOIdiiGiven)  
            model->B3SOIdii = -1.0;
        if (!model->B3SOIagidlGiven)  
            model->B3SOIagidl = 0.0;
        if (!model->B3SOIbgidlGiven)  
            model->B3SOIbgidl = 0.0;
        if (!model->B3SOIngidlGiven)  
            model->B3SOIngidl = 1.2;
        if (!model->B3SOIndiodeGiven)  
            model->B3SOIndiode = 1.0;
        if (!model->B3SOIntunGiven)  
            model->B3SOIntun = 10.0;
        if (!model->B3SOIisbjtGiven)  
            model->B3SOIisbjt = 1e-6;
        if (!model->B3SOIisdifGiven)  
            model->B3SOIisdif = 0.0;
        if (!model->B3SOIisrecGiven)  
            model->B3SOIisrec = 1e-5;
        if (!model->B3SOIistunGiven)  
            model->B3SOIistun = 0.0;
        if (!model->B3SOIxbjtGiven)  
            model->B3SOIxbjt = 2;
        if (!model->B3SOIxdifGiven)  
            model->B3SOIxdif = 2;
        if (!model->B3SOIxrecGiven)  
            model->B3SOIxrec = 20;
        if (!model->B3SOIxtunGiven)  
            model->B3SOIxtun = 0;
        if (!model->B3SOIedlGiven)  
            model->B3SOIedl = 2e-6;
        if (!model->B3SOIkbjt1Given)  
            model->B3SOIkbjt1 = 0;
        if (!model->B3SOIttGiven)  
            model->B3SOItt = 1e-12;
        if (!model->B3SOIasdGiven)  
            model->B3SOIasd = 0.3;

        /* unit degree celcius */
        if (!model->B3SOItnomGiven)  
	    model->B3SOItnom = ckt->CKTnomTemp; 
        if (!model->B3SOILintGiven)  
           model->B3SOILint = 0.0;
        if (!model->B3SOILlGiven)  
           model->B3SOILl = 0.0;
        if (!model->B3SOILlnGiven)  
           model->B3SOILln = 1.0;
        if (!model->B3SOILwGiven)  
           model->B3SOILw = 0.0;
        if (!model->B3SOILwnGiven)  
           model->B3SOILwn = 1.0;
        if (!model->B3SOILwlGiven)  
           model->B3SOILwl = 0.0;
        if (!model->B3SOILminGiven)  
           model->B3SOILmin = 0.0;
        if (!model->B3SOILmaxGiven)  
           model->B3SOILmax = 1.0;
        if (!model->B3SOIWintGiven)  
           model->B3SOIWint = 0.0;
        if (!model->B3SOIWlGiven)  
           model->B3SOIWl = 0.0;
        if (!model->B3SOIWlnGiven)  
           model->B3SOIWln = 1.0;
        if (!model->B3SOIWwGiven)  
           model->B3SOIWw = 0.0;
        if (!model->B3SOIWwnGiven)  
           model->B3SOIWwn = 1.0;
        if (!model->B3SOIWwlGiven)  
           model->B3SOIWwl = 0.0;
        if (!model->B3SOIWminGiven)  
           model->B3SOIWmin = 0.0;
        if (!model->B3SOIWmaxGiven)  
           model->B3SOIWmax = 1.0;
        if (!model->B3SOIdwcGiven)  
           model->B3SOIdwc = model->B3SOIWint;
        if (!model->B3SOIdlcGiven)  
           model->B3SOIdlc = model->B3SOILint;

/* Added for binning - START */
        /* Length dependence */
        if (!model->B3SOIlnpeakGiven)
            model->B3SOIlnpeak = 0.0;
        if (!model->B3SOIlnsubGiven)
            model->B3SOIlnsub = 0.0;
        if (!model->B3SOIlngateGiven)
            model->B3SOIlngate = 0.0;
        if (!model->B3SOIlvth0Given)
           model->B3SOIlvth0 = 0.0;
        if (!model->B3SOIlk1Given)
            model->B3SOIlk1 = 0.0;
        if (!model->B3SOIlk2Given)
            model->B3SOIlk2 = 0.0;
        if (!model->B3SOIlk3Given)
            model->B3SOIlk3 = 0.0;
        if (!model->B3SOIlk3bGiven)
            model->B3SOIlk3b = 0.0;
        if (!model->B3SOIlvbsaGiven)
            model->B3SOIlvbsa = 0.0;
        if (!model->B3SOIldelpGiven)
            model->B3SOIldelp = 0.0;
        if (!model->B3SOIlkb1Given)
           model->B3SOIlkb1 = 0.0;
        if (!model->B3SOIlkb3Given)
           model->B3SOIlkb3 = 1.0;
        if (!model->B3SOIldvbd0Given)
           model->B3SOIldvbd0 = 1.0;
        if (!model->B3SOIldvbd1Given)
           model->B3SOIldvbd1 = 1.0;
        if (!model->B3SOIlw0Given)
            model->B3SOIlw0 = 0.0;
        if (!model->B3SOIlnlxGiven)
            model->B3SOIlnlx = 0.0;
        if (!model->B3SOIldvt0Given)
            model->B3SOIldvt0 = 0.0;
        if (!model->B3SOIldvt1Given)
            model->B3SOIldvt1 = 0.0;
        if (!model->B3SOIldvt2Given)
            model->B3SOIldvt2 = 0.0;
        if (!model->B3SOIldvt0wGiven)
            model->B3SOIldvt0w = 0.0;
        if (!model->B3SOIldvt1wGiven)
            model->B3SOIldvt1w = 0.0;
        if (!model->B3SOIldvt2wGiven)
            model->B3SOIldvt2w = 0.0;
        if (!model->B3SOIlu0Given)
            model->B3SOIlu0 = 0.0;
        if (!model->B3SOIluaGiven)
            model->B3SOIlua = 0.0;
        if (!model->B3SOIlubGiven)
            model->B3SOIlub = 0.0;
        if (!model->B3SOIlucGiven)
            model->B3SOIluc = 0.0;
        if (!model->B3SOIlvsatGiven)
            model->B3SOIlvsat = 0.0;
        if (!model->B3SOIla0Given)
            model->B3SOIla0 = 0.0;
        if (!model->B3SOIlagsGiven)
            model->B3SOIlags = 0.0;
        if (!model->B3SOIlb0Given)
            model->B3SOIlb0 = 0.0;
        if (!model->B3SOIlb1Given)
            model->B3SOIlb1 = 0.0;
        if (!model->B3SOIlketaGiven)
            model->B3SOIlketa = 0.0;
        if (!model->B3SOIlabpGiven)
            model->B3SOIlabp = 0.0;
        if (!model->B3SOIlmxcGiven)
            model->B3SOIlmxc = 0.0;
        if (!model->B3SOIladice0Given)
            model->B3SOIladice0 = 0.0;
        if (!model->B3SOIla1Given)
            model->B3SOIla1 = 0.0;
        if (!model->B3SOIla2Given)
            model->B3SOIla2 = 0.0;
        if (!model->B3SOIlrdswGiven)
            model->B3SOIlrdsw = 0.0;
        if (!model->B3SOIlprwbGiven)
            model->B3SOIlprwb = 0.0;
        if (!model->B3SOIlprwgGiven)
            model->B3SOIlprwg = 0.0;
        if (!model->B3SOIlwrGiven)
            model->B3SOIlwr = 0.0;
        if (!model->B3SOIlnfactorGiven)
            model->B3SOIlnfactor = 0.0;
        if (!model->B3SOIldwgGiven)
            model->B3SOIldwg = 0.0;
        if (!model->B3SOIldwbGiven)
            model->B3SOIldwb = 0.0;
        if (!model->B3SOIlvoffGiven)
            model->B3SOIlvoff = 0.0;
        if (!model->B3SOIleta0Given)
            model->B3SOIleta0 = 0.0;
        if (!model->B3SOIletabGiven)
            model->B3SOIletab = 0.0;
        if (!model->B3SOIldsubGiven)
            model->B3SOIldsub = 0.0;
        if (!model->B3SOIlcitGiven)
            model->B3SOIlcit = 0.0;
        if (!model->B3SOIlcdscGiven)
            model->B3SOIlcdsc = 0.0;
        if (!model->B3SOIlcdscbGiven)
            model->B3SOIlcdscb = 0.0;
        if (!model->B3SOIlcdscdGiven)
            model->B3SOIlcdscd = 0.0;
        if (!model->B3SOIlpclmGiven)
            model->B3SOIlpclm = 0.0;
        if (!model->B3SOIlpdibl1Given)
            model->B3SOIlpdibl1 = 0.0;
        if (!model->B3SOIlpdibl2Given)
            model->B3SOIlpdibl2 = 0.0;
        if (!model->B3SOIlpdiblbGiven)
            model->B3SOIlpdiblb = 0.0;
        if (!model->B3SOIldroutGiven)
            model->B3SOIldrout = 0.0;
        if (!model->B3SOIlpvagGiven)
            model->B3SOIlpvag = 0.0;
        if (!model->B3SOIldeltaGiven)
            model->B3SOIldelta = 0.0;
        if (!model->B3SOIlaiiGiven)
            model->B3SOIlaii = 0.0;
        if (!model->B3SOIlbiiGiven)
            model->B3SOIlbii = 0.0;
        if (!model->B3SOIlciiGiven)
            model->B3SOIlcii = 0.0;
        if (!model->B3SOIldiiGiven)
            model->B3SOIldii = 0.0;
        if (!model->B3SOIlalpha0Given)
            model->B3SOIlalpha0 = 0.0;
        if (!model->B3SOIlalpha1Given)
            model->B3SOIlalpha1 = 0.0;
        if (!model->B3SOIlbeta0Given)
            model->B3SOIlbeta0 = 0.0;
        if (!model->B3SOIlagidlGiven)
            model->B3SOIlagidl = 0.0;
        if (!model->B3SOIlbgidlGiven)
            model->B3SOIlbgidl = 0.0;
        if (!model->B3SOIlngidlGiven)
            model->B3SOIlngidl = 0.0;
        if (!model->B3SOIlntunGiven)
            model->B3SOIlntun = 0.0;
        if (!model->B3SOIlndiodeGiven)
            model->B3SOIlndiode = 0.0;
        if (!model->B3SOIlisbjtGiven)
            model->B3SOIlisbjt = 0.0;
        if (!model->B3SOIlisdifGiven)
            model->B3SOIlisdif = 0.0;
        if (!model->B3SOIlisrecGiven)
            model->B3SOIlisrec = 0.0;
        if (!model->B3SOIlistunGiven)
            model->B3SOIlistun = 0.0;
        if (!model->B3SOIledlGiven)
            model->B3SOIledl = 0.0;
        if (!model->B3SOIlkbjt1Given)
            model->B3SOIlkbjt1 = 0.0;
	/* CV Model */
        if (!model->B3SOIlvsdfbGiven)
            model->B3SOIlvsdfb = 0.0;
        if (!model->B3SOIlvsdthGiven)
            model->B3SOIlvsdth = 0.0;
        /* Width dependence */
        if (!model->B3SOIwnpeakGiven)
            model->B3SOIwnpeak = 0.0;
        if (!model->B3SOIwnsubGiven)
            model->B3SOIwnsub = 0.0;
        if (!model->B3SOIwngateGiven)
            model->B3SOIwngate = 0.0;
        if (!model->B3SOIwvth0Given)
           model->B3SOIwvth0 = 0.0;
        if (!model->B3SOIwk1Given)
            model->B3SOIwk1 = 0.0;
        if (!model->B3SOIwk2Given)
            model->B3SOIwk2 = 0.0;
        if (!model->B3SOIwk3Given)
            model->B3SOIwk3 = 0.0;
        if (!model->B3SOIwk3bGiven)
            model->B3SOIwk3b = 0.0;
        if (!model->B3SOIwvbsaGiven)
            model->B3SOIwvbsa = 0.0;
        if (!model->B3SOIwdelpGiven)
            model->B3SOIwdelp = 0.0;
        if (!model->B3SOIwkb1Given)
           model->B3SOIwkb1 = 0.0;
        if (!model->B3SOIwkb3Given)
           model->B3SOIwkb3 = 1.0;
        if (!model->B3SOIwdvbd0Given)
           model->B3SOIwdvbd0 = 1.0;
        if (!model->B3SOIwdvbd1Given)
           model->B3SOIwdvbd1 = 1.0;
        if (!model->B3SOIww0Given)
            model->B3SOIww0 = 0.0;
        if (!model->B3SOIwnlxGiven)
            model->B3SOIwnlx = 0.0;
        if (!model->B3SOIwdvt0Given)
            model->B3SOIwdvt0 = 0.0;
        if (!model->B3SOIwdvt1Given)
            model->B3SOIwdvt1 = 0.0;
        if (!model->B3SOIwdvt2Given)
            model->B3SOIwdvt2 = 0.0;
        if (!model->B3SOIwdvt0wGiven)
            model->B3SOIwdvt0w = 0.0;
        if (!model->B3SOIwdvt1wGiven)
            model->B3SOIwdvt1w = 0.0;
        if (!model->B3SOIwdvt2wGiven)
            model->B3SOIwdvt2w = 0.0;
        if (!model->B3SOIwu0Given)
            model->B3SOIwu0 = 0.0;
        if (!model->B3SOIwuaGiven)
            model->B3SOIwua = 0.0;
        if (!model->B3SOIwubGiven)
            model->B3SOIwub = 0.0;
        if (!model->B3SOIwucGiven)
            model->B3SOIwuc = 0.0;
        if (!model->B3SOIwvsatGiven)
            model->B3SOIwvsat = 0.0;
        if (!model->B3SOIwa0Given)
            model->B3SOIwa0 = 0.0;
        if (!model->B3SOIwagsGiven)
            model->B3SOIwags = 0.0;
        if (!model->B3SOIwb0Given)
            model->B3SOIwb0 = 0.0;
        if (!model->B3SOIwb1Given)
            model->B3SOIwb1 = 0.0;
        if (!model->B3SOIwketaGiven)
            model->B3SOIwketa = 0.0;
        if (!model->B3SOIwabpGiven)
            model->B3SOIwabp = 0.0;
        if (!model->B3SOIwmxcGiven)
            model->B3SOIwmxc = 0.0;
        if (!model->B3SOIwadice0Given)
            model->B3SOIwadice0 = 0.0;
        if (!model->B3SOIwa1Given)
            model->B3SOIwa1 = 0.0;
        if (!model->B3SOIwa2Given)
            model->B3SOIwa2 = 0.0;
        if (!model->B3SOIwrdswGiven)
            model->B3SOIwrdsw = 0.0;
        if (!model->B3SOIwprwbGiven)
            model->B3SOIwprwb = 0.0;
        if (!model->B3SOIwprwgGiven)
            model->B3SOIwprwg = 0.0;
        if (!model->B3SOIwwrGiven)
            model->B3SOIwwr = 0.0;
        if (!model->B3SOIwnfactorGiven)
            model->B3SOIwnfactor = 0.0;
        if (!model->B3SOIwdwgGiven)
            model->B3SOIwdwg = 0.0;
        if (!model->B3SOIwdwbGiven)
            model->B3SOIwdwb = 0.0;
        if (!model->B3SOIwvoffGiven)
            model->B3SOIwvoff = 0.0;
        if (!model->B3SOIweta0Given)
            model->B3SOIweta0 = 0.0;
        if (!model->B3SOIwetabGiven)
            model->B3SOIwetab = 0.0;
        if (!model->B3SOIwdsubGiven)
            model->B3SOIwdsub = 0.0;
        if (!model->B3SOIwcitGiven)
            model->B3SOIwcit = 0.0;
        if (!model->B3SOIwcdscGiven)
            model->B3SOIwcdsc = 0.0;
        if (!model->B3SOIwcdscbGiven)
            model->B3SOIwcdscb = 0.0;
        if (!model->B3SOIwcdscdGiven)
            model->B3SOIwcdscd = 0.0;
        if (!model->B3SOIwpclmGiven)
            model->B3SOIwpclm = 0.0;
        if (!model->B3SOIwpdibl1Given)
            model->B3SOIwpdibl1 = 0.0;
        if (!model->B3SOIwpdibl2Given)
            model->B3SOIwpdibl2 = 0.0;
        if (!model->B3SOIwpdiblbGiven)
            model->B3SOIwpdiblb = 0.0;
        if (!model->B3SOIwdroutGiven)
            model->B3SOIwdrout = 0.0;
        if (!model->B3SOIwpvagGiven)
            model->B3SOIwpvag = 0.0;
        if (!model->B3SOIwdeltaGiven)
            model->B3SOIwdelta = 0.0;
        if (!model->B3SOIwaiiGiven)
            model->B3SOIwaii = 0.0;
        if (!model->B3SOIwbiiGiven)
            model->B3SOIwbii = 0.0;
        if (!model->B3SOIwciiGiven)
            model->B3SOIwcii = 0.0;
        if (!model->B3SOIwdiiGiven)
            model->B3SOIwdii = 0.0;
        if (!model->B3SOIwalpha0Given)
            model->B3SOIwalpha0 = 0.0;
        if (!model->B3SOIwalpha1Given)
            model->B3SOIwalpha1 = 0.0;
        if (!model->B3SOIwbeta0Given)
            model->B3SOIwbeta0 = 0.0;
        if (!model->B3SOIwagidlGiven)
            model->B3SOIwagidl = 0.0;
        if (!model->B3SOIwbgidlGiven)
            model->B3SOIwbgidl = 0.0;
        if (!model->B3SOIwngidlGiven)
            model->B3SOIwngidl = 0.0;
        if (!model->B3SOIwntunGiven)
            model->B3SOIwntun = 0.0;
        if (!model->B3SOIwndiodeGiven)
            model->B3SOIwndiode = 0.0;
        if (!model->B3SOIwisbjtGiven)
            model->B3SOIwisbjt = 0.0;
        if (!model->B3SOIwisdifGiven)
            model->B3SOIwisdif = 0.0;
        if (!model->B3SOIwisrecGiven)
            model->B3SOIwisrec = 0.0;
        if (!model->B3SOIwistunGiven)
            model->B3SOIwistun = 0.0;
        if (!model->B3SOIwedlGiven)
            model->B3SOIwedl = 0.0;
        if (!model->B3SOIwkbjt1Given)
            model->B3SOIwkbjt1 = 0.0;
	/* CV Model */
        if (!model->B3SOIwvsdfbGiven)
            model->B3SOIwvsdfb = 0.0;
        if (!model->B3SOIwvsdthGiven)
            model->B3SOIwvsdth = 0.0;
        /* Cross-term dependence */
        if (!model->B3SOIpnpeakGiven)
            model->B3SOIpnpeak = 0.0;
        if (!model->B3SOIpnsubGiven)
            model->B3SOIpnsub = 0.0;
        if (!model->B3SOIpngateGiven)
            model->B3SOIpngate = 0.0;
        if (!model->B3SOIpvth0Given)
           model->B3SOIpvth0 = 0.0;
        if (!model->B3SOIpk1Given)
            model->B3SOIpk1 = 0.0;
        if (!model->B3SOIpk2Given)
            model->B3SOIpk2 = 0.0;
        if (!model->B3SOIpk3Given)
            model->B3SOIpk3 = 0.0;
        if (!model->B3SOIpk3bGiven)
            model->B3SOIpk3b = 0.0;
        if (!model->B3SOIpvbsaGiven)
            model->B3SOIpvbsa = 0.0;
        if (!model->B3SOIpdelpGiven)
            model->B3SOIpdelp = 0.0;
        if (!model->B3SOIpkb1Given)
           model->B3SOIpkb1 = 0.0;
        if (!model->B3SOIpkb3Given)
           model->B3SOIpkb3 = 1.0;
        if (!model->B3SOIpdvbd0Given)
           model->B3SOIpdvbd0 = 1.0;
        if (!model->B3SOIpdvbd1Given)
           model->B3SOIpdvbd1 = 1.0;
        if (!model->B3SOIpw0Given)
            model->B3SOIpw0 = 0.0;
        if (!model->B3SOIpnlxGiven)
            model->B3SOIpnlx = 0.0;
        if (!model->B3SOIpdvt0Given)
            model->B3SOIpdvt0 = 0.0;
        if (!model->B3SOIpdvt1Given)
            model->B3SOIpdvt1 = 0.0;
        if (!model->B3SOIpdvt2Given)
            model->B3SOIpdvt2 = 0.0;
        if (!model->B3SOIpdvt0wGiven)
            model->B3SOIpdvt0w = 0.0;
        if (!model->B3SOIpdvt1wGiven)
            model->B3SOIpdvt1w = 0.0;
        if (!model->B3SOIpdvt2wGiven)
            model->B3SOIpdvt2w = 0.0;
        if (!model->B3SOIpu0Given)
            model->B3SOIpu0 = 0.0;
        if (!model->B3SOIpuaGiven)
            model->B3SOIpua = 0.0;
        if (!model->B3SOIpubGiven)
            model->B3SOIpub = 0.0;
        if (!model->B3SOIpucGiven)
            model->B3SOIpuc = 0.0;
        if (!model->B3SOIpvsatGiven)
            model->B3SOIpvsat = 0.0;
        if (!model->B3SOIpa0Given)
            model->B3SOIpa0 = 0.0;
        if (!model->B3SOIpagsGiven)
            model->B3SOIpags = 0.0;
        if (!model->B3SOIpb0Given)
            model->B3SOIpb0 = 0.0;
        if (!model->B3SOIpb1Given)
            model->B3SOIpb1 = 0.0;
        if (!model->B3SOIpketaGiven)
            model->B3SOIpketa = 0.0;
        if (!model->B3SOIpabpGiven)
            model->B3SOIpabp = 0.0;
        if (!model->B3SOIpmxcGiven)
            model->B3SOIpmxc = 0.0;
        if (!model->B3SOIpadice0Given)
            model->B3SOIpadice0 = 0.0;
        if (!model->B3SOIpa1Given)
            model->B3SOIpa1 = 0.0;
        if (!model->B3SOIpa2Given)
            model->B3SOIpa2 = 0.0;
        if (!model->B3SOIprdswGiven)
            model->B3SOIprdsw = 0.0;
        if (!model->B3SOIpprwbGiven)
            model->B3SOIpprwb = 0.0;
        if (!model->B3SOIpprwgGiven)
            model->B3SOIpprwg = 0.0;
        if (!model->B3SOIpwrGiven)
            model->B3SOIpwr = 0.0;
        if (!model->B3SOIpnfactorGiven)
            model->B3SOIpnfactor = 0.0;
        if (!model->B3SOIpdwgGiven)
            model->B3SOIpdwg = 0.0;
        if (!model->B3SOIpdwbGiven)
            model->B3SOIpdwb = 0.0;
        if (!model->B3SOIpvoffGiven)
            model->B3SOIpvoff = 0.0;
        if (!model->B3SOIpeta0Given)
            model->B3SOIpeta0 = 0.0;
        if (!model->B3SOIpetabGiven)
            model->B3SOIpetab = 0.0;
        if (!model->B3SOIpdsubGiven)
            model->B3SOIpdsub = 0.0;
        if (!model->B3SOIpcitGiven)
            model->B3SOIpcit = 0.0;
        if (!model->B3SOIpcdscGiven)
            model->B3SOIpcdsc = 0.0;
        if (!model->B3SOIpcdscbGiven)
            model->B3SOIpcdscb = 0.0;
        if (!model->B3SOIpcdscdGiven)
            model->B3SOIpcdscd = 0.0;
        if (!model->B3SOIppclmGiven)
            model->B3SOIppclm = 0.0;
        if (!model->B3SOIppdibl1Given)
            model->B3SOIppdibl1 = 0.0;
        if (!model->B3SOIppdibl2Given)
            model->B3SOIppdibl2 = 0.0;
        if (!model->B3SOIppdiblbGiven)
            model->B3SOIppdiblb = 0.0;
        if (!model->B3SOIpdroutGiven)
            model->B3SOIpdrout = 0.0;
        if (!model->B3SOIppvagGiven)
            model->B3SOIppvag = 0.0;
        if (!model->B3SOIpdeltaGiven)
            model->B3SOIpdelta = 0.0;
        if (!model->B3SOIpaiiGiven)
            model->B3SOIpaii = 0.0;
        if (!model->B3SOIpbiiGiven)
            model->B3SOIpbii = 0.0;
        if (!model->B3SOIpciiGiven)
            model->B3SOIpcii = 0.0;
        if (!model->B3SOIpdiiGiven)
            model->B3SOIpdii = 0.0;
        if (!model->B3SOIpalpha0Given)
            model->B3SOIpalpha0 = 0.0;
        if (!model->B3SOIpalpha1Given)
            model->B3SOIpalpha1 = 0.0;
        if (!model->B3SOIpbeta0Given)
            model->B3SOIpbeta0 = 0.0;
        if (!model->B3SOIpagidlGiven)
            model->B3SOIpagidl = 0.0;
        if (!model->B3SOIpbgidlGiven)
            model->B3SOIpbgidl = 0.0;
        if (!model->B3SOIpngidlGiven)
            model->B3SOIpngidl = 0.0;
        if (!model->B3SOIpntunGiven)
            model->B3SOIpntun = 0.0;
        if (!model->B3SOIpndiodeGiven)
            model->B3SOIpndiode = 0.0;
        if (!model->B3SOIpisbjtGiven)
            model->B3SOIpisbjt = 0.0;
        if (!model->B3SOIpisdifGiven)
            model->B3SOIpisdif = 0.0;
        if (!model->B3SOIpisrecGiven)
            model->B3SOIpisrec = 0.0;
        if (!model->B3SOIpistunGiven)
            model->B3SOIpistun = 0.0;
        if (!model->B3SOIpedlGiven)
            model->B3SOIpedl = 0.0;
        if (!model->B3SOIpkbjt1Given)
            model->B3SOIpkbjt1 = 0.0;
	/* CV Model */
        if (!model->B3SOIpvsdfbGiven)
            model->B3SOIpvsdfb = 0.0;
        if (!model->B3SOIpvsdthGiven)
            model->B3SOIpvsdth = 0.0;
/* Added for binning - END */

	if (!model->B3SOIcfGiven)
            model->B3SOIcf = 2.0 * EPSOX / PI
			   * log(1.0 + 0.4e-6 / model->B3SOItox);
        if (!model->B3SOIcgdoGiven)
	{   if (model->B3SOIdlcGiven && (model->B3SOIdlc > 0.0))
	    {   model->B3SOIcgdo = model->B3SOIdlc * model->B3SOIcox
				 - model->B3SOIcgdl ;
	    }
	    else
	        model->B3SOIcgdo = 0.6 * model->B3SOIxj * model->B3SOIcox; 
	}
        if (!model->B3SOIcgsoGiven)
	{   if (model->B3SOIdlcGiven && (model->B3SOIdlc > 0.0))
	    {   model->B3SOIcgso = model->B3SOIdlc * model->B3SOIcox
				 - model->B3SOIcgsl ;
	    }
	    else
	        model->B3SOIcgso = 0.6 * model->B3SOIxj * model->B3SOIcox; 
	}

        if (!model->B3SOIcgeoGiven)
	{   model->B3SOIcgeo = 0.0;
	}
        if (!model->B3SOIxpartGiven)
            model->B3SOIxpart = 0.0;
        if (!model->B3SOIsheetResistanceGiven)
            model->B3SOIsheetResistance = 0.0;
        if (!model->B3SOIcsdeswGiven)
            model->B3SOIcsdesw = 0.0;
        if (!model->B3SOIunitLengthGateSidewallJctCapGiven)
            model->B3SOIunitLengthGateSidewallJctCap = 1e-10;
        if (!model->B3SOIGatesidewallJctPotentialGiven)
            model->B3SOIGatesidewallJctPotential = 0.7;
        if (!model->B3SOIbodyJctGateSideGradingCoeffGiven)
            model->B3SOIbodyJctGateSideGradingCoeff = 0.5;
        if (!model->B3SOIoxideTrapDensityAGiven)
	{   if (model->B3SOItype == NMOS)
                model->B3SOIoxideTrapDensityA = 1e20;
            else
                model->B3SOIoxideTrapDensityA=9.9e18;
	}
        if (!model->B3SOIoxideTrapDensityBGiven)
	{   if (model->B3SOItype == NMOS)
                model->B3SOIoxideTrapDensityB = 5e4;
            else
                model->B3SOIoxideTrapDensityB = 2.4e3;
	}
        if (!model->B3SOIoxideTrapDensityCGiven)
	{   if (model->B3SOItype == NMOS)
                model->B3SOIoxideTrapDensityC = -1.4e-12;
            else
                model->B3SOIoxideTrapDensityC = 1.4e-12;

	}
        if (!model->B3SOIemGiven)
            model->B3SOIem = 4.1e7; /* V/m */
        if (!model->B3SOIefGiven)
            model->B3SOIef = 1.0;
        if (!model->B3SOIafGiven)
            model->B3SOIaf = 1.0;
        if (!model->B3SOIkfGiven)
            model->B3SOIkf = 0.0;
        if (!model->B3SOInoifGiven)
            model->B3SOInoif = 1.0;

        /* loop through all the instances of the model */
        for (here = model->B3SOIinstances; here != NULL ;
             here=here->B3SOInextInstance) 
	{   /* allocate a chunk of the state vector */
            here->B3SOIstates = *states;
            *states += B3SOInumStates;
            /* perform the parameter defaulting */
            if (!here->B3SOIdrainAreaGiven)
                here->B3SOIdrainArea = 0.0;
            if (!here->B3SOIdrainPerimeterGiven)
                here->B3SOIdrainPerimeter = 0.0;
            if (!here->B3SOIdrainSquaresGiven)
                here->B3SOIdrainSquares = 1.0;
            if (!here->B3SOIicVBSGiven)
                here->B3SOIicVBS = 0;
            if (!here->B3SOIicVDSGiven)
                here->B3SOIicVDS = 0;
            if (!here->B3SOIicVGSGiven)
                here->B3SOIicVGS = 0;
            if (!here->B3SOIicVESGiven)
                here->B3SOIicVES = 0;
            if (!here->B3SOIicVPSGiven)
                here->B3SOIicVPS = 0;
	    if (!here->B3SOIbjtoffGiven)
		here->B3SOIbjtoff = 0;
	    if (!here->B3SOIdebugModGiven)
		here->B3SOIdebugMod = 0;
	    if (!here->B3SOIrth0Given)
		here->B3SOIrth0 = model->B3SOIrth0;
	    if (!here->B3SOIcth0Given)
		here->B3SOIcth0 = model->B3SOIcth0;
            if (!here->B3SOIbodySquaresGiven)
                here->B3SOIbodySquares = 1.0;
            if (!here->B3SOIlGiven)
                here->B3SOIl = 5e-6;
            if (!here->B3SOIsourceAreaGiven)
                here->B3SOIsourceArea = 0;
            if (!here->B3SOIsourcePerimeterGiven)
                here->B3SOIsourcePerimeter = 0;
            if (!here->B3SOIsourceSquaresGiven)
                here->B3SOIsourceSquares = 1;
            if (!here->B3SOIwGiven)
                here->B3SOIw = 5e-6;

            if (!here->B3SOIoffGiven)
                here->B3SOIoff = 0;
 

            /* process drain series resistance */
            if ((model->B3SOIsheetResistance > 0.0) && 
                (here->B3SOIdrainSquares > 0.0 ) &&
                (here->B3SOIdNodePrime == 0))
	    {   error = CKTmkVolt(ckt,&tmp,here->B3SOIname,"drain");
                if(error) return(error);
                here->B3SOIdNodePrime = tmp->number;
            }
	    else
	    {   here->B3SOIdNodePrime = here->B3SOIdNode;
            }
                   
            /* process source series resistance */
            if ((model->B3SOIsheetResistance > 0.0) && 
                (here->B3SOIsourceSquares > 0.0 ) &&
                (here->B3SOIsNodePrime == 0)) 
	    {   error = CKTmkVolt(ckt,&tmp,here->B3SOIname,"source");
                if(error) return(error);
                here->B3SOIsNodePrime = tmp->number;
            }
	    else 
	    {   here->B3SOIsNodePrime = here->B3SOIsNode;
            }

            /* process effective silicon film thickness */
            model->B3SOIcbox = 3.453133e-11 / model->B3SOItbox;
            model->B3SOIcsi = 1.03594e-10 / model->B3SOItsi;
            Cboxt = model->B3SOIcbox * model->B3SOIcsi / (model->B3SOIcbox + model->B3SOIcsi);
            model->B3SOIqsi = Charge_q*model->B3SOInpeak*1e6*model->B3SOItsi;
            /* Tsieff */
            tmp1 = 2.0 * EPSSI * model->B3SOIvbsa / Charge_q 
                 / (1e6*model->B3SOInpeak);
            tmp2 = model->B3SOItsi * model->B3SOItsi;
            if (tmp2 < tmp1)
            {
               fprintf(stderr, "vbsa = %.3f is too large for this tsi = %.3e and is automatically set to zero\n", model->B3SOIvbsa, model->B3SOItsi);
               model->B3SOIcsieff = model->B3SOIcsi;
               model->B3SOIqsieff = model->B3SOIqsi;
            }
            else
            {
               tmp1 = sqrt(model->B3SOItsi * model->B3SOItsi - 
                      2.0 * EPSSI * model->B3SOIvbsa / Charge_q / 
                      (1e6*model->B3SOInpeak));
               model->B3SOIcsieff = 1.03594e-10 / tmp1;
               model->B3SOIqsieff = Charge_q*model->B3SOInpeak*1e6*tmp1;
            }
            model->B3SOIcsit = 1/(1/model->B3SOIcox + 1/model->B3SOIcsieff);
            model->B3SOIcboxt = 1/(1/model->B3SOIcbox + 1/model->B3SOIcsieff);
            nfb0 = 1/(1 + model->B3SOIcbox / model->B3SOIcsit);
            model->B3SOInfb = model->B3SOIkb3 * nfb0;
            model->B3SOIadice = model->B3SOIadice0  / ( 1 + Cboxt / model->B3SOIcox);

            here->B3SOIfloat = 0;
	    if (here->B3SOIbNode == -1) 
            /* no internal body node is needed for SPICE iteration  */
            {   here->B3SOIbNode = here->B3SOIpNode = 0;
                here->B3SOIbodyMod = 0;
	    }
            else /* body tied */ 
            {   if ((model->B3SOIrbody == 0.0) && (model->B3SOIrbsh == 0.0))
                {  /* ideal body tie */
                   here->B3SOIbodyMod = 2;
                   /* pNode is not used in this case */
                }
                else { 
                   error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Body");
                   if(error) return(error);
                   here->B3SOIbodyMod = 1;
                   here->B3SOIpNode = here->B3SOIbNode;
                   here->B3SOIbNode = tmp->number;
                }
            }

            if ((model->B3SOIshMod == 1) && (here->B3SOIrth0!=0))
            {
	        error = CKTmkVolt(ckt,&tmp,here->B3SOIname,"Temp");
                if(error) return(error);
                   here->B3SOItempNode = tmp->number;

            } else {
                here->B3SOItempNode = 0;
            }

/* here for debugging purpose only */
            if ((here->B3SOIdebugMod > 1) || (here->B3SOIdebugMod == -1))
            {
               /* The real Vbs value */
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vbs");
               if(error) return(error);
               here->B3SOIvbsNode = tmp->number;   

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Ids");
               if(error) return(error);
               here->B3SOIidsNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Ic");
               if(error) return(error);
               here->B3SOIicNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Ibs");
               if(error) return(error);
               here->B3SOIibsNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Ibd");
               if(error) return(error);
               here->B3SOIibdNode = tmp->number;
       
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Iii");
               if(error) return(error);
               here->B3SOIiiiNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Igidl");
               if(error) return(error);
               here->B3SOIigidlNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Itun");
               if(error) return(error);
               here->B3SOIitunNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Ibp");
               if(error) return(error);
               here->B3SOIibpNode = tmp->number;
       
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Abeff");
               if(error) return(error);
               here->B3SOIabeffNode = tmp->number;
       
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vbs0eff");
               if(error) return(error);
               here->B3SOIvbs0effNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vbseff");
               if(error) return(error);
               here->B3SOIvbseffNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Xc");
               if(error) return(error);
               here->B3SOIxcNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Cbb");
               if(error) return(error);
               here->B3SOIcbbNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Cbd");
               if(error) return(error);
               here->B3SOIcbdNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Cbg");
               if(error) return(error);
               here->B3SOIcbgNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qbody");
               if(error) return(error);
               here->B3SOIqbNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qbf");
               if(error) return(error);
               here->B3SOIqbfNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qjs");
               if(error) return(error);
               here->B3SOIqjsNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qjd");
               if(error) return(error);
               here->B3SOIqjdNode = tmp->number;

               /* clean up last */
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Gm");
               if(error) return(error);
               here->B3SOIgmNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Gmbs");
               if(error) return(error);
               here->B3SOIgmbsNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Gds");
               if(error) return(error);
               here->B3SOIgdsNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Gme");
               if(error) return(error);
               here->B3SOIgmeNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vbs0teff");
               if(error) return(error);
               here->B3SOIvbs0teffNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vth");
               if(error) return(error);
               here->B3SOIvthNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vgsteff");
               if(error) return(error);
               here->B3SOIvgsteffNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Xcsat");
               if(error) return(error);
               here->B3SOIxcsatNode = tmp->number;
       
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qac0");
               if(error) return(error);
               here->B3SOIqaccNode = tmp->number;
       
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qsub0");
               if(error) return(error);
               here->B3SOIqsub0Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qsubs1");
               if(error) return(error);
               here->B3SOIqsubs1Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qsubs2");
               if(error) return(error);
               here->B3SOIqsubs2Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qsub");
               if(error) return(error);
               here->B3SOIqeNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qdrn");
               if(error) return(error);
               here->B3SOIqdNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Qgate");
               if(error) return(error);
               here->B3SOIqgNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vdscv");
               if(error) return(error);
               here->B3SOIvdscvNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Vcscv");
               if(error) return(error);
               here->B3SOIvcscvNode = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Cbe");
               if(error) return(error);
               here->B3SOIcbeNode = tmp->number;
       
               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Dum1");
               if(error) return(error);
               here->B3SOIdum1Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Dum2");
               if(error) return(error);
               here->B3SOIdum2Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Dum3");
               if(error) return(error);
               here->B3SOIdum3Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Dum4");
               if(error) return(error);
               here->B3SOIdum4Node = tmp->number;

               error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Dum5");
               if(error) return(error);
               here->B3SOIdum5Node = tmp->number;
           }

        /* set Sparse Matrix Pointers */

/* macro to make elements with built in test for out of memory */
#define TSTALLOC(ptr,first,second) \
if((here->ptr = SMPmakeElt(matrix,here->first,here->second))==(double *)NULL){\
    return(E_NOMEM);\
}


        if ((model->B3SOIshMod == 1) && (here->B3SOIrth0!=0.0)) {
            TSTALLOC(B3SOITemptempPtr, B3SOItempNode, B3SOItempNode)
            TSTALLOC(B3SOITempdpPtr, B3SOItempNode, B3SOIdNodePrime)
            TSTALLOC(B3SOITempspPtr, B3SOItempNode, B3SOIsNodePrime)
            TSTALLOC(B3SOITempgPtr, B3SOItempNode, B3SOIgNode)
            TSTALLOC(B3SOITempbPtr, B3SOItempNode, B3SOIbNode)
            TSTALLOC(B3SOITempePtr, B3SOItempNode, B3SOIeNode)

            TSTALLOC(B3SOIGtempPtr, B3SOIgNode, B3SOItempNode)
            TSTALLOC(B3SOIDPtempPtr, B3SOIdNodePrime, B3SOItempNode)
            TSTALLOC(B3SOISPtempPtr, B3SOIsNodePrime, B3SOItempNode)
            TSTALLOC(B3SOIEtempPtr, B3SOIeNode, B3SOItempNode)
            TSTALLOC(B3SOIBtempPtr, B3SOIbNode, B3SOItempNode)

            if (here->B3SOIbodyMod == 1) {
                TSTALLOC(B3SOIPtempPtr, B3SOIpNode, B3SOItempNode)
            }
        }
        if (here->B3SOIbodyMod == 2) {
            /* Don't create any Jacobian entry for pNode */
        }
        else if (here->B3SOIbodyMod == 1) {
            TSTALLOC(B3SOIBpPtr, B3SOIbNode, B3SOIpNode)
            TSTALLOC(B3SOIPbPtr, B3SOIpNode, B3SOIbNode)
            TSTALLOC(B3SOIPpPtr, B3SOIpNode, B3SOIpNode)
            TSTALLOC(B3SOIPgPtr, B3SOIpNode, B3SOIgNode)
            TSTALLOC(B3SOIPdpPtr, B3SOIpNode, B3SOIdNodePrime)
            TSTALLOC(B3SOIPspPtr, B3SOIpNode, B3SOIsNodePrime)
            TSTALLOC(B3SOIPePtr, B3SOIpNode, B3SOIeNode)
        }

        TSTALLOC(B3SOIEgPtr, B3SOIeNode, B3SOIgNode)
        TSTALLOC(B3SOIEdpPtr, B3SOIeNode, B3SOIdNodePrime)
        TSTALLOC(B3SOIEspPtr, B3SOIeNode, B3SOIsNodePrime)
        TSTALLOC(B3SOIGePtr, B3SOIgNode, B3SOIeNode)
        TSTALLOC(B3SOIDPePtr, B3SOIdNodePrime, B3SOIeNode)
        TSTALLOC(B3SOISPePtr, B3SOIsNodePrime, B3SOIeNode)

        TSTALLOC(B3SOIEbPtr, B3SOIeNode, B3SOIbNode)
        TSTALLOC(B3SOIEePtr, B3SOIeNode, B3SOIeNode)

        TSTALLOC(B3SOIGgPtr, B3SOIgNode, B3SOIgNode)
        TSTALLOC(B3SOIGdpPtr, B3SOIgNode, B3SOIdNodePrime)
        TSTALLOC(B3SOIGspPtr, B3SOIgNode, B3SOIsNodePrime)

        TSTALLOC(B3SOIDPgPtr, B3SOIdNodePrime, B3SOIgNode)
        TSTALLOC(B3SOIDPdpPtr, B3SOIdNodePrime, B3SOIdNodePrime)
        TSTALLOC(B3SOIDPspPtr, B3SOIdNodePrime, B3SOIsNodePrime)
        TSTALLOC(B3SOIDPdPtr, B3SOIdNodePrime, B3SOIdNode)

        TSTALLOC(B3SOISPgPtr, B3SOIsNodePrime, B3SOIgNode)
        TSTALLOC(B3SOISPdpPtr, B3SOIsNodePrime, B3SOIdNodePrime)
        TSTALLOC(B3SOISPspPtr, B3SOIsNodePrime, B3SOIsNodePrime)
        TSTALLOC(B3SOISPsPtr, B3SOIsNodePrime, B3SOIsNode)

        TSTALLOC(B3SOIDdPtr, B3SOIdNode, B3SOIdNode)
        TSTALLOC(B3SOIDdpPtr, B3SOIdNode, B3SOIdNodePrime)

        TSTALLOC(B3SOISsPtr, B3SOIsNode, B3SOIsNode)
        TSTALLOC(B3SOISspPtr, B3SOIsNode, B3SOIsNodePrime)

/* here for debugging purpose only */
         if ((here->B3SOIdebugMod > 1) || (here->B3SOIdebugMod == -1))
         {
            TSTALLOC(B3SOIVbsPtr, B3SOIvbsNode, B3SOIvbsNode) 
            TSTALLOC(B3SOIIdsPtr, B3SOIidsNode, B3SOIidsNode)
            TSTALLOC(B3SOIIcPtr, B3SOIicNode, B3SOIicNode)
            TSTALLOC(B3SOIIbsPtr, B3SOIibsNode, B3SOIibsNode)
            TSTALLOC(B3SOIIbdPtr, B3SOIibdNode, B3SOIibdNode)
            TSTALLOC(B3SOIIiiPtr, B3SOIiiiNode, B3SOIiiiNode)
            TSTALLOC(B3SOIIgidlPtr, B3SOIigidlNode, B3SOIigidlNode)
            TSTALLOC(B3SOIItunPtr, B3SOIitunNode, B3SOIitunNode)
            TSTALLOC(B3SOIIbpPtr, B3SOIibpNode, B3SOIibpNode)
            TSTALLOC(B3SOIAbeffPtr, B3SOIabeffNode, B3SOIabeffNode)
            TSTALLOC(B3SOIVbs0effPtr, B3SOIvbs0effNode, B3SOIvbs0effNode)
            TSTALLOC(B3SOIVbseffPtr, B3SOIvbseffNode, B3SOIvbseffNode)
            TSTALLOC(B3SOIXcPtr, B3SOIxcNode, B3SOIxcNode)
            TSTALLOC(B3SOICbbPtr, B3SOIcbbNode, B3SOIcbbNode)
            TSTALLOC(B3SOICbdPtr, B3SOIcbdNode, B3SOIcbdNode)
            TSTALLOC(B3SOICbgPtr, B3SOIcbgNode, B3SOIcbgNode)
            TSTALLOC(B3SOIqbPtr, B3SOIqbNode, B3SOIqbNode)
            TSTALLOC(B3SOIQbfPtr, B3SOIqbfNode, B3SOIqbfNode)
            TSTALLOC(B3SOIQjsPtr, B3SOIqjsNode, B3SOIqjsNode)
            TSTALLOC(B3SOIQjdPtr, B3SOIqjdNode, B3SOIqjdNode)

            /* clean up last */
            TSTALLOC(B3SOIGmPtr, B3SOIgmNode, B3SOIgmNode)
            TSTALLOC(B3SOIGmbsPtr, B3SOIgmbsNode, B3SOIgmbsNode)
            TSTALLOC(B3SOIGdsPtr, B3SOIgdsNode, B3SOIgdsNode)
            TSTALLOC(B3SOIGmePtr, B3SOIgmeNode, B3SOIgmeNode)
            TSTALLOC(B3SOIVbs0teffPtr, B3SOIvbs0teffNode, B3SOIvbs0teffNode)
            TSTALLOC(B3SOIVthPtr, B3SOIvthNode, B3SOIvthNode)
            TSTALLOC(B3SOIVgsteffPtr, B3SOIvgsteffNode, B3SOIvgsteffNode)
            TSTALLOC(B3SOIXcsatPtr, B3SOIxcsatNode, B3SOIxcsatNode)
            TSTALLOC(B3SOIVcscvPtr, B3SOIvcscvNode, B3SOIvcscvNode)
            TSTALLOC(B3SOIVdscvPtr, B3SOIvdscvNode, B3SOIvdscvNode)
            TSTALLOC(B3SOICbePtr, B3SOIcbeNode, B3SOIcbeNode)
            TSTALLOC(B3SOIDum1Ptr, B3SOIdum1Node, B3SOIdum1Node)
            TSTALLOC(B3SOIDum2Ptr, B3SOIdum2Node, B3SOIdum2Node)
            TSTALLOC(B3SOIDum3Ptr, B3SOIdum3Node, B3SOIdum3Node)
            TSTALLOC(B3SOIDum4Ptr, B3SOIdum4Node, B3SOIdum4Node)
            TSTALLOC(B3SOIDum5Ptr, B3SOIdum5Node, B3SOIdum5Node)
            TSTALLOC(B3SOIQaccPtr, B3SOIqaccNode, B3SOIqaccNode)
            TSTALLOC(B3SOIQsub0Ptr, B3SOIqsub0Node, B3SOIqsub0Node)
            TSTALLOC(B3SOIQsubs1Ptr, B3SOIqsubs1Node, B3SOIqsubs1Node)
            TSTALLOC(B3SOIQsubs2Ptr, B3SOIqsubs2Node, B3SOIqsubs2Node)
            TSTALLOC(B3SOIqePtr, B3SOIqeNode, B3SOIqeNode)
            TSTALLOC(B3SOIqdPtr, B3SOIqdNode, B3SOIqdNode)
            TSTALLOC(B3SOIqgPtr, B3SOIqgNode, B3SOIqgNode)
         }

        }
    }
    return(OK);
}  

int
B3SOIunsetup(inModel,ckt)
    GENmodel *inModel;
    CKTcircuit *ckt;
{
#ifndef HAS_BATCHSIM
    B3SOImodel *model;
    B3SOIinstance *here;
 
    for (model = (B3SOImodel *)inModel; model != NULL;
            model = model->B3SOInextModel)
    {
        for (here = model->B3SOIinstances; here != NULL;
                here=here->B3SOInextInstance)
        {
            if (here->B3SOIdNodePrime
                    && here->B3SOIdNodePrime != here->B3SOIdNode)
            {
                CKTdltNNum(ckt, here->B3SOIdNodePrime);
                here->B3SOIdNodePrime = 0;
            }
            if (here->B3SOIsNodePrime
                    && here->B3SOIsNodePrime != here->B3SOIsNode)
            {
                CKTdltNNum(ckt, here->B3SOIsNodePrime);
                here->B3SOIsNodePrime = 0;
            }
        }
    }
#endif
    return OK;
}

