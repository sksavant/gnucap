/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung
File: b3soiitf.h
**********/
#ifdef DEV_b3soi

#ifndef DEV_B3SOI
#define DEV_B3SOI

#include "b3soiext.h"

extern IFparm B3SOIpTable[ ];
extern IFparm B3SOImPTable[ ];
extern char *B3SOInames[ ];
extern int B3SOIpTSize;
extern int B3SOImPTSize;
extern int B3SOInSize;
extern int B3SOIiSize;
extern int B3SOImSize;

SPICEdev B3SOIinfo = {
    {   "B3SOI",
        "Berkeley SOI MOSFET model version 2.0",

        &B3SOInSize,
        &B3SOInSize,
        B3SOInames,

        &B3SOIpTSize,
        B3SOIpTable,

        &B3SOImPTSize,
        B3SOImPTable,
	DEV_DEFAULT
    },

    B3SOIparam,
    B3SOImParam,
    B3SOIload,
    B3SOIsetup,
    B3SOIunsetup,
    B3SOIsetup,
    B3SOItemp,
    B3SOItrunc,
    NULL,
    B3SOIacLoad,
    NULL,
    B3SOIdestroy,
#ifdef DELETES
    B3SOImDelete,
    B3SOIdelete, 
#else /* DELETES */
    NULL,
    NULL,
#endif /* DELETES */
    B3SOIgetic,
    B3SOIask,
    B3SOImAsk,
#ifdef AN_pz
    B3SOIpzLoad,
#else /* AN_pz */
    NULL,
#endif /* AN_pz */
#ifdef NEWCONV
    B3SOIconvTest,
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
    B3SOInoise,
#else   /* AN_noise */
    NULL,
#endif  /* AN_noise */

    &B3SOIiSize,
    &B3SOImSize

};

#endif
#endif

