//////////////////////////////////////////////////

#include "libgplayer.h"

static CustomData player_data;

static gboolean
Message_handler(CustomData *player,
				GstMessage *msg
				)
{
	//GMainLoop *loop = (GMainLoop *) data;
	//CustomData *player = (CustomData *)data;
	//g_print ("handle message!\n");
	
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			//gst_element_set_state (player->pipeline, GST_STATE_READY);
			player->terminate = 1;
			//g_main_loop_quit (loop);
			break;
	
		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;
	
			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);
	
			g_printerr ("Error: %s\n", error->message);
			g_error_free (error);

			gst_element_set_state (player->pipeline, GST_STATE_READY);
			player-> terminate = 1;
			//g_main_loop_quit (loop);
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
				g_print ("Pipeline state changed from %s to %s:\n",
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
						g_print ("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
						GST_TIME_ARGS (start), GST_TIME_ARGS (end));
					} else {
						g_print ("Seeking is DISABLED for this stream.\n");
					}
				} else {
					g_printerr ("Seeking query failed.");
				}
				gst_query_unref (query);
				
				GstFormat fmt = GST_FORMAT_TIME;
				gint64 current = -1;
				
				/* Query the current position of the stream */
				if (!gst_element_query_position (player->pipeline, fmt, &current)) {
					g_printerr ("Could not query current position.\n");
				}
				
				/* If we didn't know it yet, query the stream duration */
				if (!GST_CLOCK_TIME_IS_VALID (player->duration)) {
					if (!gst_element_query_duration (player->pipeline, fmt, &player->duration)) {
						g_printerr ("Could not query current duration.\n");
					}
				}
				/* Print current position and total duration */
				g_print ("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
						GST_TIME_ARGS (current), GST_TIME_ARGS (player->duration));
			}
		
			break;
		}
		default:
		{
			if(GST_MESSAGE_SRC (msg) == GST_OBJECT (player->pipeline)){
				g_print("GST_MSG = 0x%x.\n",GST_MESSAGE_TYPE (msg));
			}
			break;
		}
	}
return TRUE;
}

static gint
init_player(CustomData *player)
{
	gint ret;
	GstMessage *msg;
	char buffer [MAX_BUF_SIZE];
	char str_sx[8];
	char str_sy[8];
	char str_disp_width[8];
	char str_disp_height[8];
	
	//Init varible of player
	player_data.playing = FALSE;
	player_data.terminate = FALSE;
	player_data.seek_enabled = FALSE;
	player_data.live_stream = FALSE;
	player_data.duration = GST_CLOCK_TIME_NONE;
	
	sprintf(str_sx, "%d", player->windowpos.sx);
	sprintf(str_sy, "%d", player->windowpos.sy);
	sprintf(str_disp_width, "%d", player->windowpos.disp_width);
	sprintf(str_disp_height, "%d", player->windowpos.disp_height);
	
	/* init GStreamer */
	//gst_init (&argc, &argv);
	gst_init (NULL, NULL);
	
	/* make sure we have a URI */
	if (player->url == NULL)
	{
		g_print ("Usage: No <URI>\n");
		return -1;
	}
	else
	{
		g_print("Input path %s\n", player->url);	
		if(strlen(player->url)>MAX_BUF_SIZE)
		{
			g_print("Path is too long.\n");	
			return -1;
		}
		#if 0 //for TCC test
		//(void)sprintf (buffer, "playbin uri=%s audio-sink=%s video-sink=v4l2sink overlay-top=%s overlay-left=%s overlay-width=%s overlay-height=%s %s",
		//						player->url, AUDIO_SINK_ARG, str_sx, str_sy, str_disp_width, str_disp_height, EXTRE_ARGS);
		(void)sprintf (buffer, "playbin uri=%s audio-sink=%s video-sink=%s %s",
								player->url, AUDIO_SINK_ARG, VIDEO_SINK_ARG, EXTRE_ARGS);
		#else //for PC test
		(void)sprintf (buffer, "playbin uri=%s audio-sink=\"alsasink\"",
								player->url);
		#endif
		g_print("Gst command %s\n", buffer);  		
	}						 
	
	/* Build the pipeline */  
	player->pipeline = gst_parse_launch (buffer, NULL);	
	if(player->pipeline == NULL)
	{	
		g_printerr("Create playbin failed!!\n");
		return -1;
	}
	
	player->bus = gst_pipeline_get_bus (GST_PIPELINE(player->pipeline));
	
	if(player->bus == NULL)
		g_printerr("Create bus_watch failed!!\n");
		
//	gst_bus_add_signal_watch (player_data.bus);
//	g_signal_connect (player_data.bus, "message::state-changed", G_CALLBACK (cb_message_state_change), NULL);
	
	//================ Start playing ===================
	ret = change_state(1);
	if(ret != 0)
		return -1;

	/* now do...while */	
	do
	{
		msg = gst_bus_timed_pop_filtered (player_data.bus, 100 * GST_MSECOND,
        GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION | GST_MESSAGE_CLOCK_LOST);
        
        if (msg != NULL) {
			Message_handler (player, msg);
		}
	} while(!player->terminate);

	g_print("End of Play!\n");
	return 0;
}

