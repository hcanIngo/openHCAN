#!/usr/bin/awk -f
# $1=set, $2xxx_id, $3=id-value
BEGIN {print "digraph G{"}
BEGIN {print "graph [rankdir=BT]"} #LR (untenstehendes Beispiel), BT (von unten nach oben)
BEGIN {BlockNr=0; parseBlock = false}
{
  #fuer jede Zeile:

  if($1 == "create")
  {
	if ( ($2 == "screensaver_page") ||  ($2 == "leave_come_page") ||  ($2 == "dir_page") || ($2 == "rolladen_page") || ($2 == "powerport_page") || ($2 == "gesamt_heiz_page") || ($2 == "heiz_page") || ($2 == "temp_anzeige_page") )
	{
		parseBlock = true
		BlockZeilenOFFSET = NR
		BlockNr++
		genaralPageName[BlockNr] = $2
		#printf "\"parseBlockNr=%s %s\"\n", BlockNr, genaralPageName[BlockNr]
	}
  }
  #printf "\"page_id[%s]=%s\"\n", BlockNr, pageId[BlockNr]
  
  if(parseBlock == true)
  {
	if(NR == BlockZeilenOFFSET+1) {pageId[BlockNr] = $3}
	if(NR == BlockZeilenOFFSET+2) {parentId[BlockNr] = $3}
	if(NR == BlockZeilenOFFSET+3) {nextId[BlockNr] = $3}
	if(NR == BlockZeilenOFFSET+4) {prevId[BlockNr] = $3}
	if($2 == "name") {pageName[BlockNr] = $3}
  }

  if( ($1 == "exit") && (parseBlock == true) )
  {
	#one block finished -> save page/state -name (Page-Bezeichnung leicht aenderbar!):
	pageBez[ pageId[BlockNr] ] = genaralPageName[BlockNr]"__"pageId[BlockNr]"__"pageName[BlockNr]
	parseBlock = false
	#printf "exitGefunden,BlockNr=%s:PageBez=%s\n", BlockNr, pageBez[ pageId[BlockNr] ]
  }
}
#all lines were read:
END{
	for(i=1; i<=BlockNr; i++)
	{
	  #printf "i=%i\n", i
	  if(parentId[i] != 0) {
		printf "%s->%s [label=\"parent_id\",fontcolor=green][color=green];\n", pageBez[ pageId[i] ], pageBez[ parentId[i] ]}
	  if(nextId[i] != 0) {
		printf "%s->%s [label=\"next_id\",fontcolor=blue][color=blue];\n", pageBez[ pageId[i] ], pageBez[ nextId[i] ]}
	  if(prevId[i] != 0) {
		printf "%s->%s [label=\"prev_id\",fontcolor=pink][color=pink];\n", pageBez[ pageId[i] ], pageBez[ prevId[i] ]}
	}
}
END{print "}"}
