#include <iostream>
#include <string>
#include <utility>
#include <algorithm>
#include <poll.h>

#include "usage.hpp"
#include "bandwidth.hpp"

const std::string APP_NAME = "bmtest";
const std::string APP_VERSION = "1.0.0";

static void version_header(void) {

	std::cout << APP_NAME << " version " << APP_VERSION << "\n"
		"author: Oskari Rauta" << std::endl;
}

int main(int argc, char **argv) {

	usage_t usage = {
		.args = { argc, argv },
		.info = {
			.name = APP_NAME,
			.version = APP_VERSION,
			.author = "Oskari Rauta",
			.description = "\nMonitors the bandwidth of a network interface.\n"
		},
		.options = {
			{ "help",      { .key = "h", .word = "help", .desc = "show usage" }},
			{ "version",   { .key = "v", .word = "version", .desc = "show version" }},
			{ "list",      { .key = "l", .word = "list", .desc = "list available interfaces" }},
			{ "interface", { .key = "i", .word = "interface", .desc = "interface to monitor", .flag = usage_t::REQUIRED, .name = "interface" }},
			{ "bytes",     { .key = "b", .word = "bytes", .desc = "show KBps (kilobytes) instead of kbps (kilobits)" }},
			{ "keep",      { .key = "k", .word = "keep", .desc = "keep units, do not scale kb up to mb/gb as the number grows" }},
			{ "delay",     { .key = "d", .word = "delay", .desc = "sample interval in milliseconds, 100-4000 (default 1000)", .flag = usage_t::REQUIRED, .name = "ms", .type = usage_t::INT }}
		}
	};

	if ( usage["help"] ) {
		version_header();
		std::cout << usage << "\n" << usage.help() << std::endl;
		return 0;
	}

	if ( usage["version"] ) {
		version_header();
		return 0;
	}

	if ( !usage.validated ) {

		auto errors = usage.errors();
		std::cout << "command-line errors:\n" << errors << std::endl;

		if ( std::any_of(errors.begin(), errors.end(), [](const usage_t::error_t& e) {
				return e.error != usage_t::error_type::DUPLICATE && e.error != usage_t::error_type::UNKNOWN_OPTION;
			})) {
			std::cout << "\naborting due to fatal command-line errors." << std::endl;
			return 1;
		}
	}

	bool list_ifds = usage["list"];
	bool kbps = !usage["bytes"];
	bool keep_unit = usage["keep"];
	int interval = 1000;
	std::string ifd_name = usage["interface"] ? (std::string)usage["interface"] : "";

	if ( usage["delay"] ) {

		long d = usage["delay"].intValue();
		if ( d < 100 || d > 4000 ) {
			std::cout << "illegal interval selected. Value must be between 100 and 4000 milliseconds." << std::endl;
			return 1;
		}
		interval = (int)d;
	}

	bool ifd_ok = false;
	bandwidth_t bm;

	if ( !bm.update()) {
		std::cout << "error: failed to open/read file /proc/net/dev\nAborted" << std::endl;
		return 1;
	}

	if ( list_ifds ) {
		version_header();
		std::cout << "\nlist of available interfaces:";
	}

	for ( auto &ifd : bm.interfaces()) {

		if ( !ifd_name.empty() && ifd.name() == ifd_name )
			ifd_ok = true;

		if ( list_ifds ) std::cout << " " << ifd.name();
	}

	if ( list_ifds ) {

		std::cout << std::endl;
		return 0;

	} else if ( ifd_name.empty()) {
		std::cout << "error: interface was not defined. Use -i to select interface, or -l to list available interfaces." << std::endl;
		return 1;
	} else if ( !ifd_ok ) {
		std::cout << "error: interface \"" << ifd_name << "\" was not found. Use -l argument to list available interfaces." << std::endl;
		return 1;
	}

	version_header();
	std::cout << std::endl;

	if ( !bm.update()) {
		std::cout << "error: failed to open/read file /proc/net/dev\nAborted" << std::endl;
		return 1;
	}

	// pick the value and unit to print: when keeping units we always report in
	// kilobits/kilobytes, otherwise we scale up to Mb/Gb as the number grows
	auto format = [&](const bandwidth_t::bps_t& b) -> std::pair<double, std::string> {

		if ( keep_unit )
			return { b.kb, kbps ? "Kbps" : "Kb/s" };

		switch ( b.type()) {
			case bandwidth_t::bps_t::TYPE::M: return { b.value(), kbps ? "Mbps" : "Mb/s" };
			case bandwidth_t::bps_t::TYPE::G: return { b.value(), kbps ? "Gbps" : "Gb/s" };
			default:                          return { b.value(), kbps ? "Kbps" : "Kb/s" };
		}
	};

	bandwidth_t::bps_t rx;
	bandwidth_t::bps_t tx;

	while ( true ) {

		::poll(nullptr, 0, interval);

		if ( !bm.update()) {
			std::cout << "error: failed to open/read file /proc/net/dev\nAborted" << std::endl;
			return 1;
		}

		ifd_ok = false;

		for ( auto &ifd : bm.interfaces()) {

			if ( ifd.name() != ifd_name )
				continue;

			ifd_ok = true;

			bandwidth_t::bps_t new_rx = kbps ? ifd.rx_bps() : ifd.rx();
			bandwidth_t::bps_t new_tx = kbps ? ifd.tx_bps() : ifd.tx();

			if ( new_rx == rx && new_tx == tx )
				break;

			auto [rx_val, rx_unit] = format(new_rx);
			auto [tx_val, tx_unit] = format(new_tx);

			double rx_p = ifd.percent().rx;
			double tx_p = ifd.percent().tx;

			unsigned long long rx_b = ifd.rx_rate() * 8 / 1000;	// kilobits/s
			unsigned long long tx_b = ifd.tx_rate() * 8 / 1000;

			// force percentage up to 0.01 on >= 5kbps so a tiny transmission rate
			// still shows a percentage even on a wider broadband connection
			if ( rx_p < 0.01 && rx_b >= 5 )
				rx_p = 0.01;

			if ( tx_p < 0.01 && tx_b >= 5 )
				tx_p = 0.01;

			std::cout << ifd.name() <<
					" rx: " << (int)rx_val << rx_unit << " (" << rx_p << "%)" <<
					" tx: " << (int)tx_val << tx_unit << " (" << tx_p << "%)" <<
					std::endl;

			rx = new_rx;
			tx = new_tx;
			break;
		}

		if ( !ifd_ok ) {
			std::cout << "error: interface \"" << ifd_name << "\" is not available.\nAborted." << std::endl;
			return 1;
		}
	}

	return 0;
}
