function writeGlobalFilteredIds (pageObj, filter) {
	globalFilteredIdName = {}; // leeren
 	globalFilteredIds = ""; // leeren
	var device = globalTyp[pageObj.attr("id")];

	if (globalFehler === "") {
	    if (device !== undefined) {
	        $.each( device, function( id, name ) {
	    		if (name !== undefined && id !== undefined) {
	    			if (id == "255" || filter === "--" || name.search(filter) != -1) { // z.B "EG"; bei "--" nicht filtern
	    				globalFilteredIdName[id] = name;
	    				if (globalFilteredIds === "") {globalFilteredIds = id}
	    				else {globalFilteredIds += ","+id;}
	    			}
	    		}
	    	});
	    }
	    else {
	    	if (globalLog) logIt (pageObj.attr("id")+' device: '+JSON.stringify(device, null, ' ')+', filter='+filter+', page='+pageObj.attr("id"));
	    	device = "";
	    	globalFehler = "Keine Devices gefunden!";
	    }
		
		if (globalFilteredIds === '' || globalFilteredIds === undefined) { 		
			globalFehler = "Device-Filter ergebnislos!"; 
		}
	}
	
	if (globalFehler !== "")
	{
		$('#deviceTable', pageObj)
			.empty() // alle Kindelemente loeschen
			.append( $("<tr><td>" + globalFehler + "</td></tr>") );
				
		globalFehler = "";
	}
}

function hookFilterEvents (pageObj) {
	$('#selectFilter', pageObj).on('change', function(event, data) {
		event.preventDefault();
		if (globalPageChangeable) {
			var page = pageObj.attr("id");
			var filter = $('#selectFilter option:selected', pageObj).text(); // z.B. ("--")("KG")("EG")("OG")("DG")("AB")
			writeGlobalFilteredIds (pageObj, filter);
			
			if (gueltigeDaten (pageObj, 'newFilter')) {
				var dataToServer = {
					cmd: "",
					newpage: "f",
					page: page,
					selid: "", // nichts selektieren
					ids: globalFilteredIds,
					qstates: "t"
				}
				getDeviceStatesFromServer(dataToServer, "FilterChange:"+filter, event); // page: lampe...
			}
	
			if (globalLog) logIt (page+" selectFilter change to "+filter+" -> persistent speichern");
			globalCurrentPageSelectedFilter = filter;
			localStorage.setItem("CurrentPageSelectedFilter", filter); // selected Filter persistent speichern
			// populateLocalStorageCache (event, data, page); // Seite speichern
		}
		return false;
	}); 
}

function getfilteredEntries (pageObj, event) {
	var filter = $('#selectFilter option:selected', pageObj).text(); // ("KG")("EG")("OG")("DG")("AB")("--")
	
	if (true /*gewaehlter Filter gewechselt*/) {
		writeGlobalFilteredIds (pageObj, filter);	
	}
	
	if (gueltigeDaten (pageObj, 'hookPageShow')) {
		var dataToServer = {
				cmd: "",
				newpage: "t",
				page: pageObj.attr("id"),
				selid: "", // keine Selektion
				ids: globalFilteredIds,
				qstates: "t"
		}
		getDeviceStatesFromServer(dataToServer, "getfilteredEntries", event);
	}
}

function existsEntriesForThisFilter (pageObj, filter) {
	var existiert = false;
	var page = pageObj.attr("id");
	if (undefined === globalTyp[page]) {
		if (globalLog) logIt ('globalTyp['+ page +'] undefined mit filter='+filter);
		
		// ggf hier wdh
		if (globalLog) logIt ('wdh parseInstallation!');
		parseInstallation (page);
	}
	
	var device = globalTyp[page];
	
	if (device !== undefined) {
		$.each( device, function( id, name ) {	
			if (name !== undefined && id !== undefined) {
				if (filter === "--" || name.search(filter) != -1) {
					existiert = true; // mindestens ein Device-Eintrag exitiert fuer diesen Filter
					return false; // unterbricht nur die each-Schleife
				}
			}
		});
	}
	else {
		device = ""; // device undefiniert, dann "" hineinschreiben
		if (globalLog) logIt (page+' device undefined mit filter='+filter);
	}
	
	return existiert;
}

function setFilterEntries (pageObj) {
	var filter = $('#selectFilter', pageObj).empty(); // alle Kindelemente loeschen
	var nothingSelected = true;
	var filterEintraege = globalStrFilterEintraege.split(","); // string to array
	$.each(filterEintraege, function (index, FilterValue) {	
		if (existsEntriesForThisFilter (pageObj, FilterValue)) {
			// Filter anwenden: Nur die Einträge aufnehmen, welche mit Anwendung des Filters mindestens ein Device liefern: 
			filter.append($("<option>"+ FilterValue +"</option>"));
			if (FilterValue === globalCurrentPageSelectedFilter) {
				filter.val(FilterValue).attr('selected', true);
				nothingSelected = false;
			}
		}
	});
	
	if (nothingSelected) {
		filter[0].selectedIndex = 0; // selektieren des ersten Elements (option:selected)	
	}

	/* filter.selectmenu('refresh'); // damit die Box den aktuellen Inhalt anzeigt
	 * Error: cannot call methods on selectmenu prior to initialization; attempted to call method 'refresh'
	 * siehe http://www.gajotres.net/uncaught-error-cannot-call-methods-on-prior-to-initialization-attempted-to-call-method-refresh/ */
	filter.selectmenu().selectmenu('refresh'); // damit die Box den aktuellen Inhalt anzeigt
}