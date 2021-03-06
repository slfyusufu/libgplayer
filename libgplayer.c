//********************************************************************************************
/*
 * @file        libgplayer.c
 * @brief		Library of gstreamer player for application use. 
 *
 * @author      Yusuf.Sha, Telechips Shenzhen Rep.
 * @date        2016/11/08
 */
//********************************************************************************************

#include "libgplayer.h"

static CustomData player_data;
static EndOfStream_Callback g_eos_callback=NULL;

/* Set Callback Functions */
void EndOfStream_cb(EndOfStream_Callback cb_func)
{
	g_eos_callback = cb_func;
}

static gboolean
Message_handler(CustomData *player,
				GstMessage *msg
				)
{
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			g_print ("[libgplayer] End of stream\n");
			//gst_element_set_state (player->pipeline, GST_STATE_READY);
			player->terminate = 1;
			if(g_eos_callback != NULL)
				g_eos_callback();
			break;
	
		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;
	
			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);
	
			g_printerr ("[libgplayer] Error: %s\n", error->message);
			g_error_free (error);

			//gst_element_set_state (player->pipeline, GST_STATE_READY);
			player-> terminate = 1;
			if(g_eos_callback != NULL)
				g_eos_callback();
			break;
		}
		case GST_MESSAGE_CLOCK_LOST: {
			/* Get a new clock */  
			gst_element_set_state (player->pipeline, GST_STATE_PAUSED);  
			gst_element_set_state (player->pipeline, GST_STATE_PLAYING);  
			break;
		}
		case GST_MESSAGE_STATE_CHANGED: {
			GstState old_state, new_state, pending_state;
			gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
			if (GST_MESSAGE_SRC (msg) == GST_OBJECT (player->pipeline)) 
			{
				g_print ("[libgplayer] Pipeline state changed from %s to %s:\n",
				gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
			}
			/* Remember whether we are in the PLAYING state or not */
			player->playing = (new_state == GST_STATE_PLAYING);
         
			if (player->playing && (GST_MESSAGE_SRC (msg) == GST_OBJECT (player->pipeline)))
			{
				/* We just moved to PLAYING. Check if seeking is possible */
				GstQuery *query;
				gint64 start, end;
				query = gst_query_new_seeking (GST_FORMAT_TIME);
				if (gst_element_query (player->pipeline, query))
				{
					gst_query_parse_seeking (query, NULL, &player->seek_enabled, &start, &end);
					if (player->seek_enabled) {
						//g_print ("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
						;//GST_TIME_ARGS (start), GST_TIME_ARGS (end));
					} else {
						g_print ("[libgplayer] Seeking is DISABLED for this stream.\n");
					}
				} else {
					g_printerr ("[libgplayer] Seeking query failed.");
				}
				gst_query_unref (query);
				
				GstFormat fmt = GST_FORMAT_TIME;
				gint64 current = -1;
				
				/* Query the current position of the stream */
				if (!gst_element_query_position (player->pipeline, fmt, &current)) {
					g_printerr ("[libgplayer] Could not query current position.\n");
				}
				
				/* If we didn't know it yet, query the stream duration */
				if (!GST_CLOCK_TIME_IS_VALID (player->duration)) {
					if (!gst_element_query_duration (player->pipeline, fmt, &player->duration)) {
						g_printerr ("[libgplayer] Could not query current duration.\n");
					}
				}
				/* Print current position and total duration */
				//g_print ("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
				//		GST_TIME_ARGS (current), GST_TIME_ARGS (player->duration));
			}
		
			break;
		}
		default:
		{
			if(GST_MESSAGE_SRC (msg) == GST_OBJECT (player->pipeline)){
				g_print("[libgplayer] GST_MSG = 0x%x.\n",GST_MESSAGE_TYPE (msg));
			}
			break;
		}
	}
	return TRUE;
}

