/* Bsp. input: '148/H/aus/16°C'  mit 148=gruppe
	output Mode: 1, 2, 3 als String */
var res = input.split("/",3);
var rtn = "ng";

// Mode ermitteln: 
if      (res[2] === "aus")  rtn = "1";
else if (res[2] === "auto") rtn = "2";
else if (res[2] === "therm") {
	// TODO genauer unterscheiden: Frostschutz, kurz, theorisch abh von Zeit und Temp
	rtn = "3";
}
rtn;
