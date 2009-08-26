extern "C" {
  #include "b4soidef.h"
  #define DEV_b4soi
  #include "b4soiitf.h"
}

#define info	 B4SOIinfo
#define INSTANCE B4SOIinstance
#define MODEL	 B4SOImodel

#define SPICE_LETTER "M"
#define DEVICE_TYPE "bsimsoi4|bsim4soi4p0"
//#define MIN_NET_NODES 5
#define MIN_NET_NODES 6
#define MAX_NET_NODES 7
#define INTERNAL_NODES 25
#define MODEL_TYPE "nmos10|pmos10"
#define UNCONNECTED_NODES FLOAT

static std::string port_names[] = {"d", "g", "s", "bg", "p", "body", "temp"};
static std::string state_names[] = {};
