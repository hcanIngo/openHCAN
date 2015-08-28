
#ifndef EDS_DEFINITIONS_H
#define EDS_DEFINITIONS_H

// ------- AUTOMATICLY GENERATED FROM mk_eds_structs.xsl
// ------------------

#include <inttypes.h>



typedef struct {
    uint8_t           port;

    uint8_t           feature;

    uint8_t           gruppe;


} eds_taster_block_t;

#define EDS_taster_BLOCK_ID 10

typedef struct {
    uint8_t           port;

    uint8_t           feature;

    uint8_t           gruppe0;

    uint8_t           gruppe1;

    uint8_t           gruppe2;

    uint8_t           gruppe3;


} eds_powerport_block_t;

#define EDS_powerport_BLOCK_ID 11

typedef struct {
    uint8_t           port_power;

    uint8_t           port_dir;

    uint8_t           taster;

    uint8_t           dummy;

    uint8_t           feature;

    uint16_t          laufzeit;

    uint16_t          max_rekalib;

    uint8_t           gruppe0;

    uint8_t           gruppe1;

    uint8_t           gruppe2;

    uint8_t           gruppe3;


} eds_rolladen_block_t;

#define EDS_rolladen_BLOCK_ID 12

typedef struct {
    uint8_t           id;

    uint8_t           port;

    uint8_t           pwm_periode;

    uint8_t           sensor_id;

    uint8_t           zeitzone0_id;

    uint16_t          zeitzone0_temp;

    uint8_t           zeitzone1_id;

    uint16_t          zeitzone1_temp;

    uint8_t           zeitzone2_id;

    uint16_t          zeitzone2_temp;

    uint8_t           zeitzone3_id;

    uint16_t          zeitzone3_temp;

    uint8_t           zeitzone4_id;

    uint16_t          zeitzone4_temp;

    uint8_t           zeitzone5_id;

    uint16_t          zeitzone5_temp;

    uint8_t           zeitzone6_id;

    uint16_t          zeitzone6_temp;

    uint8_t           zeitzone7_id;

    uint16_t          zeitzone7_temp;

    uint8_t           feature;

    uint8_t           hysterese;

    uint8_t           reed0;

    uint8_t           reed1;

    uint8_t           reed2;

    uint8_t           reed3;

    uint8_t           reed4;

    uint8_t           reed5;

    uint8_t           reed6;

    uint8_t           reed7;


} eds_heizung_block_t;

#define EDS_heizung_BLOCK_ID 13

typedef struct {
    uint8_t           io_pin;

    uint8_t           gruppe;

    uint8_t           modus;

    uint8_t           interval;

    char              ow_id[17];


} eds_tempsensor_block_t;

#define EDS_tempsensor_BLOCK_ID 14

typedef struct {
    uint8_t           port;

    uint8_t           gruppe;

    uint8_t           modus;

    uint8_t           feature;


} eds_reedkontakt_block_t;

#define EDS_reedkontakt_BLOCK_ID 15

typedef struct {
    uint8_t           level;

    uint8_t           takeover_time;

    uint8_t           interval;


} eds_timeservice_block_t;

#define EDS_timeservice_BLOCK_ID 16

typedef struct {
    uint8_t           toggle_gruppe0;

    uint8_t           toggle_gruppe1;

    uint8_t           toggle_gruppe2;

    uint8_t           toggle_gruppe3;

    uint8_t           off_gruppe0;

    uint8_t           off_gruppe1;

    uint8_t           on_gruppe0;

    uint8_t           on_gruppe1;

    uint8_t           timer_delay;

    uint8_t           action_gruppe0;

    uint8_t           action_gruppe1;


} eds_lichtzone_block_t;

#define EDS_lichtzone_BLOCK_ID 17

typedef struct {
    uint8_t           gruppe;

    uint8_t           day_pattern;

    uint8_t           from_hour;

    uint8_t           from_minute;

    uint8_t           to_hour;

    uint8_t           to_minute;


} eds_zeitzone_block_t;

#define EDS_zeitzone_BLOCK_ID 18

typedef struct {
    uint8_t           port;

    uint8_t           reed_gruppe0;

    uint8_t           freischalt_taster;

    uint8_t           freischalt_zeit;

    uint8_t           reed_gruppe1;


} eds_dunstabzugport_block_t;

#define EDS_dunstabzugport_BLOCK_ID 19

typedef struct {
    uint8_t           power_gruppe;

    uint8_t           automatikEin_schalter_gruppe;

    uint8_t           zeitzone0_id;

    uint8_t           zeitzone1_id;

    uint8_t           zeitzone2_id;

    uint8_t           zeitzone3_id;

    uint8_t           zeitzone4_id;

    uint8_t           zeitzone5_id;

    uint8_t           zeitzone6_id;

    uint8_t           zeitzone7_id;


} eds_zeitschaltuhr_block_t;

