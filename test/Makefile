#
# Makefile
#
# Makefile for gstreamer
PLATFORM = ARM
#PLATFORM = ARM_ADT
#PLATFORM = PC
#PLATFORM = PC_HOME

######################################################################################################################
ifeq ($(PLATFORM), ARM)

CC = arm-none-linux-gnueabi-gcc
CFLAGS  = -O2 -Wall -fPIC -D_GNU_SOURCE -lpthread
#-static -lpthread -ldl
CFLAGS += -I/home/yusufu/mywork/open-source/glib/glib-2.40/out-arm/include/glib-2.0/
CFLAGS += -I/home/yusufu/mywork/open-source/glib/glib-2.40/out-arm/lib/glib-2.0/include/
LDFLAGS +=  -L/home/yusufu/mywork/open-source/glib/glib-2.40/out-arm/lib -lgobject-2.0 -lglib-2.0
#CFLAGS += -I/usr/include/gstreamer-1.0
CFLAGS += -I/home/yusufu/mywork/linux/telechips/linux-platform/prebuilts/include/gstreamer-1.0
LDFLAGS += -L/home/yusufu/mywork/linux/telechips/linux-platform/myout/tccgst/lib -lgstreamer-1.0
CFLAGS += -I/home/yusufu/mywork/open-source/zlib/zlib_git/zlib_arm/include
LDFLAGS += -L/home/yusufu/mywork/open-source/zlib/zlib_git/zlib_arm/lib -lz

endif

######################################################################################################################
ifeq ($(PLATFORM), ARM_ADT)

CC = /home/yusufu/mywork/workspace/qt/sdk/sysroots/x86_64-oesdk-linux/usr/bin/arm-telechips-linux-gnueabi/arm-none-linux-gnueabi-gcc
#CC = arm-none-linux-gnueabi-gcc
CFLAGS  = -O2 -Wall -fPIC -D_GNU_SOURCE 
#-static -lpthread -ldl
CFLAGS += -I/home/yusufu/mywork/workspace/qt/sdk/sysroots/cortexa7-vfp-neon-telechips-linux-gnueabi/usr/include/gstreamer-1.0
CFLAGS += -I/home/yusufu/mywork/workspace/qt/sdk/sysroots/cortexa7-vfp-neon-telechips-linux-gnueabi/usr/include/glib-2.0
CFLAGS += -I/home/yusufu/mywork/workspace/qt/sdk/sysroots/cortexa7-vfp-neon-telechips-linux-gnueabi/usr/lib/glib-2.0/include
LDFLAGS += -L/home/yusufu/mywork/workspace/qt/sdk/sysroots/cortexa7-vfp-neon-telechips-linux-gnueabi/usr/lib -lgstreamer-1.0 -lglib-2.0 -lgobject-2.0
#LDFLAGS += -L/home/yusufu/mywork/linux/telechips/linux-platform/myout/tccgst/lib -lgstreamer-1.0

endif

######################################################################################################################
ifeq ($(PLATFORM), PC)
export LD_LIBRARY_PATH=/home/yusufu/mywork/open-source/glib/glib-2.40/out-pc/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/home/yusufu/mywork/open-source/gstreamer/gst-out/lib:$LD_LIBRARY_PATH
export GST_PLUGIN_PATH=/home/yusufu/mywork/open-source/gstreamer/gst-out/lib:$GST_PLUGIN_PATH
 
CC = gcc
CFLAGS  = -O2 -Wall -fPIC -D_GNU_SOURCE -lpthread
#-static -lpthread -ldl
CFLAGS += -I/home/yusufu/mywork/open-source/glib/glib-2.40/out-pc/include
LDFLAGS +=  -L/home/yusufu/mywork/open-source/glib/glib-2.40/out-pc/lib -lgobject-2.0 -lglib-2.0
#CFLAGS += -I/usr/include/gstreamer-1.0
CFLAGS += -I/home/yusufu/mywork/open-source/gstreamer/gst-out/include/gstreamer-1.0
LDFLAGS +=  -L/home/yusufu/mywork/open-source/gstreamer/gst-out/lib -lgstreamer-1.0 


CFLAGS += -I/home/yusufu/mywork/workspace/gstreamer/libgplayer
LDFLAGS += -L/home/yusufu/mywork/workspace/gstreamer/libgplayer -lgplayer

endif

######################################################################################################################
ifeq ($(PLATFORM), PC_HOME)

CC = gcc
CFLAGS  = -O2 -Wall -fPIC -D_GNU_SOURCE -lpthread 

CFLAGS  += `pkg-config --cflags glib-2.0`
LDFLAGS += `pkg-config --libs glib-2.0`

CFLAGS  += `pkg-config --cflags gobject-2.0`
LDFLAGS += `pkg-config --libs gobject-2.0`

CFLAGS  += `pkg-config --cflags gstreamer-1.0`
LDFLAGS += `pkg-config --libs gstreamer-1.0`
endif

CFLAGS += -I/home/yusufu/mywork/workspace/gstreamer/libgplayer
LDFLAGS += -L/home/yusufu/mywork/workspace/gstreamer/libgplayer -lgplayer
######################################################################################################################

#SOURCES	= init.c
#SOURCES	= playmp3.c
#SOURCES   = gst-time.c
#SOURCES = playbin.c
#SOURCES = playbin2.c
#SOURCES = playbin3.c
SOURCES = main.c
#SOURCES = basic-player.c
#SOURCES  = seek.c
#SOURCES  = telechips/gst-seek-test.c
OBJECTS	= ${SOURCES:.c=.o}

OUT	= main

all: $(OUT)
	@echo Build DONE.

$(OUT): $(OBJECTS)
	$(CC)  $(OBJECTS) $(CFLAGS) $(LDFLAGS)  -o $(OUT)

clean:
	rm -f $(OBJECTS) *~ $$$$~* *.bak core config.log $(OUT)