static gint
init_player(CustomData *player)
{
	GstMessage *msg;
	char buffer [MAX_BUF_SIZE];
#ifdef CHECK_TIME
	clock_t clock_start, clock_end;
	clock_start = clock();
#endif
	/* init GStreamer */
	//gst_init (&argc, &argv);
	gst_init (NULL, NULL);
#ifdef CHECK_TIME
	clock_end = clock();
	g_printerr("[libgplayer] init time = (%ld)us.\n",clock_end - clock_start);
	clock_start = clock();
#endif
	#ifdef TCC //for TCC platform
	(void)sprintf (buffer, "playbin uri=%s audio-sink=%s video-sink=%s %s",
							player->url, AUDIO_SINK_ARG, VIDEO_SINK_ARG, EXTRE_ARGS);
	#else //for PC test
	(void)sprintf (buffer, "playbin uri=%s audio-sink=\"alsasink\"",
							player->url);
	#endif
	g_print("[libgplayer] Version:%s Gst command %s\n", GPLAYER_VERSION, buffer);  		
	
	/* Build the pipeline */  
	player->pipeline = gst_parse_launch (buffer, NULL);	
	if(player->pipeline == NULL)
	{	
		g_printerr("[libgplayer] Create playbin failed!!\n");
		return -1;
	}
#ifdef CHECK_TIME
	clock_end = clock();
	g_printerr("[libgplayer] pipeline time = (%ld)us.\n",clock_end - clock_start);
	clock_start = clock();
#endif
	g_object_get(player->pipeline, "video-sink", &player->video_sink, NULL);

	player->bus = gst_pipeline_get_bus (GST_PIPELINE(player->pipeline));
	if(player->bus == NULL)
		g_printerr("[libgplayer] Create bus_watch failed!!\n");
		
	/* Below two line are vilid if using main_loop */
	//gst_bus_add_signal_watch (player_data.bus);
	//g_signal_connect (player_data.bus, "message::state-changed", G_CALLBACK (cb_message_state_change), NULL);
	
	#ifdef TCC //for TCC platform
	/* Set overlay size */
	g_object_set(player->video_sink, "overlay-set-top", player->windowpos.sx, NULL);
	g_object_set(player->video_sink, "overlay-set-left", player->windowpos.sy, NULL);
	g_object_set(player->video_sink, "overlay-set-width", player->windowpos.disp_width, NULL);
	g_object_set(player->video_sink, "overlay-set-height", player->windowpos.disp_height, NULL);
	g_object_set(player->video_sink, "overlay-set-update", 1, NULL);
	#endif

	g_print("[libgplayer] Overlay-top=%d, Overlay-left=%d.\n",player->windowpos.sx, player->windowpos.sy);
	g_print("[libgplayer] Overlay-width=%d, Overlay-height=%d.\n",player->windowpos.disp_width, player->windowpos.disp_height);
	/* ================ Start playing =================== */
#ifdef CHECK_TIME
	clock_end = clock();
	g_printerr("[libgplayer] overlay time = (%ld)us.\n",clock_end - clock_start);
	clock_start = clock();
#endif
	change_state(1);
#ifdef CHECK_TIME
	clock_end = clock();
	g_printerr("[libgplayer] change time = (%ld)us.\n",clock_end - clock_start);
#endif
	player->Is_pipeline_ready = 1;
	g_print("[libgplayer] Start Playing!!!\n");
	/* now do...while */
	while(!player->terminate)
	{
		msg = gst_bus_timed_pop_filtered (player->bus, 10 * GST_MSECOND,
        GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION | GST_MESSAGE_CLOCK_LOST);
        
        if (msg != NULL) {
			Message_handler (player, msg);
		}
	} 
	g_print("[libgplayer] Player is terminated!\n");
	
	g_print("[libgplayer] Play finished! Start to Free!\n");
	if(player->pipeline != NULL)
	{
		gst_element_set_state (player->pipeline, GST_STATE_NULL);
		player->Is_pipeline_ready = 0;
		gst_object_unref (player->bus);
		gst_object_unref (player->pipeline);
	}

	return 0;
}

static void *
play_thread(void *arg)
{
	CustomData *player = (CustomData *)arg;
	
	//pthread_detach(pthread_self());
	init_player(player);
	pthread_exit(0);
	return NULL;
}

