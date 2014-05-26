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
// http://localhost/cgi-bin/hcanweb?callback=myjp&cmd=&newpage=f&page=sonstige&selid=255&ids=83,76&qstates=t
// http://192.168.1.23/?callback=myjp&cmd=aus&newpage=f&page=sonstige&selid=2&ids=33,83,102,76,75,2&qstates=t
function getDeviceStatesFromServer (dataToServer, trigger, event) {
	var pageObj = $.mobile.activePage;
	globalPageChangeable = false; // deaktiviere alle Events der Seite

	if (dataToServer.selid === undefined) dataToServer.selid = "";
	
	if (globalLog) logIt ('dataToServer.page='+dataToServer.page+
			'| .ids='+dataToServer.ids+
			'| .selid='+dataToServer.selid+ // selid = selectedId
			'| .cmd='+dataToServer.cmd+
			'| .qstates='+dataToServer.qstates);
	// query.queryStates  ggf. nur noch nach got focus/Betaetigung und 2s-Timeout
	// oder HTML5 -> ServerSentEvents (SSE)
	
/*	var async = false; // damit auf ein moegliches TO im Servers gewartet und nicht zuvor weiteren Anfragen geschickt werden!
	if (dataToServer.qstates === "f") async = true; // damit das timeout wirken kann*/
	
	var req = $.ajax({
		type: 'GET',
		url: globalCANserverURL,
		async: false, // damit auf ein moegliches TO im Servers gewartet und nicht zuvor weiteren Anfragen geschickt werden!
		dataType: 'jsonp',
		jsonpCallback: 'myjp', // anstelle einer zufaelligen Bezeichnung
		timeout: 2000, // ms -> 2 Sekunden // ggf das gleiche Timeout auch im Server einstellen!
		data: dataToServer
	});
	
	req.done(function(data) {
		if (dataToServer.qstates === "t") {
			var doUpdatePageControlElements = true;
			if (('slidestop' === trigger) && ('rolladen' === dataToServer.page)) {
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
		
		if (dataToServer.qstates === "t") {
			if (globalLog) logIt (dataToServer.page+" Fehler: Id-States JSON von "+globalCANserverURL+" nicht erhalten!");
			var doUpdatePageControlElements = false;
			writeDeviceTable ('-?-', pageObj, doUpdatePageControlElements); // ohne Zustaende, da wir diese nicht erhalten haben
		}
	});
	
	req.always(function(data) {
		if (globalLog) logIt (dataToServer.page+" Trigger: "+trigger+", "+" Event: "+event.type);
		globalPageChangeable = true; // aktiviere wieder alle Events der Seite
	});
	
	return req;
}
