/*
 * SGateG15.c
 *
 *  Created on: Feb 9, 2016
 *      Author: senia
 */

#include "G15SApps.h"

struct G15AppsData_s
{
	G15Screen* screens[MAX_SCREENS];

	pthread_t handlerThread;
	pthread_t updaterThread;
	struct threadList *threads;

	LinuxProcessList* lpl;

	int numOfScreens;
	int lastPressedKey;
	int BrightnessLevel;
};

struct G15Screen_s
{
	int id;
	char* name;				//screen name
	int background;
	unsigned char MKeysState;
	int screen_fd;
	g15canvas *canvas;

	drawFuncPtrDef drawFunc;
	updateFuncPtrDef updateFunc;

	double** values;
};

G15AppsData *newKeyboard(){
	return malloc(sizeof(G15AppsData));
}
int setKeyBoard(G15AppsData *this){
	this->lpl = xCalloc(1, sizeof(LinuxProcessList));
	setProccessList(this->lpl);

	this->screens[0] = NULL;

	this->numOfScreens=0;
	this->lastPressedKey=-1;
	this->BrightnessLevel=-1;

	return EXIT_SUCCESS;
}
G15Screen* createNewScreen(G15AppsData *this, char* name, int (*drawPtr)(G15Screen*), void (*updatePtr)(G15AppsData*)){
	int screenID = this->numOfScreens;
	if(this->screens[screenID]==NULL){
		this->screens[screenID] = newScreen();
		setScreen(this->screens[screenID], name,  drawPtr, updatePtr);
		if(this->screens[screenID]->id > this->numOfScreens)
			this->numOfScreens = this->screens[screenID]->id;
	}
	return this->screens[screenID];
}
G15Screen* getScreen(G15AppsData *this, int screenID){
	if(this->screens[screenID] != NULL)
		return this->screens[screenID];
	else
		return NULL;
}
void* getHandlerThread(G15AppsData *this){
	return &this->handlerThread;
}
void* getUpdaterThread(G15AppsData *this){
	return &this->updaterThread;
}
g15canvas* getCanvasByID(G15AppsData *this, int screenID){
	if(this->numOfScreens==0)
		return NULL;
	if(this->screens[screenID]->canvas != NULL)
		return this->screens[screenID]->canvas;
	else
		return NULL;
}
int getScreenFDByID(G15AppsData *this, int screenID){
	if(this->screens[screenID] != NULL)
		return this->screens[screenID]->screen_fd;
	else
		return -1;
}


int invokeDrawFunc(G15AppsData* this, int screenID){
	this->screens[screenID]->drawFunc(getScreen(this, screenID));
	return EXIT_SUCCESS;
}


int invokeAllDrawFuncs(G15AppsData* this){
	int i;
	for(i=0; i<this->numOfScreens ; i++){	//Last in first out.
		int ret = invokeDrawFunc(this, i);
		if(ret == EXIT_FAILURE)
			return EXIT_FAILURE;
	}
	//g15_send_cmd(this->screens[0]->screen_fd, G15DAEMON_SWITCH_PRIORITIES, i);
	colorSlidesM(this->screens[0]);
	g15_send_cmd(this->screens[0]->screen_fd, G15DAEMON_SWITCH_PRIORITIES, i);
	return EXIT_SUCCESS&i;
}
void deleteAllScreens(G15AppsData *this){
	int i;
	for(i=0;i<MAX_SCREENS && this->screens[i] == NULL;i++){
		g15r_clearScreen (this->screens[i]->canvas, G15_COLOR_WHITE);
		g15_close_screen (this->screens[i]->screen_fd);

		free(this->screens[i]->canvas);
		deleteScreen(this->screens[i]);
	}
}
void deleteKeyBoard(G15AppsData *this){
	deleteAllScreens(this);
	pthread_cancel(this->handlerThread);
	free(this);
}
void updateNClearScreen(int g15screen_fd, g15canvas* canvas){
	if (!canvas->mode_cache)
		g15_send (g15screen_fd, (char *) canvas->buffer, G15_BUFFER_LEN);
	g15r_clearScreen (canvas, G15_COLOR_WHITE);
}
int lightMKey(G15Screen *this, int G15_KEY){
	this->MKeysState = this->MKeysState | G15_KEY;
	int ret1, ret2, ret3=0, ret4=0; //TODO Add timeout error
	ret1 = g15_send_cmd (this->screen_fd, G15DAEMON_MKEYLEDS, (this->MKeysState));
	ret2 = g15_recv_oob_answer(this->screen_fd);
	g15_send_cmd(this->screen_fd, G15DAEMON_SWITCH_PRIORITIES, ret3);
	g15_send_cmd(this->screen_fd, G15DAEMON_SWITCH_PRIORITIES, ret4);

	return (ret1&&ret2&&ret3&&ret4);
}
int unlightMKey(G15Screen *this, int G15_KEY){
	this->MKeysState = this->MKeysState & G15_KEY;
	int ret1, ret2, ret3=0, ret4=0; //TODO Add timeout error
	ret1 = g15_send_cmd (this->screen_fd, G15DAEMON_MKEYLEDS, (this->MKeysState));
	ret2 = g15_recv_oob_answer(this->screen_fd);
	g15_send_cmd(this->screen_fd, G15DAEMON_SWITCH_PRIORITIES, ret3);
	g15_send_cmd(this->screen_fd, G15DAEMON_SWITCH_PRIORITIES, ret4);

	return (ret1&&ret2&&ret3&&ret4);
}
void colorSlideMKeys(G15Screen* this){
	lightMKey(this, G15_LED_M1);
	sleep(0.5);
	lightMKey(this, G15_LED_M2);
	sleep(0.5);
	lightMKey(this, G15_LED_M3);
	sleep(0.5);
	unlightMKey(this, G15_LED_M1);
	unlightMKey(this, G15_LED_M2);
	unlightMKey(this, G15_LED_M3);
}
void colorSlidesM(G15Screen* this){
	unsigned i;
	for(i=0;i<1;i++){
		colorSlideMKeys(this);
		sleep(0.5);
	}
}