gint
open_player(gchar *url, unsigned int sx, unsigned int sy, unsigned int disp_width, unsigned int disp_height)
{
	int err;
	
	memset(&player_data, 0, sizeof(CustomData));
	
	/* Init varible of player */
	player_data.playing = FALSE;
	player_data.terminate = FALSE;
	player_data.seek_enabled = FALSE;
	player_data.live_stream = FALSE;
	player_data.duration = GST_CLOCK_TIME_NONE;
	player_data.windowpos.sx = sx;
	player_data.windowpos.sy = sy;
	player_data.windowpos.disp_width = disp_width;
	player_data.windowpos.disp_height = disp_height;
	
	/* make sure we have an URI */
	if(url == NULL) {
		g_print ("[libgplayer] URL == NULL!!!!!\n");
		g_print ("[libgplayer] Usage: ./main http://192.168.1.2/abc.mp4\n");
		g_print ("                 Or ./main file:///home/root/abc.mp4\n");		
		return -1;
	}
	if(strlen(url) > MAX_BUF_SIZE) {
		g_print("[libgplayer] Path is too long. [%s]\n", url);
		return -1;
	}
	(void)sprintf (player_data.url, "%s", url);
	
	err = pthread_create(&player_data.player_thread, NULL, play_thread, (void *)(&player_data));
	if (err != 0) {
		player_data.terminate = 1;
		perror("[libgplayer] Create play_thread failed! \n");
		return -1;
	}
	return 0;
}

gint
start_player(void)
{
	gint ret;
	
	ret = change_state(1);
	return ret;
}

gint
pause_player(void)
{
	gint ret;
	
	ret = change_state(0);
	return ret;
}

gint
reset_player(void)
{
	GstStateChangeReturn ret;
	
	/* Set Pipeline to Null */
	ret = gst_element_set_state (player_data.pipeline, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("[libgplayer] Unable to set the pipeline to the NULL state.\n");
		gst_object_unref (player_data.pipeline);
		return -1;
	}
	player_data.playing = FALSE;
	
	return 0;
}

gint
change_state(gboolean state)
{
	GstStateChangeReturn ret;
	if (player_data.terminate)
		return 0;
	if(state)
		ret = gst_element_set_state (player_data.pipeline, GST_STATE_PLAYING);
	else
		ret = gst_element_set_state (player_data.pipeline, GST_STATE_PAUSED);
	
	if(ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("[libgplayer] Unable to set the pipeline to the %s state.\n", (state?"playing":"pause"));
		gst_object_unref (player_data.pipeline);
		return -1;
	} else if(ret == GST_STATE_CHANGE_NO_PREROLL) {	
		player_data.live_stream = TRUE;
		g_print("[libgplayer] Stream is live!!!\n");
	}
	player_data.playing = state;
	
	return 0;
}

gint
seek_player(gint64 seek_pos)
{
	gboolean ret;
	if(player_data.seek_enabled)
	{
		ret = gst_element_seek_simple(player_data.pipeline, GST_FORMAT_TIME,
									(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT/*GST_SEEK_FLAG_ACCURATE*/),
									GST_SECOND * seek_pos);
		if(!ret)
		{
			g_printerr("[libgplayer] Seek error!!\n");
			return -1;
		}
	} else {
		g_printerr("[libgplayer] This stream can not be seek!!\n");
	}
	return 0;
}

gint64
get_position (void)
{
	gint64 pos=-1;
	GstFormat format = GST_FORMAT_TIME;

	if (!gst_element_query_position (player_data.pipeline, format, &pos))
	{
		g_printerr("[libgplayer] Query position failed!!\n");
		return -1;
	}
	return GST_TIME_AS_MSECONDS(pos);
}

gint64
get_duration (void)
{
	gint64 dur=-1;
	GstFormat format = GST_FORMAT_TIME;

	if(!gst_element_query_duration (player_data.pipeline, format, &dur)) 
	{
		g_printerr("[libgplayer] Query duration failed!!\n");
		return -1;
	}
	return GST_TIME_AS_MSECONDS(dur);
}

gboolean
get_status (void)
{
	return player_data.playing;
}
gboolean
IsPipelineReady (void)
{
	return player_data.Is_pipeline_ready;
}

gint
release_player(void)
{	
	/* clean up */
	player_data.terminate = 1;
	pthread_join(player_data.player_thread, NULL);
	g_print("[libgplayer] Player Released and Freed!\n");

	return 0;
}

