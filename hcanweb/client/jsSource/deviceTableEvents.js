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
	
				var qstates = true;
				var dataToServer = {
					cmd: state, 
					d: pageObj.attr("id"), // device
					setid: selectedId,
					qid: qstates ? globalFilteredIds : ""
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
		var qstates = true;
		// if (selected === 255) qstates = false;
		var dataToServer = {
			cmd: "",
			d: pageObj.attr("id"), // device
			setid: selected,
			qid: qstates ? globalFilteredIds : ""
    	}
    	getDeviceStatesFromServer(dataToServer, "newSelection", event);
	}
}