G15Screen *newScreen(){				//Constuctor
	return malloc(sizeof(G15AppsData));
}
int setScreen(G15Screen *this, char* name,  int (*drawFuncPtr)(G15Screen*), void (*updateFuncPtr)(G15AppsData*)){
	static int ID = 1;
	this->id = ID;
	ID++;
	this->name = name;
	this->MKeysState=0;
	this->drawFunc = drawFuncPtr;
	this->updateFunc = updateFuncPtr;

	this->values = NULL;

	this->background=0;
	this->background = 0;
	this->canvas = (g15canvas *) malloc (sizeof (g15canvas));
	this->screen_fd = 0;
	if ((this->screen_fd = new_g15_screen (G15_G15RBUF)) < 0)
	{
		fprintf (stderr, "Sorry, can't connect to g15daemon\n");
		//TODO Break here..
		return EXIT_FAILURE;
	}
	g15r_initCanvas (this->canvas);
	this->canvas->mode_reverse = 1;
	//g15r_pixelOverlay (param->canvas, 0, 0, 160, 43, g15c_logo_data);
	g15r_clearScreen (this->canvas, 0);

	return EXIT_SUCCESS;
}
g15canvas* getCanvas(G15Screen *this){
	return this->canvas;
}
int getScreenFD(G15Screen *this){
	return (this->screen_fd);
}
drawFuncPtrDef getDrawFunc(G15Screen* this){
	return this->drawFunc;
}

void deleteScreen(G15Screen *this){	//De-stractor
	free(this);
}

//		================== Regular functions

