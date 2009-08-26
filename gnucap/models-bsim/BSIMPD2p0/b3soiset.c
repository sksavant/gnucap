/* $Id: b3soiset.c,v 2.0 99/2/15 Pin Su Release $  */
/*
$Log:   b3soiset.c,v $
 * Revision 2.0  99/2/15  Pin Su
 * BSIMPD2.0 release
 *
*/
static char rcsid[] = "$Id: b3soiset.c,v 2.0 99/2/15 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soiset.c          98/5/01
Modified by Pin Su and Jan Feng	99/2/15
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
        if (!model->B3SOIrbodyGiven)  
            model->B3SOIrbody = 0.0;
        if (!model->B3SOIrbshGiven)  
            model->B3SOIrbsh = 0.0;
        if (!model->B3SOIrth0Given)  
            model->B3SOIrth0 = 0;
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
            model->B3SOIxbjt = 1;
/*
        if (!model->B3SOIxdifGiven)  
            model->B3SOIxdif = 1;
*/
        if (!model->B3SOIxdifGiven)
            model->B3SOIxdif = model->B3SOIxbjt;

        if (!model->B3SOIxrecGiven)  
            model->B3SOIxrec = 1;
        if (!model->B3SOIxtunGiven)  
            model->B3SOIxtun = 0;
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


/* v2.0 release */
        if (!model->B3SOIk1w1Given) 
           model->B3SOIk1w1 = 0.0;
        if (!model->B3SOIk1w2Given)
           model->B3SOIk1w2 = 0.0;
        if (!model->B3SOIketasGiven)
           model->B3SOIketas = 0.0;
        if (!model->B3SOIdwbcGiven)
           model->B3SOIdwbc = 0.0;
        if (!model->B3SOIbeta0Given)
           model->B3SOIbeta0 = 0.0;
        if (!model->B3SOIbeta1Given)
           model->B3SOIbeta1 = 0.0;
        if (!model->B3SOIbeta2Given)
           model->B3SOIbeta2 = 0.1;
        if (!model->B3SOIvdsatii0Given)
           model->B3SOIvdsatii0 = 0.9;
        if (!model->B3SOItiiGiven)
           model->B3SOItii = 0.0;
        if (!model->B3SOIliiGiven)
           model->B3SOIlii = 0.0;
        if (!model->B3SOIsii0Given)
           model->B3SOIsii0 = 0.5;
        if (!model->B3SOIsii1Given)
           model->B3SOIsii1 = 0.1;
        if (!model->B3SOIsii2Given)
           model->B3SOIsii2 = 0.0;
        if (!model->B3SOIsiidGiven)
           model->B3SOIsiid = 0.0;
        if (!model->B3SOIfbjtiiGiven)
           model->B3SOIfbjtii = 0.0;
        if (!model->B3SOIesatiiGiven)
            model->B3SOIesatii = 1e7;
        if (!model->B3SOIlnGiven)
           model->B3SOIln = 2e-6;
        if (!model->B3SOIvrec0Given)
           model->B3SOIvrec0 = 0;
        if (!model->B3SOIvtun0Given)
           model->B3SOIvtun0 = 0;
        if (!model->B3SOInbjtGiven)
           model->B3SOInbjt = 1.0;
        if (!model->B3SOIlbjt0Given)
           model->B3SOIlbjt0 = 0.20e-6;
        if (!model->B3SOIldif0Given)
           model->B3SOIldif0 = 1.0;
        if (!model->B3SOIvabjtGiven)
           model->B3SOIvabjt = 10.0;
        if (!model->B3SOIaelyGiven)
           model->B3SOIaely = 0;
        if (!model->B3SOIahliGiven)
           model->B3SOIahli = 0;
        if (!model->B3SOIrbodyGiven)
           model->B3SOIrbody = 0.0;
        if (!model->B3SOIrbshGiven)
           model->B3SOIrbsh = 0.0;
        if (!model->B3SOIntrecfGiven)
           model->B3SOIntrecf = 0.0;
        if (!model->B3SOIntrecrGiven)
           model->B3SOIntrecr = 0.0;
        if (!model->B3SOIndifGiven)
           model->B3SOIndif = -1.0;
        if (!model->B3SOIdlcbGiven)
           model->B3SOIdlcb = 0.0;
        if (!model->B3SOIfbodyGiven)
           model->B3SOIfbody = 1.0;
        if (!model->B3SOItcjswgGiven)
           model->B3SOItcjswg = 0.0;
        if (!model->B3SOItpbswgGiven)
           model->B3SOItpbswg = 0.0;
        if (!model->B3SOIacdeGiven)
           model->B3SOIacde = 1.0;
        if (!model->B3SOImoinGiven)
           model->B3SOImoin = 15.0;
        if (!model->B3SOIdelvtGiven)
           model->B3SOIdelvt = 0.0;
        if (!model->B3SOIkb1Given)
           model->B3SOIkb1 = 1.0;
        if (!model->B3SOIdlbgGiven)
           model->B3SOIdlbg = 0.0;


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


