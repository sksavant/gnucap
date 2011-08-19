/*$Id$
 * vim:sw=2:et:ts=8:
 * Copyright (C) 2009 Kevin Cameron
 * Author: Kevin Cameron
 *
 * This file is a part of plugin "Gnucap-Icarus" to "Gnucap", 
 * the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * Dynamic binding of PWL signal sources to external simulators 
 */
#include "u_lang.h"
#include "globals.h"
#include "e_card.h"
#include "e_cardlist.h"
#include "s_tr.h"
#include "extlib.h"
#include "bm.h"
//#include "vvp.h"
#include "d_vvp.h"

# include  "vvp/compile.h"
# include  "vvp/schedule.h"
# include  "vvp/vpi_priv.h"
# include  "vvp/statistics.h"
# include  "vvp/vvp_cleanup.h"
# include  <cstdio>
# include  <csignal>
# include  <cstdlib>
# include  <cstring>
# include  <unistd.h>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

inline double digital_time(void)
{
  return double(schedule_simtime() * (long double) pow(10.0,vpip_get_time_precision())) ;
}

// static std::list<ExtLib*> Libs;

void ExtLib::set_active(void *,double time)
{
  if (active) {
    if (time >= next_time) {
      return;
    }
  } else {
    active    = 1;
  }
  next_time = time;
  if (time > now) {
    CKT_BASE::_sim->new_event(time);
  }
}
/*--------------------------------------------------------------------------*/
void ExtLib::SetActive(void *dl,void *handle,double time)
  //                      extLib ^^
{
  SpcDllData *spd = (SpcDllData *)dl;
  ExtLib *lib = ExtLib::Sdd2El(spd);
  lib->set_active(handle,time);
}
/*--------------------------------------------------------------------------*/
int ExtLib::init(const char *args) {

  trace0("ExtLib::init");

#ifdef DLINKED
  const char* e;
  vhbn  = (typeof(vhbn))dlsym(handle,"vpi_handle_by_name");
  assert(vhbn);
  bindnet  = (typeof(bindnet))dlsym(handle,"bindnet");
  assert(bindnet);
  startsim = (typeof(startsim))dlsym(handle,"startsim");
  assert(startsim);
  endsim   = (typeof(endsim))dlsym(handle,"endsim");
  assert(endsim);
  contsim  = (typeof(contsim))dlsym(handle,"contsim");
  assert(contsim);
  so_main =  (typeof(so_main))dlsym(handle,"so_main");
  if(!so_main){
    error(bDANGER, "so: %s\n", dlerror());
  }
  assert(so_main);
  activate = (typeof(activate))SetActive;
  assert(activate);
#else
  startsim = &vvp::startsim;
  //bindnet = &vvp::bindnet;
  //assert(bindnet);
  assert(startsim);
  endsim   = &vvp::endsim;
  assert(endsim);
  contsim  = &vvp::contsim;
  assert(contsim);
  so_main =  &vvp::init;
  if(!so_main){
    error(bDANGER, "so: %s\n", dlerror());
  }
  assert(so_main);
  //activate = &vvp::SetActive;
  //assert(activate);
#endif

  return( (*so_main)(args) );
}



void ExtSig::SetActive(void *,void *sig,double time) 
{
  // SpcDllData *spd = (SpcDllData *)dl;

  ((ExtSig*)sig)->set_active(time);
  trace1("ExtSig::SetActive", (intptr_t)sig );
}

void ExtSig::set_active(double ) {
}

void PrintInst(FILE *fp,struct __vpiScope *scope);
extern int SpcDebug;

void PrintInst(FILE *fp,struct __vpiScope *scope)
{
  if (scope->scope) {
    PrintInst(fp,scope->scope);
    fputs(".",fp);
  } 
  fputs(scope->name,fp);
}
/*-----------------------------------------------------*/

bool have_ivl_version = true;

double vvp::SimTimeD     = 0.0;
double vvp::SimTimeA     = 0.0;
// double SimTimeA     = 0.0;
double vvp::SimTimeDlast = 0.0;
double vvp::SimDelayD = -1.0;

sim_mode vvp::SimState  = SIM_ALL;


// Provide dummies
inline static void my_getrusage(struct rusage *) { }
inline static void print_rusage(struct rusage *, struct rusage *){};
/*--------------------------------------------------------------------*/

