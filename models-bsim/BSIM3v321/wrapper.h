extern "C" {
  #include "bsim3def.h"
  #define DEV_bsim3
  #include "bsim3itf.h"
}

#define info	 BSIM3info
#define INSTANCE BSIM3instance
#define MODEL	 BSIM3model

#define SPICE_LETTER "M"
#define DEVICE_TYPE "bsim321|bsim3"
#define MIN_NET_NODES 4
#define MAX_NET_NODES 4
#define INTERNAL_NODES 3
#define MODEL_TYPE "nmos8|pmos8|nmos49|pmos49|nmos53|pmos53|nmos321|pmos321"

static std::string port_names[] = {"d", "g", "s", "b"};
static std::string state_names[] = {"vbd",  "vbs",   "vgs",   "vds",    "qb",  "cqb",
				    "qg",   "cqg",   "qd",    "cqd",    "qbs", "qbd",
				    "qcheq","cqcheq","qcdump","cqcdump","tau", "qdef"};

#define IS_VALID {itested();							\
    assert(d);									\
    const COMMON_SUBCKT* c = dynamic_cast<const COMMON_SUBCKT*>(d->common());	\
    assert(c);									\
    double l_in = c->_params["l"].e_val(OPT::defl, d->scope());			\
    double w_in = c->_params["w"].e_val(OPT::defw, d->scope());			\
    double l_min = _params["lmin"].e_val(0, scope());				\
    double w_min = _params["wmin"].e_val(0, scope());				\
    double l_max = _params["lmax"].e_val(1, scope());				\
    double w_max = _params["wmax"].e_val(1, scope());				\
    trace2(d->long_label().c_str(), l_in, w_in);				\
    trace4(long_label().c_str(), l_min, l_max, w_min, w_max);			\
    return l_in >= l_min && l_in <= l_max && w_in >= w_min && w_in <= w_max;	\
  }
