

#include "io_trace.h"
#include "c_comand.h"

void read_startup_files(void)
{
  trace0("read_startup_files");
  std::string name = findfile(SYSTEMSTARTFILE, SYSTEMSTARTPATH, R_OK);
  if (name != "") {untested();
    try{
      CMD::command("get " + name, &CARD_LIST::card_list);
    } catch(Exception e){
      error(bDANGER, "%s\n",e.message().c_str());
    }
  }else{
  }
  name = findfile(USERSTARTFILE, PWD, R_OK);
  if (name != "") {untested();
    try{
      CMD::command("get " + name, &CARD_LIST::card_list);
    }catch(Exception e){
      error(bDANGER, "loading %s failed: %s\n", name.c_str(), e.message().c_str());
    }
  }else{
    name = findfile(USERSTARTFILE, USERSTARTPATH, R_OK);
    if (name != "") {untested();
      try{
        CMD::command("get " + name, &CARD_LIST::card_list);
      } catch(Exception e){
        error(bDANGER, "%s\n",e.message().c_str());
      }
    }else{
    }
  }
  trace0("clear");
  try{
    CMD::command("clear", &CARD_LIST::card_list);
  } catch(Exception e){
    error(bDANGER, "%s\n",e.message().c_str());
  }
  if (!OPT::language) {
      try{
        CMD::command(std::string("options lang=") + DEFAULT_LANGUAGE, &CARD_LIST::card_list);
      } catch(Exception e){
        error(bDANGER, "%s\n",e.message().c_str());
      }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
