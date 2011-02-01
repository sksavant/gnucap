/*
 * Copyright (c) 2008 True Circuits Inc.
 *
 * Author: Kevin Cameron
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "config.h"
# include  "parse_misc.h"
# include  "compile.h"
# include  "schedule.h"
# include  "vpi_priv.h"
# include  "vpi_user.h"
# include  "acc_user.h"
# include  "statistics.h"
# include  "extpwl.h"
# include  <stdio.h>
# include  <stdlib.h>
# include  <string.h>
# include  <unistd.h>


static SpcIvlCB *bnets[BL_LAST]; // unbound & bound lists

extern "C" SpcIvlCB **spicenets()
{
  return bnets;
}

void PrintInst(FILE *fp,struct __vpiScope *scope)
{
  if (scope->scope) {
    PrintInst(fp,scope->scope);
    fputs(".",fp);
  } 
  fputs(scope->name,fp);
}

int SpcDebug;
SpcIvlCB::pwr_fn SpcIvlCB::pBSgetPWR;

double *EvalVPI(SpcIvlCB *cb,double sim_time,eCBmode mode,
                int (*fn)(void *,...),void *handle,void *xtra)
{
  double data[4],
         thresh;
  switch (mode) {
    case CB_LOAD: {
      cb->checkPWL();
    } break;
    case CB_ACCEPT:
    case CB_TRUNC:
      if (fn) { // null current voltage probe
        struct t_vpi_value argval;
        argval.format = vpiIntVal;
        vpi_get_value(&cb->sig->base,&argval);

        int l,i   = (*fn)(handle,xtra,data), 
            leave = cb->set;

        switch (cb->mode) {
        case 1: 
        case 3: try {
                  cb->lo = cb->Pwr(0,"BSgetPWR")->last_value;
                  cb->hi = cb->Pwr(1,"BSgetPWR")->last_value;
                } catch (...) {
                  fprintf(stderr,"Error: power supplies not bound for conversion of %s\n",
                          cb->spec);
                  throw cb;
                }
        }

        thresh = cb->lo;
        if (argval.value.integer) { // falling
          thresh += cb->thrshld[l=1] * (cb->hi - thresh);
          if (data[0] < thresh) {l = 0; leave = 0;}
        } else {                    // rising
          thresh += cb->thrshld[l=0] * (cb->hi - thresh);
          if (data[0] > thresh) {l = 1; leave = 0;}
        }
        
        if (!leave) {
          
          double cross = sim_time,
                 split;
          
          if (cb->last_time >= 0) {
            double span = cb->last_value - data[0];
            if (span > 0.0) {
              split = (cb->last_value - thresh)/span;
              if (split < 0.0) {
                split = 0.0;
                if (CB_ACCEPT == mode) {
                  fprintf(stderr,
                          "Warning: overran threshold on %s @ %g\n",
                                                    cb->spec,sim_time);
                }
              }
              cross = cb->last_time + 
                              split * (sim_time - cb->last_time);
            } else {
              cross = cb->last_time;
            }
          }

          switch (mode) {
          case CB_TRUNC:
            if (!isnan(cb->prec)) {
              double tol = cb->prec/4;
              cross += tol;
              if (cross < sim_time) {
                assert(cross > cb->last_time);
                cb->coeffs[2] = cross;
                cb->coeffs[3] = cb->fillEoT(4,cb->coeffs[0]);
              }
            }
            break;
          case CB_ACCEPT:
            schedule_assign_pv_sync(cb->sig->node,l,cross);
            ActivateCB(cross,cb);
            (*cb->set_active)(cb->dll_ref,cb,sim_time);
            cb->set       = 1;
            cb->fillEoT(2,cb->coeffs[0]);
          }
        } 

        if (CB_ACCEPT == mode) { 
          cb->last_time  = sim_time;
          cb->last_value = data[0];
        }

      } else { // voltage

        if (CB_ACCEPT == mode) { 
          cb->last_time = sim_time;
          int    t = 0;
          while (cb->coeffs[t+2] < sim_time) t += 2;
          double split = (sim_time - cb->coeffs[t])
                                 /(cb->coeffs[t+2] - cb->coeffs[t]);
          cb->last_value = cb->coeffs[t+1] 
                             + split * (cb->coeffs[t+3] - cb->coeffs[t+1]);
          if (cb->last_error >= 0.0) {
            double dt = sim_time-cb->last_error;
            if (dt > cb->prec && !cb->reported) {
              cb->reported = 1;
              fprintf(stderr,
                      "Warning: possible PWL/logic mismatch on %s @ %g\n",
                                                    cb->spec,sim_time-dt);
              // schedule_assign_pv_sync(cb->sig->node,cb->go2,SimTimeD);
              // ActivateCB(SimTimeD,cb);
            }
          }
        }
      }
  }

  return cb->coeffs;
}


int VvpLoaded()
{
    static int loaded = 0;
    int l = loaded;
    loaded = 1;
    return l;
}

extern "C" void *bindnet(char *spec, char T, int *slots, void *dll_ref,
                         void (*set_active)(void *,void *,double))
{
    char *sig = spec,
          des[2000], *dp = des;
    int   sts = 0;

    while (*sig && *sig != ':') { *dp++ = *sig++; }

    if (*sig++) {
      *dp = '\0';
    } else {
      sig = spec;
    }

#ifdef VVP_SHARED
    if (!VvpLoaded()) {
      char *argv[] = {"libvvp.so","-M.","-mbindsigs",*des ? des : 0, 0};
      sts = so_main(*des ? 4 : 3, argv);
    }
#endif

    SpcIvlCB *cb   = new SpcIvlCB(1e-15);
    cb->eval       = (SpcIvlCB::SpiceCB)EvalVPI;
    cb->spec       = strdup(sig);
    cb->dll_ref    = dll_ref;
    cb->set_active = set_active;

    *slots = cb->Slots();

    cb->next          = bnets[BL_UNBOUND];
    bnets[BL_UNBOUND] = cb;

    return cb;
}

extern "C" void endsim()
{

} 

template<int S>
void SpiceCallback<S>::dumpPWL(FILE *fp,double now) {
  for (int i = 0; i < S ; i += 2) {
    fprintf(fp,"%8g\t%8g %c\n",coeffs[i],coeffs[i+1],
                               now < coeffs[i] ? '+' : '-');
    if (EndOfTimeD == coeffs[i]) break;
  }
}

template struct SpiceCallback<IVL_PWL_SLOTS>;
