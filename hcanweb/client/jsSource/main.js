// jQuery(document) alias $(document) 

var globalCurrentPage = null;
var globalCurrentPageSelectedFilter = null;
var globalPages = ["lampe","sonstige","heizung","reedkontakt","rolladen","Einstellungen"];
var globalState = "aus";
var globalTyp = {}; // soll IdName der Devices aufnehmen (typ = "lampe" usw.)
var globalPageChangeable = true;
var globalLog = false;

var globalXml = null; // haelt die xml-Daten aus der installation.xml

var globalCANserverURL = '';
var globalStrFilterEintraege = '';

var globalFilteredIdName = {};
var globalFilteredIds;

var globalFehler = "";


function logIt (str) {
	console.log (str);
	// alert (str);
}

// Bind to "mobileinit" before you load jquery.mobile.js
$( document ).on('mobileinit', function () {
	$.mobile.defaultPageTransition = "none"; // default transition: slide
    //alert('width='+$(window).width());
/*    $.event.special.swipe.horizontalDistanceThreshold = $(window).width()/6; // default: 30px - Swipe horizontal displacement must be more than this. 
    $.event.special.swipe.verticalDistanceThreshold = 150; // default: 75px - Swipe vertical displacement must be less than this.
    $.event.special.swipe.scrollSupressionThreshold = 20; // default: 10px - More than this horizontal displacement, and we will suppress scrolling.
*/
    
	//localStorage.clear(); // falls die App sich mal aufhaengt: Z.B. usb_ubuntu.txt oeffnen. Grr -> TODO
	
	getPersitenzOrDefaults ();
    hookElementarPageEvents ();
});
