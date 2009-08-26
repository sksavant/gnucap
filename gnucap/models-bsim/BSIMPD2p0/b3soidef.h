/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung
File: b3soidef.h
Modified by Pin Su and Jan Feng	99/2/15
**********/

#ifndef B3SOI
#define B3SOI

#define SOICODE
/*  #define BULKCODE  */

#include "ifsim.h"
#include "gendefs.h"
#include "cktdefs.h"
#include "complex.h"
#include "noisedef.h"         

typedef struct sB3SOIinstance
{
    struct sB3SOImodel *B3SOImodPtr;
    struct sB3SOIinstance *B3SOInextInstance;
    IFuid B3SOIname;
    int B3SOIstates;     /* index into state table for this device */

    int B3SOIdNode;
    int B3SOIgNode;
    int B3SOIsNode;
    int B3SOIeNode;
    int B3SOIpNode;
    int B3SOIbNode;
    int B3SOItempNode;
    int B3SOIdNodePrime;
    int B3SOIsNodePrime;

    int B3SOIvbsNode;
    /* for Debug */
    int B3SOIidsNode;
    int B3SOIicNode;
    int B3SOIibsNode;
    int B3SOIibdNode;
    int B3SOIiiiNode;
    int B3SOIigidlNode;
    int B3SOIitunNode;
    int B3SOIibpNode;
    int B3SOIcbbNode;
    int B3SOIcbdNode;
    int B3SOIcbgNode;
    int B3SOIqbNode;
    int B3SOIqbfNode;
    int B3SOIqjsNode;
    int B3SOIqjdNode;


    double B3SOIphi;
    double B3SOIvtm;
    double B3SOIni;
    double B3SOIueff;
    double B3SOIthetavth; 
    double B3SOIvon;
    double B3SOIvdsat;
    double B3SOIcgdo;
    double B3SOIcgso;
    double B3SOIids;
    double B3SOIic;
    double B3SOIibs;
    double B3SOIibd;
    double B3SOIiii;
    double B3SOIigidl;
    double B3SOIitun;
    double B3SOIibp;
    double B3SOIabeff;
    double B3SOIvbseff;
    double B3SOIcbg;
    double B3SOIcbb;
    double B3SOIcbd;
    double B3SOIqb;
    double B3SOIqbf;
    double B3SOIqjs;
    double B3SOIqjd;
    int    B3SOIfloat;


    double B3SOIl;
    double B3SOIw;
    double B3SOIdrainArea;
    double B3SOIsourceArea;
    double B3SOIdrainSquares;
    double B3SOIsourceSquares;
    double B3SOIdrainPerimeter;
    double B3SOIsourcePerimeter;
    double B3SOIsourceConductance;
    double B3SOIdrainConductance;

    double B3SOIicVBS;
    double B3SOIicVDS;
    double B3SOIicVGS;
    double B3SOIicVES;
    double B3SOIicVPS;
    int B3SOIbjtoff;
    int B3SOIbodyMod;
    int B3SOIdebugMod;
    double B3SOIrth0;
    double B3SOIcth0;
    double B3SOIbodySquares;
    double B3SOIrbodyext;


/* v2.0 release */
    double B3SOInbc;   
    double B3SOInseg;
    double B3SOIpdbcp;
    double B3SOIpsbcp;
    double B3SOIagbcp;
    double B3SOIaebcp;
    double B3SOIvbsusr;
    int B3SOItnodeout;


    int B3SOIoff;
    int B3SOImode;

    /* OP point */
    double B3SOIqinv;
    double B3SOIcd;
    double B3SOIcjs;
    double B3SOIcjd;
    double B3SOIcbody;
    double B3SOIcbodcon;
    double B3SOIcth;
    double B3SOIcsubstrate;

    double B3SOIgm;
    double B3SOIcb;
    double B3SOIcdrain;
    double B3SOIgds;
    double B3SOIgmbs;
    double B3SOIgmT;

    double B3SOIgbbs;
    double B3SOIgbgs;
    double B3SOIgbds;
    double B3SOIgbps;
    double B3SOIgbT;

    double B3SOIgjsd;
    double B3SOIgjsb;
    double B3SOIgjsg;
    double B3SOIgjsT;

    double B3SOIgjdb;
    double B3SOIgjdd;
    double B3SOIgjdg;
    double B3SOIgjdT;

    double B3SOIgbpbs;
    double B3SOIgbpps;
    double B3SOIgbpT;

    double B3SOIgtempb;
    double B3SOIgtempg;
    double B3SOIgtempd;
    double B3SOIgtempT;

    double B3SOIcggb;
    double B3SOIcgdb;
    double B3SOIcgsb;
    double B3SOIcgT;

    double B3SOIcbgb;
    double B3SOIcbdb;
    double B3SOIcbsb;
    double B3SOIcbeb;
    double B3SOIcbT;

    double B3SOIcdgb;
    double B3SOIcddb;
    double B3SOIcdsb;
    double B3SOIcdeb;
    double B3SOIcdT;

    double B3SOIceeb;
    double B3SOIceT;

    double B3SOIqse;
    double B3SOIgcse;
    double B3SOIqde;
    double B3SOIgcde;

    struct b3soiSizeDependParam  *pParam;

    unsigned B3SOIlGiven :1;
    unsigned B3SOIwGiven :1;
    unsigned B3SOIdrainAreaGiven :1;
    unsigned B3SOIsourceAreaGiven    :1;
    unsigned B3SOIdrainSquaresGiven  :1;
    unsigned B3SOIsourceSquaresGiven :1;
    unsigned B3SOIdrainPerimeterGiven    :1;
    unsigned B3SOIsourcePerimeterGiven   :1;
    unsigned B3SOIdNodePrimeSet  :1;
    unsigned B3SOIsNodePrimeSet  :1;
    unsigned B3SOIicVBSGiven :1;
    unsigned B3SOIicVDSGiven :1;
    unsigned B3SOIicVGSGiven :1;
    unsigned B3SOIicVESGiven :1;
    unsigned B3SOIicVPSGiven :1;
    unsigned B3SOIbjtoffGiven :1;
    unsigned B3SOIdebugModGiven :1;
    unsigned B3SOIrth0Given :1;
    unsigned B3SOIcth0Given :1;
    unsigned B3SOIbodySquaresGiven :1;


/* v2.0 release */
    unsigned B3SOInbcGiven :1;   
    unsigned B3SOInsegGiven :1;
    unsigned B3SOIpdbcpGiven :1;
    unsigned B3SOIpsbcpGiven :1;
    unsigned B3SOIagbcpGiven :1;
    unsigned B3SOIaebcpGiven :1;
    unsigned B3SOIvbsusrGiven :1;
    unsigned B3SOItnodeoutGiven :1;
    unsigned B3SOIoffGiven :1;


    double *B3SOIEePtr;
    double *B3SOIEbPtr;
    double *B3SOIBePtr;
    double *B3SOIEgPtr;
    double *B3SOIEdpPtr;
    double *B3SOIEspPtr;
    double *B3SOITemptempPtr;
    double *B3SOITempdpPtr;
    double *B3SOITempspPtr;
    double *B3SOITempgPtr;
    double *B3SOITempbPtr;
    double *B3SOIGtempPtr;
    double *B3SOIDPtempPtr;
    double *B3SOISPtempPtr;
    double *B3SOIEtempPtr;
    double *B3SOIBtempPtr;
    double *B3SOIPtempPtr;
    double *B3SOIBpPtr;
    double *B3SOIPbPtr;
    double *B3SOIPpPtr;
    double *B3SOIDdPtr;
    double *B3SOIGgPtr;
    double *B3SOISsPtr;
    double *B3SOIBbPtr;
    double *B3SOIDPdpPtr;
    double *B3SOISPspPtr;
    double *B3SOIDdpPtr;
    double *B3SOIGbPtr;
    double *B3SOIGdpPtr;
    double *B3SOIGspPtr;
    double *B3SOISspPtr;
    double *B3SOIBdpPtr;
    double *B3SOIBspPtr;
    double *B3SOIDPspPtr;
    double *B3SOIDPdPtr;
    double *B3SOIBgPtr;
    double *B3SOIDPgPtr;
    double *B3SOISPgPtr;
    double *B3SOISPsPtr;
    double *B3SOIDPbPtr;
    double *B3SOISPbPtr;
    double *B3SOISPdpPtr;

    double *B3SOIVbsPtr;
    /* Debug */
    double *B3SOIIdsPtr;
    double *B3SOIIcPtr;
    double *B3SOIIbsPtr;
    double *B3SOIIbdPtr;
    double *B3SOIIiiPtr;
    double *B3SOIIgidlPtr;
    double *B3SOIItunPtr;
    double *B3SOIIbpPtr;
    double *B3SOICbbPtr;
    double *B3SOICbdPtr;
    double *B3SOICbgPtr;
    double *B3SOIqbPtr;
    double *B3SOIQbfPtr;
    double *B3SOIQjsPtr;
    double *B3SOIQjdPtr;


#define B3SOIvbd B3SOIstates+ 0
#define B3SOIvbs B3SOIstates+ 1
#define B3SOIvgs B3SOIstates+ 2
#define B3SOIvds B3SOIstates+ 3
#define B3SOIves B3SOIstates+ 4
#define B3SOIvps B3SOIstates+ 5

#define B3SOIvg B3SOIstates+ 6
#define B3SOIvd B3SOIstates+ 7
#define B3SOIvs B3SOIstates+ 8
#define B3SOIvp B3SOIstates+ 9
#define B3SOIve B3SOIstates+ 10
#define B3SOIdeltemp B3SOIstates+ 11

#define B3SOIqb B3SOIstates+ 12
#define B3SOIcqb B3SOIstates+ 13
#define B3SOIqg B3SOIstates+ 14
#define B3SOIcqg B3SOIstates+ 15
#define B3SOIqd B3SOIstates+ 16
#define B3SOIcqd B3SOIstates+ 17
#define B3SOIqe B3SOIstates+ 18
#define B3SOIcqe B3SOIstates+ 19

#define B3SOIqbs  B3SOIstates+ 20
#define B3SOIqbd  B3SOIstates+ 21
#define B3SOIqbe  B3SOIstates+ 22

#define B3SOIqth B3SOIstates+ 23
#define B3SOIcqth B3SOIstates+ 24

#define B3SOInumStates 25


/* indices to the array of B3SOI NOISE SOURCES */

#define B3SOIRDNOIZ       0
#define B3SOIRSNOIZ       1
#define B3SOIIDNOIZ       2
#define B3SOIFLNOIZ       3
#define B3SOIFBNOIZ       4
#define B3SOITOTNOIZ      5

#define B3SOINSRCS        6     /* the number of MOSFET(3) noise sources */

#ifndef NONOISE
    double B3SOInVar[NSTATVARS][B3SOINSRCS];
#else /* NONOISE */
        double **B3SOInVar;
#endif /* NONOISE */

} B3SOIinstance ;

