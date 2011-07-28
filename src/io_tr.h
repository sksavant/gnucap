
#ifndef IOTR_
#define IOTR_


template <class T>
inline T& operator<<( T& o, const TRANSIENT::STEP_CAUSE &c)
{
	return o << TRANSIENT::STEP_CAUSE_label[c];
}
#endif
