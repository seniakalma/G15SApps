#include "G15SApps.h"

int main (int argc, char *argv[])
{

	/*long pages = sysconf(_SC_PHYS_PAGES);
	long page_size = sysconf(_SC_PAGE_SIZE);
	long st = pages * page_size;
	st/=1000*1000;*/


/*
	FILE *cpuInfo;
	cpuInfo = fopen("/proc/cpuinfo","r");
    char line[256];

    const char s[2] = " ";
    bool match;
    while (fgets(line, sizeof(line), cpuInfo)) {
        //printf("%s", line);
        char *token;

        // get the first token
        token = strtok(line, s);

        // walk through other tokens

        match = !strncmp(token,"model" ,5);
        while( token != NULL && match)
        {
           printf( " %s\n", token );

           token = strtok(NULL, s);
        }

    }

	FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, cpuinfo) != -1)
	{
	   puts(arg);
	}
	free(arg);
	fclose(cpuinfo);


*/
	printf("New, Width: %d, Hight: %d\n", G15_LCD_WIDTH, G15_LCD_HEIGHT);
	G15AppsData *G15Keyboard = newKeyboard();
	setKeyBoard(G15Keyboard);

	//ret = g15_send_cmd (screenFD, G15DAEMON_MKEYLEDS, G15_LED_M1 | G15_LED_M2 | sh);

	//Create first screen(SENIA KALMA)
	G15Screen* logoScreen = createNewScreen(G15Keyboard,"SENIA LOGO", &drawLogo );



	//Create second screen(Test)
	//G15Screen* testScreen = (G15Screen*)createNewScreen(G15Keyboard, "Test", &drawTest);

	//Create third screen(CPU & RAM)
	G15Screen* CPURAMScreen = (G15Screen*)createNewScreen(G15Keyboard, "CPU&RAM", &drawCPURAM);



	//	Draw all screens
	invokeAllDrawFuncs(G15Keyboard);


	//Start a new thread to listen to button presses
	void* handlThrd = (void*)getHandlerThread(G15Keyboard);
	pthread_create(handlThrd, NULL, (void*)keyboardHandlerThread,(void*)G15Keyboard);

	sleep(10000);

	deleteKeyBoard(G15Keyboard);
	g15canvas* cnv = getCanvasByID(G15Keyboard, 0);
	if(cnv!=NULL){
		cnv->mode_reverse = 0;
		g15r_clearScreen (getCanvasByID(G15Keyboard, 0), G15_COLOR_WHITE);
		g15_close_screen (getScreenFDByID(G15Keyboard, 0));

	}
	else
		assert(cnv==NULL);

	return 0;
}
