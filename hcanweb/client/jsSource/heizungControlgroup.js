function hookControlgroupEvents (pageObj) {
	var controlObj = $("div.controlClassHeizung");
	controlObj.on('change', '.click-rC', function(event) {
		event.preventDefault();
		var controlValue = $("#ControlHeizung :radio:checked").val();
		var selectedId = $('.RowSelected', pageObj).attr("id");
		
		if (gueltigeDaten (pageObj, 'HeizungControlChange')) {
			var qstates = true;
			var dataToServer = {
					cmd: controlValue,
					d: pageObj.attr("id"), // device
					setid: selectedId,
					qid: qstates ? globalFilteredIds : ""
			}
			getDeviceStatesFromServer(dataToServer, "HeizungControlChange", event);
		}
		return false;
	});
}