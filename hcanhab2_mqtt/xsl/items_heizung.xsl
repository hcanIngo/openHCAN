<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
// Enthaltene Items: 
// o HEIZUNG:  Raum-Heizkörper-Heizungsmode abfragen/vorgeben. 
// o SOLLTEMP: Raum-Soll-Tempereatur abfragen/vorgeben.
// 
//                       openHAB       openHCAN
// o Input  von HEIZMODE: Mode       &lt;- HEIZUNG_MODE_OFF_DETAILS (51), HEIZUNG_MODE_THERMOSTAT_DETAILS (55), HEIZUNG_MODE_AUTOMATIK_DETAILS (57)
// o Input  von HEIZMODE: Ist-Temp.  &lt;- HEIZMODE_TIST_REPLAY (60)
// o Output von HEIZMODE: Mode       -&gt; HEIZUNG_SET_MODE_OFF (52), HEIZUNG_SET_MODE_AUTOMATIK (58), HEIZUNG_SET_MODE_THERMOSTAT_DETAILS (56)
// o Output von SOLLTEMP: 0..100     -&gt; HEIZUNG_SET_MODE_THERMOSTAT_DETAILS (56)
// o Input  von SOLLTEMP: Soll-Temp  &lt;- HEIZUNG_MODE_THERMOSTAT_DETAILS (55), HEIZUNG_MODE_AUTOMATIK_DETAILS (57)
// 
	<xsl:apply-templates select="bereich/raum/board/heizung"/>
	<xsl:apply-templates select="bereich/board/heizung"/>
</xsl:template>

<xsl:template match="heizung">
Group <xsl:value-of select="substring-before(@name,'__')" /> "<xsl:value-of select="@stt" />heizung" (Heizungen) ["Thermostat"]
Number TEMPERATUR_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" /> [%.1f °C]" &lt;temperature&gt; (<xsl:value-of select="substring-before(@name,'__')" />) ["CurrentTemperature"] {mqtt="&lt;[bpiBroker:cb&gt;:state:JS(getTemp.js):<xsl:value-of select="@gruppe" />/T/.*]", autoupdate="true"}
Number SOLLTEMP_<xsl:value-of select="@name" /> &lt;temperature&gt; (<xsl:value-of select="substring-before(@name,'__')" />) ["TargetTemperature"] {mqtt="&lt;[bpiBroker:cb&gt;:state:JS(getSolltemp.js):<xsl:value-of select="@gruppe" />/H/(auto|therm)/.*], &gt;[bpiBroker:cb&lt;:command:*:H/therm/<xsl:value-of select="@gruppe" />/${command}]", autoupdate="true"}
String SOLLTEMP_DAUER_<xsl:value-of select="@name" /> {mqtt="&gt;[bpiBroker:cb&lt;:command:*:H/therm/<xsl:value-of select="@gruppe" />/${command}]"}
String HEIZMODE_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" />" &lt;heating&gt; (<xsl:value-of select="substring-before(@name,'__')" />) ["homekit:HeatingCooling"] {mqtt="&lt;[bpiBroker:cb&gt;:state:JS(getHeizmodeState.js):<xsl:value-of select="@gruppe" />/H/.*], &gt;[bpiBroker:cb&lt;:command:1:H/aus/<xsl:value-of select="@gruppe" />], &gt;[bpiBroker:cb&lt;:command:2:H/auto/<xsl:value-of select="@gruppe" />]", autoupdate="true"}

</xsl:template>


</xsl:stylesheet>

