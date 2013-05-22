<?xml version="1.0" encoding="iso-8859-1" ?>
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">

	<xsl:output method="text" />

<xsl:template match="block">
\subsection{<xsl:value-of select="@name" />}
\begin{tabular}{lll}
\textbf{Block:}&amp;\textbf{Datentyp:}&amp;\textbf{Feld:}\\ 
<xsl:apply-templates select="field" />&amp;&amp;
\end{tabular} <xsl:value-of select="." />
</xsl:template>
<xsl:template match="field"><xsl:value-of select="../@name" />&amp;<xsl:value-of select="@datatype" />&amp;<xsl:value-of select="@name" />\\
</xsl:template>
</xsl:stylesheet>

