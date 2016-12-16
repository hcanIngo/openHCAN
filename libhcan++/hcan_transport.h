
#ifndef HCAN_TRANSPORT_H
#define HCAN_TRANSPORT_H

// ------- AUTOMATICLY GENERATED FROM mk_transport_h.xsl
// ------------------

#include <inttypes.h>
#include <frame.h>

namespace       hcan {
    class           transport_error:public traceable_error {
      public:
	transport_error(const string & s);
    };

    class           hcan_transport {
      protected:
	in_addr_t m_hcand_ip;
      protected:
	virtual int     socket() const = 0;
	virtual frame   recv_frame(const uint16_t dst) = 0;
	virtual frame   recv_frame_with_timeout(int seconds) = 0;
	                hcan_transport(const in_addr_t hcand_ip);
      public:
	                virtual ~ hcan_transport();


	void            send_PING_REQUEST(const uint16_t src,
					  const uint16_t dst);
	void            recv_PING_REQUEST(const uint16_t src,
					  const uint16_t dst);


	void            send_PING_REPLAY(const uint16_t src,
					 const uint16_t dst);
	void            recv_PING_REPLAY(const uint16_t src,
					 const uint16_t dst);


	void            send_DEVICE_STATE_REQUEST(const uint16_t src,
						  const uint16_t dst);
	void            recv_DEVICE_STATE_REQUEST(const uint16_t src,
						  const uint16_t dst);


