#include <bandwidth.hpp>

static void decimal_fix(double &value) {

	unsigned long long i = ( value * 100 ) + 5;
	i *= 0.1;
	value = i * 0.1;
}

bandwidth_t::bps_t::bps_t(const unsigned long long bytes) {

	this -> bytes = bytes == 0 ? 0 : (( bytes / 1.049 ) * 8.3886 );
	this -> calculate();
}

void bandwidth_t::bps_t::calculate() {

	if ( this -> bytes == 0 ) {

		this -> bytes = 0;
		this -> kb = 0;
		this -> mb = 0;
		this -> gb = 0;

	} else {

		this -> kb = this -> bytes / 1024;
		decimal_fix(this -> kb);

		this -> mb = this -> kb / 1024;
		decimal_fix(this -> mb);

		this -> gb = this -> mb / 1024;
		decimal_fix(this -> gb);
	}
}

bool bandwidth_t::bps_t::operator ==(const bps_t& other) const {

	return this -> bytes == other.bytes && this -> kb == other.kb && this -> mb == other.mb && this -> gb == other.gb;
}

bool bandwidth_t::bps_t::operator !=(const bps_t& other) const {

	return this -> bytes != other.bytes || this -> kb != other.kb || this -> mb != other.mb || this -> gb != other.gb;
}

bandwidth_t::bps_t& bandwidth_t::bps_t::operator =(const bps_t& other) {

	this -> bytes = other.bytes;
	this -> kb = other.kb;
	this -> mb = other.mb;
	this -> gb = other.gb;
	return *this;
}

bandwidth_t::bps_t::TYPE bandwidth_t::bps_t::type() const {

	if ((int)this -> gb >= 1 )
		return bandwidth_t::bps_t::TYPE::G;
	else if ((int)this -> mb >= 1 )
		return bandwidth_t::bps_t::TYPE::M;
	else return bandwidth_t::bps_t::TYPE::K;
}

double bandwidth_t::bps_t::value() const {

	if ((int)this -> gb >= 1 )
		return this -> gb;
	else if ((int)this -> mb >= 1 )
		return this -> mb;
	else return this -> kb;
}

bandwidth_t::bps_t bandwidth_t::interface_t::rx() const {

	if ( this -> _rx_rate == 0 )
		return bandwidth_t::bps_t(0);

	bandwidth_t::bps_t bps;
	bps.bytes = this -> _rx_rate;
	bps.calculate();
	return bps;
}

bandwidth_t::bps_t bandwidth_t::interface_t::tx() const {

	if ( this -> _tx_rate == 0 )
		return bandwidth_t::bps_t(0);

	bandwidth_t::bps_t bps;
	bps.bytes = this -> _tx_rate;
	bps.calculate();
	return bps;
}

bandwidth_t::bps_t bandwidth_t::interface_t::max_rx() const {

	if ( this -> _max_rx_rate == 0 )
		return bandwidth_t::bps_t(0);

	bandwidth_t::bps_t bps;
	bps.bytes = this -> _max_rx_rate;
	bps.calculate();
	return bps;
}

bandwidth_t::bps_t bandwidth_t::interface_t::max_tx() const {

	if ( this -> _max_tx_rate == 0 )
		return bandwidth_t::bps_t(0);

	bandwidth_t::bps_t bps;
	bps.bytes = this -> _max_tx_rate;
	bps.calculate();
	return bps;
}