struct b3soiSizeDependParam
{
    double Width;
    double Length;
    double Rth0;
    double Cth0;

    double B3SOIcdsc;           
    double B3SOIcdscb;    
    double B3SOIcdscd;       
    double B3SOIcit;           
    double B3SOInfactor;      
    double B3SOIvsat;         
    double B3SOIat;         
    double B3SOIa0;   
    double B3SOIags;      
    double B3SOIa1;         
    double B3SOIa2;         
    double B3SOIketa;     
    double B3SOInpeak;        
    double B3SOInsub;
    double B3SOIngate;        
    double B3SOIgamma1;      
    double B3SOIgamma2;     
    double B3SOIvbx;      
    double B3SOIvbi;       
    double B3SOIvbm;       
    double B3SOIvbsc;       
    double B3SOIxt;       
    double B3SOIphi;
    double B3SOIlitl;
    double B3SOIk1;
    double B3SOIkt1;
    double B3SOIkt1l;
    double B3SOIkt2;
    double B3SOIk2;
    double B3SOIk3;
    double B3SOIk3b;
    double B3SOIw0;
    double B3SOInlx;
    double B3SOIdvt0;      
    double B3SOIdvt1;      
    double B3SOIdvt2;      
    double B3SOIdvt0w;      
    double B3SOIdvt1w;      
    double B3SOIdvt2w;      
    double B3SOIdrout;      
    double B3SOIdsub;      
    double B3SOIvth0;
    double B3SOIua;
    double B3SOIua1;
    double B3SOIub;
    double B3SOIub1;
    double B3SOIuc;
    double B3SOIuc1;
    double B3SOIu0;
    double B3SOIute;
    double B3SOIvoff;
    double B3SOIvfb;
    double B3SOIuatemp;
    double B3SOIubtemp;
    double B3SOIuctemp;
    double B3SOIrbody;
    double B3SOIrth;
    double B3SOIcth;
    double B3SOIrds0denom;
    double B3SOIvfbb;
    double B3SOIjbjt;
    double B3SOIjdif;
    double B3SOIjrec;
    double B3SOIjtun;
    double B3SOIcsesw;
    double B3SOIcdesw;
    double B3SOIcsbox;
    double B3SOIcdbox;
    double B3SOIcsmin;
    double B3SOIcdmin;
    double B3SOIsdt1;
    double B3SOIst2;
    double B3SOIst3;
    double B3SOIst4;
    double B3SOIdt2;
    double B3SOIdt3;
    double B3SOIdt4;
    double B3SOIdelta;
    double B3SOIrdsw;       
    double B3SOIrds0;       
    double B3SOIprwg;       
    double B3SOIprwb;       
    double B3SOIprt;       
    double B3SOIeta0;         
    double B3SOIetab;         
    double B3SOIpclm;      
    double B3SOIpdibl1;      
    double B3SOIpdibl2;      
    double B3SOIpdiblb;      
    double B3SOIpvag;       
    double B3SOIwr;
    double B3SOIdwg;
    double B3SOIdwb;
    double B3SOIb0;
    double B3SOIb1;
    double B3SOIalpha0;
    double B3SOIalpha1;
    double B3SOIbeta0;


