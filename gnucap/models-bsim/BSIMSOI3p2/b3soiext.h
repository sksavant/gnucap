/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung
File: b3soiext.h
**********/

#ifdef __STDC__
extern int B3SOIacLoad(GENmodel *,CKTcircuit*);
extern int B3SOIask(CKTcircuit *,GENinstance*,int,IFvalue*,IFvalue*);
extern int B3SOIconvTest(GENmodel *,CKTcircuit*);
extern int B3SOIdelete(GENmodel*,IFuid,GENinstance**);
extern void B3SOIdestroy(GENmodel**);
extern int B3SOIgetic(GENmodel*,CKTcircuit*);
extern int B3SOIload(GENmodel*,CKTcircuit*);
extern int B3SOImAsk(CKTcircuit*,GENmodel *,int, IFvalue*);
extern int B3SOImDelete(GENmodel**,IFuid,GENmodel*);
extern int B3SOImParam(int,IFvalue*,GENmodel*);
extern void B3SOImosCap(CKTcircuit*, double, double, double, double,
        double, double, double, double, double, double, double,
        double, double, double, double, double, double, double*,
        double*, double*, double*, double*, double*, double*, double*,
        double*, double*, double*, double*, double*, double*, double*, 
        double*);
extern int B3SOIparam(int,IFvalue*,GENinstance*,IFvalue*);
extern int B3SOIpzLoad(GENmodel*,CKTcircuit*,SPcomplex*);
extern int B3SOIsetup(SMPmatrix*,GENmodel*,CKTcircuit*,int*);
extern int B3SOItemp(GENmodel*,CKTcircuit*);
extern int B3SOItrunc(GENmodel*,CKTcircuit*,double*);
extern int B3SOInoise(int,int,GENmodel*,CKTcircuit*,Ndata*,double*);
extern int B3SOIunsetup(GENmodel*,CKTcircuit*);

#else /* stdc */
extern int B3SOIacLoad();
extern int B3SOIdelete();
extern void B3SOIdestroy();
extern int B3SOIgetic();
extern int B3SOIload();
extern int B3SOImDelete();
extern int B3SOIask();
extern int B3SOImAsk();
extern int B3SOIconvTest();
extern int B3SOItemp();
extern int B3SOImParam();
extern void B3SOImosCap();
extern int B3SOIparam();
extern int B3SOIpzLoad();
extern int B3SOIsetup();
extern int B3SOItrunc();
extern int B3SOInoise();
extern int B3SOIunsetup();

#endif /* stdc */

