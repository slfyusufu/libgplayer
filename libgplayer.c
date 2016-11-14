//////////////////////////////////////////////////

#include "libgplayer.h"

static CustomData player_data;


static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			g_print ("End of stream\n");
			gst_element_set_state (player_data.pipeline, GST_STATE_READY);
			g_main_loop_quit (loop);
			break;
	
		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;
	
			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);
	
			g_printerr ("Error: %s\n", error->message);
			g_error_free (error);

			gst_element_set_state (player_data.pipeline, GST_STATE_READY);
			g_main_loop_quit (loop);
			break;
		}
		case GST_MESSAGE_CLOCK_LOST: {
			/* Get a new clock */  
			gst_element_set_state (player_data.pipeline, GST_STATE_PAUSED);  
			gst_element_set_state (player_data.pipeline, GST_STATE_PLAYING);  
			break;
		}
		case GST_MESSAGE_STATE_CHANGED: {
			GstState old_state, new_state, pending_state;
			gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
			if (GST_MESSAGE_SRC (msg) == GST_OBJECT (player_data.pipeline)) 
			{
				g_print ("Pipeline state changed from %s to %s:\n",
				gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
			}
			/* Remember whether we are in the PLAYING state or not */
			player_data.playing = (new_state == GST_STATE_PLAYING);
         
			if (player_data.playing && (GST_MESSAGE_SRC (msg) == GST_OBJECT (player_data.pipeline)))
			{
				/* We just moved to PLAYING. Check if seeking is possible */
				GstQuery *query;
				gint64 start, end;
				query = gst_query_new_seeking (GST_FORMAT_TIME);
				if (gst_element_query (player_data.pipeline, query))
				{
					gst_query_parse_seeking (query, NULL, &player_data.seek_enabled, &start, &end);
					if (player_data.seek_enabled) {
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
				if (!gst_element_query_position (player_data.pipeline, fmt, &current)) {
					g_printerr ("Could not query current position.\n");
				}
				
				/* If we didn't know it yet, query the stream duration */
				if (!GST_CLOCK_TIME_IS_VALID (player_data.duration)) {
					if (!gst_element_query_duration (player_data.pipeline, fmt, &player_data.duration)) {
						g_printerr ("Could not query current duration.\n");
					}
				}
				/* Print current position and total duration */
				g_print ("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
						GST_TIME_ARGS (current), GST_TIME_ARGS (player_data.duration));
			}
		
			break;
		}
		default:
		{
			if(GST_MESSAGE_SRC (msg) == GST_OBJECT (player_data.pipeline)){
				g_print("GST_MSG = 0x%x.\n",GST_MESSAGE_TYPE (msg));
			}
			break;
		}
	}
return TRUE;
}

gint
open_player(gchar *url, unsigned int sx, unsigned int sy, unsigned int disp_width, unsigned int disp_height)
{
	gint ret;
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
	
	sprintf(str_sx, "%d", sx);
	sprintf(str_sy, "%d", sy);
	sprintf(str_disp_width, "%d", disp_width);
	sprintf(str_disp_height, "%d", disp_height);
		
	/* init GStreamer */
	//gst_init (&argc, &argv);
	gst_init (NULL, NULL);
	
	/* make sure we have a URI */
	if (url == NULL)
	{
		g_print ("Usage: No <URI>\n");
		return -1;
	}
	else
	{
		g_print("Input path %s\n", url);	
		if(strlen(url)>MAX_BUF_SIZE)
		{
			g_print("Path is too long.\n");	
			return -1;
		}
		#if 0 //for TCC test
		//(void)sprintf (buffer, "playbin uri=%s audio-sink=%s video-sink=v4l2sink overlay-top=%s overlay-left=%s overlay-width=%s overlay-height=%s %s",
		//						url, AUDIO_SINK_ARG, str_sx, str_sy, str_disp_width, str_disp_height, EXTRE_ARGS);
		(void)sprintf (buffer, "playbin uri=%s audio-sink=%s video-sink=%s %s",
								url, AUDIO_SINK_ARG, VIDEO_SINK_ARG, EXTRE_ARGS);
		#else //for PC test
		(void)sprintf (buffer, "playbin uri=%s audio-sink=\"alsasink\"",
								url);
		#endif
		g_print("Gst command %s\n", buffer);  		
	}						 
	
	/* Build the pipeline */  
	player_data.pipeline = gst_parse_launch (buffer, NULL);	
	
	player_data.bus = gst_pipeline_get_bus (GST_PIPELINE(player_data.pipeline));
	gst_bus_add_watch (player_data.bus, bus_call, player_data.loop);
	gst_object_unref (player_data.bus);

	ret = change_state(1);
	if(ret != 0)
		return -1;

	/* now run */
	player_data.loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (player_data.loop);
	
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
stop_player(void)
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
	/* clean up */
	g_print("Play finished! Start to Free!\n");
	gst_element_set_state (player_data.pipeline, GST_STATE_NULL);
	g_main_loop_quit (player_data.loop);
	gst_object_unref (player_data.pipeline);
	g_print("Free Finished!\n");

	return 0;
}

