function hookTableClick (pageObj) {
	var deviceTableObj = $('#deviceTable', pageObj);
	deviceTableObj.on("click", '#switchImg', function(event, ui) {
		event.preventDefault();
		if (globalPageChangeable) {
			var selectedId = $(this).attr('class'); // welches Image geklickt wurde
			
			if (gueltigeDaten (pageObj, 'SwitchImgClick')) {
				state = $(this).attr('clickTargetState');
				if ('-?-' === state) {
					state = (globalState === "aus") ? "ein":"aus"; // toggle
				}
	
				var dataToServer = {
					cmd: state, 
					newpage: "f",
					page: pageObj.attr("id"),
					selid: selectedId,
					ids: globalFilteredIds,
					qstates: "t"
				}
				getDeviceStatesFromServer(dataToServer, "switchChange", event);
			}
		}
		return false;
	});
	
	deviceTableObj.on("click", '.RowUnselected', function(event, ui) {
		event.preventDefault();
		if (globalPageChangeable) {
			var selectedId = $(this).attr("id"); // welche Zeile geklickt wurde
			
			// alle deselektieren: 
			$(this).closest("tr").siblings().removeClass("RowSelected").addClass("RowUnselected");
			
			$(this).removeClass("RowUnselected").addClass("RowSelected");		
			newSelection (pageObj, selectedId);
		}
		return false;
	});
	
	deviceTableObj.on("click", '.RowSelected', function(event, ui) {
		event.preventDefault();
		if (globalPageChangeable) {
			// alle deselektieren: 
			$(this).closest("tr").siblings().removeClass("RowSelected").addClass("RowUnselected");
		}
		return false;
	});
}

function newSelection (pageObj, selected) {
	if (gueltigeDaten (pageObj, 'newSelection')) {
		var dataToServer = {
			cmd: "",
			newpage: "f",
			page: pageObj.attr("id"),
			selid: selected,
			ids: globalFilteredIds,
			qstates: "t" // (selected === 255) ? "f":"t"
    	}
    	getDeviceStatesFromServer(dataToServer, "newSelection", event);
	}
}