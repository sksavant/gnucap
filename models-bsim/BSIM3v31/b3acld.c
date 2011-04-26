/* $Id: b3acld.c,v 3.1 96/12/08 19:51:00 yuhua Release $  */
/* 
$Log:	b3acld.c,v $
 * Revision 3.1  96/12/08  19:51:00  yuhua
 * BSIM3v3.1 release
 * 
*/
static char rcsid[] = "$Id: b3acld.c,v 3.1 96/12/08 19:51:00 yuhua Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3acld.c
**********/

#include "spice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "bsim3def.h"
#include "sperror.h"
#include "suffix.h"


int
BSIM3acLoad(inModel,ckt)
GENmodel *inModel;
register CKTcircuit *ckt;
{
register BSIM3model *model = (BSIM3model*)inModel;
register BSIM3instance *here;
double xcggb, xcgdb, xcgsb, xcbgb, xcbdb, xcbsb, xcddb, xcssb, xcdgb;
double gdpr, gspr, gds, gbd, gbs, capbd, capbs, xcsgb, xcdsb, xcsdb;
double cggb, cgdb, cgsb, cbgb, cbdb, cbsb, cddb, cdgb, cdsb, omega;
double GSoverlapCap, GDoverlapCap, GBoverlapCap, FwdSum, RevSum, Gm, Gmbs;

double dxpart, sxpart, cqgb, cqdb, cqsb, cqbb, xcqgb, xcqdb, xcqsb, xcqbb;

    omega = ckt->CKTomega;
    for (; model != NULL; model = model->BSIM3nextModel) 
    {    


      for (here = model->BSIM3instances; here!= NULL;
              here = here->BSIM3nextInstance) 
	 {    if (here->BSIM3mode >= 0) 
	      {   Gm = here->BSIM3gm;
		  Gmbs = here->BSIM3gmbs;
		  FwdSum = Gm + Gmbs;
		  RevSum = 0.0;
                  cggb = here->BSIM3cggb;
                  cgsb = here->BSIM3cgsb;
                  cgdb = here->BSIM3cgdb;

                  cbgb = here->BSIM3cbgb;
                  cbsb = here->BSIM3cbsb;
                  cbdb = here->BSIM3cbdb;

                  cdgb = here->BSIM3cdgb;
                  cdsb = here->BSIM3cdsb;
                  cddb = here->BSIM3cddb;

                  cqgb = here->BSIM3cqgb;
                  cqdb = here->BSIM3cqdb;
                  cqsb = here->BSIM3cqsb;
                  cqbb = here->BSIM3cqbb;
                  sxpart = 0.6;
                  dxpart = 0.4;

              } 
	      else
	      {   Gm = -here->BSIM3gm;
		  Gmbs = -here->BSIM3gmbs;
		  FwdSum = 0.0;
		  RevSum = -Gm - Gmbs;
                  cggb = here->BSIM3cggb;
                  cgsb = here->BSIM3cgdb;
                  cgdb = here->BSIM3cgsb;

                  cbgb = here->BSIM3cbgb;
                  cbsb = here->BSIM3cbdb;
                  cbdb = here->BSIM3cbsb;

                  cdgb = -(here->BSIM3cdgb + cggb + cbgb);
                  cdsb = -(here->BSIM3cddb + cgsb + cbsb);
                  cddb = -(here->BSIM3cdsb + cgdb + cbdb);

                  cqgb = here->BSIM3cqgb;
                  cqdb = here->BSIM3cqsb;
                  cqsb = here->BSIM3cqdb;
                  cqbb = here->BSIM3cqbb;
                  sxpart = 0.4;
                  dxpart = 0.6;
              }

              gdpr=here->BSIM3drainConductance;
              gspr=here->BSIM3sourceConductance;
              gds= here->BSIM3gds;
              gbd= here->BSIM3gbd;
              gbs= here->BSIM3gbs;
              capbd= here->BSIM3capbd;
              capbs= here->BSIM3capbs;

	      GSoverlapCap = here->BSIM3cgso;
	      GDoverlapCap = here->BSIM3cgdo;
	      GBoverlapCap = here->pParam->BSIM3cgbo;

              xcdgb = (cdgb - GDoverlapCap) * omega;
              xcddb = (cddb + capbd + GDoverlapCap) * omega;
              xcdsb = cdsb * omega;
              xcsgb = -(cggb + cbgb + cdgb + GSoverlapCap) * omega;
              xcsdb = -(cgdb + cbdb + cddb) * omega;
              xcssb = (capbs + GSoverlapCap - (cgsb + cbsb + cdsb)) * omega;
              xcggb = (cggb + GDoverlapCap + GSoverlapCap + GBoverlapCap)
		    * omega;
              xcgdb = (cgdb - GDoverlapCap ) * omega;
              xcgsb = (cgsb - GSoverlapCap) * omega;
              xcbgb = (cbgb - GBoverlapCap) * omega;
              xcbdb = (cbdb - capbd ) * omega;
              xcbsb = (cbsb - capbs ) * omega;
              xcqgb = cqgb * omega;
              xcqdb = cqdb * omega;
              xcqsb = cqsb * omega;
              xcqbb = cqbb * omega;

              *(here->BSIM3GgPtr +1) += xcggb;
              *(here->BSIM3BbPtr +1) -= xcbgb + xcbdb + xcbsb;
              *(here->BSIM3DPdpPtr +1) += xcddb;
              *(here->BSIM3SPspPtr +1) += xcssb;
              *(here->BSIM3GbPtr +1) -= xcggb + xcgdb + xcgsb;
              *(here->BSIM3GdpPtr +1) += xcgdb;
              *(here->BSIM3GspPtr +1) += xcgsb;
              *(here->BSIM3BgPtr +1) += xcbgb;
              *(here->BSIM3BdpPtr +1) += xcbdb;
              *(here->BSIM3BspPtr +1) += xcbsb;
              *(here->BSIM3DPgPtr +1) += xcdgb;
              *(here->BSIM3DPbPtr +1) -= xcdgb + xcddb + xcdsb;
              *(here->BSIM3DPspPtr +1) += xcdsb;
              *(here->BSIM3SPgPtr +1) += xcsgb;
              *(here->BSIM3SPbPtr +1) -= xcsgb + xcsdb + xcssb;
              *(here->BSIM3SPdpPtr +1) += xcsdb;
 
              *(here->BSIM3QqPtr +1) += omega;

              *(here->BSIM3QgPtr +1) -= xcqgb;
              *(here->BSIM3QdpPtr +1) -= xcqdb;
              *(here->BSIM3QspPtr +1) -= xcqsb;
              *(here->BSIM3QbPtr +1) -= xcqbb;


              *(here->BSIM3DdPtr) += gdpr;
              *(here->BSIM3SsPtr) += gspr;
              *(here->BSIM3BbPtr) += gbd + gbs;
              *(here->BSIM3DPdpPtr) += gdpr + gds + gbd + RevSum + dxpart*here->BSIM3gtd;
              *(here->BSIM3SPspPtr) += gspr + gds + gbs + FwdSum + sxpart*here->BSIM3gts;
              *(here->BSIM3DdpPtr) -= gdpr;
              *(here->BSIM3SspPtr) -= gspr;
              *(here->BSIM3BdpPtr) -= gbd;
              *(here->BSIM3BspPtr) -= gbs;
              *(here->BSIM3DPdPtr) -= gdpr;
              *(here->BSIM3DPgPtr) += Gm + dxpart * here->BSIM3gtg;
              *(here->BSIM3DPbPtr) -= gbd - Gmbs - dxpart * here->BSIM3gtb;
              *(here->BSIM3DPspPtr) -= gds + FwdSum - dxpart * here->BSIM3gts;
              *(here->BSIM3SPgPtr) -= Gm - sxpart * here->BSIM3gtg;
              *(here->BSIM3SPsPtr) -= gspr;
              *(here->BSIM3SPbPtr) -= gbs + Gmbs - sxpart * here->BSIM3gtg;
              *(here->BSIM3SPdpPtr) -= gds + RevSum - sxpart * here->BSIM3gtd;
              *(here->BSIM3GgPtr) -= here->BSIM3gtg;
              *(here->BSIM3GbPtr) -=  here->BSIM3gtb;
              *(here->BSIM3GdpPtr) -= here->BSIM3gtd;
              *(here->BSIM3GspPtr) -= here->BSIM3gts;

              *(here->BSIM3QqPtr) += here->BSIM3gtau;
 
              *(here->BSIM3DPqPtr) += dxpart * here->BSIM3gtau;
              *(here->BSIM3SPqPtr) += sxpart * here->BSIM3gtau;
              *(here->BSIM3GqPtr) -=  here->BSIM3gtau;
 
              *(here->BSIM3QgPtr) +=  here->BSIM3gtg;
              *(here->BSIM3QdpPtr) += here->BSIM3gtd;
              *(here->BSIM3QspPtr) += here->BSIM3gts;
              *(here->BSIM3QbPtr) += here->BSIM3gtb;

        }
    }
    return(OK);
}