#define EDS_zeitschaltuhr_BLOCK_ID 20

typedef struct {
    uint8_t           port;

    uint8_t           gruppe;

    uint8_t           feature;


} eds_poti_block_t;

#define EDS_poti_BLOCK_ID 21

typedef struct {
    uint8_t           port;

    uint8_t           feedback_guppe;

    uint8_t           feature;

    uint8_t           gruppe0;

    uint8_t           gruppe1;

    uint8_t           gruppe2;

    uint8_t           gruppe3;

    uint8_t           gruppe4;

    uint8_t           gruppe5;

    uint8_t           gruppe6;

    uint8_t           gruppe7;


} eds_tastdimmer_block_t;

#define EDS_tastdimmer_BLOCK_ID 22

typedef struct {
    uint8_t           poti_gruppe;

    uint8_t           taster_port;

    uint8_t           feature;

    uint8_t           gruppe0;

    uint8_t           gruppe1;

    uint8_t           gruppe2;

    uint8_t           gruppe3;

    uint8_t           gruppe4;

    uint8_t           gruppe5;

    uint8_t           gruppe6;

    uint8_t           gruppe7;

    uint8_t           gruppe8;

    uint8_t           gruppe9;


} eds_multitaster_block_t;

#define EDS_multitaster_BLOCK_ID 23

typedef struct {
    uint8_t           reed0;

    uint8_t           reed1;

    uint8_t           rolladen;


} eds_rolladenSchlitzpos_block_t;

#define EDS_rolladenSchlitzpos_BLOCK_ID 24

typedef struct {
    uint8_t           port;

    uint8_t           feature;

    uint8_t           gruppe;


} eds_schalter_block_t;

#define EDS_schalter_BLOCK_ID 25

typedef struct {
    uint8_t           port;

    uint8_t           gruppe;

    uint8_t           feature;

    uint8_t           sendCycleMins;


} eds_helligkeitssensor_block_t;

#define EDS_helligkeitssensor_BLOCK_ID 26

typedef struct {
    uint8_t           rolladen_gruppe;

    uint8_t           rolladen_soll_position;

    uint8_t           automatikEin_schalter_gruppe;

    uint8_t           feature;

    uint8_t           zeitzone0_id;

    uint8_t           zeitzone1_id;

    uint8_t           helligkeits_gruppe;

    uint16_t          helligkeitsschwelle;

    uint8_t           helligkeitswertueberschreitung;


} eds_rolladenAutomat_block_t;

#define EDS_rolladenAutomat_BLOCK_ID 27

typedef struct {
    uint8_t           port;

    uint8_t           feature;

    uint8_t           gruppe;


} eds_zentralheizungspumpe_block_t;

#define EDS_zentralheizungspumpe_BLOCK_ID 28

typedef struct {
    uint8_t           powerport_gruppe;

    uint8_t           steller_gruppe;

    uint8_t           automatikEin_schalter_gruppe;

    uint8_t           feature;

    uint8_t           helligkeits_gruppe;

    uint16_t          helligkeitsschwelle;

    uint8_t           helligkeitswertueberschreitung;


} eds_powerportAutomat_block_t;

#define EDS_powerportAutomat_BLOCK_ID 29

typedef struct {
    uint8_t           roll_id_0;

    uint8_t           roll_id_1;

    uint8_t           roll_id_2;

    uint8_t           roll_id_3;

    uint8_t           roll_id_4;

    uint8_t           roll_id_5;

    uint8_t           roll_id_6;

    uint8_t           roll_id_7;

    uint8_t           roll_id_8;

    uint8_t           roll_id_9;

    uint8_t           roll_id_10;

    uint8_t           roll_id_11;

    uint8_t           roll_id_12;

    uint8_t           roll_id_13;

    uint8_t           roll_id_14;

    uint8_t           roll_id_15;

    uint8_t           roll_id_16;

    uint8_t           roll_id_17;

    uint8_t           roll_id_18;

    uint8_t           roll_id_19;


} eds_rolladen_id_assignment_block_t;

#define EDS_rolladen_id_assignment_BLOCK_ID 30

typedef struct {
    uint8_t           out0;

    uint8_t           process_order;

    uint8_t           roll_conf_0;

    uint8_t           roll_conf_1;

    uint8_t           roll_conf_2;

    uint8_t           roll_conf_3;

    uint8_t           roll_conf_4;

    uint8_t           roll_conf_5;

    uint8_t           roll_conf_6;

    uint8_t           roll_conf_7;

    uint8_t           roll_conf_8;

    uint8_t           roll_conf_9;

    uint8_t           roll_conf_10;

    uint8_t           roll_conf_11;

    uint8_t           roll_conf_12;

    uint8_t           roll_conf_13;

    uint8_t           roll_conf_14;

    uint8_t           roll_conf_15;

    uint8_t           roll_conf_16;

    uint8_t           roll_conf_17;

    uint8_t           roll_conf_18;

    uint8_t           roll_conf_19;

    uint8_t           hell_grenze;

    uint8_t           dunkel_grenze;

    uint8_t           dummy0;

    uint8_t           dummy1;

    uint8_t           dummy2;

    uint8_t           dummy3;

    uint8_t           active;


} eds_tag_nacht_agent_block_t;

