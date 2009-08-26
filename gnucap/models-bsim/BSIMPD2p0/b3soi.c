/* $Id: b3soi.c,v 2.0 99/2/15 Pin Su Release $  */
/*
$Log:   b3soi.c,v $
 * Revision 2.0  99/2/15  Pin Su 
 * BSIMPD2.0 release
 *
*/
static char rcsid[] = "$Id: b3soi.c,v 2.0 99/2/15 Pin Su Release $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1998 Samuel Fung, Dennis Sinitsky and Stephen Tang
File: b3soi.c          98/5/01
Modified by Pin Su and Jan Feng	99/2/15 
**********/


#include "spice.h"
#include <stdio.h>
#include "devdefs.h"
#include "b3soidef.h"
#include "suffix.h"

IFparm B3SOIpTable[] = { /* parameters */
IOP( "l",   B3SOI_L,      IF_REAL   , "Length"),
IOP( "w",   B3SOI_W,      IF_REAL   , "Width"),
IOP( "ad",  B3SOI_AD,     IF_REAL   , "Drain area"),
IOP( "as",  B3SOI_AS,     IF_REAL   , "Source area"),
IOP( "pd",  B3SOI_PD,     IF_REAL   , "Drain perimeter"),
IOP( "ps",  B3SOI_PS,     IF_REAL   , "Source perimeter"),
IOP( "nrd", B3SOI_NRD,    IF_REAL   , "Number of squares in drain"),
IOP( "nrs", B3SOI_NRS,    IF_REAL   , "Number of squares in source"),
IOP( "off", B3SOI_OFF,    IF_FLAG   , "Device is initially off"),
IP( "ic",  B3SOI_IC,     IF_REALVEC , "Vector of DS,GS,BS initial voltages"),
OP( "gmbs",         B3SOI_GMBS,       IF_REAL,    "Gmb"),
OP( "gm",           B3SOI_GM,         IF_REAL,    "Gm"),
OP( "gm/ids",       B3SOI_GMID,       IF_REAL,    "Gm/Ids"),
OP( "gds",          B3SOI_GDS,        IF_REAL,    "Gds"),
OP( "vdsat",        B3SOI_VDSAT,      IF_REAL,    "Vdsat"),
OP( "vth",          B3SOI_VON,        IF_REAL,    "Vth"),
OP( "ids",          B3SOI_CD,         IF_REAL,    "Ids"),
OP( "vbs",          B3SOI_VBS,        IF_REAL,    "Vbs"),
OP( "vgs",          B3SOI_VGS,        IF_REAL,    "Vgs"),
OP( "vds",          B3SOI_VDS,        IF_REAL,    "Vds"),
OP( "ves",	    B3SOI_VES,	      IF_REAL,    "Ves"),
IOP( "bjtoff", B3SOI_BJTOFF,	      IF_INTEGER, "BJT on/off flag"),
IOP( "debug", B3SOI_DEBUG,	      IF_INTEGER, "BJT on/off flag"),
IOP( "rth0", B3SOI_RTH0,  IF_REAL,    "Instance Thermal Resistance"),
IOP( "cth0", B3SOI_CTH0,  IF_REAL,    "Instance Thermal Capacitance"),
IOP( "nrb", B3SOI_NRB,    IF_REAL,    "Number of squares in body"),


/* v2.0 release */
IOP( "nbc",      B3SOI_NBC,      IF_REAL, "Number of body contact isolation edge"),
IOP( "nseg",     B3SOI_NSEG,     IF_REAL, "Number segments for width partitioning"),
IOP( "pdbcp",    B3SOI_PDBCP,    IF_REAL, "Perimeter length for bc parasitics at drain side"),
IOP( "psbcp",    B3SOI_PSBCP,    IF_REAL, "Perimeter length for bc parasitics at source side"),
IOP( "agbcp",    B3SOI_AGBCP,    IF_REAL, "Gate to body overlap area for bc parasitics"),
IOP( "aebcp",    B3SOI_AEBCP,    IF_REAL, "Substrate to body overlap area for bc prasitics"),
IOP( "vbsusr",   B3SOI_VBSUSR,   IF_REAL, "Vbs specified by user"),
IOP( "tnodeout", B3SOI_TNODEOUT, IF_FLAG, "Flag indicating external temp node")

};

