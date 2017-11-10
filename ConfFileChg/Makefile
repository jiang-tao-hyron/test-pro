#######
# Makefile for G2CAM
# Copyright(C) 2002-2017 MegaChips Co.,Ltd All right reserved.
# author: "Jiangtao"<jiangtao@hyron-js.com>
####

####
# set target & soruce file
TARGET     = ConfFileChg
SRC 		= ConfFileChg.c
OBJ	    = $(SRC:.c=.o)

##########
# The rule is set. 
all:$(TARGET)

$(TARGET): $(OBJ)
	gcc -Wall -g -O2 -o $@ $(OBJ)

.c.o: $(SYSFILE)
	gcc -Wall -g -O2 -c $<

clean:
	rm -f $(OBJ) $(TARGET)

install: $(TARGET)
	@echo "" >> /dev/null

nfs: $(TARGET)
	@echo "" >> /dev/null

