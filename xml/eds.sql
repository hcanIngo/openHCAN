
-- AUTOMATICLY GENERATED FROM mk_eds_sql.xsl

		

drop table taster;
create table taster
(
	id serial primary key
	, id_board integer references board
	

,port   integer

,feature   integer

,gruppe   integer
);
	

drop table powerport;
create table powerport
(
	id serial primary key
	, id_board integer references board
	

,port   integer

,feature   integer

,gruppe0   integer

,gruppe1   integer

,gruppe2   integer

,gruppe3   integer
);
	

drop table rolladen;
create table rolladen
(
	id serial primary key
	, id_board integer references board
	

,port_power   integer

,port_dir   integer

,taster_ab   integer

,taster_auf   integer

,feature   integer

,laufzeit   integer

,max_rekalib   integer

,gruppe0   integer

,gruppe1   integer

,gruppe2   integer

,gruppe3   integer
);
	

drop table heizung;
create table heizung
(
	id serial primary key
	, id_board integer references board
	

,id   integer

,port   integer

,pwm_periode   integer

,sensor_id   integer

,zeitzone0_id   integer

,zeitzone0_temp   integer

,zeitzone1_id   integer

,zeitzone1_temp   integer

,zeitzone2_id   integer

,zeitzone2_temp   integer

,zeitzone3_id   integer

,zeitzone3_temp   integer

,zeitzone4_id   integer

,zeitzone4_temp   integer

,zeitzone5_id   integer

,zeitzone5_temp   integer

,zeitzone6_id   integer

,zeitzone6_temp   integer

,zeitzone7_id   integer

,zeitzone7_temp   integer

,feature   integer

,hysterese   integer

,reed0   integer

,reed1   integer

,reed2   integer

,reed3   integer

,reed4   integer

,reed5   integer

,reed6   integer

,reed7   integer
);
	

drop table temp_sensor;
create table temp_sensor
(
	id serial primary key
	, id_board integer references board
	

,io_pin   integer

,gruppe   integer

,modus   integer

,interval   integer

,ow_id   text
);
	

drop table reed_kontakt;
create table reed_kontakt
(
	id serial primary key
	, id_board integer references board
	

,port   integer

,gruppe   integer

,modus   integer

,feature   integer
);
	

drop table time_service;
create table time_service
(
	id serial primary key
	, id_board integer references board
	

,level   integer

,takeover_time   integer

,interval   integer
);
	

drop table licht_zone;
create table licht_zone
(
	id serial primary key
	, id_board integer references board
	

,toggle_gruppe0   integer

,toggle_gruppe1   integer

,toggle_gruppe2   integer

,toggle_gruppe3   integer

,off_gruppe0   integer

,off_gruppe1   integer

,on_gruppe0   integer

,on_gruppe1   integer

,timer_delay   integer

,action_gruppe0   integer

,action_gruppe1   integer
);
	

drop table zeit_zone;
create table zeit_zone
(
	id serial primary key
	, id_board integer references board
	

,gruppe   integer

,day_pattern   integer

,from_hour   integer

,from_minute   integer

,to_hour   integer

,to_minute   integer
);
	

drop table dunstabzugport;
create table dunstabzugport
(
	id serial primary key
	, id_board integer references board
	

,port   integer

,reed_gruppe0   integer

,freischalt_taster   integer

,freischalt_zeit   integer

,reed_gruppe1   integer
);
	

drop table zeitschaltuhr;
create table zeitschaltuhr
(
	id serial primary key
	, id_board integer references board
	

,power_gruppe   integer

,zeitzone0_id   integer

,zeitzone1_id   integer

,zeitzone2_id   integer

,zeitzone3_id   integer

,zeitzone4_id   integer

,zeitzone5_id   integer

,zeitzone6_id   integer

,zeitzone7_id   integer
);
	

