extern "C" {
  //#include "hicum0defs.h"
}

#define info	 hicum0info
#define INSTANCE hicum0instance
#define MODEL	 hicum0model

#define SPICE_LETTER "\0"
#define DEVICE_TYPE "ngspice_hicum0|Qp"
#define MIN_NET_NODES 3
#define MAX_NET_NODES 5
#define INTERNAL_NODES 3
#define MODEL_TYPE "npnhicum0|pnphicum0|npn7|pnp7"
//#define UNCONNECTED_NODES uGROUND

static std::string port_names[] = {"c", "b", "e", "s"};
static std::string state_names[] = {};

/* There are 4 internal nodes listed, but the last one
 * is mapped to another internally.
 * So, only 3 of them matter externally in the sense of
 * needing a node allocated.
 */
