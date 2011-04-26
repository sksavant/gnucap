/* $Id: b3soimdel.c,v 3.2 02/24/04 Hui Wan Release $  */
/*
$Log:   b3soimdel.c,v $
 * Revision 3.2 02/24/04  Hui Wan 
 * BSIMSOI3.2 release
 *
*/
static char rcsid[] = "$Id: b3soimdel.c,v 3.2 02/24/04 Hui Wan Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soimdel.c          98/5/01
**********/


#include "spice.h"
#include <stdio.h>
#include "util.h"
#include "b3soidef.h"
#include "sperror.h"
#include "suffix.h"

int
B3SOImDelete(inModel,modname,kill)
GENmodel **inModel;
IFuid modname;
GENmodel *kill;
{
B3SOImodel **model = (B3SOImodel**)inModel;
B3SOImodel *modfast = (B3SOImodel*)kill;
B3SOIinstance *here;
B3SOIinstance *prev = NULL;
B3SOImodel **oldmod;

    oldmod = model;
    for (; *model ; model = &((*model)->B3SOInextModel)) 
    {    if ((*model)->B3SOImodName == modname || 
             (modfast && *model == modfast))
	     goto delgot;
         oldmod = model;
    }
    return(E_NOMOD);

delgot:
    *oldmod = (*model)->B3SOInextModel; /* cut deleted device out of list */
    for (here = (*model)->B3SOIinstances; here; here = here->B3SOInextInstance)
    {    if(prev) FREE(prev);
         prev = here;
    }
    if(prev) FREE(prev);
    FREE(*model);
    return(OK);
}



