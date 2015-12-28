<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
	<xsl:apply-templates select="bereich/raum/board/rolladen"/>
	<xsl:apply-templates select="bereich/board/rolladen"/>
</xsl:template>

<xsl:template match="rolladen">
rule "Rolladen"
when
	Item ROLLADEN_<xsl:value-of select="@name" /> received command
then
	var Number percent = 0
	// nicht den aktuelle Stand des Rolladen als neue anzufahrende Position verwenden: if(ROLLADEN_OG_K1__K1_Einzelfenster.state instanceof DecimalType) percent = ROLLADEN_OG_K1__K1_Einzelfenster.state as DecimalType

	// logInfo("openHAB", percent.toString)	
<!-- item=Rollershutter -->
	// falls item=Rollershutter verwendet:
	if(receivedCommand==STOP) percent = 202
	else if(receivedCommand==DOWN) percent = 200
	else if(receivedCommand==UP) percent = 201
	if (percent >= 200) {
		sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd="+percent.toString+"&amp;d=rolladen&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
		return
	}

<!-- item=Dimmer-->
	// falls item=Dimmer verwendet oder beim rollershutter der Prozentwert:
	if(receivedCommand==DECREASE) percent = percent + 5 // "DECREASE"-Button gehalten
	if(receivedCommand==INCREASE) percent = percent - 5 // "INCREASE"-Button gehalten
	if(receivedCommand==OFF) percent = 100 // kurzer "DECREASE"-Button-Klick
	if(receivedCommand==ON) percent = 0 // kurzer "INCREASE"-Button-Klick

	if(percent&lt;0)   percent = 0
	if(percent&gt;100) percent = 100
	
	// unnoetig: postUpdate(ROLLADEN_<xsl:value-of select="@name" />, percent.toString);
	// say("Sollpos (openHAB-zu = 100 %): " + percent.toString)

	percent = 100 - percent // invertieren, da die rolladen von openhab zu openhcan invertierte Logik aufweisen
	// logInfo("openHCAN", percent.toString)
	sendHttpGetRequest("http://C1612server/?callback=myjp&amp;cmd="+percent.toString+"&amp;d=rolladen&amp;setid=<xsl:value-of select="@gruppe" />&amp;qid=")
end
</xsl:template>


</xsl:stylesheet>