int drawTest(G15Screen* screen){
	g15canvas* canvas = getCanvas(screen);
	int screenFD = getScreenFD(screen);

	updateNClearScreen(screenFD, canvas);
	g15r_clearScreen (canvas, 0);
	g15r_renderString(canvas, (unsigned char *)"Senia", 0, G15_TEXT_LARGE, 50, G15_LCD_HEIGHT/2-5);
	g15r_drawBar(canvas, 0, 0, 150, 10, 1, 0, 100, 2 );
	//g15r_drawBigNum (canvas, 0, 0, 22, 42, 0, 3);
	updateNClearScreen(screenFD, (g15canvas*)canvas);
	usleep(2*SEC2MICRO);

	g15r_renderString((g15canvas*)canvas, (unsigned char *)"Senia", 0, G15_TEXT_LARGE, 50, G15_LCD_HEIGHT/2-5);
	g15r_drawBar(canvas, 0, 0, 150, 10, 1, 20, 100, 2 );
	updateNClearScreen((int)screenFD, (g15canvas*)canvas);
	usleep(1*SEC2MICRO);

	g15r_renderString(canvas, (unsigned char *)"Senia! in 4", 0, G15_TEXT_LARGE, 50, G15_LCD_HEIGHT/2-5);
	g15r_drawBar(canvas, 0, 0, 150, 10, 1, 40, 100, 2 );
	updateNClearScreen((int)screenFD, (g15canvas*)canvas);
	usleep(1*SEC2MICRO);

	g15r_renderString(canvas, (unsigned char *)"Senia! in 3", 0, G15_TEXT_LARGE, 50, G15_LCD_HEIGHT/2-5);
	g15r_drawBar(canvas, 0, 0, 150, 10, 1, 60, 100, 2 );
	updateNClearScreen((int)screenFD, (g15canvas*)canvas);
	usleep(1*SEC2MICRO);

	g15r_renderString(canvas, (unsigned char *)"Senia! in 2", 0, G15_TEXT_LARGE, 50, G15_LCD_HEIGHT/2-5);
	g15r_drawBar(canvas, 0, 0, 150, 10, 1, 80, 100, 2 );
	updateNClearScreen((int)screenFD, (g15canvas*)canvas);
	usleep(1*SEC2MICRO);

	g15r_renderString(canvas, (unsigned char *)"Senia! 1", 0, G15_TEXT_LARGE, 50, G15_LCD_HEIGHT/2-5);
	g15r_drawBar(canvas, 0, 0, 150, 10, 1, 100, 100, 2 );
	updateNClearScreen((int)screenFD, (g15canvas*)canvas);
	usleep(3*SEC2MICRO);

	return EXIT_SUCCESS;
}

int drawLogo(G15Screen* screen){
	int screenFD = getScreenFD(screen);
	g15canvas* cnv = getCanvas(screen);

	g15r_ttfPrint (cnv, G15_LCD_WIDTH*0.25, G15_LCD_HEIGHT*0.35, 16, 1, 0, 1, "Senia Kalma");
	updateNClearScreen(screenFD, cnv);
	//g15r_clearScreen (cnv, 0);

	return EXIT_SUCCESS;
}

void printStats(){
	double load[3];
	int i=0;

	for(;;)
	{
		printf("Number Of cores: %d \n", getNumberOfCores());
		for(i=0;i<getNumberOfCores();i++)
			printf("Core %d utilization is : %f \n", i, getCPULoadByCore(i));
		printf("Total avraged CPU utilization is : %f \n", getCPULoad());

		if (getloadavg(load, 3) != -1)
			printf("load average : %f , %f , %f\n", load[0],load[1],load[2]);


		printf("Total physical mem: %f \n", getTotalPhysicalMem());
		printf("Used physical mem: %f \n", getUsedPhysicalMem());

		printf("Total virtual mem: %f \n", getTotalVirtualMem());
		printf("Used virtual mem: %f \n\n", getUsedVirutalMem());
		printf("===================================\n");
		sleep(2);
	}
}
void updateCPUMEM(G15AppsData* this){
	int CPUMEMScreen;
	int i;
	bool Bflag=0;
	LinuxProcessList_scanMemoryInfo(&this->lpl->super);
	LinuxProcessList_scanCPUTime(this->lpl);
	for(i=0; i<this->numOfScreens; i++){
		if(strcmp(this->screens[i]->name, "CPU&RAM")==0){	//If the two strings are equal, strcmp returns 0.
			CPUMEMScreen = i;
			Bflag=1;
		}
		if(Bflag)
			break;
	}
	if(this->screens[CPUMEMScreen]->values == NULL){
		this->screens[CPUMEMScreen]->values = malloc(2 * sizeof(double*));
		this->screens[CPUMEMScreen]->values[0] = malloc(this->lpl->super.cpuCount * sizeof(double));	//allocate memory for the CPU info
		this->screens[CPUMEMScreen]->values[0][0] = this->lpl->super.cpuCount;	//first time set cpuCount as the first value in values.
	}

	for(i=1; i< (this->lpl->super.cpuCount+1);i++)		//first place reserved for number of cores.
		this->screens[CPUMEMScreen]->values[0][i] = (Platform_setCPUValues(&this->lpl->cpus[i])/100);

	/*for(i=1; i<this->lpl->super.cpuCount;i++)
		printf("CPU Val %d is: %f \n" , i ,this->screens[CPUMEMScreen]->values[0][i] );*/

	//Memory part
	this->screens[CPUMEMScreen]->values[1] = malloc(2 * sizeof(int));	//allocate memory for the Memory
	this->screens[CPUMEMScreen]->values[1][0] = this->lpl->super.usedMem;
	this->screens[CPUMEMScreen]->values[1][1] = this->lpl->super.totalMem;
}

