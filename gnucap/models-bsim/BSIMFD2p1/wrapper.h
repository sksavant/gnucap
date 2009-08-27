extern "C" {
  #include "b3soidef.h"
  #define DEV_b3soi
  #include "b3soiitf.h"
}

#define info	 B3SOIinfo
#define INSTANCE B3SOIinstance
#define MODEL	 B3SOImodel

#define SPICE_LETTER "M"
#define DEVICE_TYPE "bsimsoi3|bsimfd2p1"
#define MIN_NET_NODES 4
#define MAX_NET_NODES 7
#define INTERNAL_NODES 46
#define MODEL_TYPE "nmos9|pmos9|nmos56|pmos56"
#define UNCONNECTED_NODES uFLOAT

static std::string port_names[] = {"d", "g", "s", "bg", "body", "temp", "p"};
static std::string state_names[] = {};