/* v2.0 release */
            if (!here->B3SOInbcGiven)
                here->B3SOInbc = 0;
            if (!here->B3SOInsegGiven)
                here->B3SOInseg = 1;
            if (!here->B3SOIpdbcpGiven)
                here->B3SOIpdbcp = 0;
            if (!here->B3SOIpsbcpGiven)
                here->B3SOIpsbcp = 0;
            if (!here->B3SOIagbcpGiven)
                here->B3SOIagbcp = 0;
            if (!here->B3SOIaebcpGiven)
                here->B3SOIaebcp = 0;


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
            model->B3SOIcsit = 1/(1/model->B3SOIcox + 1/model->B3SOIcsieff);
            model->B3SOIcboxt = 1/(1/model->B3SOIcbox + 1/model->B3SOIcsieff);
            nfb0 = 1/(1 + model->B3SOIcbox / model->B3SOIcsit);


            here->B3SOIfloat = 0;
            if (here->B3SOIpNode == -1) {  /*  floating body case -- 4-node  */
                error = CKTmkVolt(ckt,&tmp,here->B3SOIname,"Body");
                if (error) return(error);
                here->B3SOIbNode = tmp->number;
                here->B3SOIpNode = 0; 
                here->B3SOIfloat = 1;
                here->B3SOIbodyMod = 0;
            }
            else /* the 5th Node has been assigned */
            {
              if (!here->B3SOItnodeoutGiven) { /* if t-node not assigned  */
                 if (here->B3SOIbNode == -1)
                 { /* 5-node body tie, bNode has not been assigned */
                    if ((model->B3SOIrbody == 0.0) && (model->B3SOIrbsh == 0.0))
                    { /* ideal body tie, pNode is not used */
                       here->B3SOIbNode = here->B3SOIpNode;
                       here->B3SOIbodyMod = 2;
                    }
                    else { /* nonideal body tie */
                      error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Body");
                      if (error) return(error);
                      here->B3SOIbNode = tmp->number;
                      here->B3SOIbodyMod = 1;
                    }
                 }
                 else { /* 6-node body tie, bNode has been assigned */
                    if ((model->B3SOIrbody == 0.0) && (model->B3SOIrbsh == 0.0))
                    { 
                       printf("\n      Warning: model parameter rbody=0!\n");
                       model->B3SOIrbody = 1e0;
                       here->B3SOIbodyMod = 1;
                    }
                    else { /* nonideal body tie */
                        here->B3SOIbodyMod = 1;
                    }
                 }
              }
              else {    /*  t-node assigned   */
                 if (here->B3SOIbNode == -1)
                 { /* 4 nodes & t-node, floating body */
                    error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Body");
                    if (error) return(error);
                    here->B3SOIbNode = tmp->number;
                    here->B3SOItempNode = here->B3SOIpNode;
                    here->B3SOIpNode = 0; 
                    here->B3SOIfloat = 1;
                    here->B3SOIbodyMod = 0;
                 }
                 else { /* 5 or 6 nodes & t-node, body-contact device */
                   if (here->B3SOItempNode == -1) { /* 5 nodes & tnode */
                     if ((model->B3SOIrbody == 0.0) && (model->B3SOIrbsh == 0.0))
                     { /* ideal body tie, pNode is not used */
                        here->B3SOItempNode = here->B3SOIbNode;
                        here->B3SOIbNode = here->B3SOIpNode;
                        here->B3SOIbodyMod = 2;
                     }
                     else { /* nonideal body tie */
                       here->B3SOItempNode = here->B3SOIbNode;
                       error = CKTmkVolt(ckt, &tmp, here->B3SOIname, "Body");
                       if (error) return(error);
                       here->B3SOIbNode = tmp->number;
                       here->B3SOIbodyMod = 1;
                     }
                   }
                   else {  /* 6 nodes & t-node */
                     if ((model->B3SOIrbody == 0.0) && (model->B3SOIrbsh == 0.0))
                     { 
                        printf("\n      Warning: model parameter rbody=0!\n");
                        model->B3SOIrbody = 1e0;
                        here->B3SOIbodyMod = 1;
                     }
                     else { /* nonideal body tie */
                        here->B3SOIbodyMod = 1;
                     }
                   }
                 }
              }
            }


            if ((model->B3SOIshMod == 1) && (here->B3SOIrth0!=0))
            {
               if (here->B3SOItempNode == -1) {
	          error = CKTmkVolt(ckt,&tmp,here->B3SOIname,"Temp");
                  if (error) return(error);
                     here->B3SOItempNode = tmp->number;
               }

            } else {
                here->B3SOItempNode = 0;
            }

/* here for debugging purpose only */
            if (here->B3SOIdebugMod != 0)
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
        }

        TSTALLOC(B3SOIEbPtr, B3SOIeNode, B3SOIbNode)
        TSTALLOC(B3SOIGbPtr, B3SOIgNode, B3SOIbNode)
        TSTALLOC(B3SOIDPbPtr, B3SOIdNodePrime, B3SOIbNode)
        TSTALLOC(B3SOISPbPtr, B3SOIsNodePrime, B3SOIbNode)
        TSTALLOC(B3SOIBePtr, B3SOIbNode, B3SOIeNode)
        TSTALLOC(B3SOIBgPtr, B3SOIbNode, B3SOIgNode)
        TSTALLOC(B3SOIBdpPtr, B3SOIbNode, B3SOIdNodePrime)
        TSTALLOC(B3SOIBspPtr, B3SOIbNode, B3SOIsNodePrime)
        TSTALLOC(B3SOIBbPtr, B3SOIbNode, B3SOIbNode)

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
         if (here->B3SOIdebugMod != 0)
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
            TSTALLOC(B3SOICbbPtr, B3SOIcbbNode, B3SOIcbbNode)
            TSTALLOC(B3SOICbdPtr, B3SOIcbdNode, B3SOIcbdNode)
            TSTALLOC(B3SOICbgPtr, B3SOIcbgNode, B3SOIcbgNode)
            TSTALLOC(B3SOIqbPtr, B3SOIqbNode, B3SOIqbNode)
            TSTALLOC(B3SOIQbfPtr, B3SOIqbfNode, B3SOIqbfNode)
            TSTALLOC(B3SOIQjsPtr, B3SOIqjsNode, B3SOIqjsNode)
            TSTALLOC(B3SOIQjdPtr, B3SOIqjdNode, B3SOIqjdNode)

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

