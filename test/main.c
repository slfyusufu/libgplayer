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


int main(gint argc, gchar *argv[])
{
	char command;
	
	url = argv[1];
	
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
	//g_print("Exit....\n");
	return 0;
}
