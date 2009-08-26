/* $Id: BSIMFD2.1 99/9/27 Pin Su  Release $  */
/*
$Log:   b3soickeck.c, FD2.1 $
 * Revision 2.1  99/9/27 Pin Su
 * BSIMFD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soickeck.c, FD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soicheck.c          98/5/01
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

int
B3SOIcheckModel(model, here, ckt)
register B3SOImodel *model;
register B3SOIinstance *here;
CKTcircuit *ckt;
{
struct b3soiSizeDependParam *pParam;
int Fatal_Flag = 0;
FILE *fplog;
    
    if ((fplog = fopen("b3soiv1check.log", "w")) != NULL)
    {   pParam = here->pParam;
	fprintf(fplog, "B3SOIV3 Parameter Check\n");
	fprintf(fplog, "Model = %s\n", model->B3SOImodName);
	fprintf(fplog, "W = %g, L = %g\n", here->B3SOIw, here->B3SOIl);
   	    

            if (pParam->B3SOInlx < -pParam->B3SOIleff)
	    {   fprintf(fplog, "Fatal: Nlx = %g is less than -Leff.\n",
			pParam->B3SOInlx);
	        printf("Fatal: Nlx = %g is less than -Leff.\n",
			pParam->B3SOInlx);
		Fatal_Flag = 1;
            }

	if (model->B3SOItox <= 0.0)
	{   fprintf(fplog, "Fatal: Tox = %g is not positive.\n",
		    model->B3SOItox);
	    printf("Fatal: Tox = %g is not positive.\n", model->B3SOItox);
	    Fatal_Flag = 1;
	}

	if (model->B3SOItbox <= 0.0)
	{   fprintf(fplog, "Fatal: Tbox = %g is not positive.\n",
		    model->B3SOItbox);
	    printf("Fatal: Tbox = %g is not positive.\n", model->B3SOItbox);
	    Fatal_Flag = 1;
	}

	if (pParam->B3SOInpeak <= 0.0)
	{   fprintf(fplog, "Fatal: Nch = %g is not positive.\n",
		    pParam->B3SOInpeak);
	    printf("Fatal: Nch = %g is not positive.\n",
		   pParam->B3SOInpeak);
	    Fatal_Flag = 1;
	}
	if (pParam->B3SOIngate < 0.0)
	{   fprintf(fplog, "Fatal: Ngate = %g is not positive.\n",
		    pParam->B3SOIngate);
	    printf("Fatal: Ngate = %g Ngate is not positive.\n",
		   pParam->B3SOIngate);
	    Fatal_Flag = 1;
	}
	if (pParam->B3SOIngate > 1.e25)
	{   fprintf(fplog, "Fatal: Ngate = %g is too high.\n",
		    pParam->B3SOIngate);
	    printf("Fatal: Ngate = %g Ngate is too high\n",
		   pParam->B3SOIngate);
	    Fatal_Flag = 1;
	}

	if (model->B3SOIdvbd1 < 0.0)
	{   fprintf(fplog, "Fatal: Dvbd1 = %g is negative.\n",
		    model->B3SOIdvbd1);   
	    printf("Fatal: Dvbd1 = %g is negative.\n", model->B3SOIdvbd1);   
	    Fatal_Flag = 1;
	}

	if (pParam->B3SOIdvt1 < 0.0)
	{   fprintf(fplog, "Fatal: Dvt1 = %g is negative.\n",
		    pParam->B3SOIdvt1);   
	    printf("Fatal: Dvt1 = %g is negative.\n", pParam->B3SOIdvt1);   
	    Fatal_Flag = 1;
	}
	    
	if (pParam->B3SOIdvt1w < 0.0)
	{   fprintf(fplog, "Fatal: Dvt1w = %g is negative.\n",
		    pParam->B3SOIdvt1w);
	    printf("Fatal: Dvt1w = %g is negative.\n", pParam->B3SOIdvt1w);
	    Fatal_Flag = 1;
	}
	    
	if (pParam->B3SOIw0 == -pParam->B3SOIweff)
	{   fprintf(fplog, "Fatal: (W0 + Weff) = 0 cauing divided-by-zero.\n");
	    printf("Fatal: (W0 + Weff) = 0 cauing divided-by-zero.\n");
	    Fatal_Flag = 1;
        }   

	if (pParam->B3SOIdsub < 0.0)
	{   fprintf(fplog, "Fatal: Dsub = %g is negative.\n", pParam->B3SOIdsub);
	    printf("Fatal: Dsub = %g is negative.\n", pParam->B3SOIdsub);
	    Fatal_Flag = 1;
	}
	if (pParam->B3SOIb1 == -pParam->B3SOIweff)
	{   fprintf(fplog, "Fatal: (B1 + Weff) = 0 causing divided-by-zero.\n");
	    printf("Fatal: (B1 + Weff) = 0 causing divided-by-zero.\n");
	    Fatal_Flag = 1;
        }  
        if (pParam->B3SOIu0temp <= 0.0)
	{   fprintf(fplog, "Fatal: u0 at current temperature = %g is not positive.\n", pParam->B3SOIu0temp);
	    printf("Fatal: u0 at current temperature = %g is not positive.\n",
		   pParam->B3SOIu0temp);
	    Fatal_Flag = 1;
        }
    
/* Check delta parameter */      
        if (pParam->B3SOIdelta < 0.0)
	{   fprintf(fplog, "Fatal: Delta = %g is less than zero.\n",
		    pParam->B3SOIdelta);
	    printf("Fatal: Delta = %g is less than zero.\n", pParam->B3SOIdelta);
	    Fatal_Flag = 1;
        }      

	if (pParam->B3SOIvsattemp <= 0.0)
	{   fprintf(fplog, "Fatal: Vsat at current temperature = %g is not positive.\n", pParam->B3SOIvsattemp);
	    printf("Fatal: Vsat at current temperature = %g is not positive.\n",
		   pParam->B3SOIvsattemp);
	    Fatal_Flag = 1;
	}
