extern "C" {
  #include "bsim4def.h"
  #define DEV_bsim4
  #include "bsim4itf.h"
}

#define info	 B4info
#define INSTANCE BSIM4instance
#define MODEL	 BSIM4model

#define SPICE_LETTER "M"
#define DEVICE_TYPE "bsim462|bsim4"
#define MIN_NET_NODES 4
#define MAX_NET_NODES 4
#define INTERNAL_NODES 8
#define MODEL_TYPE "nmos14|pmos14|nmos54|pmos54|nmos60|pmos60"

static std::string port_names[] = {"d", "g", "s", "b"};
static std::string state_names[] = {"vbd",   "vbs",   "vgs",    "vds",  "vdbs", "vdbd",
				    "vsbs",  "vges",  "vgms",   "vses", "vdes", "qb",
				    "cqb",   "qg",    "cqg",    "qd",   "cqd",  "qgmid",
				    "cqgmid","qbs",   "cqbs",   "qbd",  "cqbd", "qcheq",
				    "cqcheq","qcdump","cqcdump","qdef", "qs"};
