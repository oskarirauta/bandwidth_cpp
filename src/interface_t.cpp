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
		this -> _max_rx_rate = 0;
		this -> _max_tx_rate = 0;
		this -> _rx_percent = 0;
		this -> _tx_percent = 0;
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

	if ( this -> _max_rx_rate <= rxc ) {
		this -> _max_rx_rate = rxc;
		this -> _rx_percent = 100.0;
	} else {
		long double p = (long double)rxc / (long double)this -> _max_rx_rate;
		int _p = p * 100000;
		_p += 5;
		_p *= 0.1;
		this -> _rx_percent = _p * 0.01;
	}

	if ( this -> _max_tx_rate <= txc ) {
		this -> _max_tx_rate = txc;
		this -> _tx_percent = 100.0;
	} else {
		long double p = (long double)txc / (long double)this -> _max_tx_rate;
		int _p = p * 100000;
		_p += 5;
		_p *= 0.1;
		this -> _tx_percent = _p * 0.01;
	}

	this -> _rx_rate = rxc;
	this -> _tx_rate = txc;
}

const bandwidth_t::percent_t bandwidth_t::interface_t::percent() const {

	return bandwidth_t::percent_t { .rx = this -> _rx_percent, .tx = this -> _tx_percent };
}