    /* CV model */
    double B3SOIcgsl;
    double B3SOIcgdl;
    double B3SOIckappa;
    double B3SOIcf;
    double B3SOIclc;
    double B3SOIcle;


/* Pre-calculated constants */

    double B3SOIdw;
    double B3SOIdl;
    double B3SOIleff;
    double B3SOIweff;

    double B3SOIdwc;
    double B3SOIdlc;
    double B3SOIleffCV;
    double B3SOIweffCV;
    double B3SOIabulkCVfactor;
    double B3SOIcgso;
    double B3SOIcgdo;

    double B3SOIu0temp;       
    double B3SOIvsattemp;   
    double B3SOIsqrtPhi;   
    double B3SOIphis3;   
    double B3SOIXdep0;          
    double B3SOIsqrtXdep0;          
    double B3SOItheta0vb0;
    double B3SOIthetaRout; 


/* v2.0 release */
    double B3SOIk1eff;
    double B3SOIwdios;
    double B3SOIwdiod;
    double B3SOIwdiodCV;
    double B3SOIwdiosCV;
    double B3SOIarfabjt;
    double B3SOIlratio;
    double B3SOIlratiodif;
    double B3SOIvearly;
    double B3SOIahli;
    double B3SOIvfbzb;
    double B3SOIldeb;
    double B3SOIacde;
    double B3SOImoin;
    double B3SOIleffCVb;
    double B3SOIleffCVbg;

    double B3SOIcof1;
    double B3SOIcof2;
    double B3SOIcof3;
    double B3SOIcof4;
    double B3SOIcdep0;
    struct b3soiSizeDependParam  *pNext;
};


