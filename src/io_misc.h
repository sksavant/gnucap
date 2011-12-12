// some more output stream helpers
// these are all inline templates, because ostream is not a base of OMSTREAM
//
#ifndef IO_MISC_H
#define IO_MISC_H

#include "e_node.h"
#include "u_nodemap.h"
#include "e_cardlist.h"

class NODE;
class NODE_MAP;
/*----------------------------------------------*/
template <class S>
inline S& operator<<( S& o, const CARD_LIST &m)
{
	for (CARD_LIST::const_iterator ci=m.begin(); ci!=m.end(); ++ci) {
		o << (**ci).long_label();
		o << "(" << (*ci) << ")";
		o << " ";
	}
	return o;
}
/*----------------------------------------------*/
template<class S>
inline S& operator<<( S& o, const NODE_MAP& nm){
	o<<"\n";
	for (NODE_MAP::const_iterator ni = nm.begin(); ni != nm.end(); ++ni) {
		NODE_BASE* n = (*ni).second;
		string label = (*ni).first;
		o<<"(" <<  label << ":" << ")";
	}
	return o;
}

#endif
