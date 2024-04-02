#include <iostream>
#include <string>
#include <poll.h>
#include "cmdparser.hpp"
#include "bandwidth.hpp"

const std::string APP_NAME = "bmtest";
const std::string APP_VERSION = "1.0.0";

std::string ifd_name;
bool list_ifds = false;
bool kbps = true;
bool keep_unit = false;
int interval = 1000;

static void version_header(void) {

	std::cout << APP_NAME << " version " << APP_VERSION << "\n"
		"author: Oskari Rauta" << std::endl;
}

static void usage(const CmdParser::Arg &arg) {

	std::cout << "\nusage: " << arg.cmd << " [args]" << "\n" << std::endl;
	std::cout << "options:\n" <<
		" -h, --h               usage\n" <<
		" -v, --v               version\n" <<
		" -l, --l               list available interfaces\n" <<
		" -b, --b               display KBps instead of kbps (kilobits -> kilobytes)\n" <<
		" -k, --k               keep units, do not convert kb to mb even when number grows\n" <<
		" -d, --d <ms>          interval, 100-4000 as milliseconds, default: 1000\n" <<
		" -i, --i <interface>   selects interface to monitor\n" <<
		std::endl;
}

static void show_version(const CmdParser::Arg &arg) {

	version_header();
	exit(0);
}

static void select_ifd(const CmdParser::Arg &argv) {
	ifd_name = argv.var;
}

static void select_delay(const CmdParser::Arg &argv) {

	if ( argv.arg.empty()) {
		std::cout << "illegal interval selected. Value must be between 100 and 4000. Value was empty." << std::endl;
		exit(1);
	}

	int d = 0;

	for ( char ch : argv.var ) {

		if ( !isdigit(ch)) {
			std::cout << "illegal interval selected. Only digits are allowed as a value." << std::endl;
			exit(1);
		}

		d = ( d * 10 ) + ch - 48;
		if ( d > 4000 ) {
			std::cout << "illegal interval selected. Values must not exceed 4000." << std::endl;
			exit(1);
		}
	}

	if ( d < 100 || d > 4000 ) {
		std::cout << "illegal interval selected. Value must be between 100 and 4000." << std::endl;
		exit(1);
	}

	interval = d;
}

int main(int argc, char **argv) {

	CmdParser cmdparser(argc, argv,
		{
			{{ "-h", "--h", "-help", "--help" }, [](const CmdParser::Arg &arg) {
				version_header();
				usage(arg);
				exit(0);
			}, false },
			{{ "-v", "--v", "-version", "--version" }, show_version },
			{{ "-i", "--i", "-interface", "--interface" }, select_ifd, true },
			{{ "-l", "--l", "-list", "--list" }, [](const CmdParser::Arg &arg) {
				list_ifds = true;
			}},
			{{ "-b", "--b", "-B", "--B" }, [](const CmdParser::Arg &arg) {
				kbps = false;
			}},
			{{ "-k", "--k", "-keep", "--keep" }, [](const CmdParser::Arg &arg) {
				keep_unit = true;
			}},
			{{ "-d", "--d", "-interval", "--interval", "-delay", "--delay" }, select_delay, true },
			{{ "" }, [](const CmdParser::Arg &arg) {
				std::cout << "unknown argument " << arg.arg << "\n" <<
					"Try executing " << arg.cmd << " --h for usage" <<
					std::endl;
			}}
		});

	cmdparser.parse();

	bool ifd_ok = false;
	bandwidth_t  bm;

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

	bool ok = bm.update();
	if ( !ok ) {
		std::cout << "error: failed to open/read file /proc/net/dev\nAborted" << std::endl;
		return 1;
	}

	bandwidth_t::bps_t rx;
	bandwidth_t::bps_t tx;

	while ( true ) {

		::poll(nullptr, 0, interval);

		if (!bm.update()) {
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

			std::string rx_unit;
			if ( new_rx.type() == bandwidth_t::bps_t::TYPE::K )
				rx_unit = kbps ? "Kbps" : "Kb/s";
			else if ( new_rx.type() == bandwidth_t::bps_t::TYPE::M )
				rx_unit = kbps ? "Mbps" : "Mb/s";
			else rx_unit = kbps ? "Gbps" : "Gb/s";

			std::string tx_unit;
			if ( new_tx.type() == bandwidth_t::bps_t::TYPE::K )
				tx_unit = kbps ? "Kbps" : "Kb/s";
			else if ( new_tx.type() == bandwidth_t::bps_t::TYPE::M )
				tx_unit = kbps ? "Mbps" : "Mb/s";
			else tx_unit = kbps ? "Gbps" : "Gb/s";

			double rx_p = ifd.percent().rx;
			double tx_p = ifd.percent().tx;

			unsigned long long rx_b = ifd.rx_rate() == 0 ? 0 : ((( ifd.rx_rate() / 1.049 ) * 8.3886 ) / 1024);
			unsigned long long tx_b = ifd.tx_rate() == 0 ? 0 : ((( ifd.tx_rate() / 1.049 ) * 8.3886 ) / 1024);

			// force percentage raise on 5kbps to 0.01 to show percentage even on a very small transmission rates
			// even when a wider broadband connection is used
			if ( rx_p < 0.01 && rx_b >= 5 )
				rx_p = 0.01;

			if ( tx_p < 0.01 && tx_b >= 5 )
				tx_p = 0.01;

			std::cout << ifd.name() <<
					" rx: " << (int)new_rx.value() << rx_unit << " (" << rx_p << "%)" <<
					" tx: " << (int)new_tx.value() << tx_unit << " (" << tx_p << "%)" <<
					std::endl;

			rx = new_rx;
			tx = new_tx;
			break;
		}

		if ( !ifd_ok ) {
			std::cout << "error: interface \"" << ifd_name << "\" is not available.\nAborted." << std::endl;
			return -1;
		}

	}

	return 0;
}
