
//#include "d_mos.h"
#include "d_mos8.h"

// aging helpers.
// alpha version
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS::wdT() const{
	return ids_stress->wdT();
}
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS::tt_accept()
{
	trace0("ADP_BUILT_IN_MOS::tt_accept");
	//FIXME: move c to ADP_CARD. merge ADP_card with DEV?
	// const DEV_BUILT_IN_MOS* c = (const DEV_BUILT_IN_MOS*) (bti_stress->c());
	//SIM_DATA* sim = c->_sim;
	//  std::cerr << "ADP_BUILT_IN_MOS::tt_accept " << c->long_label() << "\n";
	//  std::cerr << "ADP_BUILT_IN_MOS::tt_accept time " << sim->_Time0 << "\n";
	//  std::cerr << "ADP_BUILT_IN_MOS::tt_accept stress " << bti_stress->get() << "\n";
	const COMMON_COMPONENT* cc = prechecked_cast<const COMMON_COMPONENT*>(_c->common());
	const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(cc->model());
	assert(m);
	assert(cc);

	if (m->use_bti()){
		btistress_taken  = bti_stress->get();
	}

}
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS::tt_probe_num(const std::string& x)const
{
	unreachable(); //?
	if( Umatch("bti ", x) ){
		return bti_stress->get();
	} else if( Umatch("dvth_bti ", x) ) {
		return bti_stress->get();
	} else if( Umatch("dvth_bti ", x) ) {
		return vthdelta_bti;
	} else if( Umatch("bti_eff ", x) ) {
		return bti_eff_voltage;
	}else{
		return 999;    //    return ADP_BUILT_IN_MOS::tr_probe_num(x); diode?
	}
}
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS::tt_commit()
{
	unreachable();


	//SIM_DATA* sim = CKT_BASE::_sim;

	//const COMMON_COMPONENT* cc = prechecked_cast<const COMMON_COMPONENT*>(_c->common());
	//const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(cc->model());

	//??
	//  double stressdelta = bti_stress->get() - btistress_taken ;
	//   bti_eff_voltage = log(stressdelta/sim->_dT0);

}
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS::tr_probe_num(const std::string& x)const
{
	if( Umatch("bti ", x) ){
		if(bti_stress) return bti_stress->tr_get();
		return 9999;
	} else if( Umatch("dvth_bti ", x) ) {
		return vthdelta_bti;
	}else{
		return 888;   //    return ADP_BUILT_IN_MOS::tr_probe_num(x); diode?
	}

}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ADP_BUILT_IN_MOS::ADP_BUILT_IN_MOS( COMPONENT* c, const std::string n) :
	ADP_BUILT_IN_DIODE(c,n),
	bti_stress(0)
{init(c);}
/*--------------------------------------------------------------------------*/
//expand?
void ADP_BUILT_IN_MOS::init(const COMPONENT* c)
{
	trace0(("ADP_BUILT_IN_MOS::init " + c->long_label()).c_str());
	const COMMON_COMPONENT* cc = prechecked_cast<const COMMON_COMPONENT*>(c->common());
	const MODEL_BUILT_IN_MOS_BASE* m = prechecked_cast<const MODEL_BUILT_IN_MOS_BASE*>(cc->model());
	assert(m);
	assert(cc);

	assert(bti_stress == 0);
	if( m->use_bti()){
		bti_stress = new ADP_NODE( c, "bti" );
	} 
	// ADP_NODE_LIST::adp_node_list.push_back( bti_stress );

	// only mos>0?

	//  if( m->use_hci()){
	//    ids_stress = new ADP_NODE( c, "ids" );
	//    igd_stress = new ADP_NODE( c, "igs" );
	//  }

	vthscale_bti = 1;
	vthdelta_bti = 0;

	delta_vth=0;

	btistress_taken = 0;
	bti_eff_voltage = 0;
}

void ADP_BUILT_IN_MOS::tr_stress_last() {
	// bti here?
	// not yet
}