typedef struct sB3SOImodel 
{
    int B3SOImodType;
    struct sB3SOImodel *B3SOInextModel;
    B3SOIinstance *B3SOIinstances;
    IFuid B3SOImodName; 
    int B3SOItype;

    int    B3SOImobMod;
    int    B3SOIcapMod;
    int    B3SOInoiMod;
    int    B3SOIshMod;
    int    B3SOIbinUnit;
    int    B3SOIparamChk;
    double B3SOIversion;             
    double B3SOItox;             
    double B3SOIcdsc;           
    double B3SOIcdscb; 
    double B3SOIcdscd;          
    double B3SOIcit;           
    double B3SOInfactor;      
    double B3SOIvsat;         
    double B3SOIat;         
    double B3SOIa0;   
    double B3SOIags;      
    double B3SOIa1;         
    double B3SOIa2;         
    double B3SOIketa;     
    double B3SOInsub;
    double B3SOInpeak;        
    double B3SOIngate;        
    double B3SOIgamma1;      
    double B3SOIgamma2;     
    double B3SOIvbx;      
    double B3SOIvbm;       
    double B3SOIxt;       
    double B3SOIk1;
    double B3SOIkt1;
    double B3SOIkt1l;
    double B3SOIkt2;
    double B3SOIk2;
    double B3SOIk3;
    double B3SOIk3b;
    double B3SOIw0;
    double B3SOInlx;
    double B3SOIdvt0;      
    double B3SOIdvt1;      
    double B3SOIdvt2;      
    double B3SOIdvt0w;      
    double B3SOIdvt1w;      
    double B3SOIdvt2w;      
    double B3SOIdrout;      
    double B3SOIdsub;      
    double B3SOIvth0;
    double B3SOIua;
    double B3SOIua1;
    double B3SOIub;
    double B3SOIub1;
    double B3SOIuc;
    double B3SOIuc1;
    double B3SOIu0;
    double B3SOIute;
    double B3SOIvoff;
    double B3SOIdelta;
    double B3SOIrdsw;       
    double B3SOIprwg;
    double B3SOIprwb;
    double B3SOIprt;       
    double B3SOIeta0;         
    double B3SOIetab;         
    double B3SOIpclm;      
    double B3SOIpdibl1;      
    double B3SOIpdibl2;      
    double B3SOIpdiblb;
    double B3SOIpvag;       
    double B3SOIwr;
    double B3SOIdwg;
    double B3SOIdwb;
    double B3SOIb0;
    double B3SOIb1;
    double B3SOIalpha0;
    double B3SOIalpha1;
    double B3SOItbox;
    double B3SOItsi;
    double B3SOIxj;
    double B3SOIkb1;
    double B3SOIrth0;
    double B3SOIcth0;
    double B3SOIngidl;
    double B3SOIagidl;
    double B3SOIbgidl;
    double B3SOIndiode;
    double B3SOIistun;
    double B3SOIxbjt;
    double B3SOIxdif;
    double B3SOIxrec;
    double B3SOIxtun;


/* v2.0 release */
    double B3SOIk1w1;  
    double B3SOIk1w2;
    double B3SOIketas;
    double B3SOIdwbc;
    double B3SOIbeta0;
    double B3SOIbeta1;
    double B3SOIbeta2;
    double B3SOIvdsatii0;
    double B3SOItii;
    double B3SOIlii;
    double B3SOIsii0;
    double B3SOIsii1;
    double B3SOIsii2;
    double B3SOIsiid;
    double B3SOIfbjtii;
    double B3SOIesatii;
    double B3SOIntun;
    double B3SOIndio;
    double B3SOInrecf0;
    double B3SOInrecr0;
    double B3SOIisbjt;
    double B3SOIisdif;
    double B3SOIisrec;
    double B3SOIln;
    double B3SOIvrec0;
    double B3SOIvtun0;
    double B3SOInbjt;
    double B3SOIlbjt0;
    double B3SOIldif0;
    double B3SOIvabjt;
    double B3SOIaely;
    double B3SOIahli;
    double B3SOIrbody;
    double B3SOIrbsh;
    double B3SOIcgeo;
    double B3SOItt;
    double B3SOIndif;
    double B3SOIvsdfb;
    double B3SOIvsdth;
    double B3SOIcsdmin;
    double B3SOIasd;
    double B3SOIntrecf;
    double B3SOIntrecr;
    double B3SOIdlcb;
    double B3SOIfbody;
    double B3SOItcjswg;
    double B3SOItpbswg;
    double B3SOIacde;
    double B3SOImoin;
    double B3SOIdelvt;
    double B3SOIdlbg;

    /* CV model */
    double B3SOIcgsl;
    double B3SOIcgdl;
    double B3SOIckappa;
    double B3SOIcf;
    double B3SOIclc;
    double B3SOIcle;
    double B3SOIdwc;
    double B3SOIdlc;

    double B3SOItnom;
    double B3SOIcgso;
    double B3SOIcgdo;
    double B3SOIxpart;
    double B3SOIcFringOut;
    double B3SOIcFringMax;

    double B3SOIsheetResistance;
    double B3SOIbodyJctGateSideGradingCoeff;
    double B3SOIGatesidewallJctPotential;
    double B3SOIunitLengthGateSidewallJctCap;
    double B3SOIcsdesw;

    double B3SOILint;
    double B3SOILl;
    double B3SOILln;
    double B3SOILw;
    double B3SOILwn;
    double B3SOILwl;
    double B3SOILmin;
    double B3SOILmax;

    double B3SOIWint;
    double B3SOIWl;
    double B3SOIWln;
    double B3SOIWw;
    double B3SOIWwn;
    double B3SOIWwl;
    double B3SOIWmin;
    double B3SOIWmax;


/* Pre-calculated constants */
    double B3SOIcbox;
    double B3SOIcsi;
    double B3SOIcsieff;
    double B3SOIcoxt;
    double B3SOIcboxt;
    double B3SOIcsit;
    double B3SOInfb;
    double B3SOIadice;
    double B3SOIqsi;
    double B3SOIqsieff;
    double B3SOIeg0;

    /* MCJ: move to size-dependent param. */
    double B3SOIvtm;   
    double B3SOIcox;
    double B3SOIcof1;
    double B3SOIcof2;
    double B3SOIcof3;
    double B3SOIcof4;
    double B3SOIvcrit;
    double B3SOIfactor1;

    double B3SOIoxideTrapDensityA;      
    double B3SOIoxideTrapDensityB;     
    double B3SOIoxideTrapDensityC;  
    double B3SOIem;  
    double B3SOIef;  
    double B3SOIaf;  
    double B3SOIkf;  
    double B3SOInoif;  

    struct b3soiSizeDependParam *pSizeDependParamKnot;

    /* Flags */

    unsigned B3SOItboxGiven:1;
    unsigned B3SOItsiGiven :1;
    unsigned B3SOIxjGiven :1;
    unsigned B3SOIkb1Given :1;
    unsigned B3SOIrth0Given :1;
    unsigned B3SOIcth0Given :1;
    unsigned B3SOIngidlGiven :1;
    unsigned B3SOIagidlGiven :1;
    unsigned B3SOIbgidlGiven :1;
    unsigned B3SOIndiodeGiven :1;
    unsigned B3SOIxbjtGiven :1;
    unsigned B3SOIxdifGiven :1;
    unsigned B3SOIxrecGiven :1;
    unsigned B3SOIxtunGiven :1;
    unsigned B3SOIttGiven :1;
    unsigned B3SOIvsdfbGiven :1;
    unsigned B3SOIvsdthGiven :1;
    unsigned B3SOIasdGiven :1;
    unsigned B3SOIcsdminGiven :1;

    unsigned  B3SOImobModGiven :1;
    unsigned  B3SOIbinUnitGiven :1;
    unsigned  B3SOIcapModGiven :1;
    unsigned  B3SOIparamChkGiven :1;
    unsigned  B3SOInoiModGiven :1;
    unsigned  B3SOIshModGiven :1;
    unsigned  B3SOItypeGiven   :1;
    unsigned  B3SOItoxGiven   :1;
    unsigned  B3SOIversionGiven   :1;

    unsigned  B3SOIcdscGiven   :1;
    unsigned  B3SOIcdscbGiven   :1;
    unsigned  B3SOIcdscdGiven   :1;
    unsigned  B3SOIcitGiven   :1;
    unsigned  B3SOInfactorGiven   :1;
    unsigned  B3SOIvsatGiven   :1;
    unsigned  B3SOIatGiven   :1;
    unsigned  B3SOIa0Given   :1;
    unsigned  B3SOIagsGiven   :1;
    unsigned  B3SOIa1Given   :1;
    unsigned  B3SOIa2Given   :1;
    unsigned  B3SOIketaGiven   :1;    
    unsigned  B3SOInsubGiven   :1;
    unsigned  B3SOInpeakGiven   :1;
    unsigned  B3SOIngateGiven   :1;
    unsigned  B3SOIgamma1Given   :1;
    unsigned  B3SOIgamma2Given   :1;
    unsigned  B3SOIvbxGiven   :1;
    unsigned  B3SOIvbmGiven   :1;
    unsigned  B3SOIxtGiven   :1;
    unsigned  B3SOIk1Given   :1;
    unsigned  B3SOIkt1Given   :1;
    unsigned  B3SOIkt1lGiven   :1;
    unsigned  B3SOIkt2Given   :1;
    unsigned  B3SOIk2Given   :1;
    unsigned  B3SOIk3Given   :1;
    unsigned  B3SOIk3bGiven   :1;
    unsigned  B3SOIw0Given   :1;
    unsigned  B3SOInlxGiven   :1;
    unsigned  B3SOIdvt0Given   :1;   
    unsigned  B3SOIdvt1Given   :1;     
    unsigned  B3SOIdvt2Given   :1;     
    unsigned  B3SOIdvt0wGiven   :1;   
    unsigned  B3SOIdvt1wGiven   :1;     
    unsigned  B3SOIdvt2wGiven   :1;     
    unsigned  B3SOIdroutGiven   :1;     
    unsigned  B3SOIdsubGiven   :1;     
    unsigned  B3SOIvth0Given   :1;
    unsigned  B3SOIuaGiven   :1;
    unsigned  B3SOIua1Given   :1;
    unsigned  B3SOIubGiven   :1;
    unsigned  B3SOIub1Given   :1;
    unsigned  B3SOIucGiven   :1;
    unsigned  B3SOIuc1Given   :1;
    unsigned  B3SOIu0Given   :1;
    unsigned  B3SOIuteGiven   :1;
    unsigned  B3SOIvoffGiven   :1;
    unsigned  B3SOIrdswGiven   :1;      
    unsigned  B3SOIprwgGiven   :1;      
    unsigned  B3SOIprwbGiven   :1;      
    unsigned  B3SOIprtGiven   :1;      
    unsigned  B3SOIeta0Given   :1;    
    unsigned  B3SOIetabGiven   :1;    
    unsigned  B3SOIpclmGiven   :1;   
    unsigned  B3SOIpdibl1Given   :1;   
    unsigned  B3SOIpdibl2Given   :1;  
    unsigned  B3SOIpdiblbGiven   :1;  
    unsigned  B3SOIpvagGiven   :1;    
    unsigned  B3SOIdeltaGiven  :1;     
    unsigned  B3SOIwrGiven   :1;
    unsigned  B3SOIdwgGiven   :1;
    unsigned  B3SOIdwbGiven   :1;
    unsigned  B3SOIb0Given   :1;
    unsigned  B3SOIb1Given   :1;
    unsigned  B3SOIalpha0Given   :1;
    unsigned  B3SOIalpha1Given   :1;


/* v2.0 release */
    unsigned  B3SOIk1w1Given   :1;   
    unsigned  B3SOIk1w2Given   :1;
    unsigned  B3SOIketasGiven  :1;
    unsigned  B3SOIdwbcGiven  :1;
    unsigned  B3SOIbeta0Given  :1;
    unsigned  B3SOIbeta1Given  :1;
    unsigned  B3SOIbeta2Given  :1;
    unsigned  B3SOIvdsatii0Given  :1;
    unsigned  B3SOItiiGiven  :1;
    unsigned  B3SOIliiGiven  :1;
    unsigned  B3SOIsii0Given  :1;
    unsigned  B3SOIsii1Given  :1;
    unsigned  B3SOIsii2Given  :1;
    unsigned  B3SOIsiidGiven  :1;
    unsigned  B3SOIfbjtiiGiven :1;
    unsigned  B3SOIesatiiGiven :1;
    unsigned  B3SOIntunGiven  :1;
    unsigned  B3SOIndioGiven  :1;
    unsigned  B3SOInrecf0Given  :1;
    unsigned  B3SOInrecr0Given  :1;
    unsigned  B3SOIisbjtGiven  :1;
    unsigned  B3SOIisdifGiven  :1;
    unsigned  B3SOIisrecGiven  :1;
    unsigned  B3SOIistunGiven  :1;
    unsigned  B3SOIlnGiven  :1;
    unsigned  B3SOIvrec0Given  :1;
    unsigned  B3SOIvtun0Given  :1;
    unsigned  B3SOInbjtGiven  :1;
    unsigned  B3SOIlbjt0Given  :1;
    unsigned  B3SOIldif0Given  :1;
    unsigned  B3SOIvabjtGiven  :1;
    unsigned  B3SOIaelyGiven  :1;
    unsigned  B3SOIahliGiven  :1;
    unsigned  B3SOIrbodyGiven :1;
    unsigned  B3SOIrbshGiven  :1;
    unsigned  B3SOIndifGiven  :1;
    unsigned  B3SOIntrecfGiven  :1;
    unsigned  B3SOIntrecrGiven  :1;
    unsigned  B3SOIdlcbGiven    :1;
    unsigned  B3SOIfbodyGiven   :1;
    unsigned  B3SOItcjswgGiven  :1;
    unsigned  B3SOItpbswgGiven  :1;
    unsigned  B3SOIacdeGiven  :1;
    unsigned  B3SOImoinGiven  :1;
    unsigned  B3SOIdelvtGiven  :1;
    unsigned  B3SOIdlbgGiven  :1;

    
    /* CV model */
    unsigned  B3SOIcgslGiven   :1;
    unsigned  B3SOIcgdlGiven   :1;
    unsigned  B3SOIckappaGiven   :1;
    unsigned  B3SOIcfGiven   :1;
    unsigned  B3SOIclcGiven   :1;
    unsigned  B3SOIcleGiven   :1;
    unsigned  B3SOIdwcGiven   :1;
    unsigned  B3SOIdlcGiven   :1;

    unsigned  B3SOIuseFringeGiven   :1;

    unsigned  B3SOItnomGiven   :1;
    unsigned  B3SOIcgsoGiven   :1;
    unsigned  B3SOIcgdoGiven   :1;
    unsigned  B3SOIcgeoGiven   :1;
    unsigned  B3SOIxpartGiven   :1;
    unsigned  B3SOIsheetResistanceGiven   :1;
    unsigned  B3SOIGatesidewallJctPotentialGiven   :1;
    unsigned  B3SOIbodyJctGateSideGradingCoeffGiven   :1;
    unsigned  B3SOIunitLengthGateSidewallJctCapGiven   :1;
    unsigned  B3SOIcsdeswGiven :1;

    unsigned  B3SOIoxideTrapDensityAGiven  :1;         
    unsigned  B3SOIoxideTrapDensityBGiven  :1;        
    unsigned  B3SOIoxideTrapDensityCGiven  :1;     
    unsigned  B3SOIemGiven  :1;     
    unsigned  B3SOIefGiven  :1;     
    unsigned  B3SOIafGiven  :1;     
    unsigned  B3SOIkfGiven  :1;     
    unsigned  B3SOInoifGiven  :1;     

    unsigned  B3SOILintGiven   :1;
    unsigned  B3SOILlGiven   :1;
    unsigned  B3SOILlnGiven   :1;
    unsigned  B3SOILwGiven   :1;
    unsigned  B3SOILwnGiven   :1;
    unsigned  B3SOILwlGiven   :1;
    unsigned  B3SOILminGiven   :1;
    unsigned  B3SOILmaxGiven   :1;

    unsigned  B3SOIWintGiven   :1;
    unsigned  B3SOIWlGiven   :1;
    unsigned  B3SOIWlnGiven   :1;
    unsigned  B3SOIWwGiven   :1;
    unsigned  B3SOIWwnGiven   :1;
    unsigned  B3SOIWwlGiven   :1;
    unsigned  B3SOIWminGiven   :1;
    unsigned  B3SOIWmaxGiven   :1;

} B3SOImodel;


