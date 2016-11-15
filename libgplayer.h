#ifndef	__LIBGPLAYER_H__
#define	__LIBGPLAYER_H__

#include <gst/gst.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>	

#define MAX_BUF_SIZE  1024
#define LCD_WIDTH   320
#define LCD_HEIGHT  240
#define AUDIO_SINK_ARG "alsasink device=\"plug:mainvol\""
#define VIDEO_SINK_ARG "v4l2sink overlay-top=0 overlay-left=0 overlay-width=800 overlay-height=480"
#define EXTRE_ARGS     "ring-buffer-max-size=8192000"

typedef struct _WindowPos {
	unsigned int sx;
	unsigned int sy;
	unsigned int disp_width;
	unsigned int disp_height;
} WindowPos;

typedef struct _CustomData {
  GMainLoop  *loop;
  GstElement *pipeline;
  GstBus     *bus;
  
  gchar       *url;
  gint64      startPos;
  gint64      duration;
  guint       watchID;
  gboolean    seek_enabled;
  gboolean    live_stream;
  gboolean    playing;  /* Playing or Paused */
  gboolean    terminate;
  WindowPos   windowpos;
  
  pthread_t   player_thread;
} CustomData;


#ifdef	__cplusplus
extern "C"{
#endif

////!!!!All function return 0 means success, -1 means error.

/* open_player
 * @url: video stream source, format is like below:
 * 		file:///home/root/abc.mp4
 * 	or	http://192.168.1.2/abc.mp4
 * 
 *  Init gstreamer and start to play. it contain a loop, so don't forget to close it.
 */
gint open_player(gchar *url, unsigned int sx, unsigned int sy, unsigned int disp_width, unsigned int disp_height);

/* start_player
 * 
 *  Change pipeline state to GST_STATE_PLAYING, video streaming will start to play.
 */
gint start_player(void);

/* stop_player
 * 
 *  Change pipeline state to GST_STATE_PAUSED, video streaming will paused.
 */
gint pause_player(void);

/* change_state
 * @state: pipeline state, 1 means GST_STATE_PLAYING, 0 means GST_STATE_PAUSED.
 * 
 *  This function combine start_player() and stop_player(), it can be controled by state parameter.
 */
gint change_state(gboolean state);

/* seek_player
 * @seek_pos: the value which from 0 to duration of stream.
 * 
 * Seek video stream to a given position, its unit is second, and range is from 0 to duration of stream.
 * 
 */
gint seek_player(gint64 seek_pos);

/* release_player
 * 
 * Close gstreamer player, free resource.
 */
gint release_player(void);

gint poll_msg(void);

#ifdef	__cplusplus
}
#endif

#endif	// __LIBGPLAYER_H__
