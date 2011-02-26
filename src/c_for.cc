/*$Id: c_sweep.cc,v 1.2 2010-09-22 13:19:50 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
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
 * Step a parameter and repeat a group of commands
 */
//testing=none 2006.07.17
#include "c_comand.h"
#include "globals.h"
#include "e_cardlist.h"
#include "u_parameter.h"
extern int swp_count[], swp_steps[];
extern int swp_type[];
extern int swp_nest;
/*--------------------------------------------------------------------------*/
namespace {
  static std::string tempfile = STEPFILE;
  char my_tempfile[128];
  std::string para_name;
  double start;
  double last;
/*--------------------------------------------------------------------------*/
/* sweep_fix: fix the value for sweep command.
 * (find value by interpolation)
 * if not sweeping, return "start" (the first arg).
 */

  // copy from c_modify
double sweep_fix(CS&, double start, double last)
{
  double value = start;
  if (swp_steps[swp_nest] != 0 )  {
    untested();
    double offset = static_cast<double>(swp_count[swp_nest]) 
      / static_cast<double>(swp_steps[swp_nest]);
    if (swp_type[swp_nest]=='L') {
      untested();
      if (start == 0.) {
	untested();
	throw Exception("log sweep can't pass zero");
	value = 0;
      }else{
	untested();
	value = start * pow( (last/start), offset );
      }
    }else{
      untested();
      value = start + (last-start) * offset;
    }
  }
  return value;
}
/*--------------------------------------------------------------------------*/
static void setup(CS& cmd)
{
  for (;;) {
    if (cmd.is_digit()) {
      swp_steps[swp_nest] = cmd.ctoi() ;
      swp_steps[swp_nest] = (swp_steps[swp_nest]) 
	? swp_steps[swp_nest]-1
	: 0;
    }else if (cmd.umatch("li{near} ")) {
      swp_type[swp_nest] = 0;
    }else if (cmd.umatch("lo{g} ")) {
      swp_type[swp_nest] = 'L';
    }else{
      break;
    }
  }
  cmd >> para_name;

  start = cmd.ctof();
  last = cmd.ctof();


  trace2(("got para name " + para_name).c_str(), start, last );
}
/*--------------------------------------------------------------------------*/
static void buildfile(CS& cmd)
{
  static FILE *fptr;
  
  setup(cmd);
  if (fptr) {
    fclose(fptr);
  }else{
  }
  sprintf(my_tempfile, "%s", tempfile.c_str());
  int fd = mkstemp(my_tempfile);
  fptr = fdopen( fd, "w+");
  if (!fptr) {
    throw Exception_File_Open("can't open temporary file:" + std::string(my_tempfile));
  }else{
  }

  fprintf(fptr, "* %s\n", cmd.fullstring().c_str());
  
  for (;;) {
    char buffer[BUFLEN];
    std::string sbuffer;
    trace0("getting things");

    switch (ENV::run_mode) {
      case rSCRIPT:
        trace0("in script mode");

        sbuffer = std::string( cmd.get_line("") );

        break;
      default:
        trace0("not in script mode");
        getcmd(">>>", buffer, BUFLEN);
        sbuffer=buffer;
    }

    if (Umatch(sbuffer.c_str(),"go ")) {
      trace0(("got go " + std::string(my_tempfile)).c_str());
      break;
    }else{
    }
    fprintf(fptr, "%s\n", sbuffer.c_str());
  }
  fclose(fptr);
  trace0(("closed tmp " + std::string( my_tempfile )).c_str());
  fptr = NULL;
}
/*--------------------------------------------------------------------------
static void doit_old(CARD_LIST* scope)
{
  static FILE *fptr;
  double para_value;
  
  for (swp_count[swp_nest]=0; swp_count[swp_nest]<=swp_steps[swp_nest];
       ++swp_count[swp_nest]) {
    if (fptr) {
      fclose(fptr);
    }else{
    }
    fptr = fopen(my_tempfile, "r");
    if (!fptr) {
      throw Exception_File_Open("can't open " + tempfile);
    }else{
    }
    char buffer[BUFLEN];
    fgets(buffer,BUFLEN,fptr);
    {
      CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
      if (cmd.umatch("sw{eep} ") || cmd.umatch(".sw{eep} ") || (std::string(cmd).c_str())[0] == '*') {
	setup(cmd);
        trace0((" sweep doit cmd " + std::string(cmd)).c_str());
      }else{
	throw Exception("bad file format: " + std::string(my_tempfile) + " (" + std::string(cmd) + ")" );
      }
      unsigned ind = cmd.cursor();

      para_value = sweep_fix( cmd, start, last );

      strncpy(buffer, "* parameter                            ", ind);
      buffer[ind-1] = ' ';		// make sure there is a delimiter   

      scope->params()->set( para_name, para_value );

    }					// in case the words run together   
    for (;;) {				// may wipe out one letter of fault 
      {
        trace2((" sweep doit " + std::string(buffer)).c_str(), swp_count[swp_nest], para_value);
	CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
	CMD::cmdproc(cmd, scope);
      }
      if (!fgets(buffer,BUFLEN,fptr)) {
	break;
      }else{
      }
      {
	CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
	if (cmd.umatch("sw{eep} ")) {
	  cmd.warn(bDANGER, "command not allowed in sweep");
	  buffer[0] = '\'';
	}else{
	}
      }
      // IO::mstdout << swp_count[swp_nest]+1 << "> " << buffer;
    }
  }
  fclose(fptr);
  fptr = NULL;
  swp_count[swp_nest] = 0;
}
--------------------------------------------------------------------------*/
static void doit(CARD_LIST* scope)
{
  static FILE *fptr;
  double para_value;
  
  for (swp_count[swp_nest]=0; swp_count[swp_nest]<=swp_steps[swp_nest];
       ++swp_count[swp_nest]) {
    if (fptr) {
      fclose(fptr);
    }else{
    }
    fptr = fopen(my_tempfile, "r");
    if (!fptr) {
      throw Exception_File_Open("can't open " + tempfile);
    }else{
    }
    char buffer[BUFLEN];
    fgets(buffer,BUFLEN,fptr);

    CS cmd(CS::_STRING, buffer); //fgets from local file, obsolete
    if (cmd.umatch("sw{eep} ") || cmd.umatch(".sw{eep} ") || (std::string(cmd).c_str())[0] == '*') {
       // setup(cmd);
      trace0((" sweep doit cmd " + std::string(cmd)).c_str());
    }else{
      throw Exception("bad file format: " + std::string(my_tempfile) + " (" + std::string(cmd) + ")" );
    }

    para_value = sweep_fix( cmd, start, last );

    trace1(("setting " + para_name).c_str(), para_value);
    scope->params()->set( para_name, para_value );

    trace1(("doit excuting " + std::string( my_tempfile )).c_str(), (long int)ENV::run_mode);

    // FIXME? makes interactive mode impossble
    SET_RUN_MODE xx(rBATCH);
    CMD::command(std::string("< ") +std::string(my_tempfile) , scope);
  }
  fclose(fptr);
  fptr = NULL;
  swp_count[swp_nest] = 0;
}
/*--------------------------------------------------------------------------*/
class CMD_SWEEP : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    if (cmd.more()) {
      buildfile(cmd);
    }else{
    }
    doit(Scope);
//    command("unfault", Scope);
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "sweep|for", &p);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