#ifndef NMOS
#define NMOS 1
#define PMOS -1
#endif /*NMOS*/


/* device parameters */
#define B3SOI_W 1
#define B3SOI_L 2
#define B3SOI_AS 3
#define B3SOI_AD 4
#define B3SOI_PS 5
#define B3SOI_PD 6
#define B3SOI_NRS 7
#define B3SOI_NRD 8
#define B3SOI_OFF 9
#define B3SOI_IC_VBS 10
#define B3SOI_IC_VDS 11
#define B3SOI_IC_VGS 12
#define B3SOI_IC_VES 13
#define B3SOI_IC_VPS 14
#define B3SOI_BJTOFF 15
#define B3SOI_RTH0 16
#define B3SOI_CTH0 17
#define B3SOI_NRB 18
#define B3SOI_IC 19
#define B3SOI_NQSMOD 20
#define B3SOI_DEBUG  21


/* v2.0 release */
#define B3SOI_NBC    22  
#define B3SOI_NSEG   23
#define B3SOI_PDBCP  24
#define B3SOI_PSBCP  25
#define B3SOI_AGBCP  26
#define B3SOI_AEBCP  27
#define B3SOI_VBSUSR 28
#define B3SOI_TNODEOUT 29


/* model parameters */
#define B3SOI_MOD_CAPMOD          101
#define B3SOI_MOD_NQSMOD          102
#define B3SOI_MOD_MOBMOD          103    
#define B3SOI_MOD_NOIMOD          104    
#define B3SOI_MOD_SHMOD           105
#define B3SOI_MOD_DDMOD           106

