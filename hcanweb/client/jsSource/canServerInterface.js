function gueltigeDaten (pageObj, location)
{
	if (globalFilteredIds === '' || globalFilteredIds === undefined) {
		if (globalLog) logIt (pageObj.attr("id")+' NICHT getDeviceStatesFromServer() @ '+location+', da ids: '+globalFilteredIds);
		return false;
	}
	else {
		return true; 
	}
}

// z.B. http://192.168.1.40/ 
// http://localhost/cgi-bin/hcanweb?callback=myjp&cmd=&newPage=f&page=sonstige&selectedId=255&ids=83,76&queryStates=t
function getDeviceStatesFromServer (dataToServer, trigger, event) {
	var withoutAjax = false;
	
	var pageObj = $.mobile.activePage;
	globalPageChangeable = false; // deaktiviere alle Events der Seite

	if (withoutAjax)
	{
		if (globalLog) logIt ("___ "+dataToServer.page+" Ohne Ajax! ___");
		writeDeviceTable ('-?-', pageObj); // ohne Zustaende, da wir diese nicht erhalten haben
	} else {
		if (dataToServer.selectedId === undefined) dataToServer.selectedId = "";
		
		if (globalLog) logIt ('dataToServer.page='+dataToServer.page+
				'| .ids='+dataToServer.ids+
				'| .selectedId='+dataToServer.selectedId+
				'| .cmd='+dataToServer.cmd);
		// query.queryStates  ggf. nur noch nach got focus/Betaetigung und 2s-Timeout
		// oder HTML5 -> ServerSentEvents (SSE)
		
		var req = $.ajax({
			type: 'GET',
			url: globalCANserverURL,
			async: true, // damit das timeout wirken kann
			dataType: 'jsonp',
			jsonpCallback: 'myjp', // anstelle einer zufaelligen Bezeichnung
			timeout: 500, // ms -> 0,5 Sekunden // ggf das gleiche Timeout auch im Server einstellen!
			data: dataToServer
		});
		
		req.done(function(data) {
			if (dataToServer.queryStates === "t") {
				writeDeviceTable (data, pageObj); // mit den aktuellen Zustaenden
			}
		});
		
		req.fail(function (x, t, m) {
			if (t==="timeout") {
	        	if (globalLog) logIt ("getDeviceStatesFromServer: timeout");
	        } else {
	        	if (globalLog) logIt ("getDeviceStatesFromServer: kein timeout: "+ t);
	        }
			
			if (dataToServer.queryStates === "t") {
				if (globalLog) logIt (dataToServer.page+" Fehler: Id-States JSON von "+globalCANserverURL+" nicht erhalten!");
				writeDeviceTable ('-?-', pageObj); // ohne Zustaende, da wir diese nicht erhalten haben
			}
		});
		
		req.always(function(data) {
			if (globalLog) logIt (dataToServer.page+" Trigger: "+trigger+", "+" Event: "+event.type);
			globalPageChangeable = true; // aktiviere wieder alle Events der Seite
		});
	}
	
	return req;
}
