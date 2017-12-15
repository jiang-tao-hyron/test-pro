#######

####
# set global macro
ifndef TOPDIR
PWD   := $(shell pwd)
TOPDIR =$(PWD)/../..
endif
include $(TOPDIR)/make.rules

####
# set target & soruce file
TARGET      = histread

SRC         = histdummy.c
OBJ         = $(SRC:.c=.o)

#####
# include local macro
include $(TOPDIR)/tool/tool_make.flags
INCS = -I$(TOPDIR)/process/hdd/arm_sqlite/include/ -I$(TOPDIR)/process/sysmain/sysmain -I$(TOPDIR)/include
LIBS = -L$(TOPDIR)/process/hdd/arm_sqlite/lib -lsqlite3 -lreadline -lhistory

##########
# The rule is set. 
all:	$(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS_DEF) -o $@ $(OBJ) $(EXTOBJS) $(LIBS)

.c.o: $(SYSFILE)
	$(CC) $(INCS) $(ADD_INCS) $(CFLAGS_DEF) -c $<

clean:
	$(RM) $(OBJ) $(TARGET)

install: $(TARGET)
	$(CP) $(TARGET) $(RELEASE_PATH)/bin

copy:
	@cp -pR ./ $(DEST_DIR)

euc:
	@(cd $(EUC_TOP_DIR); bin/NKF.sh $(DEST_DIR))
