#pragma once

#include <string>
#include <chrono>
#include <list>

class bandwidth_t {

	public:

		struct bps_t {

			public:

				enum TYPE { K, M, G };

				unsigned long long bytes;
				double kb;
				double mb;
				double gb;

				bool operator ==(const bps_t& other) const;
				bool operator !=(const bps_t& other) const;
				bps_t& operator =(const bps_t& other);

				TYPE type() const;
				double value() const;

				bps_t(const unsigned long long bytes = 0);
		};

		class interface_t {

			friend class bandwidth_t;

			private:
				std::string _name;
				unsigned long long _rx_bytes;
				unsigned long long _rx_packets;
				unsigned long long _rx_errors;
				unsigned long long _tx_bytes;
				unsigned long long _tx_packets;
				unsigned long long _tx_errors;
				unsigned long long _rx_rate;
				unsigned long long _tx_rate;
				std::chrono::milliseconds _millis;

				interface_t(std::string name,
					  unsigned long long rxb = 0, unsigned long long rxp = 0, unsigned long long rxe = 0,
					  unsigned long long txb = 0, unsigned long long txp = 0, unsigned long long txe = 0,
					  std::chrono::milliseconds millis = std::chrono::milliseconds(0)) :
						_name(name),
						_rx_bytes(rxb), _rx_packets(rxp), _rx_errors(rxe),
						_tx_bytes(txb), _tx_packets(txp), _tx_errors(txe),
						_rx_rate(0), _tx_rate(0), _millis(millis) {};

				void update(unsigned long long rxb, unsigned long long rxp, unsigned long long rxe,
					    unsigned long long txb, unsigned long long txp, unsigned long long txe,
					    std::chrono::milliseconds new_millis);

			public:
				inline const std::string name() const { return this -> _name; }
				inline unsigned long long rx_bytes() const { return this -> _rx_bytes; }
				inline unsigned long long rx_packets() const { return this -> _rx_packets; }
				inline unsigned long long rx_errors() const { return this -> _rx_errors; }
				inline unsigned long long tx_bytes() const { return this -> _tx_bytes; }
				inline unsigned long long tx_packets() const { return this -> _tx_packets; }
				inline unsigned long long tx_errors() const { return this -> _tx_errors; }
				inline unsigned long long rx_rate() const { return this -> _rx_rate; }
				inline unsigned long long tx_rate() const { return this -> _tx_rate; }
				inline std::chrono::milliseconds millis() const { return this -> _millis; }
				inline bps_t rx_bps() const { return bandwidth_t::bps_t(this -> _rx_rate); }
				inline bps_t tx_bps() const { return bandwidth_t::bps_t(this -> _tx_rate); }
				bps_t rx() const;
				bps_t tx() const;
		};

		const std::list<bandwidth_t::interface_t> interfaces() const;
		bool update(void); // returns false on failure

		bandwidth_t();

	private:
		std::list<bandwidth_t::interface_t> _interfaces;
};
