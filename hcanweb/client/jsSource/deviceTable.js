function createStateImage (page, id, state) {
	var image = "nichts";

	if (page === "lampe") {
		if 			(0 === state) {image = "lampeAusgeschaltet";}
		else if 	(1 === state) {image = "lampeEingeschaltet";}
	}
	else if (page === "sonstige") {
		if 			(0 === state) {image = "sonstigeAus";}
		else if 	(1 === state) {image = "sonstigeEin";}
	}
	else if (page === "reedkontakt") {
		if 			(0 === state) {image = "FensterIstZu";}
		else if 	(1 === state) {image = "FensterIstAuf";}
	}
	
	if (0 === state) {clickTargetState = "ein";}
	else if (1 === state) {clickTargetState = "aus";}
	else clickTargetState = '-?-';
	
	var stateImage = $("<a></a>", {"href":"javascript:void(0);", "id":"switchImg", "class":id, "clickTargetState":clickTargetState /*, "text":id*/}); // Kommentar entfernen und die id's sind sichtbar
	stateImage.append($("<img></img>", {"src": "javascript/images/my48/"+image+".png"}));
	
	return stateImage;
}

function row (pageObj, id, state, name) {
	var page = pageObj.attr("id");
	var r = $("<tr id=\"" + id + "\"></tr>");
	var d1 = $("<td></td>");
	var d2 = $("<td>" + name + "</td>");
	
	if (id != 255) { // fuer 255 = "alle" keinen Zustand anzeigen
		if (page === "lampe" || page === "sonstige" || page === "reedkontakt")
			d1.append(createStateImage(page, id, state));
		else if ('-?-' !== state) {
			if (page === "rolladen")
				d1.append(state + "% "); // Zustand mit Einheit
			else if (page === "heizung")
				d1.append(state + "°C "); // Zustand mit Einheit
		}
	}
	
	r.append(d1).append(d2); // Spalten
	r.removeClass("RowSelected").addClass("RowUnselected"); // default
	return r; // die eine Reihe zurueckgeben
}

function getDeviceTableRow (pageObj, state, id, selectedId, name) {
	var page = pageObj.attr("id");
	if (name !== undefined  &&  name !== "") {
		var r = row (pageObj, id, state, name);
		
		if (selectedId === id) {
			r.removeClass("RowUnselected").addClass("RowSelected");
			if (('-?-' !== state) && ('255' !== selectedId)) { 
				updatePageControlElements (pageObj, state);
			}
		}
		
		return r;
	}
	else return row (pageObj, id, "", "name nicht gefunden!");
}

function getState_alle (page) {
	var state = 0;
	
	if (page === "lampe" || page === "sonstige") {
		state = 0; // ausgeschaltet
	}
	else if (page === "rolladen") {
		state = 50; // zwischen auf und zu
	}
	else if (page === "heizung") {
		state = 15; // 15 °C
	}
	
	return state;
}

function writeDeviceTable (serverIdState, pageObj, doUpdatePageControlElements) {
	var page = pageObj.attr("id");
	if (globalLog) logIt ('fromServer: serverIdState: '+JSON.stringify(serverIdState, null, ' '));

	var selectedId = $('.RowSelected', pageObj).attr("id");
	// alert('selectedId=' + selectedId);

	var deviceTable = $('#deviceTable', pageObj);
	deviceTable.empty(); // empty: alle Kindelemente loeschen
	if ('-?-' === serverIdState) {
		$.each(globalFilteredIdName, function (id, name) {
			deviceTable.append( getDeviceTableRow (pageObj, serverIdState, id, selectedId, name) );
		});
	}
	else { // states erhalten: 
		$.each(serverIdState, function (id, state) {
			deviceTable.append( getDeviceTableRow (pageObj, state, id, selectedId, globalFilteredIdName[id]) );	
		});
	}
	
	if (page === "heizung" || page === "rolladen")
		deviceTable.append(getDeviceTableRow (pageObj, getState_alle (page), "255", selectedId, "alle"));
	
	if (undefined === $('.RowSelected', pageObj).attr("id")) {
		if (globalLog) logIt ('** keine Reihe selektiert, da id = '+ $('.RowSelected', pageObj).attr("id"));
		
		var firstRow = $('.RowUnselected', pageObj).first();
		firstRow.removeClass("RowUnselected").addClass("RowSelected"); // die erste Reihe selektieren
		selectedId = $('.RowSelected', pageObj).attr("id");
		if (doUpdatePageControlElements
			&& ('-?-' !== serverIdState) 
			&& ('255' !== selectedId)) {
			updatePageControlElements (pageObj, serverIdState[selectedId]);
		}
	}
}