/* Check Rout parameters */
	if (pParam->B3SOIpclm <= 0.0)
	{   fprintf(fplog, "Fatal: Pclm = %g is not positive.\n", pParam->B3SOIpclm);
	    printf("Fatal: Pclm = %g is not positive.\n", pParam->B3SOIpclm);
	    Fatal_Flag = 1;
	}

	if (pParam->B3SOIdrout < 0.0)
	{   fprintf(fplog, "Fatal: Drout = %g is negative.\n", pParam->B3SOIdrout);
	    printf("Fatal: Drout = %g is negative.\n", pParam->B3SOIdrout);
	    Fatal_Flag = 1;
	}
      if ( model->B3SOIunitLengthGateSidewallJctCap > 0.0)
      {
	if (here->B3SOIdrainPerimeter < pParam->B3SOIweff)
	{   fprintf(fplog, "Warning: Pd = %g is less than W.\n",
		    here->B3SOIdrainPerimeter);
	   printf("Warning: Pd = %g is less than W.\n",
		    here->B3SOIdrainPerimeter);
            here->B3SOIdrainPerimeter =pParam->B3SOIweff; 
	}
	if (here->B3SOIsourcePerimeter < pParam->B3SOIweff)
	{   fprintf(fplog, "Warning: Ps = %g is less than W.\n",
		    here->B3SOIsourcePerimeter);
	   printf("Warning: Ps = %g is less than W.\n",
		    here->B3SOIsourcePerimeter);
            here->B3SOIsourcePerimeter =pParam->B3SOIweff;
	}
      }
