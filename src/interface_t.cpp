#include "bandwidth.hpp"

void bandwidth_t::interface_t::update(unsigned long long rxb, unsigned long long rxp, unsigned long long rxe,
				      unsigned long long txb, unsigned long long txp, unsigned long long txe,
				      std::chrono::milliseconds new_millis) {

	unsigned long long rxc = (double)(rxb - this -> _rx_bytes);
	unsigned long long txc = (double)(txb - this -> _tx_bytes);
	double cnt;

	if ( this -> _millis.count() == 0 ) {

		this -> _rx_bytes = rxb;
		this -> _rx_packets = rxp;
		this -> _rx_errors = rxe;
		this -> _tx_bytes = txb;
		this -> _tx_packets = txp;
		this -> _tx_errors = txe;
		this -> _rx_rate = 0;
		this -> _tx_rate = 0;
		this -> _millis = new_millis;
		return;
	}

	std::chrono::milliseconds diff = new_millis - this -> _millis;

	if ( cnt = diff.count(); cnt < 100 )
		return;

	this -> _rx_bytes = rxb;
	this -> _rx_packets = rxp;
	this -> _rx_errors = rxe;
	this -> _tx_bytes = txb;
	this -> _tx_packets = txp;
	this -> _tx_errors = txe;
	this -> _millis = new_millis;

	if ( cnt != 1000 ) {

		double multiplier = 1000 / cnt;
		rxc *= multiplier;
		txc *= multiplier;
	}

	this -> _rx_rate = rxc;
	this -> _tx_rate = txc;
}
