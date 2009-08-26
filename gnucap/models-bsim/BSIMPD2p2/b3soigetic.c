/* $Id: b3soigetic.c,v 2.2 00/3/1 Pin Su Release $  */
/*
$Log:   b3soigetic.c,v $
 * Revision 2.2  00/3/1  Pin Su
 * BSIMPD2.2 release
 *
*/
static char rcsid[] = "$Id: b3soigetic.c,v 2.2 00/3/1 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soigetic.c          98/5/01
**********/


#include "spice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "b3soidef.h"
#include "sperror.h"
#include "suffix.h"


int
B3SOIgetic(inModel,ckt)
GENmodel *inModel;
CKTcircuit *ckt;
{
B3SOImodel *model = (B3SOImodel*)inModel;
B3SOIinstance *here;

    for (; model ; model = model->B3SOInextModel) 
    {    for (here = model->B3SOIinstances; here; here = here->B3SOInextInstance)
	 {    if(!here->B3SOIicVBSGiven) 
	      {  here->B3SOIicVBS = *(ckt->CKTrhs + here->B3SOIbNode) 
				  - *(ckt->CKTrhs + here->B3SOIsNode);
              }
              if (!here->B3SOIicVDSGiven) 
	      {   here->B3SOIicVDS = *(ckt->CKTrhs + here->B3SOIdNode) 
				   - *(ckt->CKTrhs + here->B3SOIsNode);
              }
              if (!here->B3SOIicVGSGiven) 
	      {   here->B3SOIicVGS = *(ckt->CKTrhs + here->B3SOIgNode) 
				   - *(ckt->CKTrhs + here->B3SOIsNode);
              }
              if (!here->B3SOIicVESGiven) 
	      {   here->B3SOIicVES = *(ckt->CKTrhs + here->B3SOIeNode) 
				   - *(ckt->CKTrhs + here->B3SOIsNode);
              }
              if (!here->B3SOIicVPSGiven) 
	      {   here->B3SOIicVPS = *(ckt->CKTrhs + here->B3SOIpNode) 
				   - *(ckt->CKTrhs + here->B3SOIsNode);
              }
         }
    }
    return(OK);
}