//from vvp_vpi.cc
void vvp::vvp_vpi_init()
{
  trace0("not doing anything");
}
/*------------------------------------------------*/
// replacement for main...
int vvp::init(const char* design_path)
{
  static int vvp_return_value;
  ofstream debug_file;
  const char *logfile_name = 0x0;
  FILE *logfile = 0x0;
  extern void vpi_set_vlog_info(int, char**);
  extern bool stop_is_finish;

  stop_is_finish = false;

  vpip_module_path[vpip_module_path_cnt++] = ".";
  stop_is_finish = true;

  if (char*path = getenv("VVP_DEBUG")) {
    debug_file.open(path, ios::out);
  }

  /* This is needed to get the MCD I/O routines ready for
     anything. It is done early because it is plausible that the
     compile might affect it, and it is cheap to do. */

  if (logfile_name) {
    if (!strcmp(logfile_name, "-"))
      logfile = stderr;
    else {
      logfile = fopen(logfile_name, "w");
      if (!logfile) {
        perror(logfile_name);
        exit(1);
      }
      //setvbuf(logfile, log_buffer, _IOLBF, sizeof(log_buffer));
    }
  }

  //   vpip_mcd_init(logfile); // not shared yet.
  vpip_mcd_init(logfile);
  //
  // 	 
  trace0( "Init VVP ...\n");
  vvp_vpi_init();

  char *argv[]={0};
  vpi_set_vlog_info(0, argv );

  trace0( "Compiling VVP "+(string)design_path);
  COMPILE*c=new COMPILE();
  c->init();

  // for m in modules...
  // vpip_load_module("bindsigs2");
  int ret_cd = c->design(design_path);

  trace1( " ...\n", ret_cd);

  destroy_lexor();
  print_vpi_call_errors();
  if (ret_cd) return ret_cd;

  if (!have_ivl_version) {
    if (c->verbose_flag) vpi_mcd_printf(1, "... ");
    vpi_mcd_printf(1, "Warning: vvp input file may not be correct "
        "version!\n");
  }

  vpi_mcd_printf(1, "Compile cleanup...\n");

  c->cleanup();

  if (c->errors > 0) {
    vpi_mcd_printf(1, "%s: Program not runnable, %u errors.\n",
        design_path, c->errors);
    load_module_delete(); 
    return c->errors;
  }

#ifdef DO_TRACE
  fprintf(stdout, "vpi_mcd_printf ...\n");
  vpi_mcd_printf(1, " ... %8lu functors (net_fun pool=%zu bytes)\n",
      (long unsigned)count_functors, vvp_net_fun_t::heap_total());
  vpi_mcd_printf(1, "           %8lu logic\n",  count_functors_logic);
  vpi_mcd_printf(1, "           %8lu bufif\n",  count_functors_bufif);
  vpi_mcd_printf(1, "           %8lu resolv\n",count_functors_resolv);
  vpi_mcd_printf(1, "           %8lu signals\n", count_functors_sig);
  vpi_mcd_printf(1, " ... %8lu filters (net_fil pool=%zu bytes)\n",
      count_filters, vvp_net_fil_t::heap_total());
  vpi_mcd_printf(1, " ... %8lu opcodes (%zu bytes)\n",
      count_opcodes, size_opcodes);
  vpi_mcd_printf(1, " ... %8lu nets\n",     count_vpi_nets);
  vpi_mcd_printf(1, " ... %8lu vvp_nets (%zu bytes)\n",
      count_vvp_nets, size_vvp_nets);
  vpi_mcd_printf(1, " ... %8lu arrays (%lu words)\n",
      count_net_arrays, count_net_array_words);
  vpi_mcd_printf(1, " ... %8lu memories\n",
      count_var_arrays+count_real_arrays);
  vpi_mcd_printf(1, "           %8lu logic (%lu words)\n",
      count_var_arrays, count_var_array_words);
  vpi_mcd_printf(1, "           %8lu real (%lu words)\n",
      count_real_arrays, count_real_array_words);
  vpi_mcd_printf(1, " ... %8lu scopes\n",   count_vpi_scopes);


  vpi_mcd_printf(1, "Not Running ...\n");
  // schedule_simulate();

#endif

  return vvp_return_value;
}
/*--------------------------------------------------------------------*/
void vvp::signals_capture(void)
{
  trace0("vvp::signals_capture not implemented");
}
void vvp::signals_revert(void)
{
  signal(SIGINT, SIG_DFL);
}
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
double vvp::getdtime(struct event_time_s *et)
{
  return (double) et->delay * pow(10.0,vpip_get_time_precision());
}
/*--------------------------------------------------------------------
enum sim_mode {SIM_ALL,
               SIM_INIT,SIM_CONT0,SIM_CONT1,
               SIM_PREM,SIM_DONE};
--------------------------------------------------------------------*/
sim_mode vvp::schedule_simulate_m(sim_mode mode)
{
  trace3("schedule_simulate_m", mode, SimTimeA, SimTimeD);
  struct event_s      *cur  = 0;
  struct event_time_s *ctim = 0;
  double               d_dly;

  switch (mode) {
    case SIM_CONT0: if ((ctim = schedule_list())){
                      trace1("schedule list", ctim->delay);
                      goto sim_cont0;
                    }
                      goto done;
    case SIM_CONT1: goto sim_cont1;
    default:
                    break;
  }

  assert(schedule_simtime()==0);

  trace0("starting vvp");

  vpiEndOfCompile();
  exec_init_list();
  vpiStartOfSim();

  signals_capture();
  trace1("signals_capture Done", schedule_runnable());

  trace0("schedule_list?");
  if (schedule_runnable())
    while (schedule_list()) {
      trace2("schedule list ... ", mode, schedule_stopped() );

      if (schedule_stopped()) {
        schedule_start();
        incomplete();
//        stop_handler(0);
        // You can finish from the debugger without a time change.
        if (!schedule_runnable()) break;
        goto cycle_done;
      }

      /* ctim is the current time step. */
      ctim = schedule_list();

      /* If the time is advancing, then first run the
         postponed sync events. Run them all. */
      if (ctim->delay > 0) {
        switch (mode) {
          case SIM_CONT0:
          case SIM_CONT1:
          case SIM_INIT:

            trace0("SIM_SOME");
            d_dly = getdtime(ctim);
            if (d_dly > 0) {
              trace0("skip ExtPWL");
              //doExtPwl(sched_list->nbassign,ctim);
              SimDelayD = d_dly; return SIM_CONT0; 
sim_cont0:
              double dly = getdtime(ctim),
                     te  = SimTimeDlast + dly;

              trace4("cont0", SimTimeDlast, te, dly, SimTimeA);

              if (te > SimTimeA) {
                SimDelayD = te - SimTimeA;
                trace1("schedule_sim  SimTimeD PREM", SimDelayD);
                return SIM_PREM; 
              }
              trace1("schedule_sim SimTimeD increasing...",dly);
              SimTimeD  = SimTimeDlast + dly;
            }
            break;
          default:
            fprintf(stderr,"deffault?\n");
        }

        if (!schedule_runnable()) break;
        schedule_sim(schedule_simtime() + ctim->delay);
        ctim->delay = 0;

        vpiNextSimTime();
      }


      /* If there are no more active events, advance the event
         queues. If there are not events at all, then release
         the event_time object. */
      if (ctim->active == 0) {
        ctim->active = ctim->nbassign;
        ctim->nbassign = 0;

        if (ctim->active == 0) {
          ctim->active = ctim->rwsync;
          ctim->rwsync = 0;

          /* If out of rw events, then run the rosync
             events and delete this time step. This also
             deletes threads as needed. */
          if (ctim->active == 0) {
            trace0("rosync");
            run_rosync(ctim);
            schedule_enlist( ctim->next);
            switch (mode) {
              case SIM_CONT0:
              case SIM_CONT1:
              case SIM_INIT: 

                d_dly = getdtime(ctim);
                if (d_dly > 0) {
                  trace0("noextPWL again");
                  // doExtPwl(sched_list->nbassign,ctim);
                  SimDelayD = d_dly;
                  delete ctim;
                  return SIM_CONT1;
sim_cont1:
                  untested();
                  // SimTimeD += ???;
                  goto cycle_done;
                }
              default:
                trace1("default ", mode);
            }
            delete ctim;
            goto cycle_done;
          }
        }
      }

      /* Pull the first item off the list. If this is the last
         cell in the list, then clear the list. Execute that
         event type, and delete it. */
      cur = ctim->active->next;
      if (cur->next == cur) {
        ctim->active = 0;
      } else {
        ctim->active->next = cur->next;
      }
      cur->run_run();

      delete (cur);

cycle_done:;
           trace0("cycle done");
    }

  if (SIM_ALL == mode) {

    signals_revert();

    // Execute post-simulation callbacks
    vpiPostsim();
  }

done:
  trace1("schedule_simulate_m done.", SimTimeD);
  return SIM_DONE;
}
/*--------------------------------------------------------------------*/
double vvp::startsim(const char *)
{
  SimDelayD  = -1;
  SimState = schedule_simulate_m(SIM_INIT);
  SimTimeDlast = SimTimeD;
  return SimDelayD;
} 
/*--------------------------------------------------------------------*/
double vvp::contsim(const char *,double time)
{
  trace5("contsim", time, SimTimeA, SimTimeD, SimTimeDlast, digital_time());
  SimTimeA  = time;
  assert(CKT_BASE::_sim->_time0 == time );
  SimDelayD = -1;

  SimState = SIM_CONT0;
  while (SimTimeDlast < time) {
    trace1("contsim loop", time);
    SimState = schedule_simulate_m(SimState);
    SimTimeDlast = SimTimeD;
    trace1("contsim loop", SimTimeD);
    if (SIM_PREM <= SimState) break;
  }

  return SimDelayD;
} 
/*------------------------------------------------------*/
void vvp::endsim()
{
  vpi_handle_by_name("foo",NULL);

} 
/*------------------------------------------------------*/

void *vvp::bindnet(const char *,char ,int *,
    void *,void (*)(void *,void *,double))
{
  return NULL;
}