#define B3SOI_MOD_TOX             107



#define B3SOI_MOD_CDSC            108
#define B3SOI_MOD_CDSCB           109
#define B3SOI_MOD_CIT             110
#define B3SOI_MOD_NFACTOR         111
#define B3SOI_MOD_XJ              112
#define B3SOI_MOD_VSAT            113
#define B3SOI_MOD_AT              114
#define B3SOI_MOD_A0              115
#define B3SOI_MOD_A1              116
#define B3SOI_MOD_A2              117
#define B3SOI_MOD_KETA            118   
#define B3SOI_MOD_NSUB            119
#define B3SOI_MOD_NPEAK           120
#define B3SOI_MOD_NGATE           121
#define B3SOI_MOD_GAMMA1          122
#define B3SOI_MOD_GAMMA2          123
#define B3SOI_MOD_VBX             124
#define B3SOI_MOD_BINUNIT         125    

#define B3SOI_MOD_VBM             126

#define B3SOI_MOD_XT              127
#define B3SOI_MOD_K1              129
#define B3SOI_MOD_KT1             130
#define B3SOI_MOD_KT1L            131
#define B3SOI_MOD_K2              132
#define B3SOI_MOD_KT2             133
#define B3SOI_MOD_K3              134
#define B3SOI_MOD_K3B             135
#define B3SOI_MOD_W0              136
#define B3SOI_MOD_NLX             137