drop table dimmerport;
create table dimmerport
(
	id serial primary key
	, id_board integer references board
	

,port   integer

,feature   integer

,gruppe0   integer

,gruppe1   integer

,gruppe2   integer

,gruppe3   integer
);
	

drop table tastdimmer;
create table tastdimmer
(
	id serial primary key
	, id_board integer references board
	

,port   integer

,feedback_guppe   integer

,feature   integer

,gruppe0   integer

,gruppe1   integer

,gruppe2   integer

,gruppe3   integer

,gruppe4   integer

,gruppe5   integer

,gruppe6   integer

,gruppe7   integer
);
	

drop table rc5receiver;
create table rc5receiver
(
	id serial primary key
	, id_board integer references board
	

,port   integer
);
	

drop table rolladen_id_assignment;
create table rolladen_id_assignment
(
	id serial primary key
	, id_board integer references board
	

,roll_id_0   integer

,roll_id_1   integer

,roll_id_2   integer

,roll_id_3   integer

,roll_id_4   integer

,roll_id_5   integer

,roll_id_6   integer

,roll_id_7   integer

,roll_id_8   integer

,roll_id_9   integer

,roll_id_10   integer

,roll_id_11   integer

,roll_id_12   integer

,roll_id_13   integer

,roll_id_14   integer

,roll_id_15   integer

,roll_id_16   integer

,roll_id_17   integer

,roll_id_18   integer

,roll_id_19   integer
);
	

drop table tag_nacht_agent;
create table tag_nacht_agent
(
	id serial primary key
	, id_board integer references board
	

,out0   integer

,process_order   integer

,roll_conf_0   integer

,roll_conf_1   integer

,roll_conf_2   integer

,roll_conf_3   integer

,roll_conf_4   integer

,roll_conf_5   integer

,roll_conf_6   integer

,roll_conf_7   integer

,roll_conf_8   integer

,roll_conf_9   integer

,roll_conf_10   integer

,roll_conf_11   integer

,roll_conf_12   integer

,roll_conf_13   integer

,roll_conf_14   integer

,roll_conf_15   integer

,roll_conf_16   integer

,roll_conf_17   integer

,roll_conf_18   integer

,roll_conf_19   integer

,hell_grenze   integer

,dunkel_grenze   integer

,aufgang_von   integer

,aufgang_bis   integer

,untergang_von   integer

,untergang_bis   integer

,active   integer
);
	

drop table sonnenschutz_agent;
create table sonnenschutz_agent
(
	id serial primary key
	, id_board integer references board
	

,out0   integer

,process_order   integer

,roll_conf_0   integer

,roll_conf_1   integer

,roll_conf_2   integer

,roll_conf_3   integer

,roll_conf_4   integer

,roll_conf_5   integer

,roll_conf_6   integer

,roll_conf_7   integer

,roll_conf_8   integer

,roll_conf_9   integer

,roll_conf_10   integer

,roll_conf_11   integer

,roll_conf_12   integer

,roll_conf_13   integer

,roll_conf_14   integer

,roll_conf_15   integer

,roll_conf_16   integer

,roll_conf_17   integer

,roll_conf_18   integer

,roll_conf_19   integer

,ost_to_hour   integer

,ost_to_min   integer

,ost_schwelle   integer

,sued_from_hour   integer

,sued_from_min   integer

,sued_to_hour   integer

,sued_to_min   integer

,sued_schwelle   integer

,west_from_hour   integer

,west_from_min   integer

,west_schwelle   integer

,hysterese   integer

,flapinterval   integer

,active   integer
);
	

drop table wetterschutz_agent;
create table wetterschutz_agent
(
	id serial primary key
	, id_board integer references board
	

,out0   integer

,process_order   integer
);
	

drop table prio_agent;
create table prio_agent
(
	id serial primary key
	, id_board integer references board
	

,in0   integer

,in1   integer

,in2   integer

,out0   integer

,process_order   integer
);
	

drop table state2event_agent;
create table state2event_agent
(
	id serial primary key
	, id_board integer references board
	

,in0   integer

,out0   integer

,process_order   integer
);
	

