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

// z.B.  
// http://localhost/cgi-bin/hcanweb?callback=myjp&cmd=&d=sonstige&setid=255&qid=83,76
// http://192.168.1.23/?callback=myjp&cmd=aus&d=sonstige&setid=2&qid=33,83,102,76,75,2
function getDeviceStatesFromServer (dataToServer, trigger, event) {
	var pageObj = $.mobile.activePage;
	globalPageChangeable = false; // deaktiviere alle Events der Seite

	if (dataToServer.setid === undefined) dataToServer.setid = "";
	
	if (globalLog) logIt ('dataToServer.device(page)='+dataToServer.d+
			'| .cmd='+dataToServer.cmd+
			'| .setId='+dataToServer.setid+ // selectedId -> setId, da zu schaltende
			'| .ids='+dataToServer.qid);
			
	// Zustaende ggf. nur noch nach got focus/Betaetigung und 2s-Timeout abfragen
	// oder HTML5 -> ServerSentEvents (SSE)
		
	var req = $.ajax({
		type: 'GET',
		url: globalCANserverURL,
		async: false, // damit auf ein moegliches TO im Servers gewartet und nicht zuvor weitere Anfragen geschickt werden!
		dataType: 'jsonp',
		jsonpCallback: 'myjp', // anstelle einer zufaelligen Bezeichnung
		timeout: 2000, // ms -> 2 Sekunden // ggf das gleiche Timeout auch im Server einstellen!
		data: dataToServer
	});
	
	req.done(function(data) {
		if (dataToServer.qid !== "") { // Zustaende abfragen?
			var doUpdatePageControlElements = true;
			if (('slidestop' === trigger) && ('rolladen' === dataToServer.d)) {
				if (globalLog) logIt ("KEIN updatePageControlElements !!!");
				doUpdatePageControlElements = false;
			}
			writeDeviceTable (data, pageObj, doUpdatePageControlElements); // mit den aktuellen Zustaenden
		}
	});
	
	req.fail(function (x, t, m) {
		if (t==="timeout") {
        	if (globalLog) logIt ("getDeviceStatesFromServer: timeout");
        } else {
        	if (globalLog) logIt ("getDeviceStatesFromServer: kein timeout: "+ t);
        }
		
		if (dataToServer.qid !== "") { // Zustaende abfragen?
			if (globalLog) logIt (dataToServer.d+" Fehler: Id-States JSON von "+globalCANserverURL+" nicht erhalten!");
			var doUpdatePageControlElements = false;
			writeDeviceTable ('-?-', pageObj, doUpdatePageControlElements); // ohne Zustaende, da wir diese nicht erhalten haben
		}
	});
	
	req.always(function(data) {
		if (globalLog) logIt (dataToServer.d+" Trigger: "+trigger+", "+" Event: "+event.type);
		globalPageChangeable = true; // aktiviere wieder alle Events der Seite
	});
	
	return req;
}
