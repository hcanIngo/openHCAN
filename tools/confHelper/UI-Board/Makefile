# Verwendung per:  make

CF_FILE = 72
DOT = ./$(CF_FILE).dot
TARGET = ./$(CF_FILE).png

CF_SRC = /home/il/Dropbox/HCAN/HI_SERVER/conf

cf2png:
	awk -f cf2dot.awk $(CF_SRC)/$(CF_FILE).cf > $(DOT)
	dot -Tpng $(DOT) -o $(TARGET)
	#less $(DOT)

clean:
	rm $(DOT)
	rm $(TARGET)