drop table zeitfilter_agent;
create table zeitfilter_agent
(
	id serial primary key
	, id_board integer references board
	

,in0   integer

,out0   integer

,process_order   integer

,roll_idx_0   integer

,zz_id_0   integer

,roll_idx_1   integer

,zz_id_1   integer

,roll_idx_2   integer

,zz_id_2   integer

,roll_idx_3   integer

,zz_id_3   integer

,roll_idx_4   integer

,zz_id_4   integer

,roll_idx_5   integer

,zz_id_5   integer

,roll_idx_6   integer

,zz_id_6   integer

,roll_idx_7   integer

,zz_id_7   integer
);
	

drop table flappingfilter_agent;
create table flappingfilter_agent
(
	id serial primary key
	, id_board integer references board
	

,in0   integer

,out0   integer

,process_order   integer
);
	

drop table heizungssteuerung;
create table heizungssteuerung
(
	id serial primary key
	, id_board integer references board
	

,pwm0   integer

,pwm1   integer

,pwm2   integer

,pwm3   integer

,pwm4   integer

,pwm5   integer

,pwm6   integer

,pwm7   integer

,pwm8   integer

,pwm9   integer

,pwm10   integer

,pwm11   integer

,pwm12   integer

,pwm13   integer

,pwm14   integer

,pwm15   integer

,pwm16   integer

,pwm17   integer

,pwm18   integer

,pwm19   integer

,pwm20   integer

,pwm21   integer

,pwm22   integer

,pwm23   integer

,pwm_heizung_off   integer

,a   integer

,b   integer

,RL_max   integer

,VT_min   integer
);
	

drop table wetterstation_data;
create table wetterstation_data
(
	id serial primary key
	, id_board integer references board
	

,offset_licht_ost   integer

,offset_licht_sued   integer

,offset_licht_west   integer

,dummy0   integer

,dummy1   integer

,dummy2   integer
);
	

drop table userpanel_shortcuts;
create table userpanel_shortcuts
(
	id serial primary key
	, id_board integer references board
	

,F1   integer

,F2   integer

,F3   integer

,F4   integer

,F5   integer

,F6   integer

,F7   integer

,F8   integer
);
	

drop table screensaver_page;
create table screensaver_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer
);
	

drop table dir_page;
create table dir_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,name   text

,dummy   integer
);
	

drop table heiz_page;
create table heiz_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,name   text

,heiz_id   integer
);
	

drop table rolladen_page;
create table rolladen_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,feature   integer

,gruppe   integer

,pos   integer

,line0   text

,line1   text
);
	

drop table taster_page;
create table taster_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,feature   integer

,gruppe   integer

,line0   text

,line1   text
);
	

drop table powerport_page;
create table powerport_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,feature   integer

,gruppe   integer

,line0   text

,line1   text
);
	

drop table lcd_light_settings;
create table lcd_light_settings
(
	id serial primary key
	, id_board integer references board
	

,zeitzone_id   integer

,backlight   integer

,contrast   integer
);
	

drop table fenster_monitor_page;
create table fenster_monitor_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,reed0   integer

,reed1   integer

,reed2   integer

,reed3   integer

,reed4   integer

,reed5   integer

,reed6   integer

,reed7   integer

,reed8   integer

,reed9   integer

,reed10   integer

,reed11   integer

,reed12   integer

,reed13   integer

,reed14   integer

,reed15   integer

,reed16   integer

,reed17   integer

,reed18   integer

,reed19   integer

,reed20   integer

,reed21   integer

,reed22   integer

,reed23   integer
);
	

drop table temp_anzeige_page;
create table temp_anzeige_page
(
	id serial primary key
	, id_board integer references board
	

,page_id   integer

,parent_id   integer

,next_id   integer

,prev_id   integer

,feature   integer

,tempsensor_id   integer

,name   text
);
	