IFparm B3SOImPTable[] = { /* model parameters */
IOP( "capmod", B3SOI_MOD_CAPMOD, IF_INTEGER, "Capacitance model selector"),
IOP( "mobmod", B3SOI_MOD_MOBMOD, IF_INTEGER, "Mobility model selector"),
IOP( "noimod", B3SOI_MOD_NOIMOD, IF_INTEGER, "Noise model selector"),
IOP( "paramchk", B3SOI_MOD_PARAMCHK, IF_INTEGER, "Model parameter checking selector"),
IOP( "binunit", B3SOI_MOD_BINUNIT, IF_INTEGER, "Bin  unit  selector"),
IOP( "version", B3SOI_MOD_VERSION, IF_REAL, " parameter for model version"),
IOP( "tox", B3SOI_MOD_TOX, IF_REAL, "Gate oxide thickness in meters"),

IOP( "cdsc", B3SOI_MOD_CDSC, IF_REAL, "Drain/Source and channel coupling capacitance"),
IOP( "cdscb", B3SOI_MOD_CDSCB, IF_REAL, "Body-bias dependence of cdsc"), 
IOP( "cdscd", B3SOI_MOD_CDSCD, IF_REAL, "Drain-bias dependence of cdsc"), 
IOP( "cit", B3SOI_MOD_CIT, IF_REAL, "Interface state capacitance"),
IOP( "nfactor", B3SOI_MOD_NFACTOR, IF_REAL, "Subthreshold swing Coefficient"),
IOP( "vsat", B3SOI_MOD_VSAT, IF_REAL, "Saturation velocity at tnom"),
IOP( "at", B3SOI_MOD_AT, IF_REAL, "Temperature coefficient of vsat"),
IOP( "a0", B3SOI_MOD_A0, IF_REAL, "Non-uniform depletion width effect coefficient."), 
IOP( "ags", B3SOI_MOD_AGS, IF_REAL, "Gate bias  coefficient of Abulk."), 
IOP( "a1", B3SOI_MOD_A1, IF_REAL, "Non-saturation effect coefficient"),
IOP( "a2", B3SOI_MOD_A2, IF_REAL, "Non-saturation effect coefficient"),
IOP( "keta", B3SOI_MOD_KETA, IF_REAL, "Body-bias coefficient of non-uniform depletion width effect."),
IOP( "nsub", B3SOI_MOD_NSUB, IF_REAL, "Substrate doping concentration with polarity"),
IOP( "nch", B3SOI_MOD_NPEAK, IF_REAL, "Channel doping concentration"),
IOP( "ngate", B3SOI_MOD_NGATE, IF_REAL, "Poly-gate doping concentration"),
IOP( "gamma1", B3SOI_MOD_GAMMA1, IF_REAL, "Vth body coefficient"),
IOP( "gamma2", B3SOI_MOD_GAMMA2, IF_REAL, "Vth body coefficient"),
IOP( "vbx", B3SOI_MOD_VBX, IF_REAL, "Vth transition body Voltage"),
IOP( "vbm", B3SOI_MOD_VBM, IF_REAL, "Maximum body voltage"),

IOP( "xt", B3SOI_MOD_XT, IF_REAL, "Doping depth"),
IOP( "k1", B3SOI_MOD_K1, IF_REAL, "Bulk effect coefficient 1"),
IOP( "kt1", B3SOI_MOD_KT1, IF_REAL, "Temperature coefficient of Vth"),
IOP( "kt1l", B3SOI_MOD_KT1L, IF_REAL, "Temperature coefficient of Vth"),
IOP( "kt2",  B3SOI_MOD_KT2, IF_REAL, "Body-coefficient of kt1"),
IOP( "k2",   B3SOI_MOD_K2,  IF_REAL, "Bulk effect coefficient 2"),
IOP( "k3",   B3SOI_MOD_K3,  IF_REAL, "Narrow width effect coefficient"),
IOP( "k3b",  B3SOI_MOD_K3B, IF_REAL, "Body effect coefficient of k3"),
IOP( "w0",   B3SOI_MOD_W0,  IF_REAL, "Narrow width effect parameter"),
IOP( "nlx",  B3SOI_MOD_NLX, IF_REAL, "Lateral non-uniform doping effect"),
IOP( "dvt0", B3SOI_MOD_DVT0, IF_REAL, "Short channel effect coeff. 0"),
IOP( "dvt1", B3SOI_MOD_DVT1, IF_REAL, "Short channel effect coeff. 1"),
IOP( "dvt2", B3SOI_MOD_DVT2, IF_REAL, "Short channel effect coeff. 2"),
IOP( "dvt0w", B3SOI_MOD_DVT0W, IF_REAL, "Narrow Width coeff. 0"),
IOP( "dvt1w", B3SOI_MOD_DVT1W, IF_REAL, "Narrow Width effect coeff. 1"),
IOP( "dvt2w", B3SOI_MOD_DVT2W, IF_REAL, "Narrow Width effect coeff. 2"),
IOP( "drout", B3SOI_MOD_DROUT, IF_REAL, "DIBL coefficient of output resistance"),
IOP( "dsub", B3SOI_MOD_DSUB, IF_REAL, "DIBL coefficient in the subthreshold region"),
IOP( "vth0", B3SOI_MOD_VTH0, IF_REAL,"Threshold voltage"),
IOP( "vtho", B3SOI_MOD_VTH0, IF_REAL,"Threshold voltage"),
IOP( "ua", B3SOI_MOD_UA, IF_REAL, "Linear gate dependence of mobility"),
IOP( "ua1", B3SOI_MOD_UA1, IF_REAL, "Temperature coefficient of ua"),
IOP( "ub", B3SOI_MOD_UB, IF_REAL, "Quadratic gate dependence of mobility"),
IOP( "ub1", B3SOI_MOD_UB1, IF_REAL, "Temperature coefficient of ub"),
IOP( "uc", B3SOI_MOD_UC, IF_REAL, "Body-bias dependence of mobility"),
IOP( "uc1", B3SOI_MOD_UC1, IF_REAL, "Temperature coefficient of uc"),
IOP( "u0", B3SOI_MOD_U0, IF_REAL, "Low-field mobility at Tnom"),
IOP( "ute", B3SOI_MOD_UTE, IF_REAL, "Temperature coefficient of mobility"),
IOP( "voff", B3SOI_MOD_VOFF, IF_REAL, "Threshold voltage offset"),
IOP( "tnom", B3SOI_MOD_TNOM, IF_REAL, "Parameter measurement temperature"),
IOP( "cgso", B3SOI_MOD_CGSO, IF_REAL, "Gate-source overlap capacitance per width"),
IOP( "cgdo", B3SOI_MOD_CGDO, IF_REAL, "Gate-drain overlap capacitance per width"),
IOP( "cgeo", B3SOI_MOD_CGEO, IF_REAL, "Gate-substrate overlap capacitance"),
IOP( "xpart", B3SOI_MOD_XPART, IF_REAL, "Channel charge partitioning"),
IOP( "delta", B3SOI_MOD_DELTA, IF_REAL, "Effective Vds parameter"),
IOP( "rsh", B3SOI_MOD_RSH, IF_REAL, "Source-drain sheet resistance"),
IOP( "rdsw", B3SOI_MOD_RDSW, IF_REAL, "Source-drain resistance per width"),    

IOP( "prwg", B3SOI_MOD_PRWG, IF_REAL, "Gate-bias effect on parasitic resistance "),    
IOP( "prwb", B3SOI_MOD_PRWB, IF_REAL, "Body-effect on parasitic resistance "),    

IOP( "prt", B3SOI_MOD_PRT, IF_REAL, "Temperature coefficient of parasitic resistance "),    
IOP( "eta0", B3SOI_MOD_ETA0, IF_REAL, "Subthreshold region DIBL coefficient"),
IOP( "etab", B3SOI_MOD_ETAB, IF_REAL, "Subthreshold region DIBL coefficient"),
IOP( "pclm", B3SOI_MOD_PCLM, IF_REAL, "Channel length modulation Coefficient"),
IOP( "pdiblc1", B3SOI_MOD_PDIBL1, IF_REAL, "Drain-induced barrier lowering coefficient"),   
IOP( "pdiblc2", B3SOI_MOD_PDIBL2, IF_REAL, "Drain-induced barrier lowering coefficient"),   
IOP( "pdiblcb", B3SOI_MOD_PDIBLB, IF_REAL, "Body-effect on drain-induced barrier lowering"),   

IOP( "pvag", B3SOI_MOD_PVAG, IF_REAL, "Gate dependence of output resistance parameter"),   

IOP( "shmod", B3SOI_MOD_SHMOD, IF_INTEGER, "Self heating mode selector"),
IOP( "ddmod", B3SOI_MOD_DDMOD, IF_INTEGER, "Dynamic depletion mode selector"),
IOP( "tbox", B3SOI_MOD_TBOX, IF_REAL, "Back gate oxide thickness in meters"),   
IOP( "tsi", B3SOI_MOD_TSI, IF_REAL, "Silicon-on-insulator thickness in meters"),   
IOP( "xj", B3SOI_MOD_XJ, IF_REAL, "Junction Depth"),   
IOP( "rbody", B3SOI_MOD_RBODY, IF_REAL, "Intrinsic body contact sheet resistance"),   
IOP( "rbsh",  B3SOI_MOD_RBSH,  IF_REAL, "Extrinsic body contact sheet resistance"),
IOP( "rth0", B3SOI_MOD_RTH0, IF_REAL, "Self-heating thermal resistance"),   
IOP( "cth0", B3SOI_MOD_CTH0, IF_REAL, "Self-heating thermal capacitance"),
IOP( "ngidl", B3SOI_MOD_NGIDL, IF_REAL, "GIDL first parameter"),   
IOP( "agidl", B3SOI_MOD_AGIDL, IF_REAL, "GIDL second parameter"),   
IOP( "bgidl", B3SOI_MOD_BGIDL, IF_REAL, "GIDL third parameter"),   
IOP( "ndiode", B3SOI_MOD_NDIODE, IF_REAL, "Diode non-ideality factor"),   
IOP( "ntun", B3SOI_MOD_NTUN, IF_REAL, "Reverse tunneling non-ideality factor"),   
IOP( "isbjt", B3SOI_MOD_ISBJT, IF_REAL, "BJT emitter injection constant"),   
IOP( "isdif", B3SOI_MOD_ISDIF, IF_REAL, "Body to S/D injection constant"),   
IOP( "isrec", B3SOI_MOD_ISREC, IF_REAL, "Recombination in depletion constant"),   
IOP( "istun", B3SOI_MOD_ISTUN, IF_REAL, "Tunneling diode constant"),   
IOP( "xbjt", B3SOI_MOD_XBJT, IF_REAL, "Temperature coefficient for Isbjt"),   
IOP( "xdif", B3SOI_MOD_XBJT, IF_REAL, "Temperature coefficient for Isdif"),   
IOP( "xrec", B3SOI_MOD_XREC, IF_REAL, "Temperature coefficient for Isrec"),   
IOP( "xtun", B3SOI_MOD_XTUN, IF_REAL, "Temperature coefficient for Istun"),   
IOP( "tt", B3SOI_MOD_TT, IF_REAL, "Diffusion capacitance transit time coefficient"),
IOP( "vsdth", B3SOI_MOD_VSDTH, IF_REAL, "Source/Drain diffusion threshold voltage"),
IOP( "vsdfb", B3SOI_MOD_VSDFB, IF_REAL, "Source/Drain diffusion flatband voltage"),
IOP( "csdmin", B3SOI_MOD_CSDMIN, IF_REAL, "Source/Drain diffusion bottom minimum capacitance"),
IOP( "asd", B3SOI_MOD_ASD, IF_REAL, "Source/Drain diffusion smoothing parameter"),

IOP( "pbswg", B3SOI_MOD_PBSWG, IF_REAL, "Source/drain (gate side) sidewall junction capacitance built in potential"),
IOP( "mjswg", B3SOI_MOD_MJSWG, IF_REAL, "Source/drain (gate side) sidewall junction capacitance grading coefficient"),

IOP( "cjswg", B3SOI_MOD_CJSWG, IF_REAL, "Source/drain (gate side) sidewall junction capacitance per unit width"),
IOP( "csdesw", B3SOI_MOD_CSDESW, IF_REAL, "Source/drain sidewall fringing constant"),
IOP( "lint", B3SOI_MOD_LINT, IF_REAL, "Length reduction parameter"),
IOP( "ll",   B3SOI_MOD_LL, IF_REAL, "Length reduction parameter"),
IOP( "lln",  B3SOI_MOD_LLN, IF_REAL, "Length reduction parameter"),
IOP( "lw",   B3SOI_MOD_LW,  IF_REAL, "Length reduction parameter"),
IOP( "lwn",  B3SOI_MOD_LWN, IF_REAL, "Length reduction parameter"),
IOP( "lwl",  B3SOI_MOD_LWL, IF_REAL, "Length reduction parameter"),

IOP( "wr",   B3SOI_MOD_WR, IF_REAL, "Width dependence of rds"),
IOP( "wint", B3SOI_MOD_WINT, IF_REAL, "Width reduction parameter"),
IOP( "dwg",  B3SOI_MOD_DWG, IF_REAL, "Width reduction parameter"),
IOP( "dwb",  B3SOI_MOD_DWB, IF_REAL, "Width reduction parameter"),

IOP( "wl",   B3SOI_MOD_WL, IF_REAL, "Width reduction parameter"),
IOP( "wln",  B3SOI_MOD_WLN, IF_REAL, "Width reduction parameter"),
IOP( "ww",   B3SOI_MOD_WW, IF_REAL, "Width reduction parameter"),
IOP( "wwn",  B3SOI_MOD_WWN, IF_REAL, "Width reduction parameter"),
IOP( "wwl",  B3SOI_MOD_WWL, IF_REAL, "Width reduction parameter"),

IOP( "b0",  B3SOI_MOD_B0, IF_REAL, "Abulk narrow width parameter"),
IOP( "b1",  B3SOI_MOD_B1, IF_REAL, "Abulk narrow width parameter"),

IOP( "cgsl", B3SOI_MOD_CGSL, IF_REAL, "New C-V model parameter"),
IOP( "cgdl", B3SOI_MOD_CGDL, IF_REAL, "New C-V model parameter"),
IOP( "ckappa", B3SOI_MOD_CKAPPA, IF_REAL, "New C-V model parameter"),
IOP( "cf",  B3SOI_MOD_CF, IF_REAL, "Fringe capacitance parameter"),
IOP( "clc", B3SOI_MOD_CLC, IF_REAL, "Vdsat parameter for C-V model"),
IOP( "cle", B3SOI_MOD_CLE, IF_REAL, "Vdsat parameter for C-V model"),
IOP( "dwc", B3SOI_MOD_DWC, IF_REAL, "Delta W for C-V model"),
IOP( "dlc", B3SOI_MOD_DLC, IF_REAL, "Delta L for C-V model"),

IOP( "alpha0", B3SOI_MOD_ALPHA0, IF_REAL, "substrate current model parameter"),

IOP( "noia", B3SOI_MOD_NOIA, IF_REAL, "Flicker noise parameter"),
IOP( "noib", B3SOI_MOD_NOIB, IF_REAL, "Flicker noise parameter"),
IOP( "noic", B3SOI_MOD_NOIC, IF_REAL, "Flicker noise parameter"),
IOP( "em", B3SOI_MOD_EM, IF_REAL, "Flicker noise parameter"),
IOP( "ef", B3SOI_MOD_EF, IF_REAL, "Flicker noise frequency exponent"),
IOP( "af", B3SOI_MOD_AF, IF_REAL, "Flicker noise exponent"),
IOP( "kf", B3SOI_MOD_KF, IF_REAL, "Flicker noise coefficient"),
IOP( "noif", B3SOI_MOD_NOIF, IF_REAL, "Floating body excess noise ideality factor"),


/* v2.0 release */
IOP( "k1w1", B3SOI_MOD_K1W1, IF_REAL, "First Body effect width dependent parameter"),    
IOP( "k1w2", B3SOI_MOD_K1W2, IF_REAL, "Second Boby effect width dependent parameter"),
IOP( "ketas", B3SOI_MOD_KETAS, IF_REAL, "Surface potential adjustment for bulk charge effect"),
IOP( "dwbc", B3SOI_MOD_DWBC, IF_REAL, "Width offset for body contact isolation edge"),
IOP( "beta0", B3SOI_MOD_BETA0, IF_REAL, "First Vds dependent parameter of impact ionizition current"),
IOP( "beta1", B3SOI_MOD_BETA1, IF_REAL, "Second Vds dependent parameter of impact ionizition current"),
IOP( "beta2", B3SOI_MOD_BETA2, IF_REAL, "Third Vds dependent parameter of impact ionizition current"),
IOP( "vdsatii0", B3SOI_MOD_VDSATII0, IF_REAL, "Nominal drain saturation voltage at threshold for impact ionizition current"),
IOP( "tii", B3SOI_MOD_TII, IF_REAL, "Temperature dependent parameter for impact ionizition"),
IOP( "lii", B3SOI_MOD_LII, IF_REAL, "Channel length dependent parameter at threshold for impact ionizition current"),
IOP( "sii0", B3SOI_MOD_SII0, IF_REAL, "First Vgs dependent parameter for impact ionizition current"),
IOP( "sii1", B3SOI_MOD_SII1, IF_REAL, "Second Vgs dependent parameter for impact ionizition current"),
IOP( "sii2", B3SOI_MOD_SII2, IF_REAL, "Third Vgs dependent parameter for impact ionizition current"),
IOP( "siid", B3SOI_MOD_SIID, IF_REAL, "Vds dependent parameter of drain saturation voltage for impact ionizition current"),
IOP( "fbjtii", B3SOI_MOD_FBJTII, IF_REAL, "Fraction of bipolar current affecting the impact ionization"),
IOP( "esatii", B3SOI_MOD_ESATII, IF_REAL, "Saturation electric field for impact ionization"),
IOP( "ntun", B3SOI_MOD_NTUN, IF_REAL, "Reverse tunneling non-ideality factor"),
IOP( "ndio", B3SOI_MOD_NDIO, IF_REAL, "Diode non-ideality factor"),
IOP( "nrecf0", B3SOI_MOD_NRECF0, IF_REAL, "Recombination non-ideality factor at forward bias"),
IOP( "nrecr0", B3SOI_MOD_NRECR0, IF_REAL, "Recombination non-ideality factor at reversed bias"),
IOP( "isbjt", B3SOI_MOD_ISBJT, IF_REAL, "BJT injection saturation current"),
IOP( "isdif", B3SOI_MOD_ISDIF, IF_REAL, "Body to source/drain injection saturation current"),
IOP( "isrec", B3SOI_MOD_ISREC, IF_REAL, "Recombination in depletion saturation current"),
IOP( "istun", B3SOI_MOD_ISTUN, IF_REAL, "Reverse tunneling saturation current"),
IOP( "ln", B3SOI_MOD_LN, IF_REAL, "Electron/hole diffusion length"),
IOP( "vrec0", B3SOI_MOD_VREC0, IF_REAL, "Voltage dependent parameter for recombination current"),
IOP( "vtun0", B3SOI_MOD_VTUN0, IF_REAL, "Voltage dependent parameter for tunneling current"),
IOP( "nbjt", B3SOI_MOD_NBJT, IF_REAL, "Power coefficient of channel length dependency for bipolar current"),
IOP( "lbjt0", B3SOI_MOD_LBJT0, IF_REAL, "Refferenc channel length for bipolar cuurent"),
IOP( "ldif0", B3SOI_MOD_LDIF0, IF_REAL, "Channel-length dependency coefficient of diffusion cap"),
IOP( "vabjt", B3SOI_MOD_VABJT, IF_REAL, "Early voltage for bipolar current"),
IOP( "aely", B3SOI_MOD_AELY, IF_REAL, "Channel length dependency of early voltage for bipolar cuurent"),
IOP( "ahli", B3SOI_MOD_AHLI, IF_REAL, "High level injection parameter for bipolar current"),
IOP( "rbody", B3SOI_MOD_RBODY, IF_REAL, "Intrinsic body contact sheet resistance"),
IOP( "rbsh", B3SOI_MOD_RBSH, IF_REAL, "Extrinsic body contact sheet resistance"),
IOP( "cgeo", B3SOI_MOD_CGEO, IF_REAL, "Gate substrate overlap capacitance per unit channel length"),
IOP( "tt", B3SOI_MOD_TT, IF_REAL, "Diffusion capacitance transit time coefficient"),
IOP( "ndif", B3SOI_MOD_NDIF, IF_REAL, "Power coefficient of channel length dependency for diffusion capacitance"),
IOP( "vsdfb", B3SOI_MOD_VSDFB, IF_REAL, "Source/drain bottom diffusion capacitance flatband voltage"),
IOP( "vsdth", B3SOI_MOD_VSDTH, IF_REAL, "Source/drain bottom diffusion capacitance threshold voltage"),
IOP( "csdmin", B3SOI_MOD_CSDMIN, IF_REAL, "Source/drain bottom diffusion minimum capacitance"),
IOP( "asd", B3SOI_MOD_ASD, IF_REAL, "Source/drain bottom diffusion smoothing parameter"),
IOP( "csdesw", B3SOI_MOD_CSDESW, IF_REAL, "Source/drain sidewall fringing capacitance per unit length"),
IOP( "ntrecf", B3SOI_MOD_NTRECF, IF_REAL, "Temperature coefficient for Nrecf"),
IOP( "ntrecr", B3SOI_MOD_NTRECR, IF_REAL, "Temperature coefficient for Nrecr"),
IOP( "dlcb", B3SOI_MOD_DLCB, IF_REAL, "Length offset fitting parameter for body charge"),
IOP( "fbody", B3SOI_MOD_FBODY, IF_REAL, "Scaling factor for body charge"),
IOP( "tcjswg", B3SOI_MOD_TCJSWG, IF_REAL, "Temperature coefficient of Cjswg"),
IOP( "tpbswg", B3SOI_MOD_TPBSWG, IF_REAL, "Temperature coefficient of Pbswg"),
IOP( "acde", B3SOI_MOD_ACDE, IF_REAL, "Exponential coefficient for charge thickness in capMod=3 for accumulation and depletion regions"),
IOP( "moin", B3SOI_MOD_MOIN, IF_REAL, "Coefficient for the gate-bias dependent surface potential"),
IOP( "delvt", B3SOI_MOD_DELVT, IF_REAL, "Threshold voltage adjust for CV"),
IOP( "kb1", B3SOI_MOD_KB1, IF_REAL, "Coefficient of Vbs0 dependency on Ves"),
IOP( "dlbg", B3SOI_MOD_DLBG, IF_REAL, "Length offset fitting parameter for backgate charge"),

IP( "nmos", B3SOI_MOD_NMOS,  IF_FLAG, "Flag to indicate NMOS"),
IP( "pmos", B3SOI_MOD_PMOS,  IF_FLAG, "Flag to indicate PMOS"),
};

char *B3SOInames[] = {
   "Drain",
   "Gate",
   "Source",
   "Backgate",
   "",
   "Body",
   "Temp",
   "Charge",
};

int	B3SOInSize = NUMELEMS(B3SOInames);
int	B3SOIpTSize = NUMELEMS(B3SOIpTable);
int	B3SOImPTSize = NUMELEMS(B3SOImPTable);
int	B3SOIiSize = sizeof(B3SOIinstance);
int	B3SOImSize = sizeof(B3SOImodel);


