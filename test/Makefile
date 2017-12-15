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
TARGET1      = test
TARGET2      = a.out
SRC1         = test.c
SRC2         = HistPlus.c
OBJ1         = $(SRC1:.c=.o)
OBJ2         = $(SRC2:.c=.o)

#####
# include local macro
include $(TOPDIR)/tool/tool_make.flags
INCS = -I$(TOPDIR)/process/hdd/arm_sqlite/include/ -I$(TOPDIR)/process/sysmain/sysmain -I$(TOPDIR)/include
LIBS = -L$(TOPDIR)/process/hdd/arm_sqlite/lib -lsqlite3 -lreadline -lhistory

##########
# The rule is set. 
all:	$(TARGET1) $(TARGET2)

$(TARGET1): $(OBJ1)
	$(CC) $(CFLAGS_DEF) -o $@ $(OBJ1) $(EXTOBJS) $(LIBS)
	
$(TARGET2): $(OBJ2)
	$(CC) $(CFLAGS_DEF) -o $@ $(OBJ2) $(EXTOBJS) $(LIBS)

.c.o: $(SYSFILE)
	$(CC) $(INCS) $(ADD_INCS) $(CFLAGS_DEF) -c $<

clean:
	$(RM) $(OBJ1) $(TARGET1)

copy:
	@cp -pR ./ $(DEST_DIR)

euc:
	@(cd $(EUC_TOP_DIR); bin/NKF.sh $(DEST_DIR))