static void *
play_thread(void *arg)
{
	CustomData *player = (CustomData *)arg;
	
	init_player(player);
	pthread_exit((void *)"play thread exit\n");
	return NULL;
}

gint
open_player(gchar *url, unsigned int sx, unsigned int sy, unsigned int disp_width, unsigned int disp_height)
{
	int err;
	
	memset(&player_data, 0, sizeof(CustomData));
	
	player_data.windowpos.sx = sx;
	player_data.windowpos.sy = sy;
	player_data.windowpos.disp_width = disp_width;
	player_data.windowpos.disp_height = disp_height;
	player_data.url = url;
	
	err = pthread_create(&player_data.player_thread, NULL, play_thread, (void *)(&player_data));
	if (err != 0)
	{
		player_data.terminate = 1;
		perror("Create play_thread failed! \n");
		return -1;
	}
	return 0;
}

gint
start_player(void)
{
	gint ret;
	
	ret = change_state(1);
	if(ret != 0)
		return -1;
	
	return 0;
}

gint
pause_player(void)
{
	gint ret;
	
	ret = change_state(0);
	if(ret != 0)
		return -1;
	
	return 0;
}

gint
reset_player(void)
{
	GstStateChangeReturn ret;
	
	/* Start playing */
	ret = gst_element_set_state (player_data.pipeline, GST_STATE_NULL);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the NULL state.\n");
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
	
	if(state)
		ret = gst_element_set_state (player_data.pipeline, GST_STATE_PLAYING);
	else
		ret = gst_element_set_state (player_data.pipeline, GST_STATE_PAUSED);
	
	if(ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the %s state.\n", (state?"playing":"pause"));
		gst_object_unref (player_data.pipeline);
		return -1;
	} else if(ret == GST_STATE_CHANGE_NO_PREROLL) {	
		player_data.live_stream = TRUE;
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
			g_printerr("Seek error!!\n");
			return -1;
		}
	} else {
		g_printerr("This stream can not be seek!!\n");
	}
	return 0;
}

gint
release_player(void)
{
	int err;
	
	/* clean up */
	g_print("Play finished! Start to Free!\n");
	gst_element_set_state (player_data.pipeline, GST_STATE_NULL);
	//g_main_loop_quit (player_data.loop);
	player_data.terminate = 1;
	gst_object_unref (player_data.pipeline);
	g_print("Free Finished!\n");
	
	err = pthread_join(player_data.player_thread, NULL);
	if (err != 0)
	{
		perror("Thread join faild: ");
	}
	g_print("Stop player!\n");

	return 0;
}