int Meter_humanUnit(char* buffer, unsigned long int value, int size) {
   const char * prefix = "KMGTPEZY";
   unsigned long int powi = 1;
   unsigned int written, powj = 1, precision = 2;

   for(;;) {
      if (value / 1024 < powi)
         break;

      if (prefix[1] == 0)
         break;

      powi *= 1024;
      ++prefix;
   }

   if (*prefix == 'K')
      precision = 0;

   for (; precision > 0; precision--) {
      powj *= 10;
      if (value / powi < powj)
         break;
   }

   written = snprintf(buffer, size, "%.*f%c",
      precision, (double) value / powi, *prefix);

   return written;
}

int drawCPURAM(G15Screen* this){
	if(this->values == NULL)	//If no values set by updateCPUMEM function, return.
		return EXIT_FAILURE;
	int cores= this->values[0][0];
	double load[3];
	float coreLoad, CPULoad = this->values[0][1];	//CPULOAD = Total CPU load
	float RAMLoad = (float)this->values[1][0]/(float)this->values[1][1];
	char* buf;
	/*int wri  = Meter_humanUnit(buf, this->values[1][0], 5);
	printf("org: %f writ: %s\n",this->values[1][0], buf );
	wri  = Meter_humanUnit(buf, this->values[1][1], 5);
	printf("org2: %f writ2: %s\n",this->values[1][1], buf );*/

	//CPULoad = (updateCPUMEM(lpl)/100);

	int i, innerLineLen;
	innerLineLen = ((G15_LCD_WIDTH-11)-12);	//Midlle part, <11>| ********* |<12>

	int screenFD = getScreenFD(this);
	g15canvas* cnv = getCanvas(this);

	/*for(i=0; i<(cores);i++)
		printf("CPU Val(inside) %d is: %f \n" , i ,this->values[i] );*/

	g15r_clearScreen (cnv, 0);
	//CPU text & bar
	g15r_ttfPrint (cnv, 10, 2, 10, 1, 0, 0, "CPU:");
	g15r_pixelBox(cnv, 10, 12, (G15_LCD_WIDTH-10), 13+(2*cores), G15_COLOR_WHITE, 1, 0);	//outer
	for(i=1; i<cores; i++){																		//inner(by cores)
		coreLoad = this->values[0][i];
		g15r_drawLine(cnv, 12, 12+(i*2), 12 + (innerLineLen*coreLoad), 12+(i*2), 0);
		g15r_drawLine(cnv, 12, 13+(i*2), 12 + (innerLineLen*coreLoad), 13+(i*2), 0);
	}

	//RAM text & bar
	g15r_ttfPrint (cnv, 10, 16+(2*cores), 10, 1, 0, 0, "RAM:");
	g15r_pixelBox(cnv, 10, 26+(2*cores), (G15_LCD_WIDTH-10), (G15_LCD_HEIGHT-1), G15_COLOR_WHITE, 1, 0);	//outer
	g15r_pixelBox(cnv, 12, 28+(2*cores), 12+(innerLineLen*RAMLoad), (G15_LCD_HEIGHT-3), G15_COLOR_WHITE, 1, 1);	//Fill

	//CPU Percent & load(1, 5, 15 min)
	char int_string[5];
	sprintf(int_string, "%d", (int)(CPULoad*100));
	char CPUString[10] = "%";
	strcat(int_string, CPUString);
	g15r_ttfPrint (cnv, (G15_LCD_WIDTH-35), 2, 10, 1, 0, 0, int_string);

	getloadavg(load, 3);
	char loadString[30] = "| ";
	sprintf(int_string, "%.2f", (float)(load[0]));
	strcat(loadString, int_string);
	strcat(loadString, "%, ");
	sprintf(int_string, "%.2f", (float)(load[1]));
	strcat(loadString, int_string);
	strcat(loadString, "%, ");
	sprintf(int_string, "%.2f", (float)(load[2]));
	strcat(loadString, int_string);
	strcat(loadString, "% |");
	g15r_ttfPrint (cnv, (G15_LCD_WIDTH-25)/2-23, 4, 5, 1, 0, 0, loadString);

	//RAM Percent
	sprintf(int_string, "%d", (int)(RAMLoad*100));
	char RAMString[10] = "%";
	strcat(int_string, RAMString);
	g15r_ttfPrint (cnv, (G15_LCD_WIDTH-35), 16+(2*cores), 10, 1, 0, 0, int_string);
	updateNClearScreen(screenFD, cnv);
	return EXIT_SUCCESS;
}

