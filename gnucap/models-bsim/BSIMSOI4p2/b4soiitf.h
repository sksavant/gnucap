/**********
Copyright 2009 Regents of the University of California.  All rights reserved.
Author: 2005 Hui Wan (based on Samuel Fung's b3soiitf.h)
Authors: 2009- Wenwei Yang, Chung-Hsun Lin, Ali Niknejad, Chenming Hu.
Authors: 2009- Tanvir Morshed, Ali Niknejad, Chenming Hu.
File: b4soiitf.h
**********/
#ifdef DEV_b4soi

#ifndef DEV_B4SOI
#define DEV_B4SOI

#include "b4soiext.h"

extern IFparm B4SOIpTable[ ];
extern IFparm B4SOImPTable[ ];
extern char *B4SOInames[ ];
extern int B4SOIpTSize;
extern int B4SOImPTSize;
extern int B4SOInSize;
extern int B4SOIiSize;
extern int B4SOImSize;

SPICEdev B4SOIinfo = {
    {   "B4SOI",
        "Berkeley SOI MOSFET model version 2.0",

        &B4SOInSize,
        &B4SOInSize,
        B4SOInames,

        &B4SOIpTSize,
        B4SOIpTable,

        &B4SOImPTSize,
        B4SOImPTable,
	DEV_DEFAULT
    },

    B4SOIparam,
    B4SOImParam,
    B4SOIload,
    B4SOIsetup,
    B4SOIunsetup,
    B4SOIsetup,
    B4SOItemp,
    B4SOItrunc,
    NULL,
    B4SOIacLoad,
    NULL,
    B4SOIdestroy,
#ifdef DELETES
    B4SOImDelete,
    B4SOIdelete, 
#else /* DELETES */
    NULL,
    NULL,
#endif /* DELETES */
    B4SOIgetic,
    B4SOIask,
    B4SOImAsk,
#ifdef AN_pz
    B4SOIpzLoad,
#else /* AN_pz */
    NULL,
#endif /* AN_pz */
#ifdef NEWCONV
    B4SOIconvTest,
#else /* NEWCONV */
    NULL,
#endif /* NEWCONV */
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

#ifdef AN_noise
    B4SOInoise,
#else   /* AN_noise */
    NULL,
#endif  /* AN_noise */

    &B4SOIiSize,
    &B4SOImSize

};

#endif
#endif

