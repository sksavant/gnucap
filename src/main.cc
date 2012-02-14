/*$Id: main.cc,v 1.7 2010-09-22 13:19:50 felix Exp $ -*- C++ -*-
 * vim:ts=8:sw=2:et:
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
 * top level module
 * it all starts here
 */
//testing=script 2006.07.14
#include "e_cardlist.h"
#include "u_lang.h"
#include "ap.h"
#include "patchlev.h"
#include "c_comand.h"
#include "declare.h"	/* plclose */
#include "startup.h"	
// #define COMMENT_CHAR "*"
 extern char *optarg;
        extern int optind, opterr, optopt;
           #include <getopt.h>

/*--------------------------------------------------------------------------*/
struct JMP_BUF{
  sigjmp_buf p;
} env;
/*--------------------------------------------------------------------------*/
static void sign_on(void)
{
  if (OPT::quiet) return;
  IO::mstdout <<
    COMMENT_CHAR " Gnucap "  PATCHLEVEL  "\n"
    COMMENT_CHAR " Copyright 2009-2011 Felix Salfelder\n"
    COMMENT_CHAR " Copyright 1982-2009, Albert Davis\n"
    COMMENT_CHAR " Gnucap comes with ABSOLUTELY NO WARRANTY\n"
    COMMENT_CHAR " This is free software, and you are welcome\n"
    COMMENT_CHAR " to redistribute it under the terms of \n"
    COMMENT_CHAR " the GNU General Public License, version 3 or later.\n"
    COMMENT_CHAR " See the file \"COPYING\" for details.\n";
}
/*--------------------------------------------------------------------------*/
/* sig_abrt: trap asserts
 */
extern "C" {
  static void sig_abrt(SIGNALARGS)
  {itested();
    signal(SIGINT,sig_abrt);
    static int count = 10;
    if (--count > 0) {itested();
      error(bDANGER, "\n");
    }else{untested();
      exit(1);
    }
  }
}
/*--------------------------------------------------------------------------*/
/* sig_int: what to do on receipt of interrupt signal (SIGINT)
 * cancel batch files, then back to command mode.
 * (actually, control-c trap)
 */
extern "C" {
  static void sig_int(SIGNALARGS)
  {
    signal(SIGINT,sig_int);
    if (ENV::run_mode == rBATCH) {untested();
      exit(1);
    }else{
      IO::error << '\n';
      siglongjmp(env.p,1);
    }
  }
}
/*--------------------------------------------------------------------------*/
extern "C" {
  static void sig_fpe(SIGNALARGS)
  {
    untested();
    signal(SIGFPE,sig_fpe);
    error(bDANGER, "floating point error\n");
  }
}
/*--------------------------------------------------------------------------*/
static void setup_traps(void)
{
  signal(SIGFPE,sig_fpe);
  signal(SIGINT,sig_int);
  signal(SIGABRT,sig_abrt);
}
/*--------------------------------------------------------------------------*/
/* finish: clean up after a command
 * deallocates space, closes plot windows, resets i/o redirection, etc.
 * This is done separately for exception handling.
 * If a command aborts, clean-up is still done, leaving a consistent state.
 * //BUG// It is a function to call as a remnant of old C code.
 * Should be in a destructor, so it doesn't need to be explicitly called.
 */
static void finish(void)
{
  plclose();
  IO::mstdout.outreset();
}
/*--------------------------------------------------------------------------*/

