
#ifndef OAP_DEF_H
#define OAP_DEF_H

#ifndef true
#define true 1
#define false 0
#endif

#define OAP_TYPE_BOOL        1
#define OAP_TYPE_INT         2
#define OAP_TYPE_TIME        3

#define OAP_RW               0
#define OAP_PERSISTENT       1
#define OAP_ARRAY            2

#define OAP_FLAGS_RW_PERS_ARRAY    7
#define OAP_FLAGS_RW_PERS          3

// ------- AUTOMATICLY GENERATED FROM mk_oap_def.xsl -----------------



// class: taster
#define OAP_CLASS_taster   1

// class: powerport
#define OAP_CLASS_powerport   2

// class: dunstabzugport
#define OAP_CLASS_dunstabzugport   3

// class: lichtzone
#define OAP_CLASS_lichtzone   4

// class: tempsensor
#define OAP_CLASS_tempsensor   5

// class: reedkontakt
#define OAP_CLASS_reedkontakt   6

// class: rolladen
#define OAP_CLASS_rolladen   7

// class: zeitschaltuhr
#define OAP_CLASS_zeitschaltuhr   8

// class: zeitzone
#define OAP_CLASS_zeitzone   9

// class: rolladen_steuerung
#define OAP_CLASS_rolladen_steuerung   20
#define OAP_FIELD_rolladen_steuerung_enabled   0

// class: tag_nacht_agent
#define OAP_CLASS_tag_nacht_agent   21
#define OAP_FIELD_tag_nacht_agent_active   0
#define OAP_FIELD_tag_nacht_agent_tag_active   1
#define OAP_FIELD_tag_nacht_agent_nacht_active   2
#define OAP_FIELD_tag_nacht_agent_nacht_pos   3
#define OAP_FIELD_tag_nacht_agent_hell_grenze   4
#define OAP_FIELD_tag_nacht_agent_dunkel_grenze   5

// class: sonnenschutz_agent
#define OAP_CLASS_sonnenschutz_agent   22
#define OAP_FIELD_sonnenschutz_agent_active   0
#define OAP_FIELD_sonnenschutz_agent_ost_active   1
#define OAP_FIELD_sonnenschutz_agent_sued_active   2
#define OAP_FIELD_sonnenschutz_agent_west_active   3
#define OAP_FIELD_sonnenschutz_agent_soschu_pos   4
#define OAP_FIELD_sonnenschutz_agent_ost_to_time   5
#define OAP_FIELD_sonnenschutz_agent_sued_from_time   6
#define OAP_FIELD_sonnenschutz_agent_sued_to_time   7
#define OAP_FIELD_sonnenschutz_agent_west_from_time   8
#define OAP_FIELD_sonnenschutz_agent_ost_schwelle   9
#define OAP_FIELD_sonnenschutz_agent_sued_schwelle   10
#define OAP_FIELD_sonnenschutz_agent_west_schwelle   11
#define OAP_FIELD_sonnenschutz_agent_hysterese   12
#define OAP_FIELD_sonnenschutz_agent_flapinterval   13
#define OAP_FIELD_sonnenschutz_agent_licht_ost   14
#define OAP_FIELD_sonnenschutz_agent_licht_sued   15
#define OAP_FIELD_sonnenschutz_agent_licht_west   16

// class: prio_agent
#define OAP_CLASS_prio_agent   23

// class: state2event_agent
#define OAP_CLASS_state2event_agent   24

// class: zeitfilter_agent
#define OAP_CLASS_zeitfilter_agent   25

// class: flappingfilter_agent
#define OAP_CLASS_flappingfilter_agent   26

#endif
