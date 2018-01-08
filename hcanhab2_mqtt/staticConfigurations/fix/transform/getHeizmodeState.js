/* Bsp. input: '148/H/therm/16.5/2.8'  mit 148=gruppe
	output Mode: 1,2,3,33,4,5 als String */
var res = input.split("/",5);
var rtn = "ng";

// Mode ermitteln: 
if      (res[2] === "aus")  rtn = "1";
else if (res[2] === "auto") rtn = "2";
else if (res[2] === "therm")
{
	if (res[3] > 10.0)
	{
		if (Math.round(res[4]) === 0.0) rtn = "3"; // 0= dauer unbegrenzt
		else rtn = "33"; // sonst vermutlich 3h		
	}
	else
	{
		if (Math.round(res[4]) === 0.0) rtn = "4"; // 0= dauer unbegrenzt
		else rtn = "34"; // sonst vermutlich 3h		
	}
}
rtn;
