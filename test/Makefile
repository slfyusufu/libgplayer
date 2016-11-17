#
# Makefile
#
# Makefile for gstreamer
#PLATFORM = ARM
PLATFORM = ARM_228
#PLATFORM = PC
#PLATFORM = PC_HOME

######################################################################################################################
ifeq ($(PLATFORM), ARM)

CC = arm-none-linux-gnueabi-gcc
CFLAGS  = -lrt -O2 -Wall -fPIC -D_GNU_SOURCE -lpthread
#-static -lpthread -ldl
CFLAGS += -I/home/yusufu/mywork/open-source/glib/glib-2.40/out-arm/include/glib-2.0/
CFLAGS += -I/home/yusufu/mywork/open-source/glib/glib-2.40/out-arm/lib/glib-2.0/include/
LDFLAGS +=  -L/home/yusufu/mywork/open-source/glib/glib-2.40/out-arm/lib -lgobject-2.0 -lglib-2.0
#CFLAGS += -I/usr/include/gstreamer-1.0
CFLAGS += -I/home/yusufu/mywork/linux/telechips/linux-platform/prebuilts/include/gstreamer-1.0
LDFLAGS += -L/home/yusufu/mywork/linux/telechips/linux-platform/myout/tccgst/lib -lgstreamer-1.0
CFLAGS += -I/home/yusufu/mywork/open-source/zlib/zlib_git/zlib_arm/include
LDFLAGS += -L/home/yusufu/mywork/open-source/zlib/zlib_git/zlib_arm/lib -lz

CFLAGS += -I/home/yusufu/mywork/workspace/gstreamer/libgplayer
LDFLAGS += -L/home/yusufu/mywork/workspace/gstreamer/libgplayer -lgplayer

endif

######################################################################################################################
ifeq ($(PLATFORM), ARM_228)

SOURCE_PATH=/home/s100018/mywork/linux/telechips/als-linux-official/build/tcc8925_carbit/tmp/work/cortexa5-vfp-neon-telechips-linux-gnueabi

CC = arm-none-linux-gnueabi-gcc
CFLAGS  = -O2 -Wall -fPIC -D_GNU_SOURCE -lpthread
#-static -lpthread -ldl
CFLAGS += -I$(SOURCE_PATH)/glib-2.0/1_2.44.1-r0/image/usr/include/glib-2.0
CFLAGS += -I$(SOURCE_PATH)/glib-2.0/1_2.44.1-r0/image/usr/lib/glib-2.0/include/
LDFLAGS += -L$(SOURCE_PATH)/glib-2.0/1_2.44.1-r0/image/usr/lib -lgobject-2.0 -lglib-2.0 -lgmodule-2.0
CFLAGS += -I$(SOURCE_PATH)/gstreamer1.0/1.2.4-r0/image/usr/include/gstreamer-1.0
LDFLAGS += -L$(SOURCE_PATH)/gstreamer1.0/1.2.4-r0/image/usr/lib -lgstreamer-1.0
CFLAGS += -I$(SOURCE_PATH)/zlib/1.2.8-r0/image/usr/include
LDFLAGS += -L$(SOURCE_PATH)/zlib/1.2.8-r0/image/usr/lib -lz
CFLAGS += -I$(SOURCE_PATH)/libffi/3.2.1-r0/image/usr/lib/libffi-3.2.1/include/
LDFLAGS += -L$(SOURCE_PATH)/libffi/3.2.1-r0/image/usr/lib/ -lffi

CFLAGS += -I../
LDFLAGS += -L../ -lgplayer

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

CFLAGS += -I../
LDFLAGS += -L../ -lgplayer

endif

######################################################################################################################

SOURCES = main.c

OBJECTS	= ${SOURCES:.c=.o}

OUT	= main

all: $(OUT)
	@echo Build DONE.

$(OUT): $(OBJECTS)
	$(CC)  $(OBJECTS) $(CFLAGS) $(LDFLAGS)  -o $(OUT)

clean:
	rm -f $(OBJECTS) *~ $$$$~* *.bak core config.log $(OUT)
