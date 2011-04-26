/* $Id: BSIMDD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soidest.c, DD2.1 $
 * Revision 2.1  99/9/27 Pin Su 
 * BSIMDD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soidest.c, DD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soidest.c          98/5/01
**********/


#include "spice.h"
#include <stdio.h>
#include "util.h"
#include "b3soidef.h"
#include "suffix.h"

void
B3SOIdestroy(inModel)
GENmodel **inModel;
{
B3SOImodel **model = (B3SOImodel**)inModel;
B3SOIinstance *here;
B3SOIinstance *prev = NULL;
B3SOImodel *mod = *model;
B3SOImodel *oldmod = NULL;

    for (; mod ; mod = mod->B3SOInextModel)
    {    if(oldmod) FREE(oldmod);
         oldmod = mod;
         prev = (B3SOIinstance *)NULL;
         for (here = mod->B3SOIinstances; here; here = here->B3SOInextInstance)
	 {    if(prev) FREE(prev);
              prev = here;
         }
         if(prev) FREE(prev);
    }
    if(oldmod) FREE(oldmod);
    *model = NULL;
    return;
}