/* Check capacitance parameters */
        if (pParam->B3SOIclc < 0.0)
	{   fprintf(fplog, "Fatal: Clc = %g is negative.\n", pParam->B3SOIclc);
	    printf("Fatal: Clc = %g is negative.\n", pParam->B3SOIclc);
	    Fatal_Flag = 1;
        }      
      if (model->B3SOIparamChk ==1)
      {
/* Check L and W parameters */ 
	if (pParam->B3SOIleff <= 5.0e-8)
	{   fprintf(fplog, "Warning: Leff = %g may be too small.\n",
	            pParam->B3SOIleff);
	    printf("Warning: Leff = %g may be too small.\n",
		    pParam->B3SOIleff);
	}    
	
	if (pParam->B3SOIleffCV <= 5.0e-8)
	{   fprintf(fplog, "Warning: Leff for CV = %g may be too small.\n",
		    pParam->B3SOIleffCV);
	    printf("Warning: Leff for CV = %g may be too small.\n",
		   pParam->B3SOIleffCV);
	}  
	
        if (pParam->B3SOIweff <= 1.0e-7)
	{   fprintf(fplog, "Warning: Weff = %g may be too small.\n",
		    pParam->B3SOIweff);
	    printf("Warning: Weff = %g may be too small.\n",
		   pParam->B3SOIweff);
	}             
	
	if (pParam->B3SOIweffCV <= 1.0e-7)
	{   fprintf(fplog, "Warning: Weff for CV = %g may be too small.\n",
		    pParam->B3SOIweffCV);
	    printf("Warning: Weff for CV = %g may be too small.\n",
		   pParam->B3SOIweffCV);
	}        
	
/* Check threshold voltage parameters */
	if (pParam->B3SOInlx < 0.0)
	{   fprintf(fplog, "Warning: Nlx = %g is negative.\n", pParam->B3SOInlx);
	   printf("Warning: Nlx = %g is negative.\n", pParam->B3SOInlx);
        }
	if (model->B3SOItox < 1.0e-9)
	{   fprintf(fplog, "Warning: Tox = %g is less than 10A.\n",
	            model->B3SOItox);
	    printf("Warning: Tox = %g is less than 10A.\n", model->B3SOItox);
        }

        if (pParam->B3SOInpeak <= 1.0e15)
	{   fprintf(fplog, "Warning: Nch = %g may be too small.\n",
	            pParam->B3SOInpeak);
	    printf("Warning: Nch = %g may be too small.\n",
	           pParam->B3SOInpeak);
	}
	else if (pParam->B3SOInpeak >= 1.0e21)
	{   fprintf(fplog, "Warning: Nch = %g may be too large.\n",
	            pParam->B3SOInpeak);
	    printf("Warning: Nch = %g may be too large.\n",
	           pParam->B3SOInpeak);
	}

	if (fabs(pParam->B3SOInsub) >= 1.0e21)
	{   fprintf(fplog, "Warning: Nsub = %g may be too large.\n",
	            pParam->B3SOInsub);
	    printf("Warning: Nsub = %g may be too large.\n",
	           pParam->B3SOInsub);
	}

	if ((pParam->B3SOIngate > 0.0) &&
	    (pParam->B3SOIngate <= 1.e18))
	{   fprintf(fplog, "Warning: Ngate = %g is less than 1.E18cm^-3.\n",
	            pParam->B3SOIngate);
	    printf("Warning: Ngate = %g is less than 1.E18cm^-3.\n",
	           pParam->B3SOIngate);
	}

        if (model->B3SOIdvbd0 < 0.0)
	{   fprintf(fplog, "Warning: Dvbd0 = %g is negative.\n",
		    model->B3SOIdvbd0);   
	    printf("Warning: Dvbd0 = %g is negative.\n", model->B3SOIdvbd0);   
	}
       
        if (pParam->B3SOIdvt0 < 0.0)
	{   fprintf(fplog, "Warning: Dvt0 = %g is negative.\n",
		    pParam->B3SOIdvt0);   
	    printf("Warning: Dvt0 = %g is negative.\n", pParam->B3SOIdvt0);   
	}
	    
	if (fabs(1.0e-6 / (pParam->B3SOIw0 + pParam->B3SOIweff)) > 10.0)
	{   fprintf(fplog, "Warning: (W0 + Weff) may be too small.\n");
	    printf("Warning: (W0 + Weff) may be too small.\n");
        }

/* Check subthreshold parameters */
	if (pParam->B3SOInfactor < 0.0)
	{   fprintf(fplog, "Warning: Nfactor = %g is negative.\n",
		    pParam->B3SOInfactor);
	    printf("Warning: Nfactor = %g is negative.\n", pParam->B3SOInfactor);
	}
	if (model->B3SOIkb3 < 0.0)
	{   fprintf(fplog, "Warning: Kb3 = %g is negative.\n",
		    model->B3SOIkb3);
	    printf("Warning: Kb3 = %g is negative.\n", model->B3SOIkb3);
	}

	if (pParam->B3SOIcdsc < 0.0)
	{   fprintf(fplog, "Warning: Cdsc = %g is negative.\n",
		    pParam->B3SOIcdsc);
	    printf("Warning: Cdsc = %g is negative.\n", pParam->B3SOIcdsc);
	}
	if (pParam->B3SOIcdscd < 0.0)
	{   fprintf(fplog, "Warning: Cdscd = %g is negative.\n",
		    pParam->B3SOIcdscd);
	    printf("Warning: Cdscd = %g is negative.\n", pParam->B3SOIcdscd);
	}
/* Check DIBL parameters */
	if (pParam->B3SOIeta0 < 0.0)
	{   fprintf(fplog, "Warning: Eta0 = %g is negative.\n",
		    pParam->B3SOIeta0); 
	    printf("Warning: Eta0 = %g is negative.\n", pParam->B3SOIeta0); 
	}
	      
/* Check Abulk parameters */	    
        if (fabs(1.0e-6 / (pParam->B3SOIb1 + pParam->B3SOIweff)) > 10.0)
       	{   fprintf(fplog, "Warning: (B1 + Weff) may be too small.\n");
       	    printf("Warning: (B1 + Weff) may be too small.\n");
        }    

	if (model->B3SOIadice0 > 1.0)
	{   fprintf(fplog, "Warning: Adice0 = %g should be smaller than 1.\n",
		    model->B3SOIadice0); 
	    printf("Warning: Adice0 = %g should be smaller than 1.\n", model->B3SOIadice0); 
	}

	if (model->B3SOIabp < 0.2)
	{   fprintf(fplog, "Warning: Abp = %g is too small.\n",
		    model->B3SOIabp); 
	    printf("Warning: Abp = %g is too small.\n", model->B3SOIabp); 
	}
    
	if ((model->B3SOImxc < -1.0) || (model->B3SOImxc > 1.0))
	{   fprintf(fplog, "Warning: Mxc = %g should be within (-1, 1).\n",
		    model->B3SOImxc); 
	    printf("Warning: Mxc = %g should be within (-1, 1).\n", model->B3SOImxc); 
	}

/* Check Saturation parameters */
     	if (pParam->B3SOIa2 < 0.01)
	{   fprintf(fplog, "Warning: A2 = %g is too small. Set to 0.01.\n", pParam->B3SOIa2);
	    printf("Warning: A2 = %g is too small. Set to 0.01.\n",
		   pParam->B3SOIa2);
	    pParam->B3SOIa2 = 0.01;
	}
	else if (pParam->B3SOIa2 > 1.0)
	{   fprintf(fplog, "Warning: A2 = %g is larger than 1. A2 is set to 1 and A1 is set to 0.\n",
		    pParam->B3SOIa2);
	    printf("Warning: A2 = %g is larger than 1. A2 is set to 1 and A1 is set to 0.\n",
		   pParam->B3SOIa2);
	    pParam->B3SOIa2 = 1.0;
	    pParam->B3SOIa1 = 0.0;

	}

	if (pParam->B3SOIrdsw < 0.0)
	{   fprintf(fplog, "Warning: Rdsw = %g is negative. Set to zero.\n",
		    pParam->B3SOIrdsw);
	    printf("Warning: Rdsw = %g is negative. Set to zero.\n",
		   pParam->B3SOIrdsw);
	    pParam->B3SOIrdsw = 0.0;
	    pParam->B3SOIrds0 = 0.0;
	}
	else if ((pParam->B3SOIrds0 > 0.0) && (pParam->B3SOIrds0 < 0.001))
	{   fprintf(fplog, "Warning: Rds at current temperature = %g is less than 0.001 ohm. Set to zero.\n",
		    pParam->B3SOIrds0);
	    printf("Warning: Rds at current temperature = %g is less than 0.001 ohm. Set to zero.\n",
		   pParam->B3SOIrds0);
	    pParam->B3SOIrds0 = 0.0;
	}
	 if (pParam->B3SOIvsattemp < 1.0e3)
	{   fprintf(fplog, "Warning: Vsat at current temperature = %g may be too small.\n", pParam->B3SOIvsattemp);
	   printf("Warning: Vsat at current temperature = %g may be too small.\n", pParam->B3SOIvsattemp);
	}

	if (pParam->B3SOIpdibl1 < 0.0)
	{   fprintf(fplog, "Warning: Pdibl1 = %g is negative.\n",
		    pParam->B3SOIpdibl1);
	    printf("Warning: Pdibl1 = %g is negative.\n", pParam->B3SOIpdibl1);
	}
	if (pParam->B3SOIpdibl2 < 0.0)
	{   fprintf(fplog, "Warning: Pdibl2 = %g is negative.\n",
		    pParam->B3SOIpdibl2);
	    printf("Warning: Pdibl2 = %g is negative.\n", pParam->B3SOIpdibl2);
	}
/* Check overlap capacitance parameters */
        if (model->B3SOIcgdo < 0.0)
	{   fprintf(fplog, "Warning: cgdo = %g is negative. Set to zero.\n", model->B3SOIcgdo);
	    printf("Warning: cgdo = %g is negative. Set to zero.\n", model->B3SOIcgdo);
	    model->B3SOIcgdo = 0.0;
        }      
        if (model->B3SOIcgso < 0.0)
	{   fprintf(fplog, "Warning: cgso = %g is negative. Set to zero.\n", model->B3SOIcgso);
	    printf("Warning: cgso = %g is negative. Set to zero.\n", model->B3SOIcgso);
	    model->B3SOIcgso = 0.0;
        }      
        if (model->B3SOIcgeo < 0.0)
	{   fprintf(fplog, "Warning: cgeo = %g is negative. Set to zero.\n", model->B3SOIcgeo);
	    printf("Warning: cgeo = %g is negative. Set to zero.\n", model->B3SOIcgeo);
	    model->B3SOIcgeo = 0.0;
        }

	if (model->B3SOIntun < 0.0)
	{   fprintf(fplog, "Warning: Ntun = %g is negative.\n",
		    model->B3SOIntun); 
	    printf("Warning: Ntun = %g is negative.\n", model->B3SOIntun); 
	}

	if (model->B3SOIndiode < 0.0)
	{   fprintf(fplog, "Warning: Ndiode = %g is negative.\n",
		    model->B3SOIndiode); 
	    printf("Warning: Ndiode = %g is negative.\n", model->B3SOIndiode); 
	}

	if (model->B3SOIisbjt < 0.0)
	{   fprintf(fplog, "Warning: Isbjt = %g is negative.\n",
		    model->B3SOIisbjt); 
	    printf("Warning: Isbjt = %g is negative.\n", model->B3SOIisbjt); 
	}

	if (model->B3SOIisdif < 0.0)
	{   fprintf(fplog, "Warning: Isdif = %g is negative.\n",
		    model->B3SOIisdif); 
	    printf("Warning: Isdif = %g is negative.\n", model->B3SOIisdif); 
	}

	if (model->B3SOIisrec < 0.0)
	{   fprintf(fplog, "Warning: Isrec = %g is negative.\n",
		    model->B3SOIisrec); 
	    printf("Warning: Isrec = %g is negative.\n", model->B3SOIisrec); 
	}

	if (model->B3SOIistun < 0.0)
	{   fprintf(fplog, "Warning: Istun = %g is negative.\n",
		    model->B3SOIistun); 
	    printf("Warning: Istun = %g is negative.\n", model->B3SOIistun); 
	}

	if (model->B3SOIedl < 0.0)
	{   fprintf(fplog, "Warning: Edl = %g is negative.\n",
		    model->B3SOIedl); 
	    printf("Warning: Edl = %g is negative.\n", model->B3SOIedl); 
	}

	if (model->B3SOIkbjt1 < 0.0)
	{   fprintf(fplog, "Warning: Kbjt1 = %g is negative.\n",
		    model->B3SOIkbjt1); 
	    printf("Warning: kbjt1 = %g is negative.\n", model->B3SOIkbjt1); 
	}

	if (model->B3SOItt < 0.0)
	{   fprintf(fplog, "Warning: Tt = %g is negative.\n",
		    model->B3SOItt); 
	    printf("Warning: Tt = %g is negative.\n", model->B3SOItt); 
	}

	if (model->B3SOIcsdmin < 0.0)
	{   fprintf(fplog, "Warning: Csdmin = %g is negative.\n",
		    model->B3SOIcsdmin); 
	    printf("Warning: Csdmin = %g is negative.\n", model->B3SOIcsdmin); 
	}

	if (model->B3SOIcsdesw < 0.0)
	{   fprintf(fplog, "Warning: Csdesw = %g is negative.\n",
		    model->B3SOIcsdesw); 
	    printf("Warning: Csdesw = %g is negative.\n", model->B3SOIcsdesw); 
	}

	if ((model->B3SOIasd < 0.0) || (model->B3SOImxc > 1.0))
	{   fprintf(fplog, "Warning: Asd = %g should be within (0, 1).\n",
		    model->B3SOIasd); 
	    printf("Warning: Asd = %g should be within (0, 1).\n", model->B3SOIasd); 
	}

	if (model->B3SOIrth0 < 0.0)
	{   fprintf(fplog, "Warning: Rth0 = %g is negative.\n",
		    model->B3SOIrth0); 
	    printf("Warning: Rth0 = %g is negative.\n", model->B3SOIrth0); 
	}

	if (model->B3SOIcth0 < 0.0)
	{   fprintf(fplog, "Warning: Cth0 = %g is negative.\n",
		    model->B3SOIcth0); 
	    printf("Warning: Cth0 = %g is negative.\n", model->B3SOIcth0); 
	}

	if (model->B3SOIrbody < 0.0)
	{   fprintf(fplog, "Warning: Rbody = %g is negative.\n",
		    model->B3SOIrbody); 
	    printf("Warning: Rbody = %g is negative.\n", model->B3SOIrbody); 
	}

	if (model->B3SOIrbsh < 0.0)
	{   fprintf(fplog, "Warning: Rbsh = %g is negative.\n",
		    model->B3SOIrbsh); 
	    printf("Warning: Rbsh = %g is negative.\n", model->B3SOIrbsh); 
	}

	if (model->B3SOIxj > model->B3SOItsi)
	{   fprintf(fplog, "Warning: Xj = %g is thicker than Tsi = %g.\n",
		    model->B3SOIxj, model->B3SOItsi); 
	    printf("Warning: Xj = %g is thicker than Tsi = %g.\n", 
		    model->B3SOIxj, model->B3SOItsi); 
	}

        if (model->B3SOIcapMod < 2)
	{   fprintf(fplog, "Warning: capMod < 2 is not supported by BSIM3SOI.\n");
	    printf("Warning: Warning: capMod < 2 is not supported by BSIM3SOI.\n");
	}

	if (model->B3SOIcii > 2.0)
	{   fprintf(fplog, "Warning: Cii = %g is larger than 2.0.\n", model->B3SOIcii);
	    printf("Warning: Cii = %g is larger than 2.0.\n", model->B3SOIcii);
	}

	if (model->B3SOIdii > 1.5)
	{   fprintf(fplog, "Warning: Dii = %g is larger than 1.5.\n", model->B3SOIcii);
	    printf("Warning: Dii = %g is too larger than 1.5.\n", model->B3SOIcii);
	}

     }/* loop for the parameter check for warning messages */      
	fclose(fplog);
    }
    else
    {   fprintf(stderr, "Warning: Can't open log file. Parameter checking skipped.\n");
    }

    return(Fatal_Flag);
}

