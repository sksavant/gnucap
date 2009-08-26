extern "C" {
  #include "bsim3def.h"
  #define DEV_bsim3
  #include "bsim3itf.h"
}

#define info	 B3info
#define INSTANCE BSIM3instance
#define MODEL	 BSIM3model

#define SPICE_LETTER "M"
#define DEVICE_TYPE "bsim323|bsim3"
#define MIN_NET_NODES 4
#define MAX_NET_NODES 4
#define INTERNAL_NODES 3
#define MODEL_TYPE "nmos8|pmos8|nmos49|pmos49|nmos53|pmos53"

static std::string port_names[] = {"d", "g", "s", "b"};
static std::string state_names[] = {"vbd",  "vbs",   "vgs",   "vds",    "qb",  "cqb",
				    "qg",   "cqg",   "qd",    "cqd",    "qbs", "qbd",
				    "qcheq","cqcheq","qcdump","cqcdump","qdef"};
