function getPersitenzOrDefaults () {	
	globalCurrentPage = localStorage.getItem("currentPage");
	if (globalLog) logIt ('currentPage: '+globalCurrentPage);
	if (globalCurrentPage === null || globalCurrentPage == undefined) {
		globalCurrentPage = "heizung"; // Default
		if (globalLog) logIt ('currentPage: '+globalCurrentPage);
	}
	
	globalCurrentPageSelectedFilter = localStorage.getItem("CurrentPageSelectedFilter");
	if (globalLog) logIt ('CurrentPageSelectedFilter: '+globalCurrentPageSelectedFilter);
	if (globalCurrentPageSelectedFilter === null || globalCurrentPageSelectedFilter == undefined) {
		globalCurrentPageSelectedFilter = "EG"; // Default
		if (globalLog) logIt ('CurrentPageSelectedFilter: '+globalCurrentPageSelectedFilter);
	}
	
	globalCANserverURL = localStorage.getItem("CANserverURL");
	if (globalLog) logIt ('CANserverURL: '+globalCANserverURL);
	if (globalCANserverURL === null) {
		globalCANserverURL = "http://192.168.1.23/"; // Default, or http://localhost/cgi-bin/hcanweb	
	}

	globalStrFilterEintraege = localStorage.getItem("FilterEintraege");
	if (globalLog) logIt ('FilterEintraege: '+globalStrFilterEintraege);
	if (globalStrFilterEintraege === null) {
		globalStrFilterEintraege = "EG,OG,AB,DG,KG,--"; // Default		
	}
	
	globalXml = localStorage.getItem("installationXML");
	if (globalLog) logIt ('installationXML: '+globalXml);
	if (globalXml === null) {
    	if (globalLog) logIt ('Default-installationBsp.xml laden');
    	var req = $.ajax({
			type: 'GET',
			url: 'installationBsp.xml', // installation.xml, http://192.168.1.31/installation.xml, http://localhost/installation.xml
			async: false, // damit sich die Anfragen nicht ueberschneiden
			dataType: 'xml',
			timeout: 2000, // ms -> 2 Sekunde
		});
		
		req.fail(function (x, t, m) {
			if (t==="timeout") {
	        	if (globalLog) logIt ("ajax GET installationBsp.xml: timeout");
	        } else {
	        	if (globalLog) logIt ("ajax GET installationBsp.xml: kein timeout: "+ t);
	        }
		});
		
		req.done(function(xml) {
			globalXml = xml;
		});
	}
}

function setDefaults () {
	localStorage.clear();
	getPersitenzOrDefaults (); // Defaults durch das vorangegangene clear setzen
}

function parseInstallationAndSetFilterEntries () {
	if (globalLog) logIt ('parseInstallationAndSetFilterEntries');
	var xmlReadSuccess = false;
	$.each(globalPages, function (index, page) {
		if (page != "Einstellungen") {
			var pageObj = $('#'+page);
			if (globalLog) logIt ("*** parseInstallationAndSetFilterEntries fuer "+page);
			$('#selectFilter', pageObj).empty(); // alle Kindelemente loeschen, sodass erfolgloses lesen auffaellt
			xmlReadSuccess = parseInstallation (page);
			if (xmlReadSuccess) {
				globalFehler = ""; // zuruecksetzen
				setFilterEntries (pageObj);
			}
			else {
				globalFehler = "installation.xml konnte nicht gelesen werden. Bitte unter Einstellungen auswählen.";
			}
		}
	});
	
	return xmlReadSuccess;
}

