/* $Id: b3soiask.c,v 2.2.3 02/3/5 Pin Su Release $  */
/*
$Log:   b3soiask.c,v $
 * Revision 2.2.3  02/3/5  Pin Su
 * BSIMPD2.2.3 release
 *
*/
static char rcsid[] = "$Id: b3soiask.c,v 2.2.3 02/3/5 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soiask.c          98/5/01
Modified by Pin Su	99/4/30
Modified by Pin Su      01/2/15
**********/


#include "spice.h"
#include <stdio.h>
#include <math.h>
#include "ifsim.h"
#include "cktdefs.h"
#include "devdefs.h"
#include "b3soidef.h"
#include "sperror.h"
#include "util.h"
#include "suffix.h"

int
B3SOIask(ckt,inst,which,value,select)
CKTcircuit *ckt;
GENinstance *inst;
int which;
IFvalue *value;
IFvalue *select;
{
B3SOIinstance *here = (B3SOIinstance*)inst;

    switch(which) 
    {   case B3SOI_L:
            value->rValue = here->B3SOIl;
            return(OK);
        case B3SOI_W:
            value->rValue = here->B3SOIw;
            return(OK);
        case B3SOI_AS:
            value->rValue = here->B3SOIsourceArea;
            return(OK);
        case B3SOI_AD:
            value->rValue = here->B3SOIdrainArea;
            return(OK);
        case B3SOI_PS:
            value->rValue = here->B3SOIsourcePerimeter;
            return(OK);
        case B3SOI_PD:
            value->rValue = here->B3SOIdrainPerimeter;
            return(OK);
        case B3SOI_NRS:
            value->rValue = here->B3SOIsourceSquares;
            return(OK);
        case B3SOI_NRD:
            value->rValue = here->B3SOIdrainSquares;
            return(OK);
        case B3SOI_OFF:
            value->iValue = here->B3SOIoff;
            return(OK);
        case B3SOI_BJTOFF:
            value->iValue = here->B3SOIbjtoff;
            return(OK);
        case B3SOI_RTH0:
            value->rValue = here->B3SOIrth0;
            return(OK);
        case B3SOI_CTH0:
            value->rValue = here->B3SOIcth0;
            return(OK);
        case B3SOI_NRB:
            value->rValue = here->B3SOIbodySquares;
            return(OK);
        case B3SOI_FRBODY:
            value->rValue = here->B3SOIfrbody;
            return(OK);


/* v2.0 release */
        case B3SOI_NBC:
            value->rValue = here->B3SOInbc;
            return(OK);
        case B3SOI_NSEG:
            value->rValue = here->B3SOInseg;
            return(OK);
        case B3SOI_PDBCP:
            value->rValue = here->B3SOIpdbcp;
            return(OK);
        case B3SOI_PSBCP:
            value->rValue = here->B3SOIpsbcp;
            return(OK);
        case B3SOI_AGBCP:
            value->rValue = here->B3SOIagbcp;
            return(OK);
        case B3SOI_AEBCP:
            value->rValue = here->B3SOIaebcp;
            return(OK);
        case B3SOI_VBSUSR:
            value->rValue = here->B3SOIvbsusr;
            return(OK);
        case B3SOI_TNODEOUT:
            value->iValue = here->B3SOItnodeout;
            return(OK);


        case B3SOI_IC_VBS:
            value->rValue = here->B3SOIicVBS;
            return(OK);
        case B3SOI_IC_VDS:
            value->rValue = here->B3SOIicVDS;
            return(OK);
        case B3SOI_IC_VGS:
            value->rValue = here->B3SOIicVGS;
            return(OK);
        case B3SOI_IC_VES:
            value->rValue = here->B3SOIicVES;
            return(OK);
        case B3SOI_IC_VPS:
            value->rValue = here->B3SOIicVPS;
            return(OK);
        case B3SOI_DNODE:
            value->iValue = here->B3SOIdNode;
            return(OK);
        case B3SOI_GNODE:
            value->iValue = here->B3SOIgNode;
            return(OK);
        case B3SOI_SNODE:
            value->iValue = here->B3SOIsNode;
            return(OK);
        case B3SOI_BNODE:
            value->iValue = here->B3SOIbNode;
            return(OK);
        case B3SOI_ENODE:
            value->iValue = here->B3SOIeNode;
            return(OK);
        case B3SOI_DNODEPRIME:
            value->iValue = here->B3SOIdNodePrime;
            return(OK);
        case B3SOI_SNODEPRIME:
            value->iValue = here->B3SOIsNodePrime;
            return(OK);
        case B3SOI_SOURCECONDUCT:
            value->rValue = here->B3SOIsourceConductance;
            return(OK);
        case B3SOI_DRAINCONDUCT:
            value->rValue = here->B3SOIdrainConductance;
            return(OK);
        case B3SOI_VBD:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIvbd);
            return(OK);
        case B3SOI_VBS:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIvbs);
            return(OK);
        case B3SOI_VGS:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIvgs);
            return(OK);
        case B3SOI_VES:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIves);
            return(OK);
        case B3SOI_VDS:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIvds);
            return(OK);
        case B3SOI_CD:
            value->rValue = here->B3SOIcd; 
            return(OK);
        case B3SOI_CBS:
            value->rValue = here->B3SOIcjs; 
            return(OK);
        case B3SOI_CBD:
            value->rValue = here->B3SOIcjd; 
            return(OK);
        case B3SOI_GM:
            value->rValue = here->B3SOIgm; 
            return(OK);
        case B3SOI_GMID:
            value->rValue = here->B3SOIgm/here->B3SOIcd; 
            return(OK);
        case B3SOI_GDS:
            value->rValue = here->B3SOIgds; 
            return(OK);
        case B3SOI_GMBS:
            value->rValue = here->B3SOIgmbs; 
            return(OK);
        case B3SOI_GBD:
            value->rValue = here->B3SOIgjdb; 
            return(OK);
        case B3SOI_GBS:
            value->rValue = here->B3SOIgjsb; 
            return(OK);
        case B3SOI_QB:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIqb); 
            return(OK);
        case B3SOI_CQB:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIcqb); 
            return(OK);
        case B3SOI_QG:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIqg); 
            return(OK);
        case B3SOI_CQG:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIcqg); 
            return(OK);
        case B3SOI_QD:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIqd); 
            return(OK);
        case B3SOI_CQD:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIcqd); 
            return(OK);
        case B3SOI_CGG:
            value->rValue = here->B3SOIcggb; 
            return(OK);
        case B3SOI_CGD:
            value->rValue = here->B3SOIcgdb;
            return(OK);
        case B3SOI_CGS:
            value->rValue = here->B3SOIcgsb;
            return(OK);
        case B3SOI_CDG:
            value->rValue = here->B3SOIcdgb; 
            return(OK);
        case B3SOI_CDD:
            value->rValue = here->B3SOIcddb; 
            return(OK);
        case B3SOI_CDS:
            value->rValue = here->B3SOIcdsb; 
            return(OK);
        case B3SOI_CBG:
            value->rValue = here->B3SOIcbgb;
            return(OK);
        case B3SOI_CBDB:
            value->rValue = here->B3SOIcbdb;
            return(OK);
        case B3SOI_CBSB:
            value->rValue = here->B3SOIcbsb;
            return(OK);
        case B3SOI_VON:
            value->rValue = here->B3SOIvon; 
            return(OK);
        case B3SOI_VDSAT:
            value->rValue = here->B3SOIvdsat; 
            return(OK);
        case B3SOI_QBS:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIqbs); 
            return(OK);
        case B3SOI_QBD:
            value->rValue = *(ckt->CKTstate0 + here->B3SOIqbd); 
            return(OK);
        default:
            return(E_BADPARM);
    }
    /* NOTREACHED */
}

