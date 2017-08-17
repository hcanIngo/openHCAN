<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
//
//                        openHAB      openHCAN
// Input vom TEMPSENSOR: Number °C &lt;- 1WIRE_TEMPERATURE (34), 1WIRE_TEMPERATURE_REPLAY (36)
//
		<xsl:apply-templates select="bereich/raum/board/tempsensor"/>
		<xsl:apply-templates select="bereich/board/tempsensor"/>
</xsl:template>

<xsl:template match="tempsensor">
// getaggt geht: 
// in Heizung verwendet:    
//ist in Heizung enthalten: Number TEMPERATUR_<xsl:value-of select="@name" /> 	"<xsl:value-of select="@stt" />-Sensor [%.1f °C]" &lt;temperature&gt; (Temperaturen) [ "CurrentTemperature" ] {mqtt="&lt;[bpiBroker:cb&gt;:state:JS(getTemp.js):<xsl:value-of select="@gruppe" />/T/.*]", autoupdate="true"}
</xsl:template>


</xsl:stylesheet>