#define B3SOI_MOD_DVT0            138
#define B3SOI_MOD_DVT1            139
#define B3SOI_MOD_DVT2            140

#define B3SOI_MOD_DVT0W           141
#define B3SOI_MOD_DVT1W           142
#define B3SOI_MOD_DVT2W           143

#define B3SOI_MOD_DROUT           144
#define B3SOI_MOD_DSUB            145
#define B3SOI_MOD_VTH0            146
#define B3SOI_MOD_UA              147
#define B3SOI_MOD_UA1             148
#define B3SOI_MOD_UB              149
#define B3SOI_MOD_UB1             150
#define B3SOI_MOD_UC              151
#define B3SOI_MOD_UC1             152
#define B3SOI_MOD_U0              153
#define B3SOI_MOD_UTE             154
#define B3SOI_MOD_VOFF            155
#define B3SOI_MOD_DELTA           156
#define B3SOI_MOD_RDSW            157
#define B3SOI_MOD_PRT             158
#define B3SOI_MOD_LDD             159
#define B3SOI_MOD_ETA             160
#define B3SOI_MOD_ETA0            161
#define B3SOI_MOD_ETAB            162
#define B3SOI_MOD_PCLM            163
#define B3SOI_MOD_PDIBL1          164
#define B3SOI_MOD_PDIBL2          165
#define B3SOI_MOD_PSCBE1          166
#define B3SOI_MOD_PSCBE2          167
#define B3SOI_MOD_PVAG            168
#define B3SOI_MOD_WR              169
#define B3SOI_MOD_DWG             170
#define B3SOI_MOD_DWB             171
#define B3SOI_MOD_B0              172
#define B3SOI_MOD_B1              173
#define B3SOI_MOD_ALPHA0          174
#define B3SOI_MOD_PDIBLB          178

#define B3SOI_MOD_PRWG            179
#define B3SOI_MOD_PRWB            180

#define B3SOI_MOD_CDSCD           181
#define B3SOI_MOD_AGS             182

#define B3SOI_MOD_FRINGE          184
#define B3SOI_MOD_CGSL            186
#define B3SOI_MOD_CGDL            187
#define B3SOI_MOD_CKAPPA          188
#define B3SOI_MOD_CF              189
#define B3SOI_MOD_CLC             190
#define B3SOI_MOD_CLE             191
#define B3SOI_MOD_PARAMCHK        192
#define B3SOI_MOD_VERSION         193

#define B3SOI_MOD_TBOX            195
#define B3SOI_MOD_TSI             196
#define B3SOI_MOD_KB1             197
#define B3SOI_MOD_KB3             198
#define B3SOI_MOD_DVBD0           199
#define B3SOI_MOD_DVBD1           200
#define B3SOI_MOD_DELP            201
#define B3SOI_MOD_VBSA            202
#define B3SOI_MOD_RBODY           204
#define B3SOI_MOD_ADICE0          205
#define B3SOI_MOD_ABP             206
#define B3SOI_MOD_MXC             207
#define B3SOI_MOD_RTH0            208
#define B3SOI_MOD_CTH0            209
#define B3SOI_MOD_ALPHA1	  214
#define B3SOI_MOD_NGIDL           215
#define B3SOI_MOD_AGIDL           216
#define B3SOI_MOD_BGIDL           217
#define B3SOI_MOD_NDIODE          218
#define B3SOI_MOD_LDIOF           219
#define B3SOI_MOD_LDIOR           220
#define B3SOI_MOD_NTUN            221
#define B3SOI_MOD_ISBJT           222
#define B3SOI_MOD_ISDIF           223
#define B3SOI_MOD_ISREC           224
#define B3SOI_MOD_ISTUN           225
#define B3SOI_MOD_XBJT            226
#define B3SOI_MOD_XDIF            227
#define B3SOI_MOD_XREC            228
#define B3SOI_MOD_XTUN            229
#define B3SOI_MOD_TT              232
#define B3SOI_MOD_VSDTH           233
#define B3SOI_MOD_VSDFB           234
#define B3SOI_MOD_ASD             235
#define B3SOI_MOD_CSDMIN          236
#define B3SOI_MOD_RBSH            237
#define B3SOI_MOD_ESATII          238