#define EDS_tag_nacht_agent_BLOCK_ID 31

typedef struct {
    uint8_t           out0;

    uint8_t           process_order;

    uint8_t           roll_conf_0;

    uint8_t           roll_conf_1;

    uint8_t           roll_conf_2;

    uint8_t           roll_conf_3;

    uint8_t           roll_conf_4;

    uint8_t           roll_conf_5;

    uint8_t           roll_conf_6;

    uint8_t           roll_conf_7;

    uint8_t           roll_conf_8;

    uint8_t           roll_conf_9;

    uint8_t           roll_conf_10;

    uint8_t           roll_conf_11;

    uint8_t           roll_conf_12;

    uint8_t           roll_conf_13;

    uint8_t           roll_conf_14;

    uint8_t           roll_conf_15;

    uint8_t           roll_conf_16;

    uint8_t           roll_conf_17;

    uint8_t           roll_conf_18;

    uint8_t           roll_conf_19;

    uint8_t           ost_to_hour;

    uint8_t           ost_to_min;

    uint8_t           ost_schwelle;

    uint8_t           sued_from_hour;

    uint8_t           sued_from_min;

    uint8_t           sued_to_hour;

    uint8_t           sued_to_min;

    uint8_t           sued_schwelle;

    uint8_t           west_from_hour;

    uint8_t           west_from_min;

    uint8_t           west_schwelle;

    uint8_t           hysterese;

    uint8_t           flapinterval;

    uint8_t           active;


} eds_sonnenschutz_agent_block_t;

#define EDS_sonnenschutz_agent_BLOCK_ID 32

typedef struct {
    uint8_t           out0;

    uint8_t           process_order;


} eds_wetterschutz_agent_block_t;

#define EDS_wetterschutz_agent_BLOCK_ID 33

typedef struct {
    uint8_t           in0;

    uint8_t           in1;

    uint8_t           in2;

    uint8_t           out0;

    uint8_t           process_order;


} eds_prio_agent_block_t;

#define EDS_prio_agent_BLOCK_ID 34

typedef struct {
    uint8_t           in0;

    uint8_t           out0;

    uint8_t           process_order;


} eds_state2event_agent_block_t;

#define EDS_state2event_agent_BLOCK_ID 35

typedef struct {
    uint8_t           in0;

    uint8_t           out0;

    uint8_t           process_order;

    uint8_t           roll_idx_0;

    uint8_t           zz_id_0;

    uint8_t           roll_idx_1;

    uint8_t           zz_id_1;

    uint8_t           roll_idx_2;

    uint8_t           zz_id_2;

    uint8_t           roll_idx_3;

    uint8_t           zz_id_3;

    uint8_t           roll_idx_4;

    uint8_t           zz_id_4;

    uint8_t           roll_idx_5;

    uint8_t           zz_id_5;

    uint8_t           roll_idx_6;

    uint8_t           zz_id_6;

    uint8_t           roll_idx_7;

    uint8_t           zz_id_7;


} eds_zeitfilter_agent_block_t;

#define EDS_zeitfilter_agent_BLOCK_ID 36

typedef struct {
    uint8_t           in0;

    uint8_t           out0;

    uint8_t           process_order;


} eds_flappingfilter_agent_block_t;

#define EDS_flappingfilter_agent_BLOCK_ID 37

typedef struct {
    uint8_t           aktivierenGruppe;

    uint8_t           abschaltGruppe;

    uint8_t           feature;


} eds_analogComparator_block_t;

#define EDS_analogComparator_BLOCK_ID 38

typedef struct {
    uint8_t           base;

    uint8_t           expander0;

    uint8_t           expander1;

    uint8_t           expander2;

    uint8_t           expander3;


} eds_ports_block_t;

#define EDS_ports_BLOCK_ID 50

typedef struct {
    uint8_t           pwm0;

    uint8_t           pwm1;

    uint8_t           pwm2;

    uint8_t           pwm3;

    uint8_t           pwm4;

    uint8_t           pwm5;

    uint8_t           pwm6;

    uint8_t           pwm7;

    uint8_t           pwm8;

    uint8_t           pwm9;

    uint8_t           pwm10;

    uint8_t           pwm11;

    uint8_t           pwm12;

    uint8_t           pwm13;

    uint8_t           pwm14;

    uint8_t           pwm15;

    uint8_t           pwm16;

    uint8_t           pwm17;

    uint8_t           pwm18;

    uint8_t           pwm19;

    uint8_t           pwm20;

    uint8_t           pwm21;

    uint8_t           pwm22;

    uint8_t           pwm23;

    uint8_t           pwm_heizung_off;

    uint8_t           a;

    uint8_t           b;

    uint8_t           RL_max;

    uint8_t           VT_min;


} eds_heizungssteuerung_block_t;