static void do_getopt(int argc,  char * const * argv)
{
  int flags, opt;
  int nsecs, tfnd;

  nsecs = 0;
  tfnd = 0;
  flags = 0;
  while ((opt = getopt(argc, argv, "bqt:")) != -1) {
    switch (opt) {
      case 'q':
        OPT::quiet = true;
        break;
      case 't':
        nsecs = atoi(optarg);
        tfnd = 1;
        break;
      case 'b':
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-q] name\n", argv[0]); }
  }

//  printf("flags=%d; tfnd=%d; optind=%d, argc=%i\n", flags, tfnd, optind, argc);

  if (optind > argc) {
    fprintf(stderr, "Expected argument after options\n");
    exit(EXIT_FAILURE);
  }

}
/*--------------------------------------------------------------------------*/
static void process_cmd_line(int argc, char *const  *argv)
{
  for (int ii = 1;  ii < argc;  /*inside*/) {
    try {
      if (   !strcasecmp(argv[ii], "-v")  ){
        // FIXME. use git hash for development versions.
        cout << "gnucap " << PATCHLEVEL << endl;
        exit(1);
      } else if (   !strcasecmp(argv[ii], "-E")  
          || !strcasecmp(argv[ii], "-e") ) {
        ++ii;

        if (ii < argc) {
          string v = "";
          if (   !strcasecmp(argv[ii], "-E") )  
            v="verbose ";
            // dashier startet den OPT::language modus
	    CMD::command(string("expect ") + v + argv[ii++], &CARD_LIST::card_list);
        } else {
          incomplete();
        }
      }else if (strcasecmp(argv[ii], "-q") == 0) {
        ++ii;
          // doesnt do anything.
      }else if (strcasecmp(argv[ii], "-i") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CMD::command(std::string("include ") + argv[ii++], &CARD_LIST::card_list);
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-c") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CS cmd(CS::_STRING, argv[ii++]); // command line
	  CMD::cmdproc(cmd, &CARD_LIST::card_list); 
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-b") == 0) {
	try {
	  ++ii;
	  if (ii < argc) {
            // dashier startet den OPT::language modus
            trace0("-b...");
	    CMD::command(std::string("< ") + argv[ii++], &CARD_LIST::card_list);
	  }else{untested();
	    CMD::command(std::string("< /dev/stdin"), &CARD_LIST::card_list);
	  }
	}catch (Exception& e) {
	  error(bDANGER, e.message() + '\n');
          throw(Exception("error"));
	  finish();
	}
	if (ii >= argc) {
	  CMD::command("end", &CARD_LIST::card_list);
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-a") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CMD::command(std::string("attach ") + argv[ii++], &CARD_LIST::card_list);
	}else{untested();
	}
      }else{itested();
	CMD::command(std::string("include ") + argv[ii++], &CARD_LIST::card_list);
      }
    }catch (Exception& e) {itested();
      error(bDANGER, e.message() + '\n');
      throw(Exception("error"));
      finish();
    }
  }
}
/*--------------------------------------------------------------------------*/
int main(int argc, char * const * argv)
{
  ENV::error = 0;

  do_getopt(argc,argv);
  // sigsetjmp unneeded here (isnt it?)
  //
  // FIXME:  parse -v and -q _now_
  //
  CMD::command("options lang=acs", &CARD_LIST::card_list);
  assert(OPT::language);
  read_startup_files();
  sign_on();

  {
    SET_RUN_MODE xx(rBATCH);
    trace0("batch mode");
    if (!sigsetjmp(env.p, true)) {
      try {
        trace0("... \n");
	setup_traps();
        trace0("done traps");
	process_cmd_line(argc,argv);
        trace0("done cmdline  mode");
      }catch (Exception& e) {untested();
        ENV::error++;
	error(bDANGER, e.message() + '\n');
	finish();		/* error clean up (from longjmp()) */
	CMD::command("quit", &CARD_LIST::card_list);
	unreachable();
	exit(1);
      }
    }else{
      trace0("finish batch");
      finish();		/* error clean up (from longjmp()) */
      CMD::command("quit", &CARD_LIST::card_list);
      exit(0);
    }
  }
  {itested();
    SET_RUN_MODE xx(rINTERACTIVE);
    trace0("interactive mode");
    CS cmd(CS::_STDIN);
    for (;;) {itested();
      if (!sigsetjmp(env.p, true)) {itested();
	try {
	  if (OPT::language) {
	    OPT::language->parse_top_item(cmd, &CARD_LIST::card_list);
	  }else{
	    CMD::cmdproc(cmd.get_line(I_PROMPT), &CARD_LIST::card_list);
	  }
	}catch (Exception_End_Of_Input& e) {itested();
	  error(bDANGER, e.message() + '\n');
	  finish();
	  CMD::command("quit", &CARD_LIST::card_list);
	  exit(0);
	}catch (Exception& e) {itested();
	  error(bDANGER, e.message() + '\n');
          ENV::error++;
	  finish();
	}
      }else{itested();
	finish();		/* error clean up (from longjmp()) */
      }
    }
  }
  unreachable();
  return 1;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:et:
