#include <gst/gst.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>	
#include <time.h>

#include "../libgplayer.h"

char *url=NULL;
gint64 seek_pos=0;
gboolean running=1;

void eos_cb(void)
{
	release_player();
	running = 0;
	//do something you need, when end of stream;
}

int main(gint argc, gchar *argv[])
{
	char command;
	gint64 pos, dur;
	url = argv[1];
	
	EndOfStream_cb(eos_cb);
	open_player(url, 0, 0, 800, 480);

	while(running)
	{
		command=getchar();
		if(command == 'q')
		{
			release_player();
			running =0;
		}
		if(command == 'p')
			pause_player();
		if(command == 's')
			start_player();
		if(command == 'S')
		{
			srand((unsigned)time(NULL));
			seek_pos = rand()%200;
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
		if(command == '2')
		{
			seek_player(180);
		}
		if(command == 'd')
		{
			pos= get_position();
			dur= get_duration();
			printf("postion: %ld/%ld.\n", (unsigned long)pos/1000000, (unsigned long)dur/1000000);
		}
		if(command == 'g')
		{
			printf("The player is %s.\n",get_status() ? "playing" : "paused");
		}
	}
	g_print("Exit....\n");
	return 0;
}