#define EDS_heizungssteuerung_BLOCK_ID 90

typedef struct {
    uint8_t           offset_licht_ost;

    uint8_t           offset_licht_sued;

    uint8_t           offset_licht_west;

    uint8_t           dummy0;

    uint8_t           dummy1;

    uint8_t           dummy2;


} eds_wetterstation_data_block_t;

#define EDS_wetterstation_data_BLOCK_ID 91

typedef struct {
    uint8_t           zeitzone_id;

    uint8_t           backlight;

    uint8_t           contrast;


} eds_lcd_light_settings_block_t;

#define EDS_lcd_light_settings_BLOCK_ID 98

typedef struct {
    uint8_t           F1;

    uint8_t           F2;

    uint8_t           F3;

    uint8_t           F4;

    uint8_t           F5;

    uint8_t           F6;

    uint8_t           F7;

    uint8_t           F8;


} eds_userpanel_shortcuts_block_t;

#define EDS_userpanel_shortcuts_BLOCK_ID 99

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;


} eds_screensaver_page_block_t;

#define EDS_screensaver_page_BLOCK_ID 100

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    char              name[12];

    uint8_t           dummy;


} eds_dir_page_block_t;

#define EDS_dir_page_BLOCK_ID 101

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    char              name[8];

    uint8_t           heiz_id;


} eds_heiz_page_block_t;

#define EDS_heiz_page_BLOCK_ID 102

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    uint8_t           feature;

    uint8_t           gruppe;

    uint8_t           pos;

    char              line0[17];

    char              line1[17];


} eds_rolladen_page_block_t;

#define EDS_rolladen_page_BLOCK_ID 103

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    uint8_t           feature;

    uint8_t           gruppe;

    char              line0[17];

    char              line1[17];


} eds_taster_page_block_t;

#define EDS_taster_page_BLOCK_ID 104

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    uint8_t           feature;

    uint8_t           gruppe;

    char              line0[17];

    char              line1[9];


} eds_powerport_page_block_t;

#define EDS_powerport_page_BLOCK_ID 105

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    uint8_t           reed0;

    uint8_t           reed1;

    uint8_t           reed2;

    uint8_t           reed3;

    uint8_t           reed4;

    uint8_t           reed5;

    uint8_t           reed6;

    uint8_t           reed7;

    uint8_t           reed8;

    uint8_t           reed9;

    uint8_t           reed10;

    uint8_t           reed11;

    uint8_t           reed12;

    uint8_t           reed13;

    uint8_t           reed14;

    uint8_t           reed15;

    uint8_t           reed16;

    uint8_t           reed17;

    uint8_t           reed18;

    uint8_t           reed19;

    uint8_t           reed20;

    uint8_t           reed21;

    uint8_t           reed22;

    uint8_t           reed23;


} eds_fenster_monitor_page_block_t;

#define EDS_fenster_monitor_page_BLOCK_ID 107

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    uint8_t           feature;

    uint8_t           tempsensor_id;

    char              name[17];


} eds_temp_anzeige_page_block_t;

#define EDS_temp_anzeige_page_BLOCK_ID 108

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    uint8_t           feature;

    uint8_t           weck_id;

    char              name[8];


} eds_weck_page_block_t;

#define EDS_weck_page_BLOCK_ID 109

typedef struct {
    uint8_t           page_id;

    uint8_t           parent_id;

    uint8_t           next_id;

    uint8_t           prev_id;

    char              name[8];

    uint8_t           heiz_id0;

    uint8_t           heiz_id1;

    uint8_t           heiz_id2;

    uint8_t           heiz_id3;

    uint8_t           heiz_id4;

    uint8_t           heiz_id5;

    uint8_t           heiz_id6;

    uint8_t           heiz_id7;

    uint8_t           heiz_id8;

    uint8_t           heiz_id9;

    uint8_t           heiz_id10;

    uint8_t           heiz_id11;


} eds_gesamt_heiz_page_block_t;

#define EDS_gesamt_heiz_page_BLOCK_ID 110

typedef struct {
    uint8_t           level;

    uint16_t          takeover_time;

    uint8_t           dummy0;

    uint8_t           port;

    uint8_t           id;

    uint8_t           enable_error_reporting;


} eds_dcf77_receiver_block_t;

#define EDS_dcf77_receiver_BLOCK_ID 200

#endif
