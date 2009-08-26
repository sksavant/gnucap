/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3cvtest.c
**********/

#include "spice.h"
#include <stdio.h>
#include <math.h>
#include "util.h"
#include "cktdefs.h"
#include "bsim3def.h"
#include "trandefs.h"
#include "const.h"
#include "devdefs.h"
#include "sperror.h"
#include "suffix.h"


int
BSIM3convTest(inModel,ckt)
GENmodel *inModel;
register CKTcircuit *ckt;
{
register BSIM3model *model = (BSIM3model*)inModel;
register BSIM3instance *here;
double delvbd, delvbs, delvds, delvgd, delvgs, vbd, vbs, vds;
double cbd, cbhat, cbs, cd, cdhat, tol, vgd, vgdo, vgs;

    /*  loop through all the BSIM3 device models */
    for (; model != NULL; model = model->BSIM3nextModel)
    {    /* loop through all the instances of the model */
         for (here = model->BSIM3instances; here != NULL ;
              here=here->BSIM3nextInstance) 
	 {    vbs = model->BSIM3type 
		  * (*(ckt->CKTrhsOld+here->BSIM3bNode) 
		  - *(ckt->CKTrhsOld+here->BSIM3sNodePrime));
              vgs = model->BSIM3type
		  * (*(ckt->CKTrhsOld+here->BSIM3gNode) 
		  - *(ckt->CKTrhsOld+here->BSIM3sNodePrime));
              vds = model->BSIM3type
		  * (*(ckt->CKTrhsOld+here->BSIM3dNodePrime) 
		  - *(ckt->CKTrhsOld+here->BSIM3sNodePrime));
              vbd = vbs - vds;
              vgd = vgs - vds;
              vgdo = *(ckt->CKTstate0 + here->BSIM3vgs) 
		   - *(ckt->CKTstate0 + here->BSIM3vds);
              delvbs = vbs - *(ckt->CKTstate0 + here->BSIM3vbs);
              delvbd = vbd - *(ckt->CKTstate0 + here->BSIM3vbd);
              delvgs = vgs - *(ckt->CKTstate0 + here->BSIM3vgs);
              delvds = vds - *(ckt->CKTstate0 + here->BSIM3vds);
              delvgd = vgd-vgdo;

              cd = here->BSIM3cd;
              if (here->BSIM3mode >= 0)
	      {   cdhat = cd - here->BSIM3gbd * delvbd 
			+ here->BSIM3gmbs * delvbs + here->BSIM3gm * delvgs
			+ here->BSIM3gds * delvds;
              }
	      else
	      {   cdhat = cd - (here->BSIM3gbd - here->BSIM3gmbs) * delvbd 
			- here->BSIM3gm * delvgd + here->BSIM3gds * delvds;
              }

            /*
             *  check convergence
             */
              if ((here->BSIM3off == 0)  || (!(ckt->CKTmode & MODEINITFIX)))
	      {   tol = ckt->CKTreltol * MAX(FABS(cdhat), FABS(cd))
		      + ckt->CKTabstol;
                  if (FABS(cdhat - cd) >= tol)
		  {   ckt->CKTnoncon++;
                      return(OK);
                  } 
                  cbs = here->BSIM3cbs;
                  cbd = here->BSIM3cbd;
                  cbhat = cbs + cbd + here->BSIM3gbd * delvbd 
		        + here->BSIM3gbs * delvbs;
                  tol = ckt->CKTreltol * MAX(FABS(cbhat), FABS(cbs + cbd))
		      + ckt->CKTabstol;
                  if (FABS(cbhat - (cbs + cbd)) > tol) 
		  {   ckt->CKTnoncon++;
                      return(OK);
                  }
              }
         }
    }
    return(OK);
}

