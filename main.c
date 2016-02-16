#include "G15SApps.h"

int main (int argc, char *argv[])
{

	/*long pages = sysconf(_SC_PHYS_PAGES);
	long page_size = sysconf(_SC_PAGE_SIZE);
	long st = pages * page_size;
	st/=1000*1000;*/


	long double a[4], b[4], loadavg;
	FILE *fp;
	char dump[50];

	for(;;)
	{
		fp = fopen("/proc/stat","r");
		fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
		fclose(fp);
		sleep(1);

		fp = fopen("/proc/stat","r");
		fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
		fclose(fp);

		loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
		printf("The current CPU utilization is : %Lf %\n",loadavg*100);
	}

	for(;;){
		printf("Number Of cores: %d \n", getNumberOfCores());
		printf("Current value: %f \n", getCurrentValue());

		printf("Total physical mem: %f \n", getTotalPhysicalMem());
		printf("Used physical mem: %f \n", getUsedPhysicalMem());

		printf("Total virtual mem: %f \n", getTotalVirtualMem());
		printf("Used virtual mem: %f \n\n", getUsedVirutalMem());

		sleep(3);
	}

	FILE *cpuInfo;
	cpuInfo = fopen("/proc/cpuinfo","r");
    char line[256];

    const char s[2] = " ";
    bool match;
    while (fgets(line, sizeof(line), cpuInfo)) {
        //printf("%s", line);
        char *token;

        /* get the first token */
        token = strtok(line, s);

        /* walk through other tokens */

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



	printf("New, Width: %d, Hight: %d\n", G15_LCD_WIDTH, G15_LCD_HEIGHT);
	G15AppsData *G15Keyboard = newKeyboard();
	setKeyBoard(G15Keyboard);

	//Create first screen(SENIA KALMA)
	G15Screen* logoScreen = createNewScreen(G15Keyboard,"SENIA LOGO", &drawLogo );


	//Create second screen(Test)
	//G15Screen* testScreen = (G15Screen*)createNewScreen(G15Keyboard, "Test", &drawTest);


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
