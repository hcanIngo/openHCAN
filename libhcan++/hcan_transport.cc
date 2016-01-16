
// ------- AUTOMATICLY GENERATED FROM mk_transport_cc.xsl
// -----------------

#include <libhcan++/frame.h>
#include <assert.h>
#include <hcan_transport.h>

using namespace hcan;

hcan_transport::hcan_transport(const in_addr_t hcand_ip):
m_hcand_ip(hcand_ip)
{
}

hcan_transport::~hcan_transport()
{
}



	/**
	 * Zum Testen, ob ein Geraet auf diese Adresse horcht
	 * 
	 * 
	 */
void
hcan_transport::send_PING_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	1,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Zum Testen, ob ein Geraet auf diese Adresse horcht
	 * 
	 * 
	 */
void
hcan_transport::recv_PING_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	1,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Anwort des Geraetes
	 * 
	 * 
	 */
void
hcan_transport::send_PING_REPLAY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	2,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anwort des Geraetes
	 * 
	 * 
	 */
void
hcan_transport::recv_PING_REPLAY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	2,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Fragt ein Geraet nach seinem aktuellen Zustand
	 * 
	 * 
	 */
void
hcan_transport::send_DEVICE_STATE_REQUEST(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt ein Geraet nach seinem aktuellen Zustand
	 * 
	 * 
	 */
void
hcan_transport::recv_DEVICE_STATE_REQUEST(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	3,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert den aktuellen Zustand zurueck
	 * 
	 * 
	 * @param state : 0 = booting, 1 = Bootloader, 2 = Application
	 *
	 * 
	 */
void
hcan_transport::send_DEVICE_STATE_REPLAY(const uint16_t src,
					 const uint16_t dst,
					 uint8_t state)
{
    uint8_t         data[] = {
	1,
	4,
	state,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert den aktuellen Zustand zurueck
	 * 
	 * 
	 * @param state : 0 = booting, 1 = Bootloader, 2 = Application
	 *
	 * 
	 */
void
hcan_transport::recv_DEVICE_STATE_REPLAY(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * state)
{
    uint8_t         data[] = {
	1,
	4,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *state = f.data(2);

}



	/**
	 * Loest einen Watchdog Reset aus
	 * 
	 * 
	 */
void
hcan_transport::send_DEVICE_RESET(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	5,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Loest einen Watchdog Reset aus
	 * 
	 * 
	 */
void
hcan_transport::recv_DEVICE_RESET(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	5,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * stoppt den Bootvorgang; Geraet geht in den Bootloader-State
	 * 
	 * 
	 */
void
hcan_transport::send_DEVICE_BOOT_STOP(const uint16_t src,
				      const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	6,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * stoppt den Bootvorgang; Geraet geht in den Bootloader-State
	 * 
	 * 
	 */
void
hcan_transport::recv_DEVICE_BOOT_STOP(const uint16_t src,
				      const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	6,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Wenn Bootvorgang gestoppt, sendet das Geraet diese Bestaetigung
	 * 
	 * 
	 */
void
hcan_transport::send_DEVICE_BOOT_STOP_ACK(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	7,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Wenn Bootvorgang gestoppt, sendet das Geraet diese Bestaetigung
	 * 
	 * 
	 */
void
hcan_transport::recv_DEVICE_BOOT_STOP_ACK(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	7,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Laed die Applikation, sofern das Geraet im Bootloader State ist
	 * 
	 * 
	 */
void
hcan_transport::send_DEVICE_LOAD_APP(const uint16_t src,
				     const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	8,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Laed die Applikation, sofern das Geraet im Bootloader State ist
	 * 
	 * 
	 */
void
hcan_transport::recv_DEVICE_LOAD_APP(const uint16_t src,
				     const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	8,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Fragt den Geraetetyp ab
	 * 
	 * 
	 */
void
hcan_transport::send_DEVICE_TYPE_REQUEST(const uint16_t src,
					 const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	9,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt den Geraetetyp ab
	 * 
	 * 
	 */
void
hcan_transport::recv_DEVICE_TYPE_REQUEST(const uint16_t src,
					 const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	9,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 9)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert den Geraete-Typ
	 * 
	 * 
	 * @param arch : 0 = Atmega8, 1 = Atmega32, 2 = Atmega644p
	 *
	 * 
	 * @param type : Hardware-Board-Typ
	 *
	 * 
	 */
void
hcan_transport::send_DEVICE_TYPE_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t arch, uint8_t type)
{
    uint8_t         data[] = {
	1,
	10,
	arch,
	type,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert den Geraete-Typ
	 * 
	 * 
	 * @param arch : 0 = Atmega8, 1 = Atmega32, 2 = Atmega644p
	 *
	 * 
	 * @param type : Hardware-Board-Typ
	 *
	 * 
	 */
void
hcan_transport::recv_DEVICE_TYPE_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t * arch, uint8_t * type)
{
    uint8_t         data[] = {
	1,
	10,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *arch = f.data(2);

    *type = f.data(3);

}



	/**
	 * Sendet 4 Bytes zum Geraet. Details stehen in der Dokumentation zum Flashen
	 * 
	 * 
	 * @param index : definiert die Position von d[0] im Zielbuffer auf dem Device
	 *
	 * 
	 * @param d0 : Databyte
	 *
	 * 
	 * @param d1 : Databyte
	 *
	 * 
	 * @param d2 : Databyte
	 *
	 * 
	 * @param d3 : Databyte
	 *
	 * 
	 */
void
hcan_transport::send_FLASH_BUFFER_FILL(const uint16_t src,
				       const uint16_t dst,
				       uint8_t index,
				       uint8_t d0,
				       uint8_t d1, uint8_t d2, uint8_t d3)
{
    uint8_t         data[] = {
	1,
	16,
	index,
	d0,
	d1,
	d2,
	d3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Sendet 4 Bytes zum Geraet. Details stehen in der Dokumentation zum Flashen
	 * 
	 * 
	 * @param index : definiert die Position von d[0] im Zielbuffer auf dem Device
	 *
	 * 
	 * @param d0 : Databyte
	 *
	 * 
	 * @param d1 : Databyte
	 *
	 * 
	 * @param d2 : Databyte
	 *
	 * 
	 * @param d3 : Databyte
	 *
	 * 
	 */
void
hcan_transport::recv_FLASH_BUFFER_FILL(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * index,
				       uint8_t * d0,
				       uint8_t * d1,
				       uint8_t * d2, uint8_t * d3)
{
    uint8_t         data[] = {
	1,
	16,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 16)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *index = f.data(2);

    *d0 = f.data(3);

    *d1 = f.data(4);

    *d2 = f.data(5);

    *d3 = f.data(6);

}



	/**
	 * Geraet bestaetigt damit den Erhalt einer Zeile
	 * 
	 * 
	 */
void
hcan_transport::send_FLASH_BUFFER_FILL_ACK(const uint16_t src,
					   const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	17,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet bestaetigt damit den Erhalt einer Zeile
	 * 
	 * 
	 */
void
hcan_transport::recv_FLASH_BUFFER_FILL_ACK(const uint16_t src,
					   const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	17,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 17)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Geraet schreibt den Puffer an die gegebene Adresse in das Flash
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im Flash
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im Flash
	 *
	 * 
	 */
void
hcan_transport::send_FLASH_BUFFER_WRITE(const uint16_t src,
					const uint16_t dst,
					uint8_t addr_lsb,
					uint8_t addr_msb)
{
    uint8_t         data[] = {
	1,
	18,
	addr_lsb,
	addr_msb,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet schreibt den Puffer an die gegebene Adresse in das Flash
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im Flash
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im Flash
	 *
	 * 
	 */
void
hcan_transport::recv_FLASH_BUFFER_WRITE(const uint16_t src,
					const uint16_t dst,
					uint8_t * addr_lsb,
					uint8_t * addr_msb)
{
    uint8_t         data[] = {
	1,
	18,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 18)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *addr_lsb = f.data(2);

    *addr_msb = f.data(3);

}



	/**
	 * Geraet bestaetigt das Schreiben
	 * 
	 * 
	 */
void
hcan_transport::send_FLASH_BUFFER_WRITE_ACK(const uint16_t src,
					    const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	19,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet bestaetigt das Schreiben
	 * 
	 * 
	 */
void
hcan_transport::recv_FLASH_BUFFER_WRITE_ACK(const uint16_t src,
					    const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	19,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 19)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Beschreibt eine Speicherzelle des internen EEPROMs (ohne Pruefung!)
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im EEPROM
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im EEPROM
	 *
	 * 
	 * @param value : der zu schreibende Wert
	 *
	 * 
	 */
void
hcan_transport::send_INTERNAL_EEPROM_WRITE(const uint16_t src,
					   const uint16_t dst,
					   uint8_t addr_lsb,
					   uint8_t addr_msb,
					   uint8_t value)
{
    uint8_t         data[] = {
	1,
	20,
	addr_lsb,
	addr_msb,
	value,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Beschreibt eine Speicherzelle des internen EEPROMs (ohne Pruefung!)
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im EEPROM
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im EEPROM
	 *
	 * 
	 * @param value : der zu schreibende Wert
	 *
	 * 
	 */
void
hcan_transport::recv_INTERNAL_EEPROM_WRITE(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * addr_lsb,
					   uint8_t * addr_msb,
					   uint8_t * value)
{
    uint8_t         data[] = {
	1,
	20,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 20)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *addr_lsb = f.data(2);

    *addr_msb = f.data(3);

    *value = f.data(4);

}



	/**
	 * Geraet bestaetigt das Schreiben (ohne Pruefung!)
	 * 
	 * 
	 */
void
hcan_transport::send_INTERNAL_EEPROM_WRITE_ACK(const uint16_t src,
					       const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	21,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet bestaetigt das Schreiben (ohne Pruefung!)
	 * 
	 * 
	 */
void
hcan_transport::recv_INTERNAL_EEPROM_WRITE_ACK(const uint16_t src,
					       const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	21,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 21)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Leseanforderung fuer eine Speicherzelle des internen EEPROMs
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im EEPROM
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im EEPROM
	 *
	 * 
	 */
void
hcan_transport::send_INTERNAL_EEPROM_READ_REQUEST(const uint16_t src,
						  const uint16_t dst,
						  uint8_t addr_lsb,
						  uint8_t addr_msb)
{
    uint8_t         data[] = {
	1,
	22,
	addr_lsb,
	addr_msb,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Leseanforderung fuer eine Speicherzelle des internen EEPROMs
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im EEPROM
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im EEPROM
	 *
	 * 
	 */
void
hcan_transport::recv_INTERNAL_EEPROM_READ_REQUEST(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * addr_lsb,
						  uint8_t * addr_msb)
{
    uint8_t         data[] = {
	1,
	22,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 22)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *addr_lsb = f.data(2);

    *addr_msb = f.data(3);

}



	/**
	 * Geraet sendet den gelesenen Wert
	 * 
	 * 
	 * @param value : der Wert aus dem EEPROM
	 *
	 * 
	 */
void
hcan_transport::send_INTERNAL_EEPROM_READ_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t value)
{
    uint8_t         data[] = {
	1,
	23,
	value,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet sendet den gelesenen Wert
	 * 
	 * 
	 * @param value : der Wert aus dem EEPROM
	 *
	 * 
	 */
void
hcan_transport::recv_INTERNAL_EEPROM_READ_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * value)
{
    uint8_t         data[] = {
	1,
	23,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 23)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *value = f.data(2);

}



	/**
	 * Leseanforderung fuer eine Speicherzelle des internen EEPROMs, Bulkversion
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im EEPROM
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im EEPROM
	 *
	 * 
	 */
void
hcan_transport::send_INTERNAL_EEPROM_READ_BULK_REQUEST(const uint16_t src,
						       const uint16_t dst,
						       uint8_t addr_lsb,
						       uint8_t addr_msb)
{
    uint8_t         data[] = {
	1,
	24,
	addr_lsb,
	addr_msb,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Leseanforderung fuer eine Speicherzelle des internen EEPROMs, Bulkversion
	 * 
	 * 
	 * @param addr_lsb : LSB der Adresse im EEPROM
	 *
	 * 
	 * @param addr_msb : MSB der Adresse im EEPROM
	 *
	 * 
	 */
void
hcan_transport::recv_INTERNAL_EEPROM_READ_BULK_REQUEST(const uint16_t src,
						       const uint16_t dst,
						       uint8_t * addr_lsb,
						       uint8_t * addr_msb)
{
    uint8_t         data[] = {
	1,
	24,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 24)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *addr_lsb = f.data(2);

    *addr_msb = f.data(3);

}



	/**
	 * Geraet sendet 6 gelesene Bytes (Bulk) ab addr_msb:addr_lsb
	 * 
	 * 
	 * @param d0 : Byte an Offset 0
	 *
	 * 
	 * @param d1 : Byte an Offset 1
	 *
	 * 
	 * @param d2 : Byte an Offset 2
	 *
	 * 
	 * @param d3 : Byte an Offset 3
	 *
	 * 
	 * @param d4 : Byte an Offset 4
	 *
	 * 
	 * @param d5 : Byte an Offset 5
	 *
	 * 
	 */
void
hcan_transport::send_INTERNAL_EEPROM_READ_BULK_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t d0,
						      uint8_t d1,
						      uint8_t d2,
						      uint8_t d3,
						      uint8_t d4,
						      uint8_t d5)
{
    uint8_t         data[] = {
	1,
	25,
	d0,
	d1,
	d2,
	d3,
	d4,
	d5,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet sendet 6 gelesene Bytes (Bulk) ab addr_msb:addr_lsb
	 * 
	 * 
	 * @param d0 : Byte an Offset 0
	 *
	 * 
	 * @param d1 : Byte an Offset 1
	 *
	 * 
	 * @param d2 : Byte an Offset 2
	 *
	 * 
	 * @param d3 : Byte an Offset 3
	 *
	 * 
	 * @param d4 : Byte an Offset 4
	 *
	 * 
	 * @param d5 : Byte an Offset 5
	 *
	 * 
	 */
void
hcan_transport::recv_INTERNAL_EEPROM_READ_BULK_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * d0,
						      uint8_t * d1,
						      uint8_t * d2,
						      uint8_t * d3,
						      uint8_t * d4,
						      uint8_t * d5)
{
    uint8_t         data[] = {
	1,
	25,

	0,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 25)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *d0 = f.data(2);

    *d1 = f.data(3);

    *d2 = f.data(4);

    *d3 = f.data(5);

    *d4 = f.data(6);

    *d5 = f.data(7);

}



	/**
	 * Abfrage der Uptime. Das ist die vergangene Zeit seit dem letzten Reboot/Reset
	 * 
	 * 
	 */
void
hcan_transport::send_UPTIME_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	30,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Abfrage der Uptime. Das ist die vergangene Zeit seit dem letzten Reboot/Reset
	 * 
	 * 
	 */
void
hcan_transport::recv_UPTIME_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	30,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 30)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Geraet sendet die Uptime als 32Bit Wert in Sekunden
	 * 
	 * 
	 * @param u3 : MSB der 32 Bit Uptime
	 *
	 * 
	 * @param u2 : 
	 *
	 * 
	 * @param u1 : 
	 *
	 * 
	 * @param u0 : LSB der 32 Bit Uptime
	 *
	 * 
	 */
void
hcan_transport::send_UPTIME_REPLAY(const uint16_t src,
				   const uint16_t dst,
				   uint8_t u3,
				   uint8_t u2, uint8_t u1, uint8_t u0)
{
    uint8_t         data[] = {
	1,
	31,
	u3,
	u2,
	u1,
	u0,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet sendet die Uptime als 32Bit Wert in Sekunden
	 * 
	 * 
	 * @param u3 : MSB der 32 Bit Uptime
	 *
	 * 
	 * @param u2 : 
	 *
	 * 
	 * @param u1 : 
	 *
	 * 
	 * @param u0 : LSB der 32 Bit Uptime
	 *
	 * 
	 */
void
hcan_transport::recv_UPTIME_REPLAY(const uint16_t src,
				   const uint16_t dst,
				   uint8_t * u3,
				   uint8_t * u2,
				   uint8_t * u1, uint8_t * u0)
{
    uint8_t         data[] = {
	1,
	31,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 31)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *u3 = f.data(2);

    *u2 = f.data(3);

    *u1 = f.data(4);

    *u0 = f.data(5);

}



	/**
	 * Setzt den Debug-Level fuer die Syslog Botschaften
	 * 
	 * 
	 * @param level :  1 = CRITICAL, 2 = ERROR, 3 = WARNING, 4 = DEBUG 
	 *
	 * 
	 */
void
hcan_transport::send_SYSLOG_LEVEL_SET(const uint16_t src,
				      const uint16_t dst, uint8_t level)
{
    uint8_t         data[] = {
	1,
	32,
	level,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Setzt den Debug-Level fuer die Syslog Botschaften
	 * 
	 * 
	 * @param level :  1 = CRITICAL, 2 = ERROR, 3 = WARNING, 4 = DEBUG 
	 *
	 * 
	 */
void
hcan_transport::recv_SYSLOG_LEVEL_SET(const uint16_t src,
				      const uint16_t dst, uint8_t * level)
{
    uint8_t         data[] = {
	1,
	32,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 32)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *level = f.data(2);

}



	/**
	 * Fragt nach der Build Version
	 * 
	 * 
	 */
void
hcan_transport::send_BUILD_VERSION_QUERY(const uint16_t src,
					 const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	33,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Build Version
	 * 
	 * 
	 */
void
hcan_transport::recv_BUILD_VERSION_QUERY(const uint16_t src,
					 const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	33,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 33)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Build Version
	 * 
	 * 
	 * @param hi : Build Nummer, Hi Teil
	 *
	 * 
	 * @param lo : Build Nummer, Lo Teil
	 *
	 * 
	 */
void
hcan_transport::send_BUILD_VERSION_REPLAY(const uint16_t src,
					  const uint16_t dst,
					  uint8_t hi, uint8_t lo)
{
    uint8_t         data[] = {
	1,
	34,
	hi,
	lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Build Version
	 * 
	 * 
	 * @param hi : Build Nummer, Hi Teil
	 *
	 * 
	 * @param lo : Build Nummer, Lo Teil
	 *
	 * 
	 */
void
hcan_transport::recv_BUILD_VERSION_REPLAY(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * hi, uint8_t * lo)
{
    uint8_t         data[] = {
	1,
	34,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 34)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *hi = f.data(2);

    *lo = f.data(3);

}



	/**
	 * Fragt nach den CAN Error Counter Staenden
	 * 
	 * 
	 */
void
hcan_transport::send_CAN_EC_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	35,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach den CAN Error Counter Staenden
	 * 
	 * 
	 */
void
hcan_transport::recv_CAN_EC_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	35,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 35)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die CAN Error Counter Staende
	 * 
	 * 
	 * @param REC : RX Error Counter
	 *
	 * 
	 * @param TEC : TX Error Counter
	 *
	 * 
	 */
void
hcan_transport::send_CAN_EC_REPLAY(const uint16_t src,
				   const uint16_t dst,
				   uint8_t REC, uint8_t TEC)
{
    uint8_t         data[] = {
	1,
	36,
	REC,
	TEC,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die CAN Error Counter Staende
	 * 
	 * 
	 * @param REC : RX Error Counter
	 *
	 * 
	 * @param TEC : TX Error Counter
	 *
	 * 
	 */
void
hcan_transport::recv_CAN_EC_REPLAY(const uint16_t src,
				   const uint16_t dst,
				   uint8_t * REC, uint8_t * TEC)
{
    uint8_t         data[] = {
	1,
	36,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 36)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *REC = f.data(2);

    *TEC = f.data(3);

}



	/**
	 * Errechnet die CRC16 Pruefsumme ueber den Flash bis zur angegebenen Adresse
	 * 
	 * 
	 * @param hi : Adresse hi
	 *
	 * 
	 * @param lo : Adresse lo
	 *
	 * 
	 */
void
hcan_transport::send_FLASH_CRC16_QUERY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t hi, uint8_t lo)
{
    uint8_t         data[] = {
	1,
	37,
	hi,
	lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Errechnet die CRC16 Pruefsumme ueber den Flash bis zur angegebenen Adresse
	 * 
	 * 
	 * @param hi : Adresse hi
	 *
	 * 
	 * @param lo : Adresse lo
	 *
	 * 
	 */
void
hcan_transport::recv_FLASH_CRC16_QUERY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * hi, uint8_t * lo)
{
    uint8_t         data[] = {
	1,
	37,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 37)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *hi = f.data(2);

    *lo = f.data(3);

}



	/**
	 * Liefert die errechnete CRC16 Pruefsumme ueber den Flash
	 * 
	 * 
	 * @param hi : CRC16 hi
	 *
	 * 
	 * @param lo : CRC16 lo
	 *
	 * 
	 */
void
hcan_transport::send_FLASH_CRC16_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t hi, uint8_t lo)
{
    uint8_t         data[] = {
	1,
	38,
	hi,
	lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die errechnete CRC16 Pruefsumme ueber den Flash
	 * 
	 * 
	 * @param hi : CRC16 hi
	 *
	 * 
	 * @param lo : CRC16 lo
	 *
	 * 
	 */
void
hcan_transport::recv_FLASH_CRC16_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t * hi, uint8_t * lo)
{
    uint8_t         data[] = {
	1,
	38,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 38)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *hi = f.data(2);

    *lo = f.data(3);

}



	/**
	 * Fragt nach der Anzahl der Schleifen/sec
	 * 
	 * 
	 */
void
hcan_transport::send_LOOP_THROUGHPUT_QUERY(const uint16_t src,
					   const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	39,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der Schleifen/sec
	 * 
	 * 
	 */
void
hcan_transport::recv_LOOP_THROUGHPUT_QUERY(const uint16_t src,
					   const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	39,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 39)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Anzahl der Schleifen/sec
	 * 
	 * 
	 * @param hi : MSB
	 *
	 * 
	 * @param lo : LSB
	 *
	 * 
	 */
void
hcan_transport::send_LOOP_THROUGHPUT_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t hi, uint8_t lo)
{
    uint8_t         data[] = {
	1,
	40,
	hi,
	lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Anzahl der Schleifen/sec
	 * 
	 * 
	 * @param hi : MSB
	 *
	 * 
	 * @param lo : LSB
	 *
	 * 
	 */
void
hcan_transport::recv_LOOP_THROUGHPUT_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * hi, uint8_t * lo)
{
    uint8_t         data[] = {
	1,
	40,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 40)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *hi = f.data(2);

    *lo = f.data(3);

}



	/**
	 * Fragt nach der Anzahl der bisher empfangenen Frames
	 * 
	 * 
	 */
void
hcan_transport::send_RX_RECEIVED_QUERY(const uint16_t src,
				       const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	41,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der bisher empfangenen Frames
	 * 
	 * 
	 */
void
hcan_transport::recv_RX_RECEIVED_QUERY(const uint16_t src,
				       const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	41,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 41)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Anzahl der bisher empfangenen Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::send_RX_RECEIVED_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t d0,
					uint8_t d1,
					uint8_t d2, uint8_t d3)
{
    uint8_t         data[] = {
	1,
	42,
	d0,
	d1,
	d2,
	d3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Anzahl der bisher empfangenen Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::recv_RX_RECEIVED_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t * d0,
					uint8_t * d1,
					uint8_t * d2, uint8_t * d3)
{
    uint8_t         data[] = {
	1,
	42,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 42)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *d0 = f.data(2);

    *d1 = f.data(3);

    *d2 = f.data(4);

    *d3 = f.data(5);

}



	/**
	 * Fragt nach der Anzahl der bisher verlorenen Frames
	 * 
	 * 
	 */
void
hcan_transport::send_RX_LOST_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	43,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der bisher verlorenen Frames
	 * 
	 * 
	 */
void
hcan_transport::recv_RX_LOST_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	43,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 43)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Anzahl der bisher verlorenen Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::send_RX_LOST_REPLAY(const uint16_t src,
				    const uint16_t dst,
				    uint8_t d0,
				    uint8_t d1, uint8_t d2, uint8_t d3)
{
    uint8_t         data[] = {
	1,
	44,
	d0,
	d1,
	d2,
	d3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Anzahl der bisher verlorenen Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::recv_RX_LOST_REPLAY(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * d0,
				    uint8_t * d1,
				    uint8_t * d2, uint8_t * d3)
{
    uint8_t         data[] = {
	1,
	44,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 44)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *d0 = f.data(2);

    *d1 = f.data(3);

    *d2 = f.data(4);

    *d3 = f.data(5);

}



	/**
	 * Fragt nach der Anzahl der bisher gesendeten Frames
	 * 
	 * 
	 */
void
hcan_transport::send_TX_SENT_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	45,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der bisher gesendeten Frames
	 * 
	 * 
	 */
void
hcan_transport::recv_TX_SENT_QUERY(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	45,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 45)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Anzahl der bisher gesendeten Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::send_TX_SENT_REPLAY(const uint16_t src,
				    const uint16_t dst,
				    uint8_t d0,
				    uint8_t d1, uint8_t d2, uint8_t d3)
{
    uint8_t         data[] = {
	1,
	46,
	d0,
	d1,
	d2,
	d3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Anzahl der bisher gesendeten Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::recv_TX_SENT_REPLAY(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * d0,
				    uint8_t * d1,
				    uint8_t * d2, uint8_t * d3)
{
    uint8_t         data[] = {
	1,
	46,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 46)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *d0 = f.data(2);

    *d1 = f.data(3);

    *d2 = f.data(4);

    *d3 = f.data(5);

}



	/**
	 * Fragt nach der Anzahl der bisher verworfenen Frames
	 * 
	 * 
	 */
void
hcan_transport::send_TX_DROPPED_QUERY(const uint16_t src,
				      const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	47,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der bisher verworfenen Frames
	 * 
	 * 
	 */
void
hcan_transport::recv_TX_DROPPED_QUERY(const uint16_t src,
				      const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	47,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 47)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Anzahl der bisher verworfenen Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::send_TX_DROPPED_REPLAY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t d0,
				       uint8_t d1, uint8_t d2, uint8_t d3)
{
    uint8_t         data[] = {
	1,
	48,
	d0,
	d1,
	d2,
	d3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Anzahl der bisher verworfenen Frames (32bit)
	 * 
	 * 
	 * @param d0 : MSB
	 *
	 * 
	 * @param d1 : 
	 *
	 * 
	 * @param d2 : 
	 *
	 * 
	 * @param d3 : LSB
	 *
	 * 
	 */
void
hcan_transport::recv_TX_DROPPED_REPLAY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * d0,
				       uint8_t * d1,
				       uint8_t * d2, uint8_t * d3)
{
    uint8_t         data[] = {
	1,
	48,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 48)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *d0 = f.data(2);

    *d1 = f.data(3);

    *d2 = f.data(4);

    *d3 = f.data(5);

}



	/**
	 * Setzt alle RX/TX Stats Counter zurueck
	 * 
	 * 
	 */
void
hcan_transport::send_RX_TX_STATS_RESET(const uint16_t src,
				       const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	49,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Setzt alle RX/TX Stats Counter zurueck
	 * 
	 * 
	 */
void
hcan_transport::recv_RX_TX_STATS_RESET(const uint16_t src,
				       const uint16_t dst)
{
    uint8_t         data[] = {
	1,
	49,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 49)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Meldet den Wert des Resetflag-Registers direkt nach dem Booten
	 * 
	 * 
	 * @param flag : siehe Atmega8 Referenz-Handbuch, S.39
	 *
	 * 
	 */
void
hcan_transport::send_BOOT_RESETFLAG_LOG(const uint16_t src,
					const uint16_t dst, uint8_t flag)
{
    uint8_t         data[] = {
	4,
	1,
	flag,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet den Wert des Resetflag-Registers direkt nach dem Booten
	 * 
	 * 
	 * @param flag : siehe Atmega8 Referenz-Handbuch, S.39
	 *
	 * 
	 */
void
hcan_transport::recv_BOOT_RESETFLAG_LOG(const uint16_t src,
					const uint16_t dst,
					uint8_t * flag)
{
    uint8_t         data[] = {
	4,
	1,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *flag = f.data(2);

}



	/**
	 * Meldet eine fehlerhafte CRC16 Firmware Pruefsumme
	 * 
	 * 
	 */
void
hcan_transport::send_FIRMWARE_CRC16_ERROR(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	4,
	2,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet eine fehlerhafte CRC16 Firmware Pruefsumme
	 * 
	 * 
	 */
void
hcan_transport::recv_FIRMWARE_CRC16_ERROR(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	4,
	2,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Firmware und Config sind nicht kompatibel
	 * 
	 * 
	 * @param eds_block_type : betroffener EDS Block-Typ
	 *
	 * 
	 * @param eds_addr_hi : EDS Adresse Hi
	 *
	 * 
	 * @param eds_addr_lo : EDS Adresse Lo
	 *
	 * 
	 * @param size_in_config : Groesse im EDS
	 *
	 * 
	 * @param size_in_firmware : Groesse in der Firmware
	 *
	 * 
	 */
void
hcan_transport::send_FIRMWARE_CONFIG_COMPAT_ERROR(const uint16_t src,
						  const uint16_t dst,
						  uint8_t eds_block_type,
						  uint8_t eds_addr_hi,
						  uint8_t eds_addr_lo,
						  uint8_t size_in_config,
						  uint8_t
						  size_in_firmware)
{
    uint8_t         data[] = {
	4,
	3,
	eds_block_type,
	eds_addr_hi,
	eds_addr_lo,
	size_in_config,
	size_in_firmware,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Firmware und Config sind nicht kompatibel
	 * 
	 * 
	 * @param eds_block_type : betroffener EDS Block-Typ
	 *
	 * 
	 * @param eds_addr_hi : EDS Adresse Hi
	 *
	 * 
	 * @param eds_addr_lo : EDS Adresse Lo
	 *
	 * 
	 * @param size_in_config : Groesse im EDS
	 *
	 * 
	 * @param size_in_firmware : Groesse in der Firmware
	 *
	 * 
	 */
void
hcan_transport::recv_FIRMWARE_CONFIG_COMPAT_ERROR(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * eds_block_type,
						  uint8_t * eds_addr_hi,
						  uint8_t * eds_addr_lo,
						  uint8_t * size_in_config,
						  uint8_t *
						  size_in_firmware)
{
    uint8_t         data[] = {
	4,
	3,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *eds_block_type = f.data(2);

    *eds_addr_hi = f.data(3);

    *eds_addr_lo = f.data(4);

    *size_in_config = f.data(5);

    *size_in_firmware = f.data(6);

}



	/**
	 * Geraet spuckt Debug-Infos aus
	 * 
	 * 
	 * @param context : 
	 *
	 * 
	 */
void
hcan_transport::send_DUMP_DEBUG_INFO(const uint16_t src,
				     const uint16_t dst, uint8_t context)
{
    uint8_t         data[] = {
	4,
	4,
	context,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Geraet spuckt Debug-Infos aus
	 * 
	 * 
	 * @param context : 
	 *
	 * 
	 */
void
hcan_transport::recv_DUMP_DEBUG_INFO(const uint16_t src,
				     const uint16_t dst,
				     uint8_t * context)
{
    uint8_t         data[] = {
	4,
	4,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *context = f.data(2);

}



	/**
	 * Sendet ein Taster beim Druecken; Power-Ports reagieren mit Toggle (an -> aus, aus -> an)
	 * 
	 * 
	 * @param gruppe : Taster-Gruppe, auf die der Taster konfiguriert ist
	 *
	 * 
	 * @param index : Index des Taster-Ports; nur noetig fuer Logging-Zwecke, falls Gruppe nicht eindeutig
	 *
	 * 
	 */
void
hcan_transport::send_TASTER_DOWN(const uint16_t src,
				 const uint16_t dst,
				 uint8_t gruppe, uint8_t index)
{
    uint8_t         data[] = {
	5,
	1,
	gruppe,
	index,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Sendet ein Taster beim Druecken; Power-Ports reagieren mit Toggle (an -> aus, aus -> an)
	 * 
	 * 
	 * @param gruppe : Taster-Gruppe, auf die der Taster konfiguriert ist
	 *
	 * 
	 * @param index : Index des Taster-Ports; nur noetig fuer Logging-Zwecke, falls Gruppe nicht eindeutig
	 *
	 * 
	 */
void
hcan_transport::recv_TASTER_DOWN(const uint16_t src,
				 const uint16_t dst,
				 uint8_t * gruppe, uint8_t * index)
{
    uint8_t         data[] = {
	5,
	1,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *index = f.data(3);

}



	/**
	 * Sendet ein Taster beim Druecken; Power-Ports reagieren mit Toggle (an -> aus, aus -> an)
	 * 
	 * 
	 * @param gruppe : Taster-Gruppe
	 *
	 * 
	 * @param index : Index des Taster-Ports
	 *
	 * 
	 */
void
hcan_transport::send_TASTER_UP(const uint16_t src,
			       const uint16_t dst,
			       uint8_t gruppe, uint8_t index)
{
    uint8_t         data[] = {
	5,
	2,
	gruppe,
	index,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Sendet ein Taster beim Druecken; Power-Ports reagieren mit Toggle (an -> aus, aus -> an)
	 * 
	 * 
	 * @param gruppe : Taster-Gruppe
	 *
	 * 
	 * @param index : Index des Taster-Ports
	 *
	 * 
	 */
void
hcan_transport::recv_TASTER_UP(const uint16_t src,
			       const uint16_t dst,
			       uint8_t * gruppe, uint8_t * index)
{
    uint8_t         data[] = {
	5,
	2,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *index = f.data(3);

}



	/**
	 * HCAN_HES_SCHALTER_STATE_QUERY kommt immer wenn ein C1612 gerade hochgelaufen ist (reboot)
	 * 
	 * 
	 */
void
hcan_transport::send_SCHALTER_STATE_QUERY(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	4,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * HCAN_HES_SCHALTER_STATE_QUERY kommt immer wenn ein C1612 gerade hochgelaufen ist (reboot)
	 * 
	 * 
	 */
void
hcan_transport::recv_SCHALTER_STATE_QUERY(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	4,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Reaktion: Power-Port(s) schalten sich an
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_ON(const uint16_t src,
				    const uint16_t dst, uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	10,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Reaktion: Power-Port(s) schalten sich an
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_ON(const uint16_t src,
				    const uint16_t dst, uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	10,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Power-Port(s) schalten sich aus
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_OFF(const uint16_t src,
				     const uint16_t dst, uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	11,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Power-Port(s) schalten sich aus
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_OFF(const uint16_t src,
				     const uint16_t dst, uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	11,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Abfrage des Status eines oder mehrerer Power-Ports (ob an oder aus)
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_STATE_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	12,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Abfrage des Status eines oder mehrerer Power-Ports (ob an oder aus)
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_STATE_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	12,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 12)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Antwort eines Power-Ports mit dem aktuellen Status
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param status : 1 = an, 0 = aus
	 *
	 * 
	 * @param timer : dekrementiert
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_STATE_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t status,
					      uint8_t timer)
{
    uint8_t         data[] = {
	5,
	13,
	gruppe,
	status,
	timer,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Antwort eines Power-Ports mit dem aktuellen Status
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param status : 1 = an, 0 = aus
	 *
	 * 
	 * @param timer : dekrementiert
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_STATE_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * status,
					      uint8_t * timer)
{
    uint8_t         data[] = {
	5,
	13,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 13)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *status = f.data(3);

    *timer = f.data(4);

}



	/**
	 * Anfrage an eine Lichtzone o.ae. nach der Timer Stand 
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_TIMER_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	14,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anfrage an eine Lichtzone o.ae. nach der Timer Stand 
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_TIMER_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	14,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 14)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Antwort einer Lichtzone mit dem Timer Stand 
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param timer_hi : Timer-Stand, High Byte
	 *
	 * 
	 * @param timer_lo : Timer-Stand, Low Byte
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_TIMER_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t timer_hi,
					      uint8_t timer_lo)
{
    uint8_t         data[] = {
	5,
	15,
	gruppe,
	timer_hi,
	timer_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Antwort einer Lichtzone mit dem Timer Stand 
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param timer_hi : Timer-Stand, High Byte
	 *
	 * 
	 * @param timer_lo : Timer-Stand, Low Byte
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_TIMER_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * timer_hi,
					      uint8_t * timer_lo)
{
    uint8_t         data[] = {
	5,
	15,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 15)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *timer_hi = f.data(3);

    *timer_lo = f.data(4);

}



	/**
	 * Setzt den aktuellen Timer-Stand einer Lichtzone
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param timer_hi : Timer-Stand, High Byte
	 *
	 * 
	 * @param timer_lo : Timer-Stand, Low Byte
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_SET_TIMER(const uint16_t src,
					   const uint16_t dst,
					   uint8_t gruppe,
					   uint8_t timer_hi,
					   uint8_t timer_lo)
{
    uint8_t         data[] = {
	5,
	16,
	gruppe,
	timer_hi,
	timer_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Setzt den aktuellen Timer-Stand einer Lichtzone
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param timer_hi : Timer-Stand, High Byte
	 *
	 * 
	 * @param timer_lo : Timer-Stand, Low Byte
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_SET_TIMER(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * gruppe,
					   uint8_t * timer_hi,
					   uint8_t * timer_lo)
{
    uint8_t         data[] = {
	5,
	16,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 16)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *timer_hi = f.data(3);

    *timer_lo = f.data(4);

}



	/**
	 * Power-Port meldet nach Aenderung seinen Status
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param status : 1 = an, 0 = aus
	 *
	 * 
	 */
void
hcan_transport::send_POWER_GROUP_STATE_INFO(const uint16_t src,
					    const uint16_t dst,
					    uint8_t gruppe,
					    uint8_t status)
{
    uint8_t         data[] = {
	5,
	17,
	gruppe,
	status,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Power-Port meldet nach Aenderung seinen Status
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param status : 1 = an, 0 = aus
	 *
	 * 
	 */
void
hcan_transport::recv_POWER_GROUP_STATE_INFO(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * gruppe,
					    uint8_t * status)
{
    uint8_t         data[] = {
	5,
	17,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 17)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *status = f.data(3);

}



	/**
	 * Faehrt einen Rolladen auf die gegebene Position
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 2-Taster: 200=0%, 201=100%; 100% = oben
	 *
	 * 
	 */
void
hcan_transport::send_ROLLADEN_POSITION_SET(const uint16_t src,
					   const uint16_t dst,
					   uint8_t gruppe,
					   uint8_t position)
{
    uint8_t         data[] = {
	5,
	20,
	gruppe,
	position,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Faehrt einen Rolladen auf die gegebene Position
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 2-Taster: 200=0%, 201=100%; 100% = oben
	 *
	 * 
	 */
void
hcan_transport::recv_ROLLADEN_POSITION_SET(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * gruppe,
					   uint8_t * position)
{
    uint8_t         data[] = {
	5,
	20,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 20)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *position = f.data(3);

}



	/**
	 * Fragt einen Rolladen nach der aktuellen Position
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_ROLLADEN_POSITION_REQUEST(const uint16_t src,
					       const uint16_t dst,
					       uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	21,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt einen Rolladen nach der aktuellen Position
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_ROLLADEN_POSITION_REQUEST(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	21,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 21)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Der Rolladen antwortet mit der aktuellen Position
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 100% = oben
	 *
	 * 
	 */
void
hcan_transport::send_ROLLADEN_POSITION_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t position)
{
    uint8_t         data[] = {
	5,
	22,
	gruppe,
	position,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Der Rolladen antwortet mit der aktuellen Position
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 100% = oben
	 *
	 * 
	 */
void
hcan_transport::recv_ROLLADEN_POSITION_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * position)
{
    uint8_t         data[] = {
	5,
	22,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 22)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *position = f.data(3);

}



	/**
	 * Definiert die aktuelle Position eines Rolladen (!)
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 100% = oben
	 *
	 * 
	 */
void
hcan_transport::send_ROLLADEN_DEFINE_POSITION(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t position)
{
    uint8_t         data[] = {
	5,
	23,
	gruppe,
	position,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Definiert die aktuelle Position eines Rolladen (!)
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 100% = oben
	 *
	 * 
	 */
void
hcan_transport::recv_ROLLADEN_DEFINE_POSITION(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * position)
{
    uint8_t         data[] = {
	5,
	23,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 23)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *position = f.data(3);

}



	/**
	 * Besagt, dass sich die Rolladen-Position geaendert hat
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 100% = oben
	 *
	 * 
	 * @param cmdsrc_hi : Source-Adresse des Befehls (0 = manuell ueber Taster), MSB
	 *
	 * 
	 * @param cmdsrc_lo : LSB
	 *
	 * 
	 */
void
hcan_transport::send_ROLLADEN_POSITION_CHANGED_INFO(const uint16_t src,
						    const uint16_t dst,
						    uint8_t gruppe,
						    uint8_t position,
						    uint8_t cmdsrc_hi,
						    uint8_t cmdsrc_lo)
{
    uint8_t         data[] = {
	5,
	24,
	gruppe,
	position,
	cmdsrc_hi,
	cmdsrc_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Besagt, dass sich die Rolladen-Position geaendert hat
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param position : Position im Bereich [0..100]; 100% = oben
	 *
	 * 
	 * @param cmdsrc_hi : Source-Adresse des Befehls (0 = manuell ueber Taster), MSB
	 *
	 * 
	 * @param cmdsrc_lo : LSB
	 *
	 * 
	 */
void
hcan_transport::recv_ROLLADEN_POSITION_CHANGED_INFO(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * gruppe,
						    uint8_t * position,
						    uint8_t * cmdsrc_hi,
						    uint8_t * cmdsrc_lo)
{
    uint8_t         data[] = {
	5,
	24,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 24)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *position = f.data(3);

    *cmdsrc_hi = f.data(4);

    *cmdsrc_lo = f.data(5);

}



	/**
	 * Veranlasst einen 1Wire Scan an gebenen Pin
	 * 
	 * 
	 * @param pin : Pin ID des 1Wire Ports im Bereich [0..7]
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_DISCOVER(const uint16_t src,
				    const uint16_t dst, uint8_t pin)
{
    uint8_t         data[] = {
	5,
	30,
	pin,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Veranlasst einen 1Wire Scan an gebenen Pin
	 * 
	 * 
	 * @param pin : Pin ID des 1Wire Ports im Bereich [0..7]
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_DISCOVER(const uint16_t src,
				    const uint16_t dst, uint8_t * pin)
{
    uint8_t         data[] = {
	5,
	30,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 30)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *pin = f.data(2);

}



	/**
	 * ein 1Wire Device ist gefunden worden, Message Teil 1
	 * 
	 * 
	 * @param id0 : 1. Byte der 1Wire id[8] (Checksum dabei)
	 *
	 * 
	 * @param id1 : 2. Byte
	 *
	 * 
	 * @param id2 : 3. Byte
	 *
	 * 
	 * @param id3 : 4. Byte
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_DISCOVERED_PART_1(const uint16_t src,
					     const uint16_t dst,
					     uint8_t id0,
					     uint8_t id1,
					     uint8_t id2, uint8_t id3)
{
    uint8_t         data[] = {
	5,
	31,
	id0,
	id1,
	id2,
	id3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * ein 1Wire Device ist gefunden worden, Message Teil 1
	 * 
	 * 
	 * @param id0 : 1. Byte der 1Wire id[8] (Checksum dabei)
	 *
	 * 
	 * @param id1 : 2. Byte
	 *
	 * 
	 * @param id2 : 3. Byte
	 *
	 * 
	 * @param id3 : 4. Byte
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_DISCOVERED_PART_1(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * id0,
					     uint8_t * id1,
					     uint8_t * id2, uint8_t * id3)
{
    uint8_t         data[] = {
	5,
	31,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 31)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id0 = f.data(2);

    *id1 = f.data(3);

    *id2 = f.data(4);

    *id3 = f.data(5);

}



	/**
	 * ein 1Wire Device ist gefunden worden, Message Teil 1
	 * 
	 * 
	 * @param id4 : 5. Byte der 1Wire id[8] (Checksum dabei)
	 *
	 * 
	 * @param id5 : 6. Byte
	 *
	 * 
	 * @param id6 : 7. Byte
	 *
	 * 
	 * @param id7 : 8. Byte
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_DISCOVERED_PART_2(const uint16_t src,
					     const uint16_t dst,
					     uint8_t id4,
					     uint8_t id5,
					     uint8_t id6, uint8_t id7)
{
    uint8_t         data[] = {
	5,
	32,
	id4,
	id5,
	id6,
	id7,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * ein 1Wire Device ist gefunden worden, Message Teil 1
	 * 
	 * 
	 * @param id4 : 5. Byte der 1Wire id[8] (Checksum dabei)
	 *
	 * 
	 * @param id5 : 6. Byte
	 *
	 * 
	 * @param id6 : 7. Byte
	 *
	 * 
	 * @param id7 : 8. Byte
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_DISCOVERED_PART_2(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * id4,
					     uint8_t * id5,
					     uint8_t * id6, uint8_t * id7)
{
    uint8_t         data[] = {
	5,
	32,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 32)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id4 = f.data(2);

    *id5 = f.data(3);

    *id6 = f.data(4);

    *id7 = f.data(5);

}



	/**
	 * ein 1Wire Fehler trat auf
	 * 
	 * 
	 * @param error : 3 = NoSensorFound, 4 = BusError
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_ERROR(const uint16_t src,
				 const uint16_t dst, uint8_t error)
{
    uint8_t         data[] = {
	5,
	33,
	error,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * ein 1Wire Fehler trat auf
	 * 
	 * 
	 * @param error : 3 = NoSensorFound, 4 = BusError
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_ERROR(const uint16_t src,
				 const uint16_t dst, uint8_t * error)
{
    uint8_t         data[] = {
	5,
	33,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 33)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *error = f.data(2);

}



	/**
	 * Messergebnis (Fixed Point) eines DS18B20 1Wire Temperaturfuehlers
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param temp_hi : MSB
	 *
	 * 
	 * @param temp_lo : LSB; 4 LSB sind Nachkommabits
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_TEMPERATURE(const uint16_t src,
				       const uint16_t dst,
				       uint8_t gruppe,
				       uint8_t temp_hi, uint8_t temp_lo)
{
    uint8_t         data[] = {
	5,
	34,
	gruppe,
	temp_hi,
	temp_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Messergebnis (Fixed Point) eines DS18B20 1Wire Temperaturfuehlers
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param temp_hi : MSB
	 *
	 * 
	 * @param temp_lo : LSB; 4 LSB sind Nachkommabits
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_TEMPERATURE(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * gruppe,
				       uint8_t * temp_hi,
				       uint8_t * temp_lo)
{
    uint8_t         data[] = {
	5,
	34,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 34)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *temp_hi = f.data(3);

    *temp_lo = f.data(4);

}



	/**
	 * fragt einen Temperatursensor nach der aktuellen Temperatur
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_TEMPERATURE_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	35,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * fragt einen Temperatursensor nach der aktuellen Temperatur
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_TEMPERATURE_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	35,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 35)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Messergebnis (Fixed Point) eines DS18B20 1Wire Temperaturfuehlers als Antwort auf 1WIRE_TEMPERATURE_QUERY
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param temp_hi : MSB
	 *
	 * 
	 * @param temp_lo : LSB; 4 LSB sind Nachkommabits
	 *
	 * 
	 */
void
hcan_transport::send_1WIRE_TEMPERATURE_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t temp_hi,
					      uint8_t temp_lo)
{
    uint8_t         data[] = {
	5,
	36,
	gruppe,
	temp_hi,
	temp_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Messergebnis (Fixed Point) eines DS18B20 1Wire Temperaturfuehlers als Antwort auf 1WIRE_TEMPERATURE_QUERY
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param temp_hi : MSB
	 *
	 * 
	 * @param temp_lo : LSB; 4 LSB sind Nachkommabits
	 *
	 * 
	 */
void
hcan_transport::recv_1WIRE_TEMPERATURE_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * temp_hi,
					      uint8_t * temp_lo)
{
    uint8_t         data[] = {
	5,
	36,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 36)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *temp_hi = f.data(3);

    *temp_lo = f.data(4);

}



	/**
	 * Meldung fuer einen offenen Reedkontakt an Fenster oder Tuer
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_REEDKONTAKT_OFFEN(const uint16_t src,
				       const uint16_t dst, uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	40,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldung fuer einen offenen Reedkontakt an Fenster oder Tuer
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_REEDKONTAKT_OFFEN(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	40,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 40)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Fragt nach dem Status eines Reedkontakts
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_REEDKONTAKT_STATE_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	41,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach dem Status eines Reedkontakts
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_REEDKONTAKT_STATE_QUERY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	41,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 41)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Status Antwort eines Reedkontakts
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = zu, 1 = offen
	 *
	 * 
	 */
void
hcan_transport::send_REEDKONTAKT_STATE_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t state)
{
    uint8_t         data[] = {
	5,
	42,
	gruppe,
	state,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Status Antwort eines Reedkontakts
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = zu, 1 = offen
	 *
	 * 
	 */
void
hcan_transport::recv_REEDKONTAKT_STATE_REPLAY(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * state)
{
    uint8_t         data[] = {
	5,
	42,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 42)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *state = f.data(3);

}



	/**
	 * Zustandsaenderung eines Reedkontakts
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = zu, 1 = offen
	 *
	 * 
	 */
void
hcan_transport::send_REEDKONTAKT_STATE_CHANGE(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe,
					      uint8_t state)
{
    uint8_t         data[] = {
	5,
	43,
	gruppe,
	state,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Zustandsaenderung eines Reedkontakts
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = zu, 1 = offen
	 *
	 * 
	 */
void
hcan_transport::recv_REEDKONTAKT_STATE_CHANGE(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe,
					      uint8_t * state)
{
    uint8_t         data[] = {
	5,
	43,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 43)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *state = f.data(3);

}



	/**
	 * Zustandsaenderung eines PV Sensors
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::send_PVSENSOR_STATE_REQUEST(const uint16_t src,
					    const uint16_t dst,
					    uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	44,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Zustandsaenderung eines PV Sensors
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 */
void
hcan_transport::recv_PVSENSOR_STATE_REQUEST(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	44,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 44)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Zustandsaenderung eines PV Sensors
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = 0V, 1 = 230V
	 *
	 * 
	 */
void
hcan_transport::send_PVSENSOR_STATE_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t gruppe, uint8_t state)
{
    uint8_t         data[] = {
	5,
	45,
	gruppe,
	state,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Zustandsaenderung eines PV Sensors
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = 0V, 1 = 230V
	 *
	 * 
	 */
void
hcan_transport::recv_PVSENSOR_STATE_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * gruppe,
					   uint8_t * state)
{
    uint8_t         data[] = {
	5,
	45,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 45)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *state = f.data(3);

}



	/**
	 * Zustandsaenderung eines PV Sensors
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = 0V, 1 = 230V
	 *
	 * 
	 */
void
hcan_transport::send_PVSENSOR_STATE_CHANGE_INFO(const uint16_t src,
						const uint16_t dst,
						uint8_t gruppe,
						uint8_t state)
{
    uint8_t         data[] = {
	5,
	46,
	gruppe,
	state,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Zustandsaenderung eines PV Sensors
	 * 
	 * 
	 * @param gruppe : Gruppe
	 *
	 * 
	 * @param state : Zustand: 0 = 0V, 1 = 230V
	 *
	 * 
	 */
void
hcan_transport::recv_PVSENSOR_STATE_CHANGE_INFO(const uint16_t src,
						const uint16_t dst,
						uint8_t * gruppe,
						uint8_t * state)
{
    uint8_t         data[] = {
	5,
	46,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 46)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *state = f.data(3);

}



	/**
	 * UserPanel fragt nach Heizungsdetails
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_DETAILS_REQUEST(const uint16_t src,
					     const uint16_t dst,
					     uint8_t id)
{
    uint8_t         data[] = {
	5,
	50,
	id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * UserPanel fragt nach Heizungsdetails
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_DETAILS_REQUEST(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * id)
{
    uint8_t         data[] = {
	5,
	50,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 50)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

}



	/**
	 * Heizung ist aus (Off-Mode)
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_MODE_OFF_DETAILS(const uint16_t src,
					      const uint16_t dst,
					      uint8_t id)
{
    uint8_t         data[] = {
	5,
	51,
	id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Heizung ist aus (Off-Mode)
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_MODE_OFF_DETAILS(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * id)
{
    uint8_t         data[] = {
	5,
	51,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 51)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

}



	/**
	 * Heizung schaltet aus
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SET_MODE_OFF(const uint16_t src,
					  const uint16_t dst, uint8_t id)
{
    uint8_t         data[] = {
	5,
	52,
	id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Heizung schaltet aus
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SET_MODE_OFF(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * id)
{
    uint8_t         data[] = {
	5,
	52,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 52)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

}



	/**
	 * Manueller Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param rate : Heiz-Rate
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_MODE_MANUAL_DETAILS(const uint16_t src,
						 const uint16_t dst,
						 uint8_t id,
						 uint8_t rate,
						 uint8_t duration_hi,
						 uint8_t duration_lo)
{
    uint8_t         data[] = {
	5,
	53,
	id,
	rate,
	duration_hi,
	duration_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Manueller Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param rate : Heiz-Rate
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_MODE_MANUAL_DETAILS(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * id,
						 uint8_t * rate,
						 uint8_t * duration_hi,
						 uint8_t * duration_lo)
{
    uint8_t         data[] = {
	5,
	53,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 53)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *rate = f.data(3);

    *duration_hi = f.data(4);

    *duration_lo = f.data(5);

}



	/**
	 * Manueller Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param rate : Heiz-Rate
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SET_MODE_MANUAL(const uint16_t src,
					     const uint16_t dst,
					     uint8_t id,
					     uint8_t rate,
					     uint8_t duration_hi,
					     uint8_t duration_lo)
{
    uint8_t         data[] = {
	5,
	54,
	id,
	rate,
	duration_hi,
	duration_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Manueller Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param rate : Heiz-Rate
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SET_MODE_MANUAL(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * id,
					     uint8_t * rate,
					     uint8_t * duration_hi,
					     uint8_t * duration_lo)
{
    uint8_t         data[] = {
	5,
	54,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 54)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *rate = f.data(3);

    *duration_hi = f.data(4);

    *duration_lo = f.data(5);

}



	/**
	 * Thermostat Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_MODE_THERMOSTAT_DETAILS(const uint16_t src,
						     const uint16_t dst,
						     uint8_t id,
						     uint8_t temp_hi,
						     uint8_t temp_lo,
						     uint8_t duration_hi,
						     uint8_t duration_lo)
{
    uint8_t         data[] = {
	5,
	55,
	id,
	temp_hi,
	temp_lo,
	duration_hi,
	duration_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Thermostat Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_MODE_THERMOSTAT_DETAILS(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * id,
						     uint8_t * temp_hi,
						     uint8_t * temp_lo,
						     uint8_t * duration_hi,
						     uint8_t *
						     duration_lo)
{
    uint8_t         data[] = {
	5,
	55,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 55)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *temp_hi = f.data(3);

    *temp_lo = f.data(4);

    *duration_hi = f.data(5);

    *duration_lo = f.data(6);

}



	/**
	 * Set Thermostat Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::
send_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS(const uint16_t src,
					 const uint16_t dst, uint8_t id,
					 uint8_t temp_hi, uint8_t temp_lo,
					 uint8_t duration_hi,
					 uint8_t duration_lo)
{
    uint8_t         data[] = {
	5,
	56,
	id,
	temp_hi,
	temp_lo,
	duration_hi,
	duration_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Set Thermostat Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param duration_hi : Restdauer (MSB), 0 = unbegrenzt
	 *
	 * 
	 * @param duration_lo : Restdauer (LSB)
	 *
	 * 
	 */
void
hcan_transport::
recv_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS(const uint16_t src,
					 const uint16_t dst, uint8_t * id,
					 uint8_t * temp_hi,
					 uint8_t * temp_lo,
					 uint8_t * duration_hi,
					 uint8_t * duration_lo)
{
    uint8_t         data[] = {
	5,
	56,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 56)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *temp_hi = f.data(3);

    *temp_lo = f.data(4);

    *duration_hi = f.data(5);

    *duration_lo = f.data(6);

}



	/**
	 * Automatik Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param timerange_id : Zeitbereichs-ID
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_MODE_AUTOMATIK_DETAILS(const uint16_t src,
						    const uint16_t dst,
						    uint8_t id,
						    uint8_t temp_hi,
						    uint8_t temp_lo,
						    uint8_t timerange_id)
{
    uint8_t         data[] = {
	5,
	57,
	id,
	temp_hi,
	temp_lo,
	timerange_id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Automatik Modus incl Parameter
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param timerange_id : Zeitbereichs-ID
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_MODE_AUTOMATIK_DETAILS(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * id,
						    uint8_t * temp_hi,
						    uint8_t * temp_lo,
						    uint8_t *
						    timerange_id)
{
    uint8_t         data[] = {
	5,
	57,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 57)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *temp_hi = f.data(3);

    *temp_lo = f.data(4);

    *timerange_id = f.data(5);

}



	/**
	 * Set Automatik Modus
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SET_MODE_AUTOMATIK(const uint16_t src,
						const uint16_t dst,
						uint8_t id)
{
    uint8_t         data[] = {
	5,
	58,
	id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Set Automatik Modus
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SET_MODE_AUTOMATIK(const uint16_t src,
						const uint16_t dst,
						uint8_t * id)
{
    uint8_t         data[] = {
	5,
	58,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 58)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

}



	/**
	 * UserPanel fragt nach T(ist)
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_TIST_REQUEST(const uint16_t src,
					  const uint16_t dst, uint8_t id)
{
    uint8_t         data[] = {
	5,
	59,
	id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * UserPanel fragt nach T(ist)
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_TIST_REQUEST(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * id)
{
    uint8_t         data[] = {
	5,
	59,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 59)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

}



	/**
	 * Controllerboard liefert T(ist)
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Ist-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Ist-Temperatur (LSB)
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_TIST_REPLAY(const uint16_t src,
					 const uint16_t dst,
					 uint8_t id,
					 uint8_t temp_hi, uint8_t temp_lo)
{
    uint8_t         data[] = {
	5,
	60,
	id,
	temp_hi,
	temp_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Controllerboard liefert T(ist)
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param temp_hi : Ist-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Ist-Temperatur (LSB)
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_TIST_REPLAY(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * id,
					 uint8_t * temp_hi,
					 uint8_t * temp_lo)
{
    uint8_t         data[] = {
	5,
	60,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 60)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *temp_hi = f.data(3);

    *temp_lo = f.data(4);

}



	/**
	 * Heizung braucht Waerme oder nicht
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param value : [0..100]: 0 = kein Bedarf
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_WAERMEBEDARF_INFO(const uint16_t src,
					       const uint16_t dst,
					       uint8_t id, uint8_t value)
{
    uint8_t         data[] = {
	5,
	61,
	id,
	value,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Heizung braucht Waerme oder nicht
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param value : [0..100]: 0 = kein Bedarf
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_WAERMEBEDARF_INFO(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * id,
					       uint8_t * value)
{
    uint8_t         data[] = {
	5,
	61,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 61)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *value = f.data(3);

}



	/**
	 * Fragt die Heizungssteuerung nach ihrem Stand
	 * 
	 * 
	 */
void
hcan_transport::send_HEIZSTEUERUNG_STATE_REQUEST(const uint16_t src,
						 const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	62,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt die Heizungssteuerung nach ihrem Stand
	 * 
	 * 
	 */
void
hcan_transport::recv_HEIZSTEUERUNG_STATE_REQUEST(const uint16_t src,
						 const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	62,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 62)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * die (einzige) Heizungssteuerung antwortet
	 * 
	 * 
	 * @param VL_soll : Vorlauf-Soll
	 *
	 * 
	 * @param VL_ist : Vorlauf-Ist
	 *
	 * 
	 * @param RL_ist : Ruecklauf-Ist
	 *
	 * 
	 * @param BF : Brennerfreigabe
	 *
	 * 
	 */
void
hcan_transport::send_HEIZSTEUERUNG_STATE_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t VL_soll,
						uint8_t VL_ist,
						uint8_t RL_ist,
						uint8_t BF)
{
    uint8_t         data[] = {
	5,
	63,
	VL_soll,
	VL_ist,
	RL_ist,
	BF,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * die (einzige) Heizungssteuerung antwortet
	 * 
	 * 
	 * @param VL_soll : Vorlauf-Soll
	 *
	 * 
	 * @param VL_ist : Vorlauf-Ist
	 *
	 * 
	 * @param RL_ist : Ruecklauf-Ist
	 *
	 * 
	 * @param BF : Brennerfreigabe
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZSTEUERUNG_STATE_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * VL_soll,
						uint8_t * VL_ist,
						uint8_t * RL_ist,
						uint8_t * BF)
{
    uint8_t         data[] = {
	5,
	63,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 63)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *VL_soll = f.data(2);

    *VL_ist = f.data(3);

    *RL_ist = f.data(4);

    *BF = f.data(5);

}



	/**
	 * Heizungssteuerung liefert den aktuellen Status
	 * 
	 * 
	 * @param VL_soll : Vorlauf-Soll
	 *
	 * 
	 * @param GWB_hi : Gesamt-Waermebedarf
	 *
	 * 
	 * @param GWB_lo : Gesamt-Waermebedarf
	 *
	 * 
	 * @param BF : Brennerfreigabe
	 *
	 * 
	 */
void
hcan_transport::send_HEIZSTEUERUNG_STATE_INFO(const uint16_t src,
					      const uint16_t dst,
					      uint8_t VL_soll,
					      uint8_t GWB_hi,
					      uint8_t GWB_lo, uint8_t BF)
{
    uint8_t         data[] = {
	5,
	64,
	VL_soll,
	GWB_hi,
	GWB_lo,
	BF,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Heizungssteuerung liefert den aktuellen Status
	 * 
	 * 
	 * @param VL_soll : Vorlauf-Soll
	 *
	 * 
	 * @param GWB_hi : Gesamt-Waermebedarf
	 *
	 * 
	 * @param GWB_lo : Gesamt-Waermebedarf
	 *
	 * 
	 * @param BF : Brennerfreigabe
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZSTEUERUNG_STATE_INFO(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * VL_soll,
					      uint8_t * GWB_hi,
					      uint8_t * GWB_lo,
					      uint8_t * BF)
{
    uint8_t         data[] = {
	5,
	64,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 64)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *VL_soll = f.data(2);

    *GWB_hi = f.data(3);

    *GWB_lo = f.data(4);

    *BF = f.data(5);

}



	/**
	 * gibt die Soll-VL Temperatur vor
	 * 
	 * 
	 * @param VL_soll : Vorlauf-Soll
	 *
	 * 
	 */
void
hcan_transport::send_HEIZSTEUERUNG_SET_VL(const uint16_t src,
					  const uint16_t dst,
					  uint8_t VL_soll)
{
    uint8_t         data[] = {
	5,
	65,
	VL_soll,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * gibt die Soll-VL Temperatur vor
	 * 
	 * 
	 * @param VL_soll : Vorlauf-Soll
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZSTEUERUNG_SET_VL(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * VL_soll)
{
    uint8_t         data[] = {
	5,
	65,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 65)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *VL_soll = f.data(2);

}



	/**
	 * Fragt nach einer Solltemp/Zeitzonen Config Zeile
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile [0..7]
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SOLLTEMP_LINE_REQUEST(const uint16_t src,
						   const uint16_t dst,
						   uint8_t id,
						   uint8_t idx)
{
    uint8_t         data[] = {
	5,
	66,
	id,
	idx,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach einer Solltemp/Zeitzonen Config Zeile
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile [0..7]
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SOLLTEMP_LINE_REQUEST(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * id,
						   uint8_t * idx)
{
    uint8_t         data[] = {
	5,
	66,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 66)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *idx = f.data(3);

}



	/**
	 * Controllerboard liefert Zeile
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param zeitzone_id : ID der Zeitzone
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SOLLTEMP_LINE_REPLAY(const uint16_t src,
						  const uint16_t dst,
						  uint8_t id,
						  uint8_t idx,
						  uint8_t temp_hi,
						  uint8_t temp_lo,
						  uint8_t zeitzone_id)
{
    uint8_t         data[] = {
	5,
	67,
	id,
	idx,
	temp_hi,
	temp_lo,
	zeitzone_id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Controllerboard liefert Zeile
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param zeitzone_id : ID der Zeitzone
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SOLLTEMP_LINE_REPLAY(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * id,
						  uint8_t * idx,
						  uint8_t * temp_hi,
						  uint8_t * temp_lo,
						  uint8_t * zeitzone_id)
{
    uint8_t         data[] = {
	5,
	67,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 67)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *idx = f.data(3);

    *temp_hi = f.data(4);

    *temp_lo = f.data(5);

    *zeitzone_id = f.data(6);

}



	/**
	 * schreibt neue Solltemp Zeile: ACHTUNG: zeitzone_id wird bisher NICHT geschrieben!
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param zeitzone_id : ID der Zeitzone
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SOLLTEMP_WRITE_LINE(const uint16_t src,
						 const uint16_t dst,
						 uint8_t id,
						 uint8_t idx,
						 uint8_t temp_hi,
						 uint8_t temp_lo,
						 uint8_t zeitzone_id)
{
    uint8_t         data[] = {
	5,
	68,
	id,
	idx,
	temp_hi,
	temp_lo,
	zeitzone_id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * schreibt neue Solltemp Zeile: ACHTUNG: zeitzone_id wird bisher NICHT geschrieben!
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile
	 *
	 * 
	 * @param temp_hi : Soll-Temperatur (MSB)
	 *
	 * 
	 * @param temp_lo : Soll-Temperatur (LSB)
	 *
	 * 
	 * @param zeitzone_id : ID der Zeitzone
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SOLLTEMP_WRITE_LINE(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * id,
						 uint8_t * idx,
						 uint8_t * temp_hi,
						 uint8_t * temp_lo,
						 uint8_t * zeitzone_id)
{
    uint8_t         data[] = {
	5,
	68,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 68)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *idx = f.data(3);

    *temp_hi = f.data(4);

    *temp_lo = f.data(5);

    *zeitzone_id = f.data(6);

}



	/**
	 * schreibt neue Solltemp Zeile
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile
	 *
	 * 
	 */
void
hcan_transport::send_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK(const uint16_t src,
						     const uint16_t dst,
						     uint8_t id,
						     uint8_t idx)
{
    uint8_t         data[] = {
	5,
	69,
	id,
	idx,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * schreibt neue Solltemp Zeile
	 * 
	 * 
	 * @param id : Heizungs-ID
	 *
	 * 
	 * @param idx : Index der Config Zeile
	 *
	 * 
	 */
void
hcan_transport::recv_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * id,
						     uint8_t * idx)
{
    uint8_t         data[] = {
	5,
	69,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 69)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *idx = f.data(3);

}



	/**
	 * Wecker incl Parameter
	 * 
	 * 
	 * @param id : Wecker-ID
	 *
	 * 
	 * @param duration_hi : Timer in x Sekunden (MSB)
	 *
	 * 
	 * @param duration_lo : Timer in x Sekunden (LSB)
	 *
	 * 
	 */
void
hcan_transport::send_WECKER_SET(const uint16_t src,
				const uint16_t dst,
				uint8_t id,
				uint8_t duration_hi, uint8_t duration_lo)
{
    uint8_t         data[] = {
	5,
	72,
	id,
	duration_hi,
	duration_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Wecker incl Parameter
	 * 
	 * 
	 * @param id : Wecker-ID
	 *
	 * 
	 * @param duration_hi : Timer in x Sekunden (MSB)
	 *
	 * 
	 * @param duration_lo : Timer in x Sekunden (LSB)
	 *
	 * 
	 */
void
hcan_transport::recv_WECKER_SET(const uint16_t src,
				const uint16_t dst,
				uint8_t * id,
				uint8_t * duration_hi,
				uint8_t * duration_lo)
{
    uint8_t         data[] = {
	5,
	72,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 72)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *duration_hi = f.data(3);

    *duration_lo = f.data(4);

}



	/**
	 * Wecker incl Parameter
	 * 
	 * 
	 * @param id : Wecker-ID
	 *
	 * 
	 */
void
hcan_transport::send_WECKER_DETAILS_REQUEST(const uint16_t src,
					    const uint16_t dst,
					    uint8_t id)
{
    uint8_t         data[] = {
	5,
	73,
	id,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Wecker incl Parameter
	 * 
	 * 
	 * @param id : Wecker-ID
	 *
	 * 
	 */
void
hcan_transport::recv_WECKER_DETAILS_REQUEST(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * id)
{
    uint8_t         data[] = {
	5,
	73,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 73)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

}



	/**
	 * Wecker incl Parameter
	 * 
	 * 
	 * @param id : Wecker-ID
	 *
	 * 
	 * @param duration_hi : Timer in x Sekunden (MSB)
	 *
	 * 
	 * @param duration_lo : Timer in x Sekunden (LSB)
	 *
	 * 
	 */
void
hcan_transport::send_WECKER_DETAILS(const uint16_t src,
				    const uint16_t dst,
				    uint8_t id,
				    uint8_t duration_hi,
				    uint8_t duration_lo)
{
    uint8_t         data[] = {
	5,
	74,
	id,
	duration_hi,
	duration_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Wecker incl Parameter
	 * 
	 * 
	 * @param id : Wecker-ID
	 *
	 * 
	 * @param duration_hi : Timer in x Sekunden (MSB)
	 *
	 * 
	 * @param duration_lo : Timer in x Sekunden (LSB)
	 *
	 * 
	 */
void
hcan_transport::recv_WECKER_DETAILS(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * id,
				    uint8_t * duration_hi,
				    uint8_t * duration_lo)
{
    uint8_t         data[] = {
	5,
	74,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 74)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *duration_hi = f.data(3);

    *duration_lo = f.data(4);

}



	/**
	 * Liefert die Helligkeit am ADC (Aufloesung: 10Bit)
	 * 
	 * 
	 * @param guppe : Helligkeits-Gruppe
	 *
	 * 
	 * @param brightness_hi : hi byte der Helligkeit
	 *
	 * 
	 * @param brightness_lo : lo hyte der Helligkeit
	 *
	 * 
	 */
void
hcan_transport::send_HELLIGKEITS_INFO(const uint16_t src,
				      const uint16_t dst,
				      uint8_t guppe,
				      uint8_t brightness_hi,
				      uint8_t brightness_lo)
{
    uint8_t         data[] = {
	5,
	71,
	guppe,
	brightness_hi,
	brightness_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Helligkeit am ADC (Aufloesung: 10Bit)
	 * 
	 * 
	 * @param guppe : Helligkeits-Gruppe
	 *
	 * 
	 * @param brightness_hi : hi byte der Helligkeit
	 *
	 * 
	 * @param brightness_lo : lo hyte der Helligkeit
	 *
	 * 
	 */
void
hcan_transport::recv_HELLIGKEITS_INFO(const uint16_t src,
				      const uint16_t dst,
				      uint8_t * guppe,
				      uint8_t * brightness_hi,
				      uint8_t * brightness_lo)
{
    uint8_t         data[] = {
	5,
	71,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 71)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *guppe = f.data(2);

    *brightness_hi = f.data(3);

    *brightness_lo = f.data(4);

}



	/**
	 * Liefert die Position des Potis (Aufloesung: 10Bit)
	 * 
	 * 
	 * @param guppe : Poti-Gruppe
	 *
	 * 
	 * @param pos_hi : hi byte der Position
	 *
	 * 
	 * @param pos_lo : lo hyte der Position
	 *
	 * 
	 */
void
hcan_transport::send_POTI_POS_CHANGED(const uint16_t src,
				      const uint16_t dst,
				      uint8_t guppe,
				      uint8_t pos_hi, uint8_t pos_lo)
{
    uint8_t         data[] = {
	5,
	70,
	guppe,
	pos_hi,
	pos_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Position des Potis (Aufloesung: 10Bit)
	 * 
	 * 
	 * @param guppe : Poti-Gruppe
	 *
	 * 
	 * @param pos_hi : hi byte der Position
	 *
	 * 
	 * @param pos_lo : lo hyte der Position
	 *
	 * 
	 */
void
hcan_transport::recv_POTI_POS_CHANGED(const uint16_t src,
				      const uint16_t dst,
				      uint8_t * guppe,
				      uint8_t * pos_hi, uint8_t * pos_lo)
{
    uint8_t         data[] = {
	5,
	70,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 70)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *guppe = f.data(2);

    *pos_hi = f.data(3);

    *pos_lo = f.data(4);

}



	/**
	 * Meldet die festgestellten Fehler der letzten Empfangsminute
	 * 
	 * 
	 * @param id : Identifikation des DCF77-Moduls
	 *
	 * 
	 * @param error : Fehlercode
	 *
	 * 
	 */
void
hcan_transport::send_DCF77_ERROR(const uint16_t src,
				 const uint16_t dst,
				 uint8_t id, uint8_t error)
{
    uint8_t         data[] = {
	5,
	80,
	id,
	error,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet die festgestellten Fehler der letzten Empfangsminute
	 * 
	 * 
	 * @param id : Identifikation des DCF77-Moduls
	 *
	 * 
	 * @param error : Fehlercode
	 *
	 * 
	 */
void
hcan_transport::recv_DCF77_ERROR(const uint16_t src,
				 const uint16_t dst,
				 uint8_t * id, uint8_t * error)
{
    uint8_t         data[] = {
	5,
	80,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 80)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *error = f.data(3);

}



	/**
	 * Liefert die Länge des letzten Pulse in 1/100s
	 * 
	 * 
	 * @param id : Identifikation des DCF77-Moduls
	 *
	 * 
	 * @param length : Pulslaenge
	 *
	 * 
	 */
void
hcan_transport::send_DCF77_PULSE_DURATION(const uint16_t src,
					  const uint16_t dst,
					  uint8_t id, uint8_t length)
{
    uint8_t         data[] = {
	5,
	81,
	id,
	length,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Länge des letzten Pulse in 1/100s
	 * 
	 * 
	 * @param id : Identifikation des DCF77-Moduls
	 *
	 * 
	 * @param length : Pulslaenge
	 *
	 * 
	 */
void
hcan_transport::recv_DCF77_PULSE_DURATION(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * id, uint8_t * length)
{
    uint8_t         data[] = {
	5,
	81,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 81)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *id = f.data(2);

    *length = f.data(3);

}



	/**
	 * Reaktion: Controllerboard laed die Config aus dem EEPROM neu
	 * 
	 * 
	 */
void
hcan_transport::send_CONFIG_RELOAD(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	100,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Reaktion: Controllerboard laed die Config aus dem EEPROM neu
	 * 
	 * 
	 */
void
hcan_transport::recv_CONFIG_RELOAD(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	100,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 100)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Fragt ab, wieviel bytes des Config RAM verwendet werden
	 * 
	 * 
	 */
void
hcan_transport::send_CONFIG_RAM_USAGE_REQUEST(const uint16_t src,
					      const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	101,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt ab, wieviel bytes des Config RAM verwendet werden
	 * 
	 * 
	 */
void
hcan_transport::recv_CONFIG_RAM_USAGE_REQUEST(const uint16_t src,
					      const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	101,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 101)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Liefert die Anzahl der verwendeten Bytes im Config RAM
	 * 
	 * 
	 * @param usage_hi : MSB des RAM Usage
	 *
	 * 
	 * @param usage_lo : LSB des RAM Usage
	 *
	 * 
	 */
void
hcan_transport::send_CONFIG_RAM_USAGE_REPLAY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t usage_hi,
					     uint8_t usage_lo)
{
    uint8_t         data[] = {
	5,
	102,
	usage_hi,
	usage_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Anzahl der verwendeten Bytes im Config RAM
	 * 
	 * 
	 * @param usage_hi : MSB des RAM Usage
	 *
	 * 
	 * @param usage_lo : LSB des RAM Usage
	 *
	 * 
	 */
void
hcan_transport::recv_CONFIG_RAM_USAGE_REPLAY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * usage_hi,
					     uint8_t * usage_lo)
{
    uint8_t         data[] = {
	5,
	102,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 102)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *usage_hi = f.data(2);

    *usage_lo = f.data(3);

}



	/**
	 * Fragt nach den Zeitzonen-Details
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 */
void
hcan_transport::send_ZEITZONE_DETAILS_REQUEST(const uint16_t src,
					      const uint16_t dst,
					      uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	110,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach den Zeitzonen-Details
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 */
void
hcan_transport::recv_ZEITZONE_DETAILS_REQUEST(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	110,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 110)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Liefert die Zeitzonen Details
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 * @param day_pattern : Binaeres Day Pattern
	 *
	 * 
	 * @param from_hour : 
	 *
	 * 
	 * @param from_minute : 
	 *
	 * 
	 * @param to_hour : 
	 *
	 * 
	 * @param to_minute : 
	 *
	 * 
	 */
void
hcan_transport::send_ZEITZONE_DETAILS_REPLAY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t gruppe,
					     uint8_t day_pattern,
					     uint8_t from_hour,
					     uint8_t from_minute,
					     uint8_t to_hour,
					     uint8_t to_minute)
{
    uint8_t         data[] = {
	5,
	111,
	gruppe,
	day_pattern,
	from_hour,
	from_minute,
	to_hour,
	to_minute,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Zeitzonen Details
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 * @param day_pattern : Binaeres Day Pattern
	 *
	 * 
	 * @param from_hour : 
	 *
	 * 
	 * @param from_minute : 
	 *
	 * 
	 * @param to_hour : 
	 *
	 * 
	 * @param to_minute : 
	 *
	 * 
	 */
void
hcan_transport::recv_ZEITZONE_DETAILS_REPLAY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * gruppe,
					     uint8_t * day_pattern,
					     uint8_t * from_hour,
					     uint8_t * from_minute,
					     uint8_t * to_hour,
					     uint8_t * to_minute)
{
    uint8_t         data[] = {
	5,
	111,

	0,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 111)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *day_pattern = f.data(3);

    *from_hour = f.data(4);

    *from_minute = f.data(5);

    *to_hour = f.data(6);

    *to_minute = f.data(7);

}



	/**
	 * Liefert neue Zeitzonen Details, welche im EEPROM gespeichert werden
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 * @param day_pattern : Binaeres Day Pattern
	 *
	 * 
	 * @param from_hour : 
	 *
	 * 
	 * @param from_minute : 
	 *
	 * 
	 * @param to_hour : 
	 *
	 * 
	 * @param to_minute : 
	 *
	 * 
	 */
void
hcan_transport::send_ZEITZONE_WRITE_DETAILS(const uint16_t src,
					    const uint16_t dst,
					    uint8_t gruppe,
					    uint8_t day_pattern,
					    uint8_t from_hour,
					    uint8_t from_minute,
					    uint8_t to_hour,
					    uint8_t to_minute)
{
    uint8_t         data[] = {
	5,
	112,
	gruppe,
	day_pattern,
	from_hour,
	from_minute,
	to_hour,
	to_minute,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert neue Zeitzonen Details, welche im EEPROM gespeichert werden
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 * @param day_pattern : Binaeres Day Pattern
	 *
	 * 
	 * @param from_hour : 
	 *
	 * 
	 * @param from_minute : 
	 *
	 * 
	 * @param to_hour : 
	 *
	 * 
	 * @param to_minute : 
	 *
	 * 
	 */
void
hcan_transport::recv_ZEITZONE_WRITE_DETAILS(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * gruppe,
					    uint8_t * day_pattern,
					    uint8_t * from_hour,
					    uint8_t * from_minute,
					    uint8_t * to_hour,
					    uint8_t * to_minute)
{
    uint8_t         data[] = {
	5,
	112,

	0,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 112)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *day_pattern = f.data(3);

    *from_hour = f.data(4);

    *from_minute = f.data(5);

    *to_hour = f.data(6);

    *to_minute = f.data(7);

}



	/**
	 * Liefert ein ACK auf den Schreibvorgang
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 */
void
hcan_transport::send_ZEITZONE_WRITE_DETAILS_ACK(const uint16_t src,
						const uint16_t dst,
						uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	113,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert ein ACK auf den Schreibvorgang
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 */
void
hcan_transport::recv_ZEITZONE_WRITE_DETAILS_ACK(const uint16_t src,
						const uint16_t dst,
						uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	113,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 113)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Testet, ob die Zeitzone matched, d.h. zutrifft
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 */
void
hcan_transport::send_ZEITZONE_TEST_MATCH_REQUERST(const uint16_t src,
						  const uint16_t dst,
						  uint8_t gruppe)
{
    uint8_t         data[] = {
	5,
	114,
	gruppe,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Testet, ob die Zeitzone matched, d.h. zutrifft
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 */
void
hcan_transport::recv_ZEITZONE_TEST_MATCH_REQUERST(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * gruppe)
{
    uint8_t         data[] = {
	5,
	114,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 114)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

}



	/**
	 * Liefert die Info, ob die Zeitzone matched
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 * @param match : 0 = false, 1 = true
	 *
	 * 
	 */
void
hcan_transport::send_ZEITZONE_TEST_MATCH_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t gruppe,
						uint8_t match)
{
    uint8_t         data[] = {
	5,
	115,
	gruppe,
	match,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert die Info, ob die Zeitzone matched
	 * 
	 * 
	 * @param gruppe : Zeitzonen-ID
	 *
	 * 
	 * @param match : 0 = false, 1 = true
	 *
	 * 
	 */
void
hcan_transport::recv_ZEITZONE_TEST_MATCH_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * gruppe,
						uint8_t * match)
{
    uint8_t         data[] = {
	5,
	115,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 115)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *gruppe = f.data(2);

    *match = f.data(3);

}



	/**
	 * Error message for ports Device
	 * 
	 * 
	 * @param address : Adresse oder ID des nicht gefundenen Expanders
	 *
	 * 
	 */
void
hcan_transport::send_EXPANDER_NOT_FOUND(const uint16_t src,
					const uint16_t dst,
					uint8_t address)
{
    uint8_t         data[] = {
	5,
	120,
	address,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Error message for ports Device
	 * 
	 * 
	 * @param address : Adresse oder ID des nicht gefundenen Expanders
	 *
	 * 
	 */
void
hcan_transport::recv_EXPANDER_NOT_FOUND(const uint16_t src,
					const uint16_t dst,
					uint8_t * address)
{
    uint8_t         data[] = {
	5,
	120,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 120)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *address = f.data(2);

}



	/**
	 * Error message for ports Device
	 * 
	 * 
	 */
void
hcan_transport::send_ADDITIONAL_PORTS_DEVICE_IGNORED(const uint16_t src,
						     const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	121,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Error message for ports Device
	 * 
	 * 
	 */
void
hcan_transport::recv_ADDITIONAL_PORTS_DEVICE_IGNORED(const uint16_t src,
						     const uint16_t dst)
{
    uint8_t         data[] = {
	5,
	121,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 121)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Meldet die aktuelle Zeit
	 * 
	 * 
	 * @param level : Time Level; regelt den Master-/Slave Betrieb
	 *
	 * 
	 * @param day_of_week : Tag der Woche: [1..7] == [ Montag .. Sonntag ]
	 *
	 * 
	 * @param hour : Stunde [0..23]
	 *
	 * 
	 * @param minute : Minute [0..59]
	 *
	 * 
	 * @param second : Sekunde [0..59]
	 *
	 * 
	 */
void
hcan_transport::send_TIME_INFO(const uint16_t src,
			       const uint16_t dst,
			       uint8_t level,
			       uint8_t day_of_week,
			       uint8_t hour,
			       uint8_t minute, uint8_t second)
{
    uint8_t         data[] = {
	6,
	1,
	level,
	day_of_week,
	hour,
	minute,
	second,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet die aktuelle Zeit
	 * 
	 * 
	 * @param level : Time Level; regelt den Master-/Slave Betrieb
	 *
	 * 
	 * @param day_of_week : Tag der Woche: [1..7] == [ Montag .. Sonntag ]
	 *
	 * 
	 * @param hour : Stunde [0..23]
	 *
	 * 
	 * @param minute : Minute [0..59]
	 *
	 * 
	 * @param second : Sekunde [0..59]
	 *
	 * 
	 */
void
hcan_transport::recv_TIME_INFO(const uint16_t src,
			       const uint16_t dst,
			       uint8_t * level,
			       uint8_t * day_of_week,
			       uint8_t * hour,
			       uint8_t * minute, uint8_t * second)
{
    uint8_t         data[] = {
	6,
	1,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *level = f.data(2);

    *day_of_week = f.data(3);

    *hour = f.data(4);

    *minute = f.data(5);

    *second = f.data(6);

}



	/**
	 * Meldet das aktuelle Datum
	 * 
	 * 
	 * @param level : Date Level; regelt den Master-/Slave Betrieb
	 *
	 * 
	 * @param day_of_month : Tag des Monats [1..31] 
	 *
	 * 
	 * @param month_of_year : Monat [1..12] 
	 *
	 * 
	 * @param year : Jahr 2000 + [1..255] = [ 2001..2255 ]
	 *
	 * 
	 */
void
hcan_transport::send_DATE_INFO(const uint16_t src,
			       const uint16_t dst,
			       uint8_t level,
			       uint8_t day_of_month,
			       uint8_t month_of_year, uint8_t year)
{
    uint8_t         data[] = {
	6,
	2,
	level,
	day_of_month,
	month_of_year,
	year,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet das aktuelle Datum
	 * 
	 * 
	 * @param level : Date Level; regelt den Master-/Slave Betrieb
	 *
	 * 
	 * @param day_of_month : Tag des Monats [1..31] 
	 *
	 * 
	 * @param month_of_year : Monat [1..12] 
	 *
	 * 
	 * @param year : Jahr 2000 + [1..255] = [ 2001..2255 ]
	 *
	 * 
	 */
void
hcan_transport::recv_DATE_INFO(const uint16_t src,
			       const uint16_t dst,
			       uint8_t * level,
			       uint8_t * day_of_month,
			       uint8_t * month_of_year, uint8_t * year)
{
    uint8_t         data[] = {
	6,
	2,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *level = f.data(2);

    *day_of_month = f.data(3);

    *month_of_year = f.data(4);

    *year = f.data(5);

}



	/**
	 * Fragt die aktuelle Zeit: Replay ist TIME_INFO
	 * 
	 * 
	 */
void
hcan_transport::send_TIME_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	6,
	3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt die aktuelle Zeit: Replay ist TIME_INFO
	 * 
	 * 
	 */
void
hcan_transport::recv_TIME_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	6,
	3,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Fragt das aktuelle Datum: Replay ist DATE_INFO
	 * 
	 * 
	 */
void
hcan_transport::send_DATE_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	6,
	4,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt das aktuelle Datum: Replay ist DATE_INFO
	 * 
	 * 
	 */
void
hcan_transport::recv_DATE_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	6,
	4,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Fragt nach dem naechsten Block
	 * 
	 * 
	 * @param handle_hi : Iterator, hi byte
	 *
	 * 
	 * @param handle_lo : Iterator, lo byte
	 *
	 * 
	 */
void
hcan_transport::send_GET_NEXT_BLOCK(const uint16_t src,
				    const uint16_t dst,
				    uint8_t handle_hi, uint8_t handle_lo)
{
    uint8_t         data[] = {
	7,
	1,
	handle_hi,
	handle_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach dem naechsten Block
	 * 
	 * 
	 * @param handle_hi : Iterator, hi byte
	 *
	 * 
	 * @param handle_lo : Iterator, lo byte
	 *
	 * 
	 */
void
hcan_transport::recv_GET_NEXT_BLOCK(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * handle_hi,
				    uint8_t * handle_lo)
{
    uint8_t         data[] = {
	7,
	1,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *handle_hi = f.data(2);

    *handle_lo = f.data(3);

}



	/**
	 * Liefert den naechsten Block
	 * 
	 * 
	 * @param handle_hi : Iterator, hi byte
	 *
	 * 
	 * @param handle_lo : Iterator, lo byte
	 *
	 * 
	 * @param type : Typ des Blocks
	 *
	 * 
	 * @param size : Groesse des Blocks
	 *
	 * 
	 */
void
hcan_transport::send_GET_NEXT_BLOCK_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t handle_hi,
					   uint8_t handle_lo,
					   uint8_t type, uint8_t size)
{
    uint8_t         data[] = {
	7,
	2,
	handle_hi,
	handle_lo,
	type,
	size,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert den naechsten Block
	 * 
	 * 
	 * @param handle_hi : Iterator, hi byte
	 *
	 * 
	 * @param handle_lo : Iterator, lo byte
	 *
	 * 
	 * @param type : Typ des Blocks
	 *
	 * 
	 * @param size : Groesse des Blocks
	 *
	 * 
	 */
void
hcan_transport::recv_GET_NEXT_BLOCK_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * handle_hi,
					   uint8_t * handle_lo,
					   uint8_t * type, uint8_t * size)
{
    uint8_t         data[] = {
	7,
	2,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *handle_hi = f.data(2);

    *handle_lo = f.data(3);

    *type = f.data(4);

    *size = f.data(5);

}



	/**
	 * Fasst alle freien Bloecke zusammen
	 * 
	 * 
	 */
void
hcan_transport::send_DEFRAGMENT(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	7,
	3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fasst alle freien Bloecke zusammen
	 * 
	 * 
	 */
void
hcan_transport::recv_DEFRAGMENT(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	7,
	3,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Alloziert einen Block
	 * 
	 * 
	 * @param type : Typ des Blocks
	 *
	 * 
	 * @param size : Groesse des Blocks
	 *
	 * 
	 */
void
hcan_transport::send_ALLOCATE_BLOCK(const uint16_t src,
				    const uint16_t dst,
				    uint8_t type, uint8_t size)
{
    uint8_t         data[] = {
	7,
	4,
	type,
	size,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Alloziert einen Block
	 * 
	 * 
	 * @param type : Typ des Blocks
	 *
	 * 
	 * @param size : Groesse des Blocks
	 *
	 * 
	 */
void
hcan_transport::recv_ALLOCATE_BLOCK(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * type, uint8_t * size)
{
    uint8_t         data[] = {
	7,
	4,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *type = f.data(2);

    *size = f.data(3);

}



	/**
	 * Liefert ein Handle auf den allozierten Block, oder 0, falls fehlgeschlagen
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 */
void
hcan_transport::send_ALLOCATE_BLOCK_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t handle_hi,
					   uint8_t handle_lo)
{
    uint8_t         data[] = {
	7,
	5,
	handle_hi,
	handle_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert ein Handle auf den allozierten Block, oder 0, falls fehlgeschlagen
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 */
void
hcan_transport::recv_ALLOCATE_BLOCK_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * handle_hi,
					   uint8_t * handle_lo)
{
    uint8_t         data[] = {
	7,
	5,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *handle_hi = f.data(2);

    *handle_lo = f.data(3);

}



	/**
	 * Gibt einen Block frei
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 */
void
hcan_transport::send_FREE_BLOCK(const uint16_t src,
				const uint16_t dst,
				uint8_t handle_hi, uint8_t handle_lo)
{
    uint8_t         data[] = {
	7,
	6,
	handle_hi,
	handle_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Gibt einen Block frei
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 */
void
hcan_transport::recv_FREE_BLOCK(const uint16_t src,
				const uint16_t dst,
				uint8_t * handle_hi, uint8_t * handle_lo)
{
    uint8_t         data[] = {
	7,
	6,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *handle_hi = f.data(2);

    *handle_lo = f.data(3);

}



	/**
	 * Gibt einen Block frei
	 * 
	 * 
	 * @param result : 0 = ok, 1 = error
	 *
	 * 
	 */
void
hcan_transport::send_FREE_BLOCK_REPLAY(const uint16_t src,
				       const uint16_t dst, uint8_t result)
{
    uint8_t         data[] = {
	7,
	7,
	result,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Gibt einen Block frei
	 * 
	 * 
	 * @param result : 0 = ok, 1 = error
	 *
	 * 
	 */
void
hcan_transport::recv_FREE_BLOCK_REPLAY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * result)
{
    uint8_t         data[] = {
	7,
	7,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *result = f.data(2);

}



	/**
	 * Fordert 4 Bytes aus einem Block an
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 * @param index : lo byte
	 *
	 * 
	 */
void
hcan_transport::send_READ_FROM_BLOCK(const uint16_t src,
				     const uint16_t dst,
				     uint8_t handle_hi,
				     uint8_t handle_lo, uint8_t index)
{
    uint8_t         data[] = {
	7,
	8,
	handle_hi,
	handle_lo,
	index,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fordert 4 Bytes aus einem Block an
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 * @param index : lo byte
	 *
	 * 
	 */
void
hcan_transport::recv_READ_FROM_BLOCK(const uint16_t src,
				     const uint16_t dst,
				     uint8_t * handle_hi,
				     uint8_t * handle_lo, uint8_t * index)
{
    uint8_t         data[] = {
	7,
	8,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *handle_hi = f.data(2);

    *handle_lo = f.data(3);

    *index = f.data(4);

}



	/**
	 * Liefert 4 Bytes aus einem Block an
	 * 
	 * 
	 * @param d0 : Byte 0
	 *
	 * 
	 * @param d1 : Byte 1
	 *
	 * 
	 * @param d2 : Byte 2
	 *
	 * 
	 * @param d3 : Byte 3
	 *
	 * 
	 */
void
hcan_transport::send_READ_FROM_BLOCK_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t d0,
					    uint8_t d1,
					    uint8_t d2, uint8_t d3)
{
    uint8_t         data[] = {
	7,
	9,
	d0,
	d1,
	d2,
	d3,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert 4 Bytes aus einem Block an
	 * 
	 * 
	 * @param d0 : Byte 0
	 *
	 * 
	 * @param d1 : Byte 1
	 *
	 * 
	 * @param d2 : Byte 2
	 *
	 * 
	 * @param d3 : Byte 3
	 *
	 * 
	 */
void
hcan_transport::recv_READ_FROM_BLOCK_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * d0,
					    uint8_t * d1,
					    uint8_t * d2, uint8_t * d3)
{
    uint8_t         data[] = {
	7,
	9,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 9)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *d0 = f.data(2);

    *d1 = f.data(3);

    *d2 = f.data(4);

    *d3 = f.data(5);

}



	/**
	 * Schreibt ein Byte in einem Block
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 * @param index : lo byte
	 *
	 * 
	 * @param d0 : Byte
	 *
	 * 
	 */
void
hcan_transport::send_WRITE_TO_BLOCK(const uint16_t src,
				    const uint16_t dst,
				    uint8_t handle_hi,
				    uint8_t handle_lo,
				    uint8_t index, uint8_t d0)
{
    uint8_t         data[] = {
	7,
	10,
	handle_hi,
	handle_lo,
	index,
	d0,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Schreibt ein Byte in einem Block
	 * 
	 * 
	 * @param handle_hi : hi byte
	 *
	 * 
	 * @param handle_lo : lo byte
	 *
	 * 
	 * @param index : lo byte
	 *
	 * 
	 * @param d0 : Byte
	 *
	 * 
	 */
void
hcan_transport::recv_WRITE_TO_BLOCK(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * handle_hi,
				    uint8_t * handle_lo,
				    uint8_t * index, uint8_t * d0)
{
    uint8_t         data[] = {
	7,
	10,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *handle_hi = f.data(2);

    *handle_lo = f.data(3);

    *index = f.data(4);

    *d0 = f.data(5);

}



	/**
	 * Anwort auf ein geschriebenes Byte
	 * 
	 * 
	 */
void
hcan_transport::send_WRITE_TO_BLOCK_REPLAY(const uint16_t src,
					   const uint16_t dst)
{
    uint8_t         data[] = {
	7,
	11,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anwort auf ein geschriebenes Byte
	 * 
	 * 
	 */
void
hcan_transport::recv_WRITE_TO_BLOCK_REPLAY(const uint16_t src,
					   const uint16_t dst)
{
    uint8_t         data[] = {
	7,
	11,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Formatiert den EDS Bereich des EEPROM
	 * 
	 * 
	 */
void
hcan_transport::send_FORMAT(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	7,
	12,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Formatiert den EDS Bereich des EEPROM
	 * 
	 * 
	 */
void
hcan_transport::recv_FORMAT(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	7,
	12,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 12)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Fragt den aktuellen USV Zustand
	 * 
	 * 
	 */
void
hcan_transport::send_STATE_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	8,
	1,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt den aktuellen USV Zustand
	 * 
	 * 
	 */
void
hcan_transport::recv_STATE_REQUEST(const uint16_t src, const uint16_t dst)
{
    uint8_t         data[] = {
	8,
	1,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Meldet den aktuellen USV Zustand
	 * 
	 * 
	 * @param state : 0 = Netzbetrieb, 1 = Batterie
	 *
	 * 
	 */
void
hcan_transport::send_STATE_INFO(const uint16_t src,
				const uint16_t dst, uint8_t state)
{
    uint8_t         data[] = {
	8,
	2,
	state,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet den aktuellen USV Zustand
	 * 
	 * 
	 * @param state : 0 = Netzbetrieb, 1 = Batterie
	 *
	 * 
	 */
void
hcan_transport::recv_STATE_INFO(const uint16_t src,
				const uint16_t dst, uint8_t * state)
{
    uint8_t         data[] = {
	8,
	2,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *state = f.data(2);

}



	/**
	 * Fragt nach einer USV Spannung
	 * 
	 * 
	 * @param Ux : 1-5
	 *
	 * 
	 */
void
hcan_transport::send_VOLTAGE_REQUEST(const uint16_t src,
				     const uint16_t dst, uint8_t Ux)
{
    uint8_t         data[] = {
	8,
	3,
	Ux,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach einer USV Spannung
	 * 
	 * 
	 * @param Ux : 1-5
	 *
	 * 
	 */
void
hcan_transport::recv_VOLTAGE_REQUEST(const uint16_t src,
				     const uint16_t dst, uint8_t * Ux)
{
    uint8_t         data[] = {
	8,
	3,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *Ux = f.data(2);

}



	/**
	 * Meldet die USV Spannung
	 * 
	 * 
	 * @param Ux_hi : hi teil
	 *
	 * 
	 * @param Ux_lo : lo teil
	 *
	 * 
	 */
void
hcan_transport::send_VOLTAGE_REPLAY(const uint16_t src,
				    const uint16_t dst,
				    uint8_t Ux_hi, uint8_t Ux_lo)
{
    uint8_t         data[] = {
	8,
	4,
	Ux_hi,
	Ux_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Meldet die USV Spannung
	 * 
	 * 
	 * @param Ux_hi : hi teil
	 *
	 * 
	 * @param Ux_lo : lo teil
	 *
	 * 
	 */
void
hcan_transport::recv_VOLTAGE_REPLAY(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * Ux_hi, uint8_t * Ux_lo)
{
    uint8_t         data[] = {
	8,
	4,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *Ux_hi = f.data(2);

    *Ux_lo = f.data(3);

}



	/**
	 * Fragt nach den Spannungs-Statistiken
	 * 
	 * 
	 * @param Ux : 1-5
	 *
	 * 
	 */
void
hcan_transport::send_VOLTAGE_STATS_REQUEST(const uint16_t src,
					   const uint16_t dst, uint8_t Ux)
{
    uint8_t         data[] = {
	8,
	5,
	Ux,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach den Spannungs-Statistiken
	 * 
	 * 
	 * @param Ux : 1-5
	 *
	 * 
	 */
void
hcan_transport::recv_VOLTAGE_STATS_REQUEST(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * Ux)
{
    uint8_t         data[] = {
	8,
	5,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *Ux = f.data(2);

}



	/**
	 * Fragt nach den Spannungs-Statistiken
	 * 
	 * 
	 * @param U : 1-5
	 *
	 * 
	 * @param min_hi : Minimum
	 *
	 * 
	 * @param min_lo : Minimum
	 *
	 * 
	 * @param max_hi : Maximum
	 *
	 * 
	 * @param max_lo : Maximum
	 *
	 * 
	 */
void
hcan_transport::send_VOLTAGE_STATS_REPLAY(const uint16_t src,
					  const uint16_t dst,
					  uint8_t U,
					  uint8_t min_hi,
					  uint8_t min_lo,
					  uint8_t max_hi, uint8_t max_lo)
{
    uint8_t         data[] = {
	8,
	6,
	U,
	min_hi,
	min_lo,
	max_hi,
	max_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach den Spannungs-Statistiken
	 * 
	 * 
	 * @param U : 1-5
	 *
	 * 
	 * @param min_hi : Minimum
	 *
	 * 
	 * @param min_lo : Minimum
	 *
	 * 
	 * @param max_hi : Maximum
	 *
	 * 
	 * @param max_lo : Maximum
	 *
	 * 
	 */
void
hcan_transport::recv_VOLTAGE_STATS_REPLAY(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * U,
					  uint8_t * min_hi,
					  uint8_t * min_lo,
					  uint8_t * max_hi,
					  uint8_t * max_lo)
{
    uint8_t         data[] = {
	8,
	6,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *U = f.data(2);

    *min_hi = f.data(3);

    *min_lo = f.data(4);

    *max_hi = f.data(5);

    *max_lo = f.data(6);

}



	/**
	 * Setzt alle Spannungs-Statistiken zurueck
	 * 
	 * 
	 */
void
hcan_transport::send_VOLTAGE_STATS_RESET(const uint16_t src,
					 const uint16_t dst)
{
    uint8_t         data[] = {
	8,
	7,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Setzt alle Spannungs-Statistiken zurueck
	 * 
	 * 
	 */
void
hcan_transport::recv_VOLTAGE_STATS_RESET(const uint16_t src,
					 const uint16_t dst)
{
    uint8_t         data[] = {
	8,
	7,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Feuerungsautomat Block 1
	 * 
	 * 
	 * @param status : LSB bis MSB: LDW,GDW,WS,Flamme,Ventil1,Ventil2,UWP,Alarm
	 *
	 * 
	 * @param stellgrad : 0-100%
	 *
	 * 
	 * @param KT : 0-100 Grad, (DATA1c)
	 *
	 * 
	 * @param RT : 0-100 Grad
	 *
	 * 
	 * @param BT : 0-100 Grad
	 *
	 * 
	 * @param AT : -30-50 Grad (SIGN CHAR)
	 *
	 * 
	 */
void
hcan_transport::send_FA_BLOCK1_INFO(const uint16_t src,
				    const uint16_t dst,
				    uint8_t status,
				    uint8_t stellgrad,
				    uint8_t KT,
				    uint8_t RT, uint8_t BT, uint8_t AT)
{
    uint8_t         data[] = {
	9,
	1,
	status,
	stellgrad,
	KT,
	RT,
	BT,
	AT,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Feuerungsautomat Block 1
	 * 
	 * 
	 * @param status : LSB bis MSB: LDW,GDW,WS,Flamme,Ventil1,Ventil2,UWP,Alarm
	 *
	 * 
	 * @param stellgrad : 0-100%
	 *
	 * 
	 * @param KT : 0-100 Grad, (DATA1c)
	 *
	 * 
	 * @param RT : 0-100 Grad
	 *
	 * 
	 * @param BT : 0-100 Grad
	 *
	 * 
	 * @param AT : -30-50 Grad (SIGN CHAR)
	 *
	 * 
	 */
void
hcan_transport::recv_FA_BLOCK1_INFO(const uint16_t src,
				    const uint16_t dst,
				    uint8_t * status,
				    uint8_t * stellgrad,
				    uint8_t * KT,
				    uint8_t * RT,
				    uint8_t * BT, uint8_t * AT)
{
    uint8_t         data[] = {
	9,
	1,

	0,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 9)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *status = f.data(2);

    *stellgrad = f.data(3);

    *KT = f.data(4);

    *RT = f.data(5);

    *BT = f.data(6);

    *AT = f.data(7);

}



	/**
	 * Regler Daten an FA Fragment 1
	 * 
	 * 
	 * @param status : Waermeanforderung (s.S.24)
	 *
	 * 
	 * @param aktion : Ein/Ausschalten etc. (s.S.24)
	 *
	 * 
	 * @param kesseltemp_soll_lo : Grad, DATA2c, lo Byte
	 *
	 * 
	 * @param kesseltemp_soll_hi : Grad, DATA2c, hi Byte
	 *
	 * 
	 * @param kesseldr_soll_lo : bar, DATA2b, lo Byte
	 *
	 * 
	 * @param kesseldr_soll_hi : bar, DATA2b, hi Byte
	 *
	 * 
	 */
void
hcan_transport::send_REGLER_DATEN_INFO_FRG1(const uint16_t src,
					    const uint16_t dst,
					    uint8_t status,
					    uint8_t aktion,
					    uint8_t kesseltemp_soll_lo,
					    uint8_t kesseltemp_soll_hi,
					    uint8_t kesseldr_soll_lo,
					    uint8_t kesseldr_soll_hi)
{
    uint8_t         data[] = {
	9,
	2,
	status,
	aktion,
	kesseltemp_soll_lo,
	kesseltemp_soll_hi,
	kesseldr_soll_lo,
	kesseldr_soll_hi,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Regler Daten an FA Fragment 1
	 * 
	 * 
	 * @param status : Waermeanforderung (s.S.24)
	 *
	 * 
	 * @param aktion : Ein/Ausschalten etc. (s.S.24)
	 *
	 * 
	 * @param kesseltemp_soll_lo : Grad, DATA2c, lo Byte
	 *
	 * 
	 * @param kesseltemp_soll_hi : Grad, DATA2c, hi Byte
	 *
	 * 
	 * @param kesseldr_soll_lo : bar, DATA2b, lo Byte
	 *
	 * 
	 * @param kesseldr_soll_hi : bar, DATA2b, hi Byte
	 *
	 * 
	 */
void
hcan_transport::recv_REGLER_DATEN_INFO_FRG1(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * status,
					    uint8_t * aktion,
					    uint8_t * kesseltemp_soll_lo,
					    uint8_t * kesseltemp_soll_hi,
					    uint8_t * kesseldr_soll_lo,
					    uint8_t * kesseldr_soll_hi)
{
    uint8_t         data[] = {
	9,
	2,

	0,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 9)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *status = f.data(2);

    *aktion = f.data(3);

    *kesseltemp_soll_lo = f.data(4);

    *kesseltemp_soll_hi = f.data(5);

    *kesseldr_soll_lo = f.data(6);

    *kesseldr_soll_hi = f.data(7);

}



	/**
	 * Regler Daten an FA Fragment 2
	 * 
	 * 
	 * @param stellgrad : Modulationsgrad
	 *
	 * 
	 * @param ww_soll : Brauchwasser-Sollwert
	 *
	 * 
	 * @param brennstoff : 00=?, 01=Gas, 10=Oel, 11=?
	 *
	 * 
	 */
void
hcan_transport::send_REGLER_DATEN_INFO_FRG2(const uint16_t src,
					    const uint16_t dst,
					    uint8_t stellgrad,
					    uint8_t ww_soll,
					    uint8_t brennstoff)
{
    uint8_t         data[] = {
	9,
	3,
	stellgrad,
	ww_soll,
	brennstoff,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Regler Daten an FA Fragment 2
	 * 
	 * 
	 * @param stellgrad : Modulationsgrad
	 *
	 * 
	 * @param ww_soll : Brauchwasser-Sollwert
	 *
	 * 
	 * @param brennstoff : 00=?, 01=Gas, 10=Oel, 11=?
	 *
	 * 
	 */
void
hcan_transport::recv_REGLER_DATEN_INFO_FRG2(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * stellgrad,
					    uint8_t * ww_soll,
					    uint8_t * brennstoff)
{
    uint8_t         data[] = {
	9,
	3,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 9)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *stellgrad = f.data(2);

    *ww_soll = f.data(3);

    *brennstoff = f.data(4);

}



	/**
	 * Wetterdaten
	 * 
	 * 
	 * @param niederschlag : 1 = Niederschlag
	 *
	 * 
	 * @param wind : Impulse pro sec
	 *
	 * 
	 * @param licht_ost : Sonne/Helligkeit
	 *
	 * 
	 * @param licht_sued : Sonne/Helligkeit
	 *
	 * 
	 * @param licht_west : Sonne/Helligkeit
	 *
	 * 
	 */
void
hcan_transport::send_WETTER_INFO(const uint16_t src,
				 const uint16_t dst,
				 uint8_t niederschlag,
				 uint8_t wind,
				 uint8_t licht_ost,
				 uint8_t licht_sued, uint8_t licht_west)
{
    uint8_t         data[] = {
	10,
	1,
	niederschlag,
	wind,
	licht_ost,
	licht_sued,
	licht_west,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Wetterdaten
	 * 
	 * 
	 * @param niederschlag : 1 = Niederschlag
	 *
	 * 
	 * @param wind : Impulse pro sec
	 *
	 * 
	 * @param licht_ost : Sonne/Helligkeit
	 *
	 * 
	 * @param licht_sued : Sonne/Helligkeit
	 *
	 * 
	 * @param licht_west : Sonne/Helligkeit
	 *
	 * 
	 */
void
hcan_transport::recv_WETTER_INFO(const uint16_t src,
				 const uint16_t dst,
				 uint8_t * niederschlag,
				 uint8_t * wind,
				 uint8_t * licht_ost,
				 uint8_t * licht_sued,
				 uint8_t * licht_west)
{
    uint8_t         data[] = {
	10,
	1,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *niederschlag = f.data(2);

    *wind = f.data(3);

    *licht_ost = f.data(4);

    *licht_sued = f.data(5);

    *licht_west = f.data(6);

}



	/**
	 * Licht, ueber 5min gemittelt
	 * 
	 * 
	 * @param licht_ost : Helligkeit
	 *
	 * 
	 * @param licht_sued : Helligkeit
	 *
	 * 
	 * @param licht_west : Helligkeit
	 *
	 * 
	 */
void
hcan_transport::send_LICHT_STATS_5MIN(const uint16_t src,
				      const uint16_t dst,
				      uint8_t licht_ost,
				      uint8_t licht_sued,
				      uint8_t licht_west)
{
    uint8_t         data[] = {
	10,
	2,
	licht_ost,
	licht_sued,
	licht_west,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Licht, ueber 5min gemittelt
	 * 
	 * 
	 * @param licht_ost : Helligkeit
	 *
	 * 
	 * @param licht_sued : Helligkeit
	 *
	 * 
	 * @param licht_west : Helligkeit
	 *
	 * 
	 */
void
hcan_transport::recv_LICHT_STATS_5MIN(const uint16_t src,
				      const uint16_t dst,
				      uint8_t * licht_ost,
				      uint8_t * licht_sued,
				      uint8_t * licht_west)
{
    uint8_t         data[] = {
	10,
	2,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *licht_ost = f.data(2);

    *licht_sued = f.data(3);

    *licht_west = f.data(4);

}



	/**
	 * Wind, ueber 5min gemittelt
	 * 
	 * 
	 * @param wind : Impulse pro sec
	 *
	 * 
	 */
void
hcan_transport::send_WIND_STATS_5MIN(const uint16_t src,
				     const uint16_t dst, uint8_t wind)
{
    uint8_t         data[] = {
	10,
	3,
	wind,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Wind, ueber 5min gemittelt
	 * 
	 * 
	 * @param wind : Impulse pro sec
	 *
	 * 
	 */
void
hcan_transport::recv_WIND_STATS_5MIN(const uint16_t src,
				     const uint16_t dst, uint8_t * wind)
{
    uint8_t         data[] = {
	10,
	3,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *wind = f.data(2);

}



	/**
	 * Fragt nach der Anzahl der Objekte
	 * 
	 * 
	 */
void
hcan_transport::send_OBJECT_COUNT_REQUEST(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	11,
	1,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der Objekte
	 * 
	 * 
	 */
void
hcan_transport::recv_OBJECT_COUNT_REQUEST(const uint16_t src,
					  const uint16_t dst)
{
    uint8_t         data[] = {
	11,
	1,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 1)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


}



	/**
	 * Anzahl der Objekte
	 * 
	 * 
	 * @param count : Anzahl der Objekte
	 *
	 * 
	 */
void
hcan_transport::send_OBJECT_COUNT_REPLAY(const uint16_t src,
					 const uint16_t dst,
					 uint8_t count)
{
    uint8_t         data[] = {
	11,
	2,
	count,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anzahl der Objekte
	 * 
	 * 
	 * @param count : Anzahl der Objekte
	 *
	 * 
	 */
void
hcan_transport::recv_OBJECT_COUNT_REPLAY(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * count)
{
    uint8_t         data[] = {
	11,
	2,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 2)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *count = f.data(2);

}



	/**
	 * Anfrage der Attribute eines Objekts
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 */
void
hcan_transport::send_OBJECT_ATTRIBUTE_REQUEST(const uint16_t src,
					      const uint16_t dst,
					      uint8_t oid)
{
    uint8_t         data[] = {
	11,
	3,
	oid,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anfrage der Attribute eines Objekts
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 */
void
hcan_transport::recv_OBJECT_ATTRIBUTE_REQUEST(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * oid)
{
    uint8_t         data[] = {
	11,
	3,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 3)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

}



	/**
	 * Attribute eines Knotens
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param oclass : Klasse des Objekts
	 *
	 * 
	 * @param p1 : opt. Parameter
	 *
	 * 
	 * @param p2 : opt. Parameter
	 *
	 * 
	 * @param p3 : opt. Parameter
	 *
	 * 
	 * @param p4 : opt. Parameter
	 *
	 * 
	 */
void
hcan_transport::send_OBJECT_ATTRIBUTE_REPLAY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t oid,
					     uint8_t oclass,
					     uint8_t p1,
					     uint8_t p2,
					     uint8_t p3, uint8_t p4)
{
    uint8_t         data[] = {
	11,
	4,
	oid,
	oclass,
	p1,
	p2,
	p3,
	p4,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Attribute eines Knotens
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param oclass : Klasse des Objekts
	 *
	 * 
	 * @param p1 : opt. Parameter
	 *
	 * 
	 * @param p2 : opt. Parameter
	 *
	 * 
	 * @param p3 : opt. Parameter
	 *
	 * 
	 * @param p4 : opt. Parameter
	 *
	 * 
	 */
void
hcan_transport::recv_OBJECT_ATTRIBUTE_REPLAY(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * oid,
					     uint8_t * oclass,
					     uint8_t * p1,
					     uint8_t * p2,
					     uint8_t * p3, uint8_t * p4)
{
    uint8_t         data[] = {
	11,
	4,

	0,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 4)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *oclass = f.data(3);

    *p1 = f.data(4);

    *p2 = f.data(5);

    *p3 = f.data(6);

    *p4 = f.data(7);

}



	/**
	 * Fragt nach der Anzahl der Felder eines Objekts
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_COUNT_REQUEST(const uint16_t src,
					 const uint16_t dst, uint8_t oid)
{
    uint8_t         data[] = {
	11,
	5,
	oid,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach der Anzahl der Felder eines Objekts
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_COUNT_REQUEST(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * oid)
{
    uint8_t         data[] = {
	11,
	5,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 5)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

}



	/**
	 * Anzahl der Objekte
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param count : Anzahl der Felder
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_COUNT_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t oid, uint8_t count)
{
    uint8_t         data[] = {
	11,
	6,
	oid,
	count,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anzahl der Objekte
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param count : Anzahl der Felder
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_COUNT_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t * oid, uint8_t * count)
{
    uint8_t         data[] = {
	11,
	6,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 6)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *count = f.data(3);

}



	/**
	 * Anfrage der Attribute eines Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_ATTRIBUTE_REQUEST(const uint16_t src,
					     const uint16_t dst,
					     uint8_t oid, uint8_t fid)
{
    uint8_t         data[] = {
	11,
	7,
	oid,
	fid,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Anfrage der Attribute eines Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_ATTRIBUTE_REQUEST(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * oid, uint8_t * fid)
{
    uint8_t         data[] = {
	11,
	7,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 7)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *fid = f.data(3);

}



	/**
	 * Attribute eines Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param fclass : Variablen-Typ bzw. Klasse des Feldes
	 *
	 * 
	 * @param flags : bit0 = rw, bit1 = persistent, bit2 = array
	 *
	 * 
	 * @param arraysize : 
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_ATTRIBUTE_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t oid,
					    uint8_t fid,
					    uint8_t fclass,
					    uint8_t flags,
					    uint8_t arraysize)
{
    uint8_t         data[] = {
	11,
	8,
	oid,
	fid,
	fclass,
	flags,
	arraysize,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Attribute eines Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param fclass : Variablen-Typ bzw. Klasse des Feldes
	 *
	 * 
	 * @param flags : bit0 = rw, bit1 = persistent, bit2 = array
	 *
	 * 
	 * @param arraysize : 
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_ATTRIBUTE_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * oid,
					    uint8_t * fid,
					    uint8_t * fclass,
					    uint8_t * flags,
					    uint8_t * arraysize)
{
    uint8_t         data[] = {
	11,
	8,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 8)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *fid = f.data(3);

    *fclass = f.data(4);

    *flags = f.data(5);

    *arraysize = f.data(6);

}



	/**
	 * Fragt nach dem aktuellen Wert des Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_READ_REQUEST(const uint16_t src,
					const uint16_t dst,
					uint8_t oid,
					uint8_t fid, uint8_t idx)
{
    uint8_t         data[] = {
	11,
	9,
	oid,
	fid,
	idx,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Fragt nach dem aktuellen Wert des Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_READ_REQUEST(const uint16_t src,
					const uint16_t dst,
					uint8_t * oid,
					uint8_t * fid, uint8_t * idx)
{
    uint8_t         data[] = {
	11,
	9,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 9)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *fid = f.data(3);

    *idx = f.data(4);

}



	/**
	 * Liefert den Wert des Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 * @param value_hi : 
	 *
	 * 
	 * @param value_lo : 
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_READ_REPLAY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t oid,
				       uint8_t fid,
				       uint8_t idx,
				       uint8_t value_hi, uint8_t value_lo)
{
    uint8_t         data[] = {
	11,
	10,
	oid,
	fid,
	idx,
	value_hi,
	value_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * Liefert den Wert des Feldes
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 * @param value_hi : 
	 *
	 * 
	 * @param value_lo : 
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_READ_REPLAY(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * oid,
				       uint8_t * fid,
				       uint8_t * idx,
				       uint8_t * value_hi,
				       uint8_t * value_lo)
{
    uint8_t         data[] = {
	11,
	10,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 10)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *fid = f.data(3);

    *idx = f.data(4);

    *value_hi = f.data(5);

    *value_lo = f.data(6);

}



	/**
	 * 
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 * @param value_hi : 
	 *
	 * 
	 * @param value_lo : 
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_WRITE_REQUEST(const uint16_t src,
					 const uint16_t dst,
					 uint8_t oid,
					 uint8_t fid,
					 uint8_t idx,
					 uint8_t value_hi,
					 uint8_t value_lo)
{
    uint8_t         data[] = {
	11,
	11,
	oid,
	fid,
	idx,
	value_hi,
	value_lo,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * 
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 * @param value_hi : 
	 *
	 * 
	 * @param value_lo : 
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_WRITE_REQUEST(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * oid,
					 uint8_t * fid,
					 uint8_t * idx,
					 uint8_t * value_hi,
					 uint8_t * value_lo)
{
    uint8_t         data[] = {
	11,
	11,

	0,

	0,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *fid = f.data(3);

    *idx = f.data(4);

    *value_hi = f.data(5);

    *value_lo = f.data(6);

}



	/**
	 * 
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 */
void
hcan_transport::send_FIELD_WRITE_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t oid,
					uint8_t fid, uint8_t idx)
{
    uint8_t         data[] = {
	11,
	12,
	oid,
	fid,
	idx,
    };
    assert(8 >= sizeof(data));

    frame           f(src, dst, 1, data, sizeof(data));
    f.write_to(socket(), m_hcand_ip);
}

	/**
	 * 
	 * 
	 * 
	 * @param oid : Objekt ID
	 *
	 * 
	 * @param fid : Field ID
	 *
	 * 
	 * @param idx : opt. Array Index
	 *
	 * 
	 */
void
hcan_transport::recv_FIELD_WRITE_REPLAY(const uint16_t src,
					const uint16_t dst,
					uint8_t * oid,
					uint8_t * fid, uint8_t * idx)
{
    uint8_t         data[] = {
	11,
	12,

	0,

	0,

	0,
    };

    frame           f = recv_frame(dst);

    if (src)
	if (f.src() != src)
	    throw           transport_error
		("unexpected packet received: src is wrong");
    if (f.size() != sizeof(data))
	throw          
	    transport_error("unexpected size in packet received");
    if (f.data(0) != 11)
	throw          
	    transport_error
	    ("unexpected packet received: service id is wrong");
    if (f.data(1) != 12)
	throw          
	    transport_error
	    ("unexpected packet received: commmand id is wrong");


    *oid = f.data(2);

    *fid = f.data(3);

    *idx = f.data(4);

}
