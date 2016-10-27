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
			g_main_loop_quit (loop);
			break;
	
		case GST_MESSAGE_ERROR: {
			gchar  *debug;
			GError *error;
	
			gst_message_parse_error (msg, &error, &debug);
			g_free (debug);
	
			g_printerr ("Error: %s\n", error->message);
			g_error_free (error);
	
			g_main_loop_quit (loop);
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
		
			break;
		}
		default:
			break;
	}
return TRUE;
}

gint
open_player(gchar *url)
{
	gint ret;
	char buffer [MAX_BUF_SIZE];
	
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
	
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the %s state.\n", (state?"playing":"pause"));
		gst_object_unref (player_data.pipeline);
		return -1;
	}
	player_data.playing = state;
	
	return 0;
}

gint
seek_player(gint64 seek_pos)
{
	gboolean ret;
	ret = gst_element_seek_simple(player_data.pipeline, GST_FORMAT_TIME,
								(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT/*GST_SEEK_FLAG_ACCURATE*/),
								GST_SECOND * seek_pos);
	if(!ret)
	{
		g_printerr("Seek error, or the stream can not be seek!!\n");
		return -1;
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

