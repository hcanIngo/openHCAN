// einmalig beim ersten Besuch der jeweiligen Seite:  -  pageinit und der DOM ist bereit
function hookElementarPageEvents () {
	$(document).on('pageinit', function(event) {
		event.preventDefault();
		
		var pageObj = $(event.target); // $.mobile.activePage empty/incorrect in 'pageinit'
		var page = pageObj.attr("id");
		
		if (globalLog) logIt ("pageinit="+page);
		
		if (page === "Einstellungen")
		{
			hookSettingspageEvents ();
			hookPageLinkClick (pageObj); // fuer den zurueck-Button
		}
		else
		{
			hookAllPageEvents (pageObj);
			
			if (parseInstallation (page))
			{
				setFilterEntries (pageObj); 
				// getfilteredEntries mit pagebeforeshow
			}
		}
		return false;
	});

	$(document).on('pagebeforechange', function(event, data) {
		if (typeof data.toPage != "string") {
			var toPageObj = $(data.toPage);
			var toPage = toPageObj.attr('id');
			if (globalLog) logIt ("pagebeforechange (1.): data.toPage.id="+toPage+" Event: "+event.type);
			
			// findoutPageIsInDOMorCache (event, toPage);
			if (globalLog) logIt ("selektierter Filter:" + $('#selectFilter option:selected', toPageObj).text());
						
			if (toPage === "Einstellungen") {
				if (globalCurrentPage === "Einstellungen") {
					globalCurrentPage = "rolladen"; // diese Ausnahme absichern
				}
				
				if (globalLog) logIt ("set prev="+globalCurrentPage);
				$("#Einstellungen").data('prev', globalCurrentPage);
			}
		}
	});
	
	// Bei jedem Seitenwechsel: 
	$(document).on('pagebeforeshow', function(event, data) {
		event.preventDefault();
		var pageObj = $(event.target); // $.mobile.activePage
		var page = pageObj.attr("id");
		
		if (page === "root") {
			// ermittle die zuletzt aktuelle Seite und wechsel zu dieser Seite:
			if (globalLog) logIt ('gehe zu '+globalCurrentPage);
			$.mobile.loadPage ('#'+globalCurrentPage);
			$.mobile.changePage('#'+globalCurrentPage, {transition: "none", reverse: false});
		}
		else {
			localStorage.setItem("currentPage", page); // persistent speichern
			// populateLocalStorageCache (event, data, page); // fuer das abgekuendigkte pageload
			globalCurrentPage = page;
			
			if (page === "Einstellungen")
			{
				// alert('width='+$(window).width());
				if (globalLog) logIt ("pagebeforeshow="+page+": ** Defaultwerte fuer die Anzeige setzen.");
				getPersitenzOrDefaults ();
				$("label[for='footerVersion']").text(globalVersion);
				$("#serverURL").val(globalCANserverURL); // http://localhost/cgi-bin/hcanweb   ODER   http://192.168.1.40/			
				$("#FilterEintraege").val(globalStrFilterEintraege); // AB,KG,EG,OG,DG,--
				// $('#installationXMLdatei').val('...'); // bzw. .empty()
			}
			else // lampe, sonstige, ...:
			{
				if (globalLog) logIt ("pagebeforeshow (2.)="+page+" -> getfilteredEntries");
				getfilteredEntries (pageObj, event);
			}			
		}	
	});
}

function hookAllPageEvents (pageObj) {
	var page = pageObj.attr("id");
	
	switchPageSwipeOn (pageObj);
	hookPageLinkClick (pageObj);
	
	hookFilterEvents (pageObj);
	hookTableClick (pageObj);
	
	if (page === "heizung")
	{
		hookSliderEvents (pageObj);
		hookControlgroupEvents (pageObj);
	}
	else if (page === "rolladen")
	{
		hookSliderEvents (pageObj);
	}
}

function hookPageLinkClick (pageObj) {
	// Dateiname der next und der previous page aus dem data-next/data-prev Attribut der html-Seite
	
	$(pageObj).on("click", "#myLinkButtons", function(event) {
		event.preventDefault();
		if (globalPageChangeable) {
			var page = $(this).attr("name");
			var pageObj = '#'+page;
			if (globalLog) logIt ('myButton click, gehe zu '+page);
			$.mobile.changePage(pageObj, {transition: "none", reverse: false});
		}
		return false;
	});

	// zurueck von der Einstellungsseite: 
	$(pageObj).on("click", "#myZurueckButton", function(event) {
		event.preventDefault();
		if (globalLog) logIt ('myZurueckButton click, gehe zu '+pageObj.jqmData("prev"));
		$.mobile.changePage('#'+pageObj.jqmData("prev"), {transition: "none", reverse: false});
		return false;
	});
}

function handleSwipeLeft (event) {
	event.preventDefault();
	if (globalPageChangeable) {
		// Dateiname der next und der previous page aus dem data-next/data-prev Attribut der html-Seite
		var next = event.data.pageObj.jqmData("next");
		if (globalLog) logIt ('swipeL, gehe zu '+next);
		$.mobile.changePage('#'+next, {transition: "slide", reverse: false});
	}
	return false;
}

function handleSwipeRight (event) {
	event.preventDefault();
	if (globalPageChangeable) {
		var prev = event.data.pageObj.jqmData("prev");
		if (globalLog) logIt ('swipeR, gehe zu '+prev);
		$.mobile.changePage('#'+prev, {transition: "slide", reverse: true});		
	}
	return false;
}

function switchPageSwipeOn (pageObj) {
	if (globalLog) logIt ('switchPageSwipeOn: pageObj='+pageObj.attr("id"));
	globalPageChangeable = true;
	
	$(pageObj).on('swipeleft', {pageObj: pageObj}, handleSwipeLeft);
	$(pageObj).on('swiperight', {pageObj: pageObj}, handleSwipeRight);
}

function switchPageSwipeOff (pageObj) {
	if (globalLog) logIt ('switchPageSwipeOff: pageObj='+pageObj.attr("id"));
	globalPageChangeable = false;
	
	/* $(pageObj).off('swipeleft', handleSwipeLeft);	
	$(pageObj).off('swiperight', handleSwipeRight);*/	
}

function hookSettingspageEvents ()
{	
	$("#serverURL").blur(function() { // Focus abgegeben
		globalCANserverURL = $(this).val();
		localStorage.setItem("CANserverURL", globalCANserverURL); // persistent speichern
    	if (globalLog) logIt ("CANserverURL: "+globalCANserverURL);
	});

	document.getElementById('installationXMLdatei').addEventListener('change', function(event) {
		readFile ();
	}, false);
	
	$("#FilterEintraege").blur(function() {
		globalStrFilterEintraege = $(this).val();
		localStorage.setItem("FilterEintraege", globalStrFilterEintraege); // persistent speichern
		parseInstallationAndSetFilterEntries (); // Filtereintraege neu einfuegen
	});
	
	$("#setDefaultsButton").on("click", function() {
		if (globalLog) logIt ('setDefaultsButton click');
		setDefaults ();
		// und die Defaultwerte anzeigen: 
		$("#serverURL").val(globalCANserverURL);			
		$("#FilterEintraege").val(globalStrFilterEintraege);		
		// $('#installationXMLdatei').val('...');
		parseInstallationAndSetFilterEntries (); // Filtereintraege neu einfuegen
	});
}

function updatePageControlElements (pageObj, state) {
	var page = pageObj.attr("id");
	if (page === 'rolladen' || page === 'heizung') {
		$('#sliderStepInput', pageObj).val(state).slider("refresh"); // Slider an Devicezustand anpassen
	}
}