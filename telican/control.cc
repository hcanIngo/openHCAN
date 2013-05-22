#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <libhcan++/colors.h>
#include <libhcan++/dynaddress.h>
#include <libhcan++/transport_connection.h>
#include <control.h>
#include <readlinepp.h>
#include <vector>
#include <readline_completer.h>
#include <readline_completion_by_list.h>
#include <iostream>
#include <installation_data.h>
#include <command_processor.h>
#include <lampe_command.h>
#include <sonstige_command.h>
#include <reedkontakt_command.h>
#include <rolladen_command.h>
#include <heizung_command.h>
#include <tempsensor_command.h>

#define HISTFILE ".telican-control-history"


void control_mode(const in_addr_t &hcand_ip)
{
	// History laden
	char *home_env = getenv("HOME");
	string histfile = "/";
	if (home_env)
		histfile = string(home_env) + "/" + HISTFILE;
	readlinepp::history history(histfile);

	try
	{
		// HCAN Installation laden (Singleton!)
		global_installation_data.load(INSTALLATION_XML);
	}
	catch (std::exception &e)
	{
		cout << "cannot enter telican control mode:" << endl;
		cout << "error while loading " << INSTALLATION_XML << endl;
		return;
	}

	// HCAN Zugriff vorbereiten
	hcan::dynaddress dynsrc(hcand_ip);
	dynsrc.allocate();
	uint16_t src = dynsrc();
	hcan::transport_connection con(hcand_ip);

	// Readline Completion aufsetzen
	readlinepp::completion_by_list compl_lampe("^lampe.*");
	compl_lampe.add(global_installation_data.lampen_names());

	readlinepp::completion_by_list compl_sonstige("^sonstige.*");
	compl_sonstige.add(global_installation_data.sonstige_names());

	readlinepp::completion_by_list compl_rolladen("^rolladen.*");
	compl_rolladen.add(global_installation_data.rollaeden_names());

	readlinepp::completion_by_list compl_heizung("^heizung.*");
	compl_heizung.add(global_installation_data.heizungen_names());

	readlinepp::completion_by_list compl_all("^.*");
	compl_all.add("lampe");
	compl_all.add("sonstige");
	compl_all.add("reedkontakt");
	compl_all.add("rolladen");
	compl_all.add("heizung");
	compl_all.add("tempsensor");

	readlinepp::completer rl_completer;
	rl_completer.add(compl_lampe);
	rl_completer.add(compl_sonstige);
	rl_completer.add(compl_rolladen);
	rl_completer.add(compl_heizung);
	rl_completer.add(compl_all);

	// Befehlsverarbeitung aktivieren
	lampe_command lampe_cmd(con,src);
	sonstige_command sonstige_cmd(con,src);
	reedkontakt_command reedkontakt_cmd(con,src);
	rolladen_command rolladen_cmd(con,src);
	heizung_command heizung_cmd(con,src);
	tempsensor_command tempsensor_cmd(con,src);

	command_processor cmd_proc;
	cmd_proc.add(lampe_cmd);
	cmd_proc.add(sonstige_cmd);
	cmd_proc.add(reedkontakt_cmd);
	cmd_proc.add(rolladen_cmd);
	cmd_proc.add(heizung_cmd);
	cmd_proc.add(tempsensor_cmd);


	while (1)
	{
		try
		{
			cout << COLOR_BLUE;

			cout.flush();
			rl_bind_key('\t',rl_complete);
			const string line = readlinepp::read_line("> ");

			if (line == "quit")
				throw readlinepp::end_of_input();


			cout << COLOR_NORMAL;
			cout.flush();

			if (home_env && (line != ""))
				history.add(line);

			cmd_proc.exec(line);
		}
		catch (readlinepp::end_of_input &e)
		{
			cout << COLOR_NORMAL;
			cout << endl;

			// while-Schleife verlassen:
			break;
		}
	}
}