function parseInstallation (page) {
	var xmlReadSuccess = false;
	var IdName = {}; // kein Array, sondern nur ein JSON-JS-Objekt
	var Device = page;
	if (page === "lampe" || page === "sonstige") Device = "powerport";
	
	$(globalXml).find (Device).each (function() { // suche nach jedem 'device' Abschnitt
    	var $myDevice = $(this); // gefundenen Abschnitt in Variable zwischenspeichern
	    if (Device === page || page === $myDevice.attr("typ"))
	    {
	    	var name = $myDevice.attr("name");
	    	var gruppe = $myDevice.attr("gruppe");
	    	if (name !== undefined) {
	    		IdName[gruppe] = name;
	    	}
	    }
	    
	    if (IdName !== undefined && IdName !== "") {
	    	globalTyp[page] = IdName;
	    	xmlReadSuccess = true;
	    }
	    else {
	    	globalFehler = page+' parseInstallation-Fehler, da IdName=' + IdName;
	    }
	});
	
	return xmlReadSuccess;
}

function readFile () {
	var file = document.getElementById('installationXMLdatei').files[0]; // Get the reference of the input element
    if (globalLog) logIt (file);
    
    if (file) {
        if (globalLog) logIt ('Datei selektiert.');
        globalXml = null;
        var reader = new FileReader(); // Instantiate the File Reader object
        reader.readAsText(file); // The readAsText method will read the file's data as a text string. By default the string is decoded as 'UTF-8'.
        reader.onerror = function(event) {
        	alert ('Datei error.')
        	return;
        }
        reader.onload = function(event) { // onLoad event is fired when the load completes
        	globalXml = event.target.result;
        	var fileLines = globalXml.split("\n");
        	if ("<?xml" != fileLines[0].substring(0,5)) { // Dateipruefung: Erste Zeile auf "<?xml" abfragen
        		globalFehler = "Der Installation.xml-Datei fehlt die XML-Kennung. " + fileLines[0].substring(0,5);
        		if (globalLog) logIt (globalFehler);
        		alert (globalFehler);
        		return;
        	}
        	
        	localStorage.setItem("installationXML", globalXml); // persistent speichern
        	parseInstallationAndSetFilterEntries (); // installation.xml fuer alle Seiten neu einlesen
        };
    }
    else
    	if (globalLog) logIt ('Bitte eine Datei selektieren!');
}

/*
// Caching Pages in local Storage Seamlessly: http://www.prashantparashar.com/jquery-mobile-app-caching-pages-in-local-storage-seamlessly-2012-11.html
// The example only cache the pages (pages with data-role=”page”) and with attribute “data-local-storage=true“

// Mit 'pagebeforechange' finden/laden der neuen Seite, abhängig davon, ob sie im DOM oder Cache ist 
// Hinweis: page ist string von data.toPage
function findoutPageIsInDOMorCache (event, page) {
	if (typeof page === "string") {
		var storageName = "cache"+page;
		if(localStorage.getItem(storageName) != null) {
			if($("#" + storageName) == null || $("#" + storageName).length == 0) {
				var html =  localStorage.getItem(storageName);
				var all = $( "<div></div>" );
				all.get( 0 ).innerHTML = html;
				wholePage = all.find( ":jqmData(role='page'), :jqmData(role='dialog')" ).first();
				var u = page;
				if($.mobile.path.isAbsoluteUrl(u)) {
					u = u.substring($.mobile.path.parseUrl(u).domain.length);
				}
				wholePage.attr("data-url", u);
				$('body').append(wholePage);
			}
		}
	}
}		

// Mit 'pageload' die Seite in den lokalen Cache sichern
function populateLocalStorageCache (event, data, page) {
	var storageName = "cache"+page;
	if (globalLog) logIt ('populateLocalStorageCache: '+data.xhr.responseText);
	
	if(data.xhr.responseText != null) {
		var all = $( "<div></div>" );
		all.get( 0 ).innerHTML = data.xhr.responseText;
		var wholePage = all.find( ":jqmData(role='page'), :jqmData(role='dialog')" ).first();
		
		if(wholePage.jqmData("local-cache") === true) {
			localStorage.setItem(storageName, data.xhr.responseText);
			if (globalLog) logIt ('xhr.responseText: '+data.xhr.responseText);
		}
	}
}*/