void updaterThread(G15AppsData* this) {
	int i;
	double updateHz = 1;
	while(true){
		clock_t start = clock(), diff;
		for(i=0; i <this->numOfScreens; i++){
			if(this->screens[i]->updateFunc!=NULL)
				this->screens[i]->updateFunc(this);
		}
		diff = clock() - start;
		int msec = diff * 1000 / CLOCKS_PER_SEC;

		//printf("Time taken %d seconds %d milliseconds", msec/1000, msec%1000);
		int takenMili = msec%1000;
		if(takenMili < updateHz)
			sleep(updateHz - (takenMili));
	}
}

void keyboardHandlerThread(G15AppsData* Keyboard) {
    unsigned int pressedKey=0;
    int change = -1;
    int retval=0;
    int  stam;
    bool front=-1;
    printf("Light: %d\n", G15DAEMON_KEY_HANDLER);
    printf("L1: %d L2: %d L3: %d L4: %d L5: %d\n", G15_KEY_L1, G15_KEY_L2, G15_KEY_L3, G15_KEY_L4, G15_KEY_L5);
    printf("M1: %d M2: %d M3: %d MR: %d\n", G15_KEY_M1, G15_KEY_M2, G15_KEY_M3, G15_KEY_MR);
    printf("G1: %d G2: %d G3: %d G4: %d G5: %d G6: %d\n", G15_KEY_G1, G15_KEY_G2, G15_KEY_G3, G15_KEY_G4, G15_KEY_G5, G15_KEY_G6);
    //unsigned char sh=NULL;
    int screenFD = getScreenFDByID(Keyboard, 0);
    if(screenFD != -1){
		stam = g15_recv_oob_answer(screenFD);
		printf("return val is:, stam: %d\n", stam);
		usleep(1*SEC2MICRO);
		while(1) {
			drawCPURAM((G15Screen*)getScreen(Keyboard, 1));
			//printf("keyState: %d L1: %d L2: %d L3: %d L4: %d L5: %d\n",pressedKey, G15_KEY_L1, G15_KEY_L2, G15_KEY_L3, G15_KEY_L4, G15_KEY_L5);
			recv(screenFD,&pressedKey,4,0);
			retval = g15_send_cmd (screenFD, G15DAEMON_IS_FOREGROUND, 0);
			if(!pressedKey)
				continue;

			//g15_send_cmd(param->g15screen_fd, G15DAEMON_SWITCH_PRIORITIES, NULL);
			if(retval){
			  if(retval==1 && retval!=front){
				  front=1;
				  printf("We are FRONT\n");
			  }
			}
			else{
				if(retval!=front){
					front=0;
					printf("We are BACK\n");
				}
			}

			if(pressedKey!=change){
				printf("Got a change: %d\n", pressedKey);
				change=pressedKey;
			}

			//	Under screen keys
			if(pressedKey == G15_KEY_L1) {
				change=pressedKey;
			}
			if(pressedKey == G15_KEY_L2) {
				printf("Got to L2\n");
			}
			if(pressedKey == G15_KEY_L3) {
				printf("Got to L3\n");
			}
			if(pressedKey == G15_KEY_L4) {
				printf("Got to L4\n");
			}
			if(pressedKey == G15_KEY_L5) {
				printf("Got to L5\n");
			}

			// M(Mode) keys, left hand side
			if(pressedKey == G15_KEY_M1) {
				printf("Got to M1\n");
			}
			if(pressedKey == G15_KEY_M2) {
				printf("Got to M2\n");
			}
			if(pressedKey == G15_KEY_M3) {
				printf("Got to M3\n");
			}
			if(pressedKey == G15_KEY_MR) {
				printf("Got to MR\n");
			}

			//G1-6
			if(pressedKey == G15_KEY_G1) {
				printf("Got to G1\n");
			}
			if(pressedKey == G15_KEY_G2) {
				printf("Got to G2\n");
			}
			if(pressedKey == G15_KEY_G3) {
				printf("Got to G3\n");
			}
			if(pressedKey == G15_KEY_G4) {
				printf("Got to G4\n");
			}
			if(pressedKey == G15_KEY_G5) {
				printf("Got to G5\n");
			}
			if(pressedKey == G15_KEY_G6) {
				printf("Got to G6\n");
			}
		}
    }
}