	void            send_DEVICE_STATE_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t state);
	void            recv_DEVICE_STATE_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * state);


	void            send_DEVICE_RESET(const uint16_t src,
					  const uint16_t dst);
	void            recv_DEVICE_RESET(const uint16_t src,
					  const uint16_t dst);


	void            send_DEVICE_BOOT_STOP(const uint16_t src,
					      const uint16_t dst);
	void            recv_DEVICE_BOOT_STOP(const uint16_t src,
					      const uint16_t dst);


	void            send_DEVICE_BOOT_STOP_ACK(const uint16_t src,
						  const uint16_t dst);
	void            recv_DEVICE_BOOT_STOP_ACK(const uint16_t src,
						  const uint16_t dst);


	void            send_DEVICE_LOAD_APP(const uint16_t src,
					     const uint16_t dst);
	void            recv_DEVICE_LOAD_APP(const uint16_t src,
					     const uint16_t dst);


	void            send_DEVICE_TYPE_REQUEST(const uint16_t src,
						 const uint16_t dst);
	void            recv_DEVICE_TYPE_REQUEST(const uint16_t src,
						 const uint16_t dst);


	void            send_DEVICE_TYPE_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t arch,
						uint8_t type);
	void            recv_DEVICE_TYPE_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * arch,
						uint8_t * type);


	void            send_FLASH_BUFFER_FILL(const uint16_t src,
					       const uint16_t dst,
					       uint8_t index,
					       uint8_t d0,
					       uint8_t d1,
					       uint8_t d2, uint8_t d3);
	void            recv_FLASH_BUFFER_FILL(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * index,
					       uint8_t * d0,
					       uint8_t * d1,
					       uint8_t * d2,
					       uint8_t * d3);


	void            send_FLASH_BUFFER_FILL_ACK(const uint16_t src,
						   const uint16_t dst);
	void            recv_FLASH_BUFFER_FILL_ACK(const uint16_t src,
						   const uint16_t dst);


	void            send_FLASH_BUFFER_WRITE(const uint16_t src,
						const uint16_t dst,
						uint8_t addr_lsb,
						uint8_t addr_msb);
	void            recv_FLASH_BUFFER_WRITE(const uint16_t src,
						const uint16_t dst,
						uint8_t * addr_lsb,
						uint8_t * addr_msb);


	void            send_FLASH_BUFFER_WRITE_ACK(const uint16_t src,
						    const uint16_t dst);
	void            recv_FLASH_BUFFER_WRITE_ACK(const uint16_t src,
						    const uint16_t dst);


	void            send_INTERNAL_EEPROM_WRITE(const uint16_t src,
						   const uint16_t dst,
						   uint8_t addr_lsb,
						   uint8_t addr_msb,
						   uint8_t value);
	void            recv_INTERNAL_EEPROM_WRITE(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * addr_lsb,
						   uint8_t * addr_msb,
						   uint8_t * value);


	void            send_INTERNAL_EEPROM_WRITE_ACK(const uint16_t src,
						       const uint16_t
						       dst);
	void            recv_INTERNAL_EEPROM_WRITE_ACK(const uint16_t src,
						       const uint16_t
						       dst);


	void            send_INTERNAL_EEPROM_READ_REQUEST(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t addr_lsb,
							  uint8_t
							  addr_msb);
	void            recv_INTERNAL_EEPROM_READ_REQUEST(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t *
							  addr_lsb,
							  uint8_t *
							  addr_msb);


	void            send_INTERNAL_EEPROM_READ_REPLAY(const uint16_t
							 src,
							 const uint16_t
							 dst,
							 uint8_t value);
	void            recv_INTERNAL_EEPROM_READ_REPLAY(const uint16_t
							 src,
							 const uint16_t
							 dst,
							 uint8_t * value);


	void            send_INTERNAL_EEPROM_READ_BULK_REQUEST(const
							       uint16_t
							       src,
							       const
							       uint16_t
							       dst,
							       uint8_t
							       addr_lsb,
							       uint8_t
							       addr_msb);
	void            recv_INTERNAL_EEPROM_READ_BULK_REQUEST(const
							       uint16_t
							       src,
							       const
							       uint16_t
							       dst,
							       uint8_t *
							       addr_lsb,
							       uint8_t *
							       addr_msb);


	void            send_INTERNAL_EEPROM_READ_BULK_REPLAY(const
							      uint16_t src,
							      const
							      uint16_t dst,
							      uint8_t d0,
							      uint8_t d1,
							      uint8_t d2,
							      uint8_t d3,
							      uint8_t d4,
							      uint8_t d5);
	void            recv_INTERNAL_EEPROM_READ_BULK_REPLAY(const
							      uint16_t src,
							      const
							      uint16_t dst,
							      uint8_t * d0,
							      uint8_t * d1,
							      uint8_t * d2,
							      uint8_t * d3,
							      uint8_t * d4,
							      uint8_t *
							      d5);


	void            send_UPTIME_QUERY(const uint16_t src,
					  const uint16_t dst);
	void            recv_UPTIME_QUERY(const uint16_t src,
					  const uint16_t dst);


	void            send_UPTIME_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t u3,
					   uint8_t u2,
					   uint8_t u1, uint8_t u0);
	void            recv_UPTIME_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * u3,
					   uint8_t * u2,
					   uint8_t * u1, uint8_t * u0);


	void            send_SYSLOG_LEVEL_SET(const uint16_t src,
					      const uint16_t dst,
					      uint8_t level);
	void            recv_SYSLOG_LEVEL_SET(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * level);


	void            send_BUILD_VERSION_QUERY(const uint16_t src,
						 const uint16_t dst);
	void            recv_BUILD_VERSION_QUERY(const uint16_t src,
						 const uint16_t dst);


	void            send_BUILD_VERSION_REPLAY(const uint16_t src,
						  const uint16_t dst,
						  uint8_t hi, uint8_t lo);
	void            recv_BUILD_VERSION_REPLAY(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * hi,
						  uint8_t * lo);


	void            send_CAN_EC_QUERY(const uint16_t src,
					  const uint16_t dst);
	void            recv_CAN_EC_QUERY(const uint16_t src,
					  const uint16_t dst);


	void            send_CAN_EC_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t hi, uint8_t lo);
	void            recv_CAN_EC_REPLAY(const uint16_t src,
					   const uint16_t dst,
					   uint8_t * hi, uint8_t * lo);


	void            send_FLASH_CRC16_QUERY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t hi, uint8_t lo);
	void            recv_FLASH_CRC16_QUERY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * hi,
					       uint8_t * lo);


	void            send_FLASH_CRC16_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t hi, uint8_t lo);
	void            recv_FLASH_CRC16_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * hi,
						uint8_t * lo);


	void            send_LOOP_THROUGHPUT_QUERY(const uint16_t src,
						   const uint16_t dst);
	void            recv_LOOP_THROUGHPUT_QUERY(const uint16_t src,
						   const uint16_t dst);


	void            send_LOOP_THROUGHPUT_REPLAY(const uint16_t src,
						    const uint16_t dst,
						    uint8_t hi,
						    uint8_t lo);
	void            recv_LOOP_THROUGHPUT_REPLAY(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * hi,
						    uint8_t * lo);


	void            send_RX_RECEIVED_QUERY(const uint16_t src,
					       const uint16_t dst);
	void            recv_RX_RECEIVED_QUERY(const uint16_t src,
					       const uint16_t dst);


	void            send_RX_RECEIVED_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t d0,
						uint8_t d1,
						uint8_t d2, uint8_t d3);
	void            recv_RX_RECEIVED_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * d0,
						uint8_t * d1,
						uint8_t * d2,
						uint8_t * d3);


	void            send_RX_LOST_QUERY(const uint16_t src,
					   const uint16_t dst);
	void            recv_RX_LOST_QUERY(const uint16_t src,
					   const uint16_t dst);


	void            send_RX_LOST_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t d0,
					    uint8_t d1,
					    uint8_t d2, uint8_t d3);
	void            recv_RX_LOST_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * d0,
					    uint8_t * d1,
					    uint8_t * d2, uint8_t * d3);


	void            send_TX_SENT_QUERY(const uint16_t src,
					   const uint16_t dst);
	void            recv_TX_SENT_QUERY(const uint16_t src,
					   const uint16_t dst);


	void            send_TX_SENT_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t d0,
					    uint8_t d1,
					    uint8_t d2, uint8_t d3);
	void            recv_TX_SENT_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * d0,
					    uint8_t * d1,
					    uint8_t * d2, uint8_t * d3);


	void            send_TX_DROPPED_QUERY(const uint16_t src,
					      const uint16_t dst);
	void            recv_TX_DROPPED_QUERY(const uint16_t src,
					      const uint16_t dst);


	void            send_TX_DROPPED_REPLAY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t d0,
					       uint8_t d1,
					       uint8_t d2, uint8_t d3);
	void            recv_TX_DROPPED_REPLAY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * d0,
					       uint8_t * d1,
					       uint8_t * d2,
					       uint8_t * d3);


	void            send_RX_TX_STATS_RESET(const uint16_t src,
					       const uint16_t dst);
	void            recv_RX_TX_STATS_RESET(const uint16_t src,
					       const uint16_t dst);


	void            send_BOOT_RESETFLAG_LOG(const uint16_t src,
						const uint16_t dst,
						uint8_t flag);
	void            recv_BOOT_RESETFLAG_LOG(const uint16_t src,
						const uint16_t dst,
						uint8_t * flag);


	void            send_FIRMWARE_CRC16_ERROR(const uint16_t src,
						  const uint16_t dst);
	void            recv_FIRMWARE_CRC16_ERROR(const uint16_t src,
						  const uint16_t dst);


	void            send_FIRMWARE_CONFIG_COMPAT_ERROR(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t
							  eds_block_type,
							  uint8_t
							  eds_addr_hi,
							  uint8_t
							  eds_addr_lo,
							  uint8_t
							  size_in_config,
							  uint8_t
							  size_in_firmware);
	void            recv_FIRMWARE_CONFIG_COMPAT_ERROR(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t *
							  eds_block_type,
							  uint8_t *
							  eds_addr_hi,
							  uint8_t *
							  eds_addr_lo,
							  uint8_t *
							  size_in_config,
							  uint8_t *
							  size_in_firmware);


	void            send_DUMP_DEBUG_INFO(const uint16_t src,
					     const uint16_t dst,
					     uint8_t context);
	void            recv_DUMP_DEBUG_INFO(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * context);


	void            send_TASTER_DOWN(const uint16_t src,
					 const uint16_t dst,
					 uint8_t gruppe, uint8_t index);
	void            recv_TASTER_DOWN(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * gruppe,
					 uint8_t * index);


	void            send_TASTER_UP(const uint16_t src,
				       const uint16_t dst,
				       uint8_t gruppe, uint8_t index);
	void            recv_TASTER_UP(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * gruppe, uint8_t * index);


	void            send_BOARD_ACTIVE(const uint16_t src,
					  const uint16_t dst);
	void            recv_BOARD_ACTIVE(const uint16_t src,
					  const uint16_t dst);


	void            send_POWER_GROUP_ON(const uint16_t src,
					    const uint16_t dst,
					    uint8_t gruppe);
	void            recv_POWER_GROUP_ON(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * gruppe);


	void            send_POWER_GROUP_OFF(const uint16_t src,
					     const uint16_t dst,
					     uint8_t gruppe);
	void            recv_POWER_GROUP_OFF(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * gruppe);


	void            send_POWER_GROUP_STATE_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t gruppe);
	void            recv_POWER_GROUP_STATE_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * gruppe);


	void            send_POWER_GROUP_STATE_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t status,
						      uint8_t timer);
	void            recv_POWER_GROUP_STATE_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * status,
						      uint8_t * timer);


	void            send_POWER_GROUP_TIMER_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t gruppe);
	void            recv_POWER_GROUP_TIMER_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * gruppe);


	void            send_POWER_GROUP_TIMER_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t timer_hi,
						      uint8_t timer_lo);
	void            recv_POWER_GROUP_TIMER_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * timer_hi,
						      uint8_t * timer_lo);


	void            send_POWER_GROUP_SET_TIMER(const uint16_t src,
						   const uint16_t dst,
						   uint8_t gruppe,
						   uint8_t timer_hi,
						   uint8_t timer_lo);
	void            recv_POWER_GROUP_SET_TIMER(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * gruppe,
						   uint8_t * timer_hi,
						   uint8_t * timer_lo);


	void            send_POWER_GROUP_STATE_INFO(const uint16_t src,
						    const uint16_t dst,
						    uint8_t gruppe,
						    uint8_t status);
	void            recv_POWER_GROUP_STATE_INFO(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * gruppe,
						    uint8_t * status);


	void            send_ROLLADEN_POSITION_SET(const uint16_t src,
						   const uint16_t dst,
						   uint8_t gruppe,
						   uint8_t position);
	void            recv_ROLLADEN_POSITION_SET(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * gruppe,
						   uint8_t * position);


	void            send_ROLLADEN_POSITION_REQUEST(const uint16_t src,
						       const uint16_t dst,
						       uint8_t gruppe);
	void            recv_ROLLADEN_POSITION_REQUEST(const uint16_t src,
						       const uint16_t dst,
						       uint8_t * gruppe);


	void            send_ROLLADEN_POSITION_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t position);
	void            recv_ROLLADEN_POSITION_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * position);


	void            send_ROLLADEN_DEFINE_POSITION(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t position);
	void            recv_ROLLADEN_DEFINE_POSITION(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * position);


	void            send_ROLLADEN_POSITION_CHANGED_INFO(const uint16_t
							    src,
							    const uint16_t
							    dst,
							    uint8_t gruppe,
							    uint8_t
							    position,
							    uint8_t
							    cmdsrc_hi,
							    uint8_t
							    cmdsrc_lo);
	void            recv_ROLLADEN_POSITION_CHANGED_INFO(const uint16_t
							    src,
							    const uint16_t
							    dst,
							    uint8_t *
							    gruppe,
							    uint8_t *
							    position,
							    uint8_t *
							    cmdsrc_hi,
							    uint8_t *
							    cmdsrc_lo);


	void            send_SCHALTER_ON(const uint16_t src,
					 const uint16_t dst,
					 uint8_t gruppe);
	void            recv_SCHALTER_ON(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * gruppe);


	void            send_SCHALTER_OFF(const uint16_t src,
					  const uint16_t dst,
					  uint8_t gruppe);
	void            recv_SCHALTER_OFF(const uint16_t src,
					  const uint16_t dst,
					  uint8_t * gruppe);


	void            send_MUTE_ON(const uint16_t src,
				     const uint16_t dst, uint8_t gruppe);
	void            recv_MUTE_ON(const uint16_t src,
				     const uint16_t dst,
				     uint8_t * gruppe);


	void            send_MUTE_OFF(const uint16_t src,
				      const uint16_t dst, uint8_t gruppe);
	void            recv_MUTE_OFF(const uint16_t src,
				      const uint16_t dst,
				      uint8_t * gruppe);


	void            send_1WIRE_DISCOVER(const uint16_t src,
					    const uint16_t dst,
					    uint8_t pin);
	void            recv_1WIRE_DISCOVER(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * pin);


	void            send_1WIRE_DISCOVERED_PART_1(const uint16_t src,
						     const uint16_t dst,
						     uint8_t id0,
						     uint8_t id1,
						     uint8_t id2,
						     uint8_t id3);
	void            recv_1WIRE_DISCOVERED_PART_1(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * id0,
						     uint8_t * id1,
						     uint8_t * id2,
						     uint8_t * id3);


	void            send_1WIRE_DISCOVERED_PART_2(const uint16_t src,
						     const uint16_t dst,
						     uint8_t id4,
						     uint8_t id5,
						     uint8_t id6,
						     uint8_t id7);
	void            recv_1WIRE_DISCOVERED_PART_2(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * id4,
						     uint8_t * id5,
						     uint8_t * id6,
						     uint8_t * id7);


	void            send_1WIRE_ERROR(const uint16_t src,
					 const uint16_t dst,
					 uint8_t error);
	void            recv_1WIRE_ERROR(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * error);


	void            send_1WIRE_TEMPERATURE(const uint16_t src,
					       const uint16_t dst,
					       uint8_t gruppe,
					       uint8_t temp_hi,
					       uint8_t temp_lo);
	void            recv_1WIRE_TEMPERATURE(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * gruppe,
					       uint8_t * temp_hi,
					       uint8_t * temp_lo);


	void            send_1WIRE_TEMPERATURE_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t gruppe);
	void            recv_1WIRE_TEMPERATURE_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * gruppe);


	void            send_1WIRE_TEMPERATURE_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t temp_hi,
						      uint8_t temp_lo);
	void            recv_1WIRE_TEMPERATURE_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * temp_hi,
						      uint8_t * temp_lo);


	void            send_REEDKONTAKT_OFFEN(const uint16_t src,
					       const uint16_t dst,
					       uint8_t gruppe);
	void            recv_REEDKONTAKT_OFFEN(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * gruppe);


	void            send_REEDKONTAKT_STATE_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t gruppe);
	void            recv_REEDKONTAKT_STATE_QUERY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * gruppe);


	void            send_REEDKONTAKT_STATE_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t state);
	void            recv_REEDKONTAKT_STATE_REPLAY(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * state);


	void            send_REEDKONTAKT_STATE_CHANGE(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe,
						      uint8_t state);
	void            recv_REEDKONTAKT_STATE_CHANGE(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe,
						      uint8_t * state);


	void            send_PVSENSOR_STATE_REQUEST(const uint16_t src,
						    const uint16_t dst,
						    uint8_t gruppe);
	void            recv_PVSENSOR_STATE_REQUEST(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * gruppe);


	void            send_PVSENSOR_STATE_REPLAY(const uint16_t src,
						   const uint16_t dst,
						   uint8_t gruppe,
						   uint8_t state);
	void            recv_PVSENSOR_STATE_REPLAY(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * gruppe,
						   uint8_t * state);


	void            send_PVSENSOR_STATE_CHANGE_INFO(const uint16_t src,
							const uint16_t dst,
							uint8_t gruppe,
							uint8_t state);
	void            recv_PVSENSOR_STATE_CHANGE_INFO(const uint16_t src,
							const uint16_t dst,
							uint8_t * gruppe,
							uint8_t * state);


	void            send_HEIZUNG_DETAILS_REQUEST(const uint16_t src,
						     const uint16_t dst,
						     uint8_t id);
	void            recv_HEIZUNG_DETAILS_REQUEST(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * id);


	void            send_HEIZUNG_MODE_OFF_DETAILS(const uint16_t src,
						      const uint16_t dst,
						      uint8_t id);
	void            recv_HEIZUNG_MODE_OFF_DETAILS(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * id);


	void            send_HEIZUNG_SET_MODE_OFF(const uint16_t src,
						  const uint16_t dst,
						  uint8_t id);
	void            recv_HEIZUNG_SET_MODE_OFF(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * id);


	void            send_HEIZUNG_MODE_MANUAL_DETAILS(const uint16_t
							 src,
							 const uint16_t
							 dst, uint8_t id,
							 uint8_t rate,
							 uint8_t
							 duration_hi,
							 uint8_t
							 duration_lo);
	void            recv_HEIZUNG_MODE_MANUAL_DETAILS(const uint16_t
							 src,
							 const uint16_t
							 dst, uint8_t * id,
							 uint8_t * rate,
							 uint8_t *
							 duration_hi,
							 uint8_t *
							 duration_lo);


	void            send_HEIZUNG_SET_MODE_MANUAL(const uint16_t src,
						     const uint16_t dst,
						     uint8_t id,
						     uint8_t rate,
						     uint8_t duration_hi,
						     uint8_t duration_lo);
	void            recv_HEIZUNG_SET_MODE_MANUAL(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * id,
						     uint8_t * rate,
						     uint8_t * duration_hi,
						     uint8_t *
						     duration_lo);


	void            send_HEIZUNG_MODE_THERMOSTAT_DETAILS(const uint16_t
							     src,
							     const uint16_t
							     dst,
							     uint8_t id,
							     uint8_t
							     temp_hi,
							     uint8_t
							     temp_lo,
							     uint8_t
							     duration_hi,
							     uint8_t
							     duration_lo);
	void            recv_HEIZUNG_MODE_THERMOSTAT_DETAILS(const uint16_t
							     src,
							     const uint16_t
							     dst,
							     uint8_t * id,
							     uint8_t *
							     temp_hi,
							     uint8_t *
							     temp_lo,
							     uint8_t *
							     duration_hi,
							     uint8_t *
							     duration_lo);


	void            send_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS(const
								 uint16_t
								 src,
								 const
								 uint16_t
								 dst,
								 uint8_t
								 id,
								 uint8_t
								 temp_hi,
								 uint8_t
								 temp_lo,
								 uint8_t
								 duration_hi,
								 uint8_t
								 duration_lo);
	void            recv_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS(const
								 uint16_t
								 src,
								 const
								 uint16_t
								 dst,
								 uint8_t *
								 id,
								 uint8_t *
								 temp_hi,
								 uint8_t *
								 temp_lo,
								 uint8_t *
								 duration_hi,
								 uint8_t *
								 duration_lo);


	void            send_HEIZUNG_MODE_AUTOMATIK_DETAILS(const uint16_t
							    src,
							    const uint16_t
							    dst,
							    uint8_t id,
							    uint8_t
							    temp_hi,
							    uint8_t
							    temp_lo,
							    uint8_t
							    timerange_id);
	void            recv_HEIZUNG_MODE_AUTOMATIK_DETAILS(const uint16_t
							    src,
							    const uint16_t
							    dst,
							    uint8_t * id,
							    uint8_t *
							    temp_hi,
							    uint8_t *
							    temp_lo,
							    uint8_t *
							    timerange_id);


	void            send_HEIZUNG_SET_MODE_AUTOMATIK(const uint16_t src,
							const uint16_t dst,
							uint8_t id);
	void            recv_HEIZUNG_SET_MODE_AUTOMATIK(const uint16_t src,
							const uint16_t dst,
							uint8_t * id);


	void            send_HEIZUNG_TIST_REQUEST(const uint16_t src,
						  const uint16_t dst,
						  uint8_t id);
	void            recv_HEIZUNG_TIST_REQUEST(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * id);


	void            send_HEIZUNG_TIST_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t id,
						 uint8_t temp_hi,
						 uint8_t temp_lo);
	void            recv_HEIZUNG_TIST_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * id,
						 uint8_t * temp_hi,
						 uint8_t * temp_lo);


	void            send_HEIZUNG_WAERMEBEDARF_INFO(const uint16_t src,
						       const uint16_t dst,
						       uint8_t id,
						       uint8_t value);
	void            recv_HEIZUNG_WAERMEBEDARF_INFO(const uint16_t src,
						       const uint16_t dst,
						       uint8_t * id,
						       uint8_t * value);


	void            send_HEIZSTEUERUNG_STATE_REQUEST(const uint16_t
							 src,
							 const uint16_t
							 dst);
	void            recv_HEIZSTEUERUNG_STATE_REQUEST(const uint16_t
							 src,
							 const uint16_t
							 dst);


	void            send_HEIZSTEUERUNG_STATE_REPLAY(const uint16_t src,
							const uint16_t dst,
							uint8_t VL_soll,
							uint8_t VL_ist,
							uint8_t RL_ist,
							uint8_t BF);
	void            recv_HEIZSTEUERUNG_STATE_REPLAY(const uint16_t src,
							const uint16_t dst,
							uint8_t * VL_soll,
							uint8_t * VL_ist,
							uint8_t * RL_ist,
							uint8_t * BF);


	void            send_HEIZSTEUERUNG_STATE_INFO(const uint16_t src,
						      const uint16_t dst,
						      uint8_t VL_soll,
						      uint8_t GWB_hi,
						      uint8_t GWB_lo,
						      uint8_t BF);
	void            recv_HEIZSTEUERUNG_STATE_INFO(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * VL_soll,
						      uint8_t * GWB_hi,
						      uint8_t * GWB_lo,
						      uint8_t * BF);


	void            send_HEIZSTEUERUNG_SET_VL(const uint16_t src,
						  const uint16_t dst,
						  uint8_t VL_soll);
	void            recv_HEIZSTEUERUNG_SET_VL(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * VL_soll);


	void            send_HEIZUNG_SOLLTEMP_LINE_REQUEST(const uint16_t
							   src,
							   const uint16_t
							   dst, uint8_t id,
							   uint8_t idx);
	void            recv_HEIZUNG_SOLLTEMP_LINE_REQUEST(const uint16_t
							   src,
							   const uint16_t
							   dst,
							   uint8_t * id,
							   uint8_t * idx);


	void            send_HEIZUNG_SOLLTEMP_LINE_REPLAY(const uint16_t
							  src,
							  const uint16_t
							  dst, uint8_t id,
							  uint8_t idx,
							  uint8_t temp_hi,
							  uint8_t temp_lo,
							  uint8_t
							  zeitzone_id);
	void            recv_HEIZUNG_SOLLTEMP_LINE_REPLAY(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t * id,
							  uint8_t * idx,
							  uint8_t *
							  temp_hi,
							  uint8_t *
							  temp_lo,
							  uint8_t *
							  zeitzone_id);


	void            send_HEIZUNG_SOLLTEMP_WRITE_LINE(const uint16_t
							 src,
							 const uint16_t
							 dst, uint8_t id,
							 uint8_t idx,
							 uint8_t temp_hi,
							 uint8_t temp_lo,
							 uint8_t
							 zeitzone_id);
	void            recv_HEIZUNG_SOLLTEMP_WRITE_LINE(const uint16_t
							 src,
							 const uint16_t
							 dst, uint8_t * id,
							 uint8_t * idx,
							 uint8_t * temp_hi,
							 uint8_t * temp_lo,
							 uint8_t *
							 zeitzone_id);


	void            send_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK(const uint16_t
							     src,
							     const uint16_t
							     dst,
							     uint8_t id,
							     uint8_t idx);
	void            recv_HEIZUNG_SOLLTEMP_WRITE_LINE_ACK(const uint16_t
							     src,
							     const uint16_t
							     dst,
							     uint8_t * id,
							     uint8_t *
							     idx);


	void            send_WECKER_SET(const uint16_t src,
					const uint16_t dst,
					uint8_t id,
					uint8_t duration_hi,
					uint8_t duration_lo);
	void            recv_WECKER_SET(const uint16_t src,
					const uint16_t dst,
					uint8_t * id,
					uint8_t * duration_hi,
					uint8_t * duration_lo);


	void            send_WECKER_DETAILS_REQUEST(const uint16_t src,
						    const uint16_t dst,
						    uint8_t id);
	void            recv_WECKER_DETAILS_REQUEST(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * id);


	void            send_WECKER_DETAILS(const uint16_t src,
					    const uint16_t dst,
					    uint8_t id,
					    uint8_t duration_hi,
					    uint8_t duration_lo);
	void            recv_WECKER_DETAILS(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * id,
					    uint8_t * duration_hi,
					    uint8_t * duration_lo);


	void            send_HELLIGKEITS_INFO(const uint16_t src,
					      const uint16_t dst,
					      uint8_t guppe,
					      uint8_t brightness_hi,
					      uint8_t brightness_lo);
	void            recv_HELLIGKEITS_INFO(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * guppe,
					      uint8_t * brightness_hi,
					      uint8_t * brightness_lo);


	void            send_POTI_POS_CHANGED(const uint16_t src,
					      const uint16_t dst,
					      uint8_t guppe,
					      uint8_t pos_hi,
					      uint8_t pos_lo);
	void            recv_POTI_POS_CHANGED(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * guppe,
					      uint8_t * pos_hi,
					      uint8_t * pos_lo);


	void            send_DCF77_ERROR(const uint16_t src,
					 const uint16_t dst,
					 uint8_t id, uint8_t error);
	void            recv_DCF77_ERROR(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * id, uint8_t * error);


	void            send_DCF77_PULSE_DURATION(const uint16_t src,
						  const uint16_t dst,
						  uint8_t id,
						  uint8_t length);
	void            recv_DCF77_PULSE_DURATION(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * id,
						  uint8_t * length);


	void            send_CONFIG_RELOAD(const uint16_t src,
					   const uint16_t dst);
	void            recv_CONFIG_RELOAD(const uint16_t src,
					   const uint16_t dst);


	void            send_CONFIG_RAM_USAGE_REQUEST(const uint16_t src,
						      const uint16_t dst);
	void            recv_CONFIG_RAM_USAGE_REQUEST(const uint16_t src,
						      const uint16_t dst);


	void            send_CONFIG_RAM_USAGE_REPLAY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t usage_hi,
						     uint8_t usage_lo);
	void            recv_CONFIG_RAM_USAGE_REPLAY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * usage_hi,
						     uint8_t * usage_lo);


	void            send_ZEITZONE_DETAILS_REQUEST(const uint16_t src,
						      const uint16_t dst,
						      uint8_t gruppe);
	void            recv_ZEITZONE_DETAILS_REQUEST(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * gruppe);


	void            send_ZEITZONE_DETAILS_REPLAY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t gruppe,
						     uint8_t day_pattern,
						     uint8_t from_hour,
						     uint8_t from_minute,
						     uint8_t to_hour,
						     uint8_t to_minute);
	void            recv_ZEITZONE_DETAILS_REPLAY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * gruppe,
						     uint8_t * day_pattern,
						     uint8_t * from_hour,
						     uint8_t * from_minute,
						     uint8_t * to_hour,
						     uint8_t * to_minute);


	void            send_ZEITZONE_WRITE_DETAILS(const uint16_t src,
						    const uint16_t dst,
						    uint8_t gruppe,
						    uint8_t day_pattern,
						    uint8_t from_hour,
						    uint8_t from_minute,
						    uint8_t to_hour,
						    uint8_t to_minute);
	void            recv_ZEITZONE_WRITE_DETAILS(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * gruppe,
						    uint8_t * day_pattern,
						    uint8_t * from_hour,
						    uint8_t * from_minute,
						    uint8_t * to_hour,
						    uint8_t * to_minute);


	void            send_ZEITZONE_WRITE_DETAILS_ACK(const uint16_t src,
							const uint16_t dst,
							uint8_t gruppe);
	void            recv_ZEITZONE_WRITE_DETAILS_ACK(const uint16_t src,
							const uint16_t dst,
							uint8_t * gruppe);


	void            send_ZEITZONE_TEST_MATCH_REQUERST(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t gruppe);
	void            recv_ZEITZONE_TEST_MATCH_REQUERST(const uint16_t
							  src,
							  const uint16_t
							  dst,
							  uint8_t *
							  gruppe);


	void            send_ZEITZONE_TEST_MATCH_REPLAY(const uint16_t src,
							const uint16_t dst,
							uint8_t gruppe,
							uint8_t match);
	void            recv_ZEITZONE_TEST_MATCH_REPLAY(const uint16_t src,
							const uint16_t dst,
							uint8_t * gruppe,
							uint8_t * match);


	void            send_EXPANDER_NOT_FOUND(const uint16_t src,
						const uint16_t dst,
						uint8_t address);
	void            recv_EXPANDER_NOT_FOUND(const uint16_t src,
						const uint16_t dst,
						uint8_t * address);


	void            send_ADDITIONAL_PORTS_DEVICE_IGNORED(const uint16_t
							     src,
							     const uint16_t
							     dst);
	void            recv_ADDITIONAL_PORTS_DEVICE_IGNORED(const uint16_t
							     src,
							     const uint16_t
							     dst);


	void            send_TIME_INFO(const uint16_t src,
				       const uint16_t dst,
				       uint8_t level,
				       uint8_t day_of_week,
				       uint8_t hour,
				       uint8_t minute, uint8_t second);
	void            recv_TIME_INFO(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * level,
				       uint8_t * day_of_week,
				       uint8_t * hour,
				       uint8_t * minute,
				       uint8_t * second);


	void            send_DATE_INFO(const uint16_t src,
				       const uint16_t dst,
				       uint8_t level,
				       uint8_t day_of_month,
				       uint8_t month_of_year,
				       uint8_t year);
	void            recv_DATE_INFO(const uint16_t src,
				       const uint16_t dst,
				       uint8_t * level,
				       uint8_t * day_of_month,
				       uint8_t * month_of_year,
				       uint8_t * year);


	void            send_TIME_REQUEST(const uint16_t src,
					  const uint16_t dst);
	void            recv_TIME_REQUEST(const uint16_t src,
					  const uint16_t dst);


	void            send_DATE_REQUEST(const uint16_t src,
					  const uint16_t dst);
	void            recv_DATE_REQUEST(const uint16_t src,
					  const uint16_t dst);


	void            send_GET_NEXT_BLOCK(const uint16_t src,
					    const uint16_t dst,
					    uint8_t handle_hi,
					    uint8_t handle_lo);
	void            recv_GET_NEXT_BLOCK(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * handle_hi,
					    uint8_t * handle_lo);


	void            send_GET_NEXT_BLOCK_REPLAY(const uint16_t src,
						   const uint16_t dst,
						   uint8_t handle_hi,
						   uint8_t handle_lo,
						   uint8_t type,
						   uint8_t size);
	void            recv_GET_NEXT_BLOCK_REPLAY(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * handle_hi,
						   uint8_t * handle_lo,
						   uint8_t * type,
						   uint8_t * size);


	void            send_DEFRAGMENT(const uint16_t src,
					const uint16_t dst);
	void            recv_DEFRAGMENT(const uint16_t src,
					const uint16_t dst);


	void            send_ALLOCATE_BLOCK(const uint16_t src,
					    const uint16_t dst,
					    uint8_t type, uint8_t size);
	void            recv_ALLOCATE_BLOCK(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * type,
					    uint8_t * size);


	void            send_ALLOCATE_BLOCK_REPLAY(const uint16_t src,
						   const uint16_t dst,
						   uint8_t handle_hi,
						   uint8_t handle_lo);
	void            recv_ALLOCATE_BLOCK_REPLAY(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * handle_hi,
						   uint8_t * handle_lo);


	void            send_FREE_BLOCK(const uint16_t src,
					const uint16_t dst,
					uint8_t handle_hi,
					uint8_t handle_lo);
	void            recv_FREE_BLOCK(const uint16_t src,
					const uint16_t dst,
					uint8_t * handle_hi,
					uint8_t * handle_lo);


	void            send_FREE_BLOCK_REPLAY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t result);
	void            recv_FREE_BLOCK_REPLAY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * result);


	void            send_READ_FROM_BLOCK(const uint16_t src,
					     const uint16_t dst,
					     uint8_t handle_hi,
					     uint8_t handle_lo,
					     uint8_t index);
	void            recv_READ_FROM_BLOCK(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * handle_hi,
					     uint8_t * handle_lo,
					     uint8_t * index);


	void            send_READ_FROM_BLOCK_REPLAY(const uint16_t src,
						    const uint16_t dst,
						    uint8_t d0,
						    uint8_t d1,
						    uint8_t d2,
						    uint8_t d3);
	void            recv_READ_FROM_BLOCK_REPLAY(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * d0,
						    uint8_t * d1,
						    uint8_t * d2,
						    uint8_t * d3);


	void            send_WRITE_TO_BLOCK(const uint16_t src,
					    const uint16_t dst,
					    uint8_t handle_hi,
					    uint8_t handle_lo,
					    uint8_t index, uint8_t d0);
	void            recv_WRITE_TO_BLOCK(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * handle_hi,
					    uint8_t * handle_lo,
					    uint8_t * index,
					    uint8_t * d0);


	void            send_WRITE_TO_BLOCK_REPLAY(const uint16_t src,
						   const uint16_t dst);
	void            recv_WRITE_TO_BLOCK_REPLAY(const uint16_t src,
						   const uint16_t dst);


	void            send_FORMAT(const uint16_t src,
				    const uint16_t dst);
	void            recv_FORMAT(const uint16_t src,
				    const uint16_t dst);


	void            send_STATE_REQUEST(const uint16_t src,
					   const uint16_t dst);
	void            recv_STATE_REQUEST(const uint16_t src,
					   const uint16_t dst);


	void            send_STATE_INFO(const uint16_t src,
					const uint16_t dst,
					uint8_t state);
	void            recv_STATE_INFO(const uint16_t src,
					const uint16_t dst,
					uint8_t * state);


	void            send_VOLTAGE_REQUEST(const uint16_t src,
					     const uint16_t dst,
					     uint8_t Ux);
	void            recv_VOLTAGE_REQUEST(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * Ux);


	void            send_VOLTAGE_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t Ux_hi, uint8_t Ux_lo);
	void            recv_VOLTAGE_REPLAY(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * Ux_hi,
					    uint8_t * Ux_lo);


	void            send_VOLTAGE_STATS_REQUEST(const uint16_t src,
						   const uint16_t dst,
						   uint8_t Ux);
	void            recv_VOLTAGE_STATS_REQUEST(const uint16_t src,
						   const uint16_t dst,
						   uint8_t * Ux);


	void            send_VOLTAGE_STATS_REPLAY(const uint16_t src,
						  const uint16_t dst,
						  uint8_t U,
						  uint8_t min_hi,
						  uint8_t min_lo,
						  uint8_t max_hi,
						  uint8_t max_lo);
	void            recv_VOLTAGE_STATS_REPLAY(const uint16_t src,
						  const uint16_t dst,
						  uint8_t * U,
						  uint8_t * min_hi,
						  uint8_t * min_lo,
						  uint8_t * max_hi,
						  uint8_t * max_lo);


	void            send_VOLTAGE_STATS_RESET(const uint16_t src,
						 const uint16_t dst);
	void            recv_VOLTAGE_STATS_RESET(const uint16_t src,
						 const uint16_t dst);


	void            send_FA_BLOCK1_INFO(const uint16_t src,
					    const uint16_t dst,
					    uint8_t status,
					    uint8_t stellgrad,
					    uint8_t KT,
					    uint8_t RT,
					    uint8_t BT, uint8_t AT);
	void            recv_FA_BLOCK1_INFO(const uint16_t src,
					    const uint16_t dst,
					    uint8_t * status,
					    uint8_t * stellgrad,
					    uint8_t * KT,
					    uint8_t * RT,
					    uint8_t * BT, uint8_t * AT);


	void            send_REGLER_DATEN_INFO_FRG1(const uint16_t src,
						    const uint16_t dst,
						    uint8_t status,
						    uint8_t aktion,
						    uint8_t
						    kesseltemp_soll_lo,
						    uint8_t
						    kesseltemp_soll_hi,
						    uint8_t
						    kesseldr_soll_lo,
						    uint8_t
						    kesseldr_soll_hi);
	void            recv_REGLER_DATEN_INFO_FRG1(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * status,
						    uint8_t * aktion,
						    uint8_t *
						    kesseltemp_soll_lo,
						    uint8_t *
						    kesseltemp_soll_hi,
						    uint8_t *
						    kesseldr_soll_lo,
						    uint8_t *
						    kesseldr_soll_hi);


	void            send_REGLER_DATEN_INFO_FRG2(const uint16_t src,
						    const uint16_t dst,
						    uint8_t stellgrad,
						    uint8_t ww_soll,
						    uint8_t brennstoff);
	void            recv_REGLER_DATEN_INFO_FRG2(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * stellgrad,
						    uint8_t * ww_soll,
						    uint8_t * brennstoff);


	void            send_WETTER_INFO(const uint16_t src,
					 const uint16_t dst,
					 uint8_t niederschlag,
					 uint8_t wind,
					 uint8_t licht_ost,
					 uint8_t licht_sued,
					 uint8_t licht_west);
	void            recv_WETTER_INFO(const uint16_t src,
					 const uint16_t dst,
					 uint8_t * niederschlag,
					 uint8_t * wind,
					 uint8_t * licht_ost,
					 uint8_t * licht_sued,
					 uint8_t * licht_west);


	void            send_LICHT_STATS_5MIN(const uint16_t src,
					      const uint16_t dst,
					      uint8_t licht_ost,
					      uint8_t licht_sued,
					      uint8_t licht_west);
	void            recv_LICHT_STATS_5MIN(const uint16_t src,
					      const uint16_t dst,
					      uint8_t * licht_ost,
					      uint8_t * licht_sued,
					      uint8_t * licht_west);


	void            send_WIND_STATS_5MIN(const uint16_t src,
					     const uint16_t dst,
					     uint8_t wind);
	void            recv_WIND_STATS_5MIN(const uint16_t src,
					     const uint16_t dst,
					     uint8_t * wind);


	void            send_OBJECT_COUNT_REQUEST(const uint16_t src,
						  const uint16_t dst);
	void            recv_OBJECT_COUNT_REQUEST(const uint16_t src,
						  const uint16_t dst);


	void            send_OBJECT_COUNT_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t count);
	void            recv_OBJECT_COUNT_REPLAY(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * count);


	void            send_OBJECT_ATTRIBUTE_REQUEST(const uint16_t src,
						      const uint16_t dst,
						      uint8_t oid);
	void            recv_OBJECT_ATTRIBUTE_REQUEST(const uint16_t src,
						      const uint16_t dst,
						      uint8_t * oid);


	void            send_OBJECT_ATTRIBUTE_REPLAY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t oid,
						     uint8_t oclass,
						     uint8_t p1,
						     uint8_t p2,
						     uint8_t p3,
						     uint8_t p4);
	void            recv_OBJECT_ATTRIBUTE_REPLAY(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * oid,
						     uint8_t * oclass,
						     uint8_t * p1,
						     uint8_t * p2,
						     uint8_t * p3,
						     uint8_t * p4);


	void            send_FIELD_COUNT_REQUEST(const uint16_t src,
						 const uint16_t dst,
						 uint8_t oid);
	void            recv_FIELD_COUNT_REQUEST(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * oid);


	void            send_FIELD_COUNT_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t oid,
						uint8_t count);
	void            recv_FIELD_COUNT_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * oid,
						uint8_t * count);


	void            send_FIELD_ATTRIBUTE_REQUEST(const uint16_t src,
						     const uint16_t dst,
						     uint8_t oid,
						     uint8_t fid);
	void            recv_FIELD_ATTRIBUTE_REQUEST(const uint16_t src,
						     const uint16_t dst,
						     uint8_t * oid,
						     uint8_t * fid);


	void            send_FIELD_ATTRIBUTE_REPLAY(const uint16_t src,
						    const uint16_t dst,
						    uint8_t oid,
						    uint8_t fid,
						    uint8_t fclass,
						    uint8_t flags,
						    uint8_t arraysize);
	void            recv_FIELD_ATTRIBUTE_REPLAY(const uint16_t src,
						    const uint16_t dst,
						    uint8_t * oid,
						    uint8_t * fid,
						    uint8_t * fclass,
						    uint8_t * flags,
						    uint8_t * arraysize);


	void            send_FIELD_READ_REQUEST(const uint16_t src,
						const uint16_t dst,
						uint8_t oid,
						uint8_t fid, uint8_t idx);
	void            recv_FIELD_READ_REQUEST(const uint16_t src,
						const uint16_t dst,
						uint8_t * oid,
						uint8_t * fid,
						uint8_t * idx);


	void            send_FIELD_READ_REPLAY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t oid,
					       uint8_t fid,
					       uint8_t idx,
					       uint8_t value_hi,
					       uint8_t value_lo);
	void            recv_FIELD_READ_REPLAY(const uint16_t src,
					       const uint16_t dst,
					       uint8_t * oid,
					       uint8_t * fid,
					       uint8_t * idx,
					       uint8_t * value_hi,
					       uint8_t * value_lo);


	void            send_FIELD_WRITE_REQUEST(const uint16_t src,
						 const uint16_t dst,
						 uint8_t oid,
						 uint8_t fid,
						 uint8_t idx,
						 uint8_t value_hi,
						 uint8_t value_lo);
	void            recv_FIELD_WRITE_REQUEST(const uint16_t src,
						 const uint16_t dst,
						 uint8_t * oid,
						 uint8_t * fid,
						 uint8_t * idx,
						 uint8_t * value_hi,
						 uint8_t * value_lo);


	void            send_FIELD_WRITE_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t oid,
						uint8_t fid, uint8_t idx);
	void            recv_FIELD_WRITE_REPLAY(const uint16_t src,
						const uint16_t dst,
						uint8_t * oid,
						uint8_t * fid,
						uint8_t * idx);


    };
};

#endif
