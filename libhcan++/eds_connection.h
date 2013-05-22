#ifndef EDS_CONNECTION_H
#define EDS_CONNECTION_H

#include <libhcan++/board_connection.h>
#include <libhcan++/traceable_error.h>
#include <vector>
#include <eds_desc.h>
//#include <boost/variant.hpp>
//#include <boost/smart_ptr.hpp>

namespace hcan
{
	class eds_error : public traceable_error
	{
		public:
			eds_error(const string &s) : traceable_error(s) {};
	};

	class eds_connection;

	/**
	 * Repraesentiert einen EDS Block des EDS Interface. Der Constructor
	 * dieser Klasse ist private; nur ein eds_connection Objekt kann solche
	 * Objekte erzeugen. Der Grund ist, dass der Context, also die 
	 * eds_connection und die Adresse des Blocks mit uebergeben werden
	 * muessen.
	 *
	 */

	class eds_block
	{
		private:
			/**
			 * Referenz auf die eds_connection
			 */
			eds_connection &m_econ;
			board_connection &m_bcon;
			transport_connection &m_tcon;
			/**
			 * Adresse des Blocks. Ob die Adresse valide ist, stellt
			 * sich bei Zugriffen heraus. Falls das EDS Interface
			 * Zugriffs-Fehler meldet, wird eine Exception geworfen.
			 */
			uint16_t m_address;
			uint8_t m_type;
			uint8_t m_size;
			const eds_block_desc_t m_desc;
		private:
			friend class eds_connection;
			eds_block(eds_connection &econ, uint16_t address);
		public:
			eds_block &operator = (const eds_block &b);
			eds_block(const eds_block &b);
			virtual ~eds_block();
			const eds_block_desc_t init();
			const string &type_name() const;
			uint8_t type() const { return m_type; }
			uint8_t size() const { return m_size; }
			uint16_t address() const { return m_address; }
			uint8_t field(const string &key);
			uint16_t uint16_field(const string &key);
			string strfield(const string &key);
			void set_field_uint8(const string &key, uint8_t v);
			void set_field_uint16(const string &key, uint16_t v);
			void set_field_str(const string &key, const string &v);
			const eds_block_fields_t &fields() const;
			const string &field_datatype(const string &key) const;
	};

	/**
	 * Repraesentiert eine Verbindung zu einem EDS Interface eines
	 * Boards. Benoetigt wird eine Board-Verbindung (bcon). Diese
	 * liefert der eds_connection die src/dst Adressen und die fuer die
	 * Kommunikation noetige Transport Verbindung (transport_connection).
	 *
	 */

	class eds_connection
	{
		public:
			/**
			 * Container Typ der EDS Bloecke
			 */
			typedef std::vector<eds_block> eds_blocks;
		private:
			board_connection &m_bcon;
			transport_connection &m_tcon;
			eds_blocks m_blocks;

		public:
			eds_connection(board_connection &bcon);
			virtual ~eds_connection();
			/**
			 * Laed die aktuell auf dem Peer gespeicherte EDS Block
			 * Liste. Nach diesem Update kann ueber den blocks-Container
			 * auf die Bloecke zugegriffen werden.
			 */
			void update();
			/**
			 * Setzt den Status des EDS Cache auf invalide. Dies ist
			 * noetig, wenn sich die Struktur der EDS Objekte geaendert
			 * hat
			 */
			size_t space_free() const;
			size_t space_used() const;
			board_connection &bcon() { return m_bcon; };
			/**
			 * Liefert eine Referenz auf den EDS Block Container.
			 * Damit koennen Clients ueber den Container iterieren und
			 * mit den eds_block Objekten arbeiten.
			 */
			eds_blocks &blocks() { return m_blocks; };
			eds_block &block_by_address(uint16_t address);
			eds_block &create_block(const string &type_name);
			void delete_block(const eds_block &block);
			void defragment();
			void format();

	};
};

#endif
