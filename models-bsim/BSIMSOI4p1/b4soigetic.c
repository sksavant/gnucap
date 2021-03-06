/***  B4SOI 03/06/2009 Wenwei Yang Release   ***/

static char rcsid[] = "$Id: b4soigetic.c  03/06/2009 Wenwei Yang Release $";

/**********
 * Copyright 2009 Regents of the University of California.  All rights reserved.
 * Authors: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
 * Authors: 1999-2004 Pin Su, Hui Wan, Wei Jin, b3soigetic.c
 * Authors: 2005- Hui Wan, Xuemei Xi, Ali Niknejad, Chenming Hu.
 * Authors: 2009- Wenwei Yang, Chung-Hsun Lin, Ali Niknejad, Chenming Hu.
 * File: b4soigetic.c
 * Modified by Hui Wan, Xuemei Xi 11/30/2005
 * Modified by Wenwei Yang, Chung-Hsun Lin, Darsen Lu 03/06/2009
 **********/

#include "spice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "b4soidef.h"
#include "sperror.h"
#include "suffix.h"


int
B4SOIgetic(inModel,ckt)
GENmodel *inModel;
CKTcircuit *ckt;
{
B4SOImodel *model = (B4SOImodel*)inModel;
B4SOIinstance *here;

    for (; model ; model = model->B4SOInextModel) 
    {    for (here = model->B4SOIinstances; here; here = here->B4SOInextInstance)
	 {    if(!here->B4SOIicVBSGiven) 
	      {  here->B4SOIicVBS = *(ckt->CKTrhs + here->B4SOIbNode) 
				  - *(ckt->CKTrhs + here->B4SOIsNode);
              }
              if (!here->B4SOIicVDSGiven) 
	      {   here->B4SOIicVDS = *(ckt->CKTrhs + here->B4SOIdNode) 
				   - *(ckt->CKTrhs + here->B4SOIsNode);
              }
              if (!here->B4SOIicVGSGiven) 
	      {   here->B4SOIicVGS = *(ckt->CKTrhs + here->B4SOIgNode) 
				   - *(ckt->CKTrhs + here->B4SOIsNode);
              }
              if (!here->B4SOIicVESGiven) 
	      {   here->B4SOIicVES = *(ckt->CKTrhs + here->B4SOIeNode) 
				   - *(ckt->CKTrhs + here->B4SOIsNode);
              }
              if (!here->B4SOIicVPSGiven) 
	      {   here->B4SOIicVPS = *(ckt->CKTrhs + here->B4SOIpNode) 
				   - *(ckt->CKTrhs + here->B4SOIsNode);
              }
         }
    }
    return(OK);
}


