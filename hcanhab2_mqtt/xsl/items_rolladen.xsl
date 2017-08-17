<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  

// TODO Slider: PERCENT 0..100 bei * in 100...0 umsetzen

-->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
//
//                        openHAB   openHCAN
// o Input  von ROLLADEN: 0..100 &lt;- ROLLADEN_POSITION_REPLAY (22), ROLLADEN_POSITION_CHANGED_INFO (24)
// o Output von ROLLADEN: 0..100 -&gt; ROLLADEN_POSITION_SET (20)
// 
	<xsl:apply-templates select="bereich/raum/board/rolladen"/>
	<xsl:apply-templates select="bereich/board/rolladen"/>
</xsl:template>

<xsl:template match="rolladen">
Rollershutter ROLLADEN_<xsl:value-of select="@name" /> "<xsl:value-of select="@stt" />-Rolladen" &lt;rollershutter&gt; (Rolladen) {mqtt="&gt;[bpiBroker:cb&lt;:command:UP:Ro/<xsl:value-of select="@gruppe" />/201], &gt;[bpiBroker:cb&lt;:command:DOWN:Ro/<xsl:value-of select="@gruppe" />/200], &gt;[bpiBroker:cb&lt;:command:STOP:Ro/<xsl:value-of select="@gruppe" />/202], &lt;[bpiBroker:cb&gt;:state:JS(getRollershutterState.js):<xsl:value-of select="@gruppe" />/Ro/.*]", autoupdate="true"}
String SET_ROLLADEN_<xsl:value-of select="@name" /> 	"<xsl:value-of select="substring-after(@name,'__')" />" &lt;rollershutter&gt; {mqtt="&gt;[bpiBroker:cb&lt;:command:*:<xsl:value-of select="@gruppe" />/Ro/${command}]", autoupdate="true"}
</xsl:template>


</xsl:stylesheet>