/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS8::tt_probe_num(const std::string& x)const
{
	if( Umatch("hci ", x) ){
		return vthdelta_hci;
	} else if( Umatch("hci_raw ", x) ) {
		if(hci_node) return hci_node->tt();
	} else if( Umatch("dvth_hci ", x) ) {
		return vthdelta_hci;
	}else{
		return ADP_BUILT_IN_MOS::tt_probe_num(x);
	}
		return -1;
}
/*--------------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tr_accept(){


	const DEV_BUILT_IN_MOS* d = prechecked_cast<const DEV_BUILT_IN_MOS*>(owner());
	assert(d);
	const COMMON_BUILT_IN_MOS* c = asserted_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const MODEL_BUILT_IN_MOS8* m = asserted_cast<const MODEL_BUILT_IN_MOS8*>(c->model());
	//ADP_BUILT_IN_MOS8* a = (ADP_BUILT_IN_MOS8*) this;
	trace1("ADP_BUILT_IN_MOS8::tr_accept ", m->polarity );
	ADP_BUILT_IN_MOS::tr_accept();

	//  const COMMON_BUILT_IN_MOS* cprime = prechecked_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const SDP_BUILT_IN_MOS8* s = prechecked_cast<const SDP_BUILT_IN_MOS8*>(c->sdp());

	double exponent = 3; // which is m in [4]
	hp_float_t hcis = 0;
	double Wg = 0.8;
	hp_float_t Ids = fabs(d->ids);

	//  std::cerr << "DEV_BUILT_IN_MOS8::h0 of "<<  d->short_label() << " " <<   m->h0 << "\n";
	double H=m->h0;
	double W=s->w_eff;
	double Hg=m->h0;
	//  double m0 = m->m0;

	if (m->use_hci()){
		if( Ids < 1e-40) 
		{
			trace1("MODEL_BUILT_IN_MOS8::tr_stress ids too small: ", d->ids );
			// return;
		} else {
			hp_float_t Isub;
			if( d->reversed )
				Isub = d->isb;
			else
				Isub = d->idb;

			assert(Isub >= 0);
			//  assert(d->ids >= 0); isnich
			//  assert( dt >= 0 )

			switch(m->polarity){
				case dunno:
					assert(false);
				case pN:
					hcis = Ids * pow( Isub / Ids, exponent)/H/W;
					assert(is_number(hcis));
					trace1( "ADP_BUILT_IN_MOS8::tr_accept nmos", hcis );
					break;
				case pP:
					double mg = 3.0;
					double ig = d->probe_num("ig");
					hcis = ( Wg/Hg * pow( fabs(ig)/W, mg ) 
							+ (1-Wg)*Ids/H/W * pow(Isub/fabs(Ids), exponent));
					trace8( "ADP_BUILT_IN_MOS8::tr_accept pmos", d->long_label(), d->isb, d->idb, W/Hg, Isub, Ids, H, hcis);
					assert(is_number(hcis));
					break;
			}
			if (hcis > 1e-10)
			{
		}

		}

		_hci_tr += hcis *  _sim->_dt0  ;

		//    a->hci_node->add_tr( hcis );

	} // end hci

	q_eval();
}
/*--------------------------------------------------------------------------*/
double ADP_BUILT_IN_MOS8::tr_probe_num(const std::string& x)const
{
	double ret=771;
	if( Umatch("hci_raw ", x) ){
		if(hci_node) return hci_node->tt();
	}else	if( Umatch("hci ", x) ){
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
void ADP_BUILT_IN_MOS8::tt_begin()  // FIXME: tt_begin
{
	const DEV_BUILT_IN_MOS* d = asserted_cast<const DEV_BUILT_IN_MOS*>(owner());
	const COMMON_BUILT_IN_MOS* c = asserted_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const MODEL_BUILT_IN_MOS8* m = asserted_cast<const MODEL_BUILT_IN_MOS8*>(c->model());
	trace3("ADP_BUILT_IN_MOS8::tt_begin", long_label(), m->use_hci(), _sim->tt_iteration_number());

	if (m->use_hci()){
		assert(hci_node);
		hci_node->tt_set(0);
		hci_node->set_tr(-inf);
		_hci_tr=0;
		vthdelta_hci=0;
		vthscale_hci=0;
	}
	q_eval();
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tt_commit() {
	incomplete();
	ADP_BUILT_IN_MOS::tt_commit();
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tr_stress_last() {
	ADP_BUILT_IN_MOS::tr_stress_last();
	const DEV_BUILT_IN_MOS* d = asserted_cast<const DEV_BUILT_IN_MOS*>(owner());
	const COMMON_BUILT_IN_MOS* c = asserted_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const MODEL_BUILT_IN_MOS8* m = asserted_cast<const MODEL_BUILT_IN_MOS8*>(c->model());


	trace2("ADP_BUILT_IN_MOS8::tr_stress_last", long_label(), _sim->tt_iteration_number());
	if(m->use_hci()){
		trace1("ADP_BUILT_IN_MOS::tr_stress_last hci", _sim->_last_time);

		hci_node->tt() += _hci_tr; // tt at last_Time.
		hci_node->set_tr(double(_hci_tr/_sim->_last_time)); 
		hci_node->set_tr_noise(0 );
		trace2("ADP_BUILT_IN_MOS8::tr_stress_last", hci_node->tt(), _hci_tr);
		_hci_tr = 0;
		vthdelta_hci = pow(hci_node->tt(),0.3);
	}
	else{
		trace1("ADP_BUILT_IN_MOS8::tr_stress_last no hci", _hci_tr);
	}
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::stress_apply() {
	trace1("ADP_BUILT_IN_MOS8::stress_apply", long_label());
	// HIER
	const DEV_BUILT_IN_MOS* d = asserted_cast<const DEV_BUILT_IN_MOS*>(owner());
	const COMMON_BUILT_IN_MOS* c = asserted_cast<const COMMON_BUILT_IN_MOS*>(d->common());
	const MODEL_BUILT_IN_MOS8* m = asserted_cast<const MODEL_BUILT_IN_MOS8*>(c->model());

	if (m->use_hci()){


		//		_last_Time is end of last Timeframe

		double eff_now = hci_node->tr( _sim->_Time0 ); // fixme: faster?
		double eff_last_timeframe = hci_node->tr( _sim->_last_Time  ); 

		double ex_time =  _sim->_dT0 - _sim->_last_time; // stress that long

		// stress is integral_{ex_time}  eff1

		double fill_new =  hci_node->tt1(); // tt @ last_Time (?)
		fill_new += ex_time * (  eff_last_timeframe + eff_now ) / 2.0 ;

		hci_node->tt() = 0;
		hci_node->tt() = (double) fill_new;
		assert(!_hci_tr);

		vthdelta_hci = pow(hci_node->tt(),0.3);

		
	}
	q_eval();
}
/*------------------------------------------------------------------*/
void ADP_BUILT_IN_MOS8::tt_accept() {

	ADP_BUILT_IN_MOS::tt_accept();
}
/*--------------------------------------------------------------------------*/
// vim works with any ts=sw
