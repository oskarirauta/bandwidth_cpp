#include <utility>
#include <fstream>

#include "scanner.hpp"
#include "bandwidth.hpp"

bandwidth_t::bandwidth_t() {

	this -> update();
}

const std::list<bandwidth_t::interface_t> bandwidth_t::interfaces() const {

	return std::as_const(this -> _interfaces);
}

static std::string clean_up(const std::string& s) {

	if ( s.empty() || s.find_first_of(':') == std::string::npos || s.find_first_of('|') != std::string::npos )
		return "";

	std::string s1 = s;

	while ( std::isspace(s1.front()))
		s1.erase(0, 1);

	while ( std::isspace(s1.back()))
		s1.pop_back();

	if ( auto it = s1.find_first_of(':'); it != std::string::npos )
		s1.erase(it, 1);
	else return "";

	if ( s1.find_first_of(':') != std::string::npos )
		return "";

	std::string s2;
	for ( const std::string::value_type& ch : s1 )
		if ( std::isspace(ch) && std::isspace(s2.back()))
			continue;
		else if ( std::isspace(ch))
			s2 += ' ';
		else s2 += ch;

	if ( !std::isalpha(s2.front()))
		return "";

	return s2;
}

bool bandwidth_t::update(void) {

	std::ifstream fd("/proc/net/dev", std::ios::in);
	std::string line;

	if ( !fd.is_open() || !fd.good()) {

		if ( fd.is_open())
			fd.close();

		return false;
	}

	while ( !fd.eof() && std::getline(fd, line)) {

		std::chrono::milliseconds millis = std::chrono::duration_cast<std::chrono::milliseconds>
                                        (std::chrono::system_clock::now().time_since_epoch());

		if ( line = clean_up(line); line.empty())
			continue;

		std::string ifd_name;
		unsigned long long rxb, txb, rxp, txp, rxe, txe;

		size_t count = scan(line, {
			{ 0, &ifd_name },
			{ 1, &rxb },
			{ 2, &rxp },
			{ 3, &rxe },
			{ 9, &txb },
			{ 10, &txp },
			{ 11, &txe }
		});

		if ( count < 7 )
			continue;

		bool updated = false;

		for ( auto &ifd : this -> _interfaces ) {
			if ( ifd._name == ifd_name ) {
				ifd.update(rxb, rxp, rxe, txb, txp, txe, millis);
				updated = true;
			}
		}

		if ( !updated )
			this -> _interfaces.push_back(bandwidth_t::interface_t(
					ifd_name,
					rxb, rxp, rxe,
					txb, txp, txe, millis));

	}

	fd.close();
	return true;
}
