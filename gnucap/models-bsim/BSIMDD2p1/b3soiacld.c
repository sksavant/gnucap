/* $Id: BSIMDD2.1 99/9/27 Pin Su Release $  */
/*
$Log:   b3soiacld.c, DD2.1 $
 * Revision 2.1  99/9/27 Pin Su 
 * BSIMDD2.1 release
 *
*/
static char rcsid[] = "$Id: b3soiacld.c, DD2.1 99/9/27 Pin Su Release $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: Weidong Liu and Pin Su         Feb 1999
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soiacld.c          98/5/01
**********/

#include "spice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "b3soidef.h"
#include "sperror.h"
#include "suffix.h"


int
B3SOIacLoad(inModel,ckt)
GENmodel *inModel;
register CKTcircuit *ckt;
{
register B3SOImodel *model = (B3SOImodel*)inModel;
register B3SOIinstance *here;
register int selfheat;
double xcggb, xcgdb, xcgsb, xcgeb, xcgT;
double xcdgb, xcddb, xcdsb, xcdeb, xcdT;
double xcsgb, xcsdb, xcssb, xcseb, xcsT;
double xcbgb, xcbdb, xcbsb, xcbeb, xcbT;
double xcegb, xcedb, xcesb, xceeb, xceT;
double gdpr, gspr, gds;
double cggb, cgdb, cgsb, cgeb, cgT;
double cdgb, cddb, cdsb, cdeb, cdT;
double cbgb, cbdb, cbsb, cbeb, cbT;
double cegb, cedb, cesb, ceeb, ceT;
double GSoverlapCap, GDoverlapCap, GEoverlapCap, FwdSum, RevSum, Gm, Gmbs, Gme, GmT;
double omega;
double dxpart, sxpart;
double gbbg, gbbdp, gbbb, gbbe, gbbp, gbbsp, gbbT;
double gddpg, gddpdp, gddpsp, gddpb, gddpe, gddpT;
double gsspg, gsspdp, gsspsp, gsspb, gsspe, gsspT;
double gppg, gppdp, gppb, gppe, gppp, gppsp, gppT;
double xcTt, cTt, gcTt, gTtt, gTtg, gTtb, gTte, gTtdp, gTtsp;

double Dum1, Dum2, Dum3, Dum4, Dum5;
FILE *fpdebug;

    omega = ckt->CKTomega;
    for (; model != NULL; model = model->B3SOInextModel) 
    {    

      for (here = model->B3SOIinstances; here!= NULL;
              here = here->B3SOInextInstance) 
	 {    
              selfheat = (model->B3SOIshMod == 1) && (here->B3SOIrth0 != 0.0);
              if (here->B3SOIdebugMod > 2)
              {
                 fpdebug = fopen("b3soiac.log", "a");
                 fprintf(fpdebug, ".......omega=%.5e\n", omega);
              }
              if (here->B3SOImode >= 0) 
	      {   Gm = here->B3SOIgm;
		  Gmbs = here->B3SOIgmbs;
	          Gme = here->B3SOIgme;
                  GmT = model->B3SOItype * here->B3SOIgmT;
		  FwdSum = Gm + Gmbs + Gme;
		  RevSum = 0.0;

                  cbgb = here->B3SOIcbgb;
                  cbsb = here->B3SOIcbsb;
                  cbdb = here->B3SOIcbdb;
                  cbeb = here->B3SOIcbeb;
                  cbT  = model->B3SOItype * here->B3SOIcbT;

                  cegb = here->B3SOIcegb;
                  cesb = here->B3SOIcesb;
                  cedb = here->B3SOIcedb;
                  ceeb = here->B3SOIceeb;
                  ceT  = model->B3SOItype * here->B3SOIceT;

                  cggb = here->B3SOIcggb;
                  cgsb = here->B3SOIcgsb;
                  cgdb = here->B3SOIcgdb;
                  cgeb = here->B3SOIcgeb;
                  cgT  = model->B3SOItype * here->B3SOIcgT;

                  cdgb = here->B3SOIcdgb;
                  cdsb = here->B3SOIcdsb;
                  cddb = here->B3SOIcddb;
                  cdeb = here->B3SOIcdeb;
                  cdT  = model->B3SOItype * here->B3SOIcdT;

                  cTt = here->pParam->B3SOIcth;

                  gbbg  = -here->B3SOIgbgs;
                  gbbdp = -here->B3SOIgbds;
                  gbbb  = -here->B3SOIgbbs;
                  gbbe  = -here->B3SOIgbes;
                  gbbp  = -here->B3SOIgbps;
                  gbbT  = -model->B3SOItype * here->B3SOIgbT;
                  gbbsp = - ( gbbg + gbbdp + gbbb + gbbe + gbbp);

                  gddpg  = -here->B3SOIgjdg;
                  gddpdp = -here->B3SOIgjdd;
                  gddpb  = -here->B3SOIgjdb;
                  gddpe  = -here->B3SOIgjde;
                  gddpT  = -model->B3SOItype * here->B3SOIgjdT;
                  gddpsp = - ( gddpg + gddpdp + gddpb + gddpe);

                  gsspg  = -here->B3SOIgjsg;
                  gsspdp = -here->B3SOIgjsd;
                  gsspb  = -here->B3SOIgjsb;
                  gsspe  = 0.0;
                  gsspT  = -model->B3SOItype * here->B3SOIgjsT;
                  gsspsp = - (gsspg + gsspdp + gsspb + gsspe);

                  gppg = -here->B3SOIgbpgs;
                  gppdp = -here->B3SOIgbpds;
             	  gppb = -here->B3SOIgbpbs;
                  gppe = -here->B3SOIgbpes;
                  gppp = -here->B3SOIgbpps;
                  gppT = -model->B3SOItype * here->B3SOIgbpT;
                  gppsp = - (gppg + gppdp + gppb + gppe + gppp);

                  gTtg  = here->B3SOIgtempg;
                  gTtb  = here->B3SOIgtempb;
                  gTte  = here->B3SOIgtempe;
                  gTtdp = here->B3SOIgtempd;
                  gTtt  = here->B3SOIgtempT;
                  gTtsp = - (gTtg + gTtb + gTte + gTtdp);

                  sxpart = 0.6;
                  dxpart = 0.4;

              } 
	      else
	      {   Gm = -here->B3SOIgm;
		  Gmbs = -here->B3SOIgmbs;
                  Gme = -here->B3SOIgme;
                  GmT = -model->B3SOItype * here->B3SOIgmT;
		  FwdSum = 0.0;
		  RevSum = -Gm - Gmbs - Gme;

                  cdgb = - (here->B3SOIcdgb + here->B3SOIcggb + here->B3SOIcbgb
                          + here->B3SOIcegb);
                  cdsb = - (here->B3SOIcddb + here->B3SOIcgdb + here->B3SOIcbdb
                          + here->B3SOIcedb);
                  cddb = - (here->B3SOIcdsb + here->B3SOIcgsb + here->B3SOIcbsb
                          + here->B3SOIcesb);
                  cdeb = - (here->B3SOIcdeb + here->B3SOIcgeb + here->B3SOIcbeb
                          + here->B3SOIceeb);
                  cdT  = - model->B3SOItype * (here->B3SOIcgT + here->B3SOIcbT
                         + here->B3SOIcdT + here->B3SOIceT);

                  cegb = here->B3SOIcegb;
                  cesb = here->B3SOIcedb;
                  cedb = here->B3SOIcesb;
                  ceeb = here->B3SOIceeb;
                  ceT  = model->B3SOItype * here->B3SOIceT;

                  cggb = here->B3SOIcggb;
                  cgsb = here->B3SOIcgdb;
                  cgdb = here->B3SOIcgsb;
                  cgeb = here->B3SOIcgeb;
                  cgT  = model->B3SOItype * here->B3SOIcgT;

                  cbgb = here->B3SOIcbgb;
                  cbsb = here->B3SOIcbdb;
                  cbdb = here->B3SOIcbsb;
                  cbeb = here->B3SOIcbeb;
                  cbT  = model->B3SOItype * here->B3SOIcbT;

                  cTt = here->pParam->B3SOIcth;

                  gbbg  = -here->B3SOIgbgs;
                  gbbb  = -here->B3SOIgbbs;
                  gbbe  = -here->B3SOIgbes;
                  gbbp  = -here->B3SOIgbps;
                  gbbsp = -here->B3SOIgbds;
                  gbbT  = -model->B3SOItype * here->B3SOIgbT;
                  gbbdp = - ( gbbg + gbbsp + gbbb + gbbe + gbbp);

                  gddpg  = -here->B3SOIgjsg;
                  gddpsp = -here->B3SOIgjsd;
                  gddpb  = -here->B3SOIgjsb;
                  gddpe  = 0.0;
                  gddpT  = -model->B3SOItype * here->B3SOIgjsT;
                  gddpdp = - (gddpg + gddpsp + gddpb + gddpe);

                  gsspg  = -here->B3SOIgjdg;
                  gsspsp = -here->B3SOIgjdd;
                  gsspb  = -here->B3SOIgjdb;
                  gsspe  = -here->B3SOIgjde;
                  gsspT  = -model->B3SOItype * here->B3SOIgjdT;
                  gsspdp = - ( gsspg + gsspsp + gsspb + gsspe);

                  gppg = -here->B3SOIgbpgs;
                  gppsp = -here->B3SOIgbpds;
                  gppb = -here->B3SOIgbpbs;
                  gppe = -here->B3SOIgbpes;
                  gppp = -here->B3SOIgbpps;
                  gppT = -model->B3SOItype * here->B3SOIgbpT;
                  gppdp = - (gppg + gppsp + gppb + gppe + gppp);

                  gTtt = here->B3SOIgtempT;
                  gTtg = here->B3SOIgtempg;
                  gTtb = here->B3SOIgtempb;
                  gTte = here->B3SOIgtempe;
                  gTtdp = here->B3SOIgtempd;
                  gTtsp = - (gTtt + gTtg + gTtb + gTte + gTtdp);

                  gTtg  = here->B3SOIgtempg;
                  gTtb  = here->B3SOIgtempb;
                  gTte  = here->B3SOIgtempe;
                  gTtsp = here->B3SOIgtempd;
                  gTtt  = here->B3SOIgtempT;
                  gTtdp = - (gTtg + gTtb + gTte + gTtsp);

                  sxpart = 0.6;
                  sxpart = 0.4;
                  dxpart = 0.6;
              }

              gdpr=here->B3SOIdrainConductance;
              gspr=here->B3SOIsourceConductance;
              gds= here->B3SOIgds;

	      GSoverlapCap = here->B3SOIcgso;
	      GDoverlapCap = here->B3SOIcgdo;
	      GEoverlapCap = here->pParam->B3SOIcgeo;

              xcegb = (cegb - GEoverlapCap) * omega;
              xcedb = cedb * omega;
              xcesb = cesb * omega;
              xceeb = (ceeb + GEoverlapCap) * omega;
              xceT  =  ceT * omega;

              xcggb = (cggb + GDoverlapCap + GSoverlapCap + GEoverlapCap)
		    * omega;
              xcgdb = (cgdb - GDoverlapCap ) * omega;
              xcgsb = (cgsb - GSoverlapCap) * omega;
              xcgeb = (cgeb - GEoverlapCap) * omega;
              xcgT  = cgT * omega;

              xcdgb = (cdgb - GDoverlapCap) * omega;
              xcddb = (cddb + GDoverlapCap) * omega;
              xcdsb = cdsb * omega;
              xcdeb = cdeb * omega;
              xcdT  = cdT * omega;

              xcsgb = -(cggb + cbgb + cdgb + cegb + GSoverlapCap) * omega;
              xcsdb = -(cgdb + cbdb + cddb + cedb) * omega;
              xcssb = (GSoverlapCap - (cgsb + cbsb + cdsb + cesb)) * omega;
              xcseb = -(cgeb + cbeb + cdeb + ceeb) * omega;
              xcsT  = -(cgT + cbT + cdT + ceT) * omega;

              xcbgb = cbgb * omega;
              xcbdb = cbdb * omega;
              xcbsb = cbsb * omega;
              xcbeb = cbeb * omega;
              xcbT  = cbT * omega;

              xcTt = cTt * omega;

              *(here->B3SOIEgPtr  +1) += xcegb;
              *(here->B3SOIEdpPtr  +1) += xcedb;
              *(here->B3SOIEspPtr  +1) += xcesb;
              *(here->B3SOIGePtr +1)  += xcgeb;
              *(here->B3SOIDPePtr +1) += xcdeb;
              *(here->B3SOISPePtr +1) += xcseb;
              
              *(here->B3SOIEePtr  +1) += xceeb;

              *(here->B3SOIGgPtr  +1) += xcggb;
              *(here->B3SOIGdpPtr +1) += xcgdb;
              *(here->B3SOIGspPtr +1) += xcgsb;

              *(here->B3SOIDPgPtr +1) += xcdgb;
              *(here->B3SOIDPdpPtr +1) += xcddb;
              *(here->B3SOIDPspPtr +1) += xcdsb;

              *(here->B3SOISPgPtr +1) += xcsgb;
              *(here->B3SOISPdpPtr +1) += xcsdb;
              *(here->B3SOISPspPtr +1) += xcssb;

              *(here->B3SOIBePtr +1) += xcbeb;
              *(here->B3SOIBgPtr +1) += xcbgb;
              *(here->B3SOIBdpPtr +1) += xcbdb;
              *(here->B3SOIBspPtr +1) += xcbsb;

              *(here->B3SOIEbPtr  +1) -= xcegb + xcedb + xcesb + xceeb;
              *(here->B3SOIGbPtr +1) -= xcggb + xcgdb + xcgsb + xcgeb;
              *(here->B3SOIDPbPtr +1) -= xcdgb + xcddb + xcdsb + xcdeb;
              *(here->B3SOISPbPtr +1) -= xcsgb + xcsdb + xcssb + xcseb;
              *(here->B3SOIBbPtr +1) -= xcbgb + xcbdb + xcbsb + xcbeb;

              if (selfheat)
              {
                 *(here->B3SOITemptempPtr + 1) += xcTt;
                 *(here->B3SOIDPtempPtr + 1) += xcdT;
                 *(here->B3SOISPtempPtr + 1) += xcsT;
                 *(here->B3SOIBtempPtr + 1) += xcbT;
                 *(here->B3SOIEtempPtr + 1) += xceT;
                 *(here->B3SOIGtempPtr + 1) += xcgT;
              }
                                                               
 
if (here->B3SOIdebugMod > 3)
{
fprintf(fpdebug, "Cbg+Cbs+Cbe = %.5e; Cbd = %.5e;\n",
(xcbgb+xcbsb+xcbeb)/omega, xcbdb/omega);
fprintf(fpdebug, "gbs = %.5e; gbd = %.5e\n", gbbsp, gbbdp);


   fprintf(fpdebug, "AC condunctance...\n");
   fprintf(fpdebug, "Eg=%.5e; Edp=%.5e; Esp=%.5e;\nEb=%.5e; Ee=%.5e\n",
xcegb, xcedb, xcesb, -(xcegb+xcedb+xcesb+xceeb), xceeb);
   fprintf(fpdebug, "Gg=%.5e; Gdp=%.5e; Gsp=%.5e;\nGb=%.5e; Ge=%.5e\n",
xcggb, xcgdb, xcgsb, -(xcggb+xcgdb+xcgsb+xcgeb), xcgeb);
   fprintf(fpdebug, "Bg=%.5e; Bdp=%.5e; Bsp=%.5e;\nBb=%.5e; Be=%.5e\n",
xcbgb, xcbdb, xcbsb, -(xcbgb+xcbdb+xcbsb+xcbeb), xcbeb);
   fprintf(fpdebug, "DPg=%.5e; DPdp=%.5e; DPsp=%.5e;\nDPb=%.5e; DPe=%.5e\n",
xcdgb, xcddb, xcdsb, -(xcdgb+xcddb+xcdsb+xcdeb), xcdeb);
   fprintf(fpdebug, "SPg=%.5e; SPdp=%.5e; SPsp=%.5e;\nSPb=%.5e; SPe=%.5e\n",
xcsgb, xcsdb, xcssb, -(xcsgb+xcsdb+xcssb+xcseb), xcseb);
}



              *(here->B3SOIEgPtr) += 0.0;
              *(here->B3SOIEdpPtr) += 0.0;
              *(here->B3SOIEspPtr) += 0.0;
              *(here->B3SOIGePtr) -=  0.0;
              *(here->B3SOIDPePtr) += Gme + gddpe;
              *(here->B3SOISPePtr) += gsspe - Gme;
             
              *(here->B3SOIEePtr) += 0.0;

              *(here->B3SOIDPgPtr) += Gm + gddpg;
              *(here->B3SOIDPdpPtr) += gdpr + gds + gddpdp + RevSum ;
              *(here->B3SOIDPspPtr) -= gds + FwdSum - gddpsp;
              *(here->B3SOIDPdPtr) -= gdpr;

              *(here->B3SOISPgPtr) -= Gm - gsspg;
              *(here->B3SOISPdpPtr) -= gds + RevSum - gsspdp;
              *(here->B3SOISPspPtr) += gspr + gds + FwdSum + gsspsp;
              *(here->B3SOISPsPtr) -= gspr;

              *(here->B3SOIBePtr) += gbbe;
              *(here->B3SOIBgPtr)  += gbbg;
              *(here->B3SOIBdpPtr) += gbbdp;
              *(here->B3SOIBspPtr) += gbbsp;
	      *(here->B3SOIBbPtr) += gbbb;
              *(here->B3SOIEbPtr) += 0.0;
              *(here->B3SOISPbPtr) -= Gmbs - gsspb; 
              *(here->B3SOIDPbPtr) -= (-gddpb - Gmbs);

              if (selfheat)
              {
                 *(here->B3SOIDPtempPtr) += GmT + gddpT;
                 *(here->B3SOISPtempPtr) += -GmT + gsspT;
                 *(here->B3SOIBtempPtr) += gbbT;
                 if (here->B3SOIbodyMod == 1) {
                    (*(here->B3SOIPtempPtr) += gppT);
                 }

                 *(here->B3SOITemptempPtr) += gTtt + 1/here->pParam->B3SOIrth;
                 *(here->B3SOITempgPtr) += gTtg;
                 *(here->B3SOITempbPtr) += gTtb;
                 *(here->B3SOITempePtr) += gTte;
                 *(here->B3SOITempdpPtr) += gTtdp;
                 *(here->B3SOITempspPtr) += gTtsp;
              }

if (here->B3SOIdebugMod > 3)
{
   fprintf(fpdebug, "Static condunctance...\n");
   fprintf(fpdebug, "Gg=%.5e; Gdp=%.5e; Gsp=%.5e;\nGb=%.5e; Ge=%.5e\n",
   *(here->B3SOIGgPtr), *(here->B3SOIGdpPtr),
   *(here->B3SOIGspPtr), *(here->B3SOIGbPtr),
   *(here->B3SOIGePtr));
   fprintf(fpdebug, "DPg=%.5e; DPdp=%.5e; DPsp=%.5e;\nDPb=%.5e; DPe=%.5e\n",
   *(here->B3SOIDPgPtr), *(here->B3SOIDPdpPtr),
   *(here->B3SOIDPspPtr), *(here->B3SOIDPbPtr),
   *(here->B3SOIDPePtr));
   fprintf(fpdebug, "SPg=%.5e; SPdp=%.5e; SPsp=%.5e;\nSPb=%.5e; SPe=%.5e\n",
   *(here->B3SOISPgPtr), *(here->B3SOISPdpPtr),
   *(here->B3SOISPspPtr), *(here->B3SOISPbPtr),
   *(here->B3SOISPePtr));
   fprintf(fpdebug, "Bg=%.5e; Bdp=%.5e; Bsp=%.5e;\nBb=%.5e; Be=%.5e\n",
gbbg, gbbdp, gbbsp, gbbb, gbbe);
}

              *(here->B3SOIDdPtr) += gdpr;
              *(here->B3SOIDdpPtr) -= gdpr;
              *(here->B3SOISsPtr) += gspr;
              *(here->B3SOISspPtr) -= gspr;


              if (here->B3SOIbodyMod == 1) {
                 (*(here->B3SOIBpPtr) -= gppp);
                 (*(here->B3SOIPbPtr) += gppb);
                 (*(here->B3SOIPpPtr) += gppp);
                 (*(here->B3SOIPgPtr) += gppg);
                 (*(here->B3SOIPdpPtr) += gppdp);
                 (*(here->B3SOIPspPtr) += gppsp);
                 (*(here->B3SOIPePtr) += gppe);
              }
              if (here->B3SOIdebugMod > 1)
              {
                      *(here->B3SOIVbsPtr) += 1;
                      *(here->B3SOIIdsPtr) += 1;
                      *(here->B3SOIIcPtr) += 1;
                      *(here->B3SOIIbsPtr) += 1;
                      *(here->B3SOIIbdPtr) += 1;
                      *(here->B3SOIIiiPtr) += 1;
                      *(here->B3SOIIgidlPtr) += 1;
                      *(here->B3SOIItunPtr) += 1;
                      *(here->B3SOIIbpPtr) += 1;
                      *(here->B3SOIAbeffPtr) += 1;
                      *(here->B3SOIVbs0effPtr) += 1;
                      *(here->B3SOIVbseffPtr) += 1;
                      *(here->B3SOIXcPtr) += 1;
                      *(here->B3SOICbgPtr) += 1;
                      *(here->B3SOICbbPtr) += 1;
                      *(here->B3SOICbdPtr) += 1;
                      *(here->B3SOIqbPtr) += 1;
                      *(here->B3SOIQbfPtr) += 1;
                      *(here->B3SOIQjsPtr) += 1;
                      *(here->B3SOIQjdPtr) += 1;

                      /* clean up last */
                      *(here->B3SOIGmPtr) += 1;
                      *(here->B3SOIGmbsPtr) += 1;
                      *(here->B3SOIGdsPtr) += 1;
                      *(here->B3SOIGmePtr) += 1;
                      *(here->B3SOIVbs0teffPtr) += 1;
                      *(here->B3SOIVgsteffPtr) += 1;
                      *(here->B3SOICbePtr) += 1;
                      *(here->B3SOIVthPtr) += 1;
                      *(here->B3SOIXcsatPtr) += 1;
                      *(here->B3SOIVdscvPtr) += 1;
                      *(here->B3SOIVcscvPtr) += 1;
                      *(here->B3SOIQaccPtr) += 1;
                      *(here->B3SOIQsub0Ptr) += 1;
                      *(here->B3SOIQsubs1Ptr) += 1;
                      *(here->B3SOIQsubs2Ptr) += 1;
                      *(here->B3SOIqgPtr) += 1;
                      *(here->B3SOIqdPtr) += 1;
                      *(here->B3SOIqePtr) += 1;
                      *(here->B3SOIDum1Ptr) += 1;
                      *(here->B3SOIDum2Ptr) += 1;
                      *(here->B3SOIDum3Ptr) += 1;
                      *(here->B3SOIDum4Ptr) += 1;
                      *(here->B3SOIDum5Ptr) += 1;
              }

           if (here->B3SOIdebugMod > 2)
              fclose(fpdebug);
        }
    }
    return(OK);
}

