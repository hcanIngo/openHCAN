include ../ARCH.inc

TARGET = hcan-backup-config hcan-discover hcan-restore-config

.PHONY: all install

all: replaceIP install

replaceIP:
	@cp ./$(TARGET) ./install
	@find ./install/hcan-* -type f -exec sed -i "s/127.0.0.1/$(IP_HI)/g" {} \;
 
install:
	cd ./install; sudo install $(TARGET) /usr/bin
	@cd ./install; sudo rm -f $(TARGET)

clean:
	cd ./install; sudo rm -f $(TARGET)
	cd /usr/bin/; sudo rm -f $(TARGET)
