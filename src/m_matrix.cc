
#include "s_rcm.h"

void rcm(){
	unsigned n = size() - 1; // numer of nodes. 0 doesnt count

	int* xadj = new int[n];
	signed char* mask = new signed char[n];
	for (unsigned x = 0; x<n; mask[x++]=0);

	int edges=0;

	
	// FIXME: count edges during iwant.
	for(unsigned i=0; i < n; ++i ){
		edges += _adj[i].size();
	}

	int* adj = new int[edges];

	unsigned here = 0;
	for(unsigned i=0; i<n; ++i ){
		unsigned s = _adj[j].size();

		for(unsigned j=0; i<s; ++j ){
			adjx[i]=here; here+=s;
		}
	}
	assert(i == size());
	adjx[i] = here;

	_rcm = new int[n+1];
	_rcm[0] = 0;
	int* deg = new int[n];
	genrcmi(n, 0, xadj, adj, _rcm+1, mask, deg);

	delete deg;
	delete adjx;


}
