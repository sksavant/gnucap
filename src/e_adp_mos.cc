
//#include "d_mos.h"
#include "d_mos8.h"

// aging helpers.
// alpha version

/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS8::tt_probe_num(const std::string& x)const
{
	if( Umatch("hci ", x) ){
		if(hci_node)
			return hci_node->tt();
		return -1;
	} else if( Umatch("dvth_hci ", x) ) {
		return vthdelta_hci;
	}else{
		return ADP_BUILT_IN_MOS::tt_probe_num(x);
	}

}
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS8::tr_probe_num(const std::string& x)const
{
	double ret;
	if( Umatch("hci ", x) ){
		if(!hci_node) return -1;
		ret= hci_node->tr();
	} else {
		ret= ADP_BUILT_IN_MOS::tr_probe_num(x);
	}

	// maybe too small to output?
	//  std::cerr << "ADP_BUILT_IN_MOS8::tr_probe_num " << x << ": " << ret << "\n";

	return ret;

}
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tt_prepare() 
{
	trace1("ADP_BUILT_IN_MOS8::tt_prepare", long_label());
	const DEV_BUILT_IN_MOS* d = asserted_cast<const DEV_BUILT_IN_MOS*>(owner());
	const COMMON_BUILT_IN_MOS* c = asserted_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const MODEL_BUILT_IN_MOS8* m = asserted_cast<const MODEL_BUILT_IN_MOS8*>(c->model());

	if (m->use_hci()){
		assert(hci_node);
		hci_node->tt_set(0);
		hci_node->tr_set(0);
	}
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tt_commit() {
	incomplete();
	ADP_BUILT_IN_MOS::tt_commit();
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::stress_apply() {
	trace1("ADP_BUILT_IN_MOS8::stress_apply", long_label());
	// HIER
	const DEV_BUILT_IN_MOS* d = asserted_cast<const DEV_BUILT_IN_MOS*>(owner());
	const COMMON_BUILT_IN_MOS* c = asserted_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const MODEL_BUILT_IN_MOS8* m = asserted_cast<const MODEL_BUILT_IN_MOS8*>(c->model());

	if (m->use_hci())
		hci_node->tt() = 0;
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tt_accept() {
	ADP_BUILT_IN_MOS::tt_accept();
}
/*--------------------------------------------------------------------------*/
// vim works with any ts=sw
