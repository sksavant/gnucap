

#ifndef IO_NODE_H
#define IO_NODE_H

#include "e_cardlist.h"
#include "e_node.h"
#include "u_nodemap.h"
class NODE;
class NODE_MAP;

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
