/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3pzld.c
**********/

#include "spice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "complex.h"
#include "sperror.h"
#include "bsim3def.h"
#include "suffix.h"

int
BSIM3pzLoad(inModel,ckt,s)
GENmodel *inModel;
register CKTcircuit *ckt;
register SPcomplex *s;
{
register BSIM3model *model = (BSIM3model*)inModel;
register BSIM3instance *here;
double xcggb, xcgdb, xcgsb, xcbgb, xcbdb, xcbsb, xcddb, xcssb, xcdgb;
double gdpr, gspr, gds, gbd, gbs, capbd, capbs, xcsgb, xcdsb, xcsdb;
double cggb, cgdb, cgsb, cbgb, cbdb, cbsb, cddb, cdgb, cdsb;
double GSoverlapCap, GDoverlapCap, GBoverlapCap;
double FwdSum, RevSum, Gm, Gmbs;

    for (; model != NULL; model = model->BSIM3nextModel) 
    {    for (here = model->BSIM3instances; here!= NULL;
              here = here->BSIM3nextInstance) 
	 {
            if (here->BSIM3mode >= 0) 
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

            xcdgb = (cdgb - GDoverlapCap);
            xcddb = (cddb + capbd + GDoverlapCap);
            xcdsb = cdsb;
            xcsgb = -(cggb + cbgb + cdgb + GSoverlapCap);
            xcsdb = -(cgdb + cbdb + cddb);
            xcssb = (capbs + GSoverlapCap - (cgsb+cbsb+cdsb));
            xcggb = (cggb + GDoverlapCap + GSoverlapCap + GBoverlapCap);
            xcgdb = (cgdb - GDoverlapCap);
            xcgsb = (cgsb - GSoverlapCap);
            xcbgb = (cbgb - GBoverlapCap);
            xcbdb = (cbdb - capbd);
            xcbsb = (cbsb - capbs);


            *(here->BSIM3GgPtr ) += xcggb * s->real;
            *(here->BSIM3GgPtr +1) += xcggb * s->imag;
            *(here->BSIM3BbPtr ) += (-xcbgb-xcbdb-xcbsb) * s->real;
            *(here->BSIM3BbPtr +1) += (-xcbgb-xcbdb-xcbsb) * s->imag;
            *(here->BSIM3DPdpPtr ) += xcddb * s->real;
            *(here->BSIM3DPdpPtr +1) += xcddb * s->imag;
            *(here->BSIM3SPspPtr ) += xcssb * s->real;
            *(here->BSIM3SPspPtr +1) += xcssb * s->imag;
            *(here->BSIM3GbPtr ) += (-xcggb-xcgdb-xcgsb) * s->real;
            *(here->BSIM3GbPtr +1) += (-xcggb-xcgdb-xcgsb) * s->imag;
            *(here->BSIM3GdpPtr ) += xcgdb * s->real;
            *(here->BSIM3GdpPtr +1) += xcgdb * s->imag;
            *(here->BSIM3GspPtr ) += xcgsb * s->real;
            *(here->BSIM3GspPtr +1) += xcgsb * s->imag;
            *(here->BSIM3BgPtr ) += xcbgb * s->real;
            *(here->BSIM3BgPtr +1) += xcbgb * s->imag;
            *(here->BSIM3BdpPtr ) += xcbdb * s->real;
            *(here->BSIM3BdpPtr +1) += xcbdb * s->imag;
            *(here->BSIM3BspPtr ) += xcbsb * s->real;
            *(here->BSIM3BspPtr +1) += xcbsb * s->imag;
            *(here->BSIM3DPgPtr ) += xcdgb * s->real;
            *(here->BSIM3DPgPtr +1) += xcdgb * s->imag;
            *(here->BSIM3DPbPtr ) += (-xcdgb-xcddb-xcdsb) * s->real;
            *(here->BSIM3DPbPtr +1) += (-xcdgb-xcddb-xcdsb) * s->imag;
            *(here->BSIM3DPspPtr ) += xcdsb * s->real;
            *(here->BSIM3DPspPtr +1) += xcdsb * s->imag;
            *(here->BSIM3SPgPtr ) += xcsgb * s->real;
            *(here->BSIM3SPgPtr +1) += xcsgb * s->imag;
            *(here->BSIM3SPbPtr ) += (-xcsgb-xcsdb-xcssb) * s->real;
            *(here->BSIM3SPbPtr +1) += (-xcsgb-xcsdb-xcssb) * s->imag;
            *(here->BSIM3SPdpPtr ) += xcsdb * s->real;
            *(here->BSIM3SPdpPtr +1) += xcsdb * s->imag;
            *(here->BSIM3DdPtr) += gdpr;
            *(here->BSIM3SsPtr) += gspr;
            *(here->BSIM3BbPtr) += gbd+gbs;
            *(here->BSIM3DPdpPtr) += gdpr+gds+gbd+RevSum;
            *(here->BSIM3SPspPtr) += gspr+gds+gbs+FwdSum;
            *(here->BSIM3DdpPtr) -= gdpr;
            *(here->BSIM3SspPtr) -= gspr;
            *(here->BSIM3BdpPtr) -= gbd;
            *(here->BSIM3BspPtr) -= gbs;
            *(here->BSIM3DPdPtr) -= gdpr;
            *(here->BSIM3DPgPtr) += Gm;
            *(here->BSIM3DPbPtr) -= gbd - Gmbs;
            *(here->BSIM3DPspPtr) -= gds + FwdSum;
            *(here->BSIM3SPgPtr) -= Gm;
            *(here->BSIM3SPsPtr) -= gspr;
            *(here->BSIM3SPbPtr) -= gbs + Gmbs;
            *(here->BSIM3SPdpPtr) -= gds + RevSum;

        }
    }
    return(OK);
}


