#include <gst/gst.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>	
#include <time.h>

#include "../libgplayer.h"

pthread_t _PlayThread;
char *url=NULL;
gint64 seek_pos=0;
gboolean running;

static void *PlayThread(void *arg)
{
	open_player(url, 0, 0, 800, 480);
	pthread_exit((void *)"media process thread exit\n");

	return NULL;	
}

static void StartPlayThread(void *arg)
{
	int err;
	g_print("Start thread....\n");
	running =1;

	err = pthread_create(&_PlayThread, NULL, PlayThread, (void *)arg);
	if (err != 0)
	{
		running =0;
		release_player();
		perror("create thread failed: \n");
	}
}
static void StopPlayThread()
{
	//if(player_data.playing)
	{
		void *res;
		int err;
		
		running =0;
		release_player();
		err = pthread_join(_PlayThread, &res);
		if (err != 0)
		{
			perror("update playtime thread join faild: ");
		}
	}
	g_print("Stop thread....\n");
}

gint main(gint argc, gchar *argv[])
{
	char command;
	
	url = argv[1];
	
	StartPlayThread(NULL);

	while(running)
	{
		command=getchar();
		if(command == 'q')
		{
			StopPlayThread();
			//running =0;
		}
		if(command == 'p')
			stop_player();
		if(command == 's')
			start_player();
		if(command == 'S')
		{
			srand((unsigned)time(NULL));
			seek_pos = rand()%120;
			g_print("startPos = %ld.\n", (unsigned long)seek_pos);
			seek_player(seek_pos);
		}
		if(command == '1')
		{
			srand((unsigned)time(NULL));
			seek_pos = rand()%2000;
			g_print("startPos = %ld.\n", (unsigned long)seek_pos);
			seek_player(seek_pos);
		}
	}
	
	return 0;
}
