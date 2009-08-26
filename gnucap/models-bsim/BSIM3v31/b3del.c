/* $Id: b3del.c,v 3.1 96/12/08 19:53:53 yuhua Release $  */
/* 
$Log:	b3del.c,v $
 * Revision 3.1  96/12/08  19:53:53  yuhua
 * BSIM3v3.1 release
 * 
*/
static char rcsid[] = "$Id: b3del.c,v 3.1 96/12/08 19:53:53 yuhua Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3del.c
**********/
/*
 */

#include "spice.h"
#include <stdio.h>
#include "util.h"
#include "bsim3def.h"
#include "sperror.h"
#include "gendefs.h"
#include "suffix.h"


int
BSIM3delete(inModel,name,inInst)
GENmodel *inModel;
IFuid name;
GENinstance **inInst;
{
BSIM3instance **fast = (BSIM3instance**)inInst;
BSIM3model *model = (BSIM3model*)inModel;
BSIM3instance **prev = NULL;
BSIM3instance *here;

    for (; model ; model = model->BSIM3nextModel) 
    {    prev = &(model->BSIM3instances);
         for (here = *prev; here ; here = *prev) 
	 {    if (here->BSIM3name == name || (fast && here==*fast))
	      {   *prev= here->BSIM3nextInstance;
                  FREE(here);
                  return(OK);
              }
              prev = &(here->BSIM3nextInstance);
         }
    }
    return(E_NODEV);
}