/* v2.0 release */
#define B3SOI_MOD_K1W1            239     
#define B3SOI_MOD_K1W2            240
#define B3SOI_MOD_KETAS           241
#define B3SOI_MOD_DWBC            242
#define B3SOI_MOD_BETA0           243
#define B3SOI_MOD_BETA1           244
#define B3SOI_MOD_BETA2           245
#define B3SOI_MOD_VDSATII0        246
#define B3SOI_MOD_TII             247
#define B3SOI_MOD_LII             248
#define B3SOI_MOD_SII0            249
#define B3SOI_MOD_SII1            250
#define B3SOI_MOD_SII2            251
#define B3SOI_MOD_SIID            252
#define B3SOI_MOD_FBJTII          253
#define B3SOI_MOD_NDIO            254
#define B3SOI_MOD_NRECF0          255
#define B3SOI_MOD_NRECR0          256
#define B3SOI_MOD_LN              257
#define B3SOI_MOD_VREC0           258
#define B3SOI_MOD_VTUN0           259
#define B3SOI_MOD_NBJT            260
#define B3SOI_MOD_LBJT0           261
#define B3SOI_MOD_VABJT           262
#define B3SOI_MOD_AELY            263
#define B3SOI_MOD_AHLI            264
#define B3SOI_MOD_NTRECF          265
#define B3SOI_MOD_NTRECR          266
#define B3SOI_MOD_DLCB            267
#define B3SOI_MOD_FBODY           268
#define B3SOI_MOD_NDIF            269
#define B3SOI_MOD_TCJSWG          270
#define B3SOI_MOD_TPBSWG          271
#define B3SOI_MOD_ACDE            272
#define B3SOI_MOD_MOIN            273
#define B3SOI_MOD_DELVT           274
#define B3SOI_MOD_DLBG            275
#define B3SOI_MOD_LDIF0           276


#define B3SOI_MOD_TNOM             701
#define B3SOI_MOD_CGSO             702
#define B3SOI_MOD_CGDO             703
#define B3SOI_MOD_CGEO             704
#define B3SOI_MOD_XPART            705

#define B3SOI_MOD_RSH              706
#define B3SOI_MOD_NMOS             814
#define B3SOI_MOD_PMOS             815

#define B3SOI_MOD_NOIA             816
#define B3SOI_MOD_NOIB             817
#define B3SOI_MOD_NOIC             818

#define B3SOI_MOD_LINT             819
#define B3SOI_MOD_LL               820
#define B3SOI_MOD_LLN              821
#define B3SOI_MOD_LW               822
#define B3SOI_MOD_LWN              823
#define B3SOI_MOD_LWL              824

#define B3SOI_MOD_WINT             827
#define B3SOI_MOD_WL               828
#define B3SOI_MOD_WLN              829
#define B3SOI_MOD_WW               830
#define B3SOI_MOD_WWN              831
#define B3SOI_MOD_WWL              832

#define B3SOI_MOD_DWC              835
#define B3SOI_MOD_DLC              836

#define B3SOI_MOD_EM               837
#define B3SOI_MOD_EF               838
#define B3SOI_MOD_AF               839
#define B3SOI_MOD_KF               840
#define B3SOI_MOD_NOIF             841


#define B3SOI_MOD_PBSWG            843
#define B3SOI_MOD_MJSWG            844
#define B3SOI_MOD_CJSWG            845
#define B3SOI_MOD_CSDESW           846

/* device questions */
#define B3SOI_DNODE                901
#define B3SOI_GNODE                902
#define B3SOI_SNODE                903
#define B3SOI_BNODE                904
#define B3SOI_ENODE                905
#define B3SOI_DNODEPRIME           906
#define B3SOI_SNODEPRIME           907
#define B3SOI_VBD                  908
#define B3SOI_VBS                  909
#define B3SOI_VGS                  910
#define B3SOI_VES                  911
#define B3SOI_VDS                  912
#define B3SOI_CD                   913
#define B3SOI_CBS                  914
#define B3SOI_CBD                  915
#define B3SOI_GM                   916
#define B3SOI_GDS                  917
#define B3SOI_GMBS                 918
#define B3SOI_GBD                  919
#define B3SOI_GBS                  920
#define B3SOI_QB                   921
#define B3SOI_CQB                  922
#define B3SOI_QG                   923
#define B3SOI_CQG                  924
#define B3SOI_QD                   925
#define B3SOI_CQD                  926
#define B3SOI_CGG                  927
#define B3SOI_CGD                  928
#define B3SOI_CGS                  929
#define B3SOI_CBG                  930
#define B3SOI_CAPBD                931
#define B3SOI_CQBD                 932
#define B3SOI_CAPBS                933
#define B3SOI_CQBS                 934
#define B3SOI_CDG                  935
#define B3SOI_CDD                  936
#define B3SOI_CDS                  937
#define B3SOI_VON                  938
#define B3SOI_VDSAT                939
#define B3SOI_QBS                  940
#define B3SOI_QBD                  941
#define B3SOI_SOURCECONDUCT        942
#define B3SOI_DRAINCONDUCT         943
#define B3SOI_CBDB                 944
#define B3SOI_CBSB                 945
#define B3SOI_GMID                 946


#include "b3soiext.h"

#ifdef __STDC__
extern void B3SOIevaluate(double,double,double,B3SOIinstance*,B3SOImodel*,
        double*,double*,double*, double*, double*, double*, double*, 
        double*, double*, double*, double*, double*, double*, double*, 
        double*, double*, double*, double*, CKTcircuit*);
extern int B3SOIdebug(B3SOImodel*, B3SOIinstance*, CKTcircuit*, int);
extern int B3SOIcheckModel(B3SOImodel*, B3SOIinstance*, CKTcircuit*);
#else /* stdc */
extern void B3SOIevaluate();
extern int B3SOIdebug();
extern int B3SOIcheckModel();
#endif /* stdc */

#endif /*B3SOI*/

