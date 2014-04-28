function hookSliderEvents (pageObj) {
	$('#sliderStepInput', pageObj).on('slidestart', function(event) { 
		event.preventDefault();
		//event.stopImmediatePropagation ();
		switchPageSwipeOff (pageObj); // wenn wir mit 'off' arbeiten, ist es an dieser Stelle nicht funktionsfaehig 
		if (globalLog) logIt ('** slidestart -> page swipe off');
		return false;
	});
	
	$('#sliderStepInput', pageObj).on('slidestop', function(event) { 
		event.preventDefault();
		var value = $('#sliderStepInput', pageObj).val();
		sliderValueChanged (pageObj, value, 'slidestop');
		
		if (globalLog) logIt ('** slidestop @ '+pageObj.attr("id")+' -> bind page swipe');	
		return false;
	});
}

function sliderValueChanged (pageObj, value, location) {
	var selectedId = $('.RowSelected', pageObj).attr("id");
	
	var page = pageObj.attr("id");
	if (gueltigeDaten (pageObj, location)) {
		var dataToServer = {
				cmd: value,
				newPage: "f",
				page: page,
				selectedId: selectedId,
				ids: globalFilteredIds,
				queryStates: (page === "rolladen") ? "f":"t"
		}
		getDeviceStatesFromServer(dataToServer, location, event);
	}
	else {
		switchPageSwipeOn (pageObj);
	}
}