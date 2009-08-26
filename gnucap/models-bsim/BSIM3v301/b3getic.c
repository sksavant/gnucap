/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3getic.c
**********/

#include "spice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "bsim3def.h"
#include "sperror.h"
#include "suffix.h"


int
BSIM3getic(inModel,ckt)
GENmodel *inModel;
CKTcircuit *ckt;
{
BSIM3model *model = (BSIM3model*)inModel;
BSIM3instance *here;

    for (; model ; model = model->BSIM3nextModel) 
    {    for (here = model->BSIM3instances; here; here = here->BSIM3nextInstance)
	 {    if(!here->BSIM3icVBSGiven) 
	      {  here->BSIM3icVBS = *(ckt->CKTrhs + here->BSIM3bNode) 
				  - *(ckt->CKTrhs + here->BSIM3sNode);
              }
              if (!here->BSIM3icVDSGiven) 
	      {   here->BSIM3icVDS = *(ckt->CKTrhs + here->BSIM3dNode) 
				   - *(ckt->CKTrhs + here->BSIM3sNode);
              }
              if (!here->BSIM3icVGSGiven) 
	      {   here->BSIM3icVGS = *(ckt->CKTrhs + here->BSIM3gNode) 
				   - *(ckt->CKTrhs + here->BSIM3sNode);
              }
         }
    }
    return(OK);
}


