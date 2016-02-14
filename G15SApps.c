/*
 * SGateG15.c
 *
 *  Created on: Feb 9, 2016
 *      Author: senia
 */

#include "G15SApps.h"

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
	g15r_clearScreen (cnv, 0);

	return EXIT_SUCCESS;
}

void keyboardHandlerThread(G15AppsData * Keyboard) {
    unsigned int pressedKey;
    int change = -1;
    int retval=0;
    int ret, stam;
    bool front=-1;
    printf("Light: %d\n", G15DAEMON_KEY_HANDLER);
    printf("L1: %d L2: %d L3: %d L4: %d L5: %d\n", G15_KEY_L1, G15_KEY_L2, G15_KEY_L3, G15_KEY_L4, G15_KEY_L5);
    printf("M1: %d M2: %d M3: %d MR: %d\n", G15_KEY_M1, G15_KEY_M2, G15_KEY_M3, G15_KEY_MR);
    printf("G1: %d G2: %d G3: %d G4: %d G5: %d G6: %d\n", G15_KEY_G1, G15_KEY_G2, G15_KEY_G3, G15_KEY_G4, G15_KEY_G5, G15_KEY_G6);
    unsigned char sh=NULL;
    int screenFD = getScreenFDByID(Keyboard, 0);
    if(screenFD != -1){
		ret = g15_send_cmd (screenFD, G15DAEMON_MKEYLEDS, G15_LED_M1 | G15_LED_M2 | sh);
		stam = g15_recv_oob_answer(screenFD);
		printf("return val is: %d, stam: %d\n", ret, stam);
		usleep(1*SEC2MICRO);
		while(1) {
			recv(screenFD,&pressedKey,4,0);
			//pressedKey = g15_send_cmd (param->g15screen_fd, G15DAEMON_GET_KEYSTATE, NULL);
			//printf("keyState: %d L1: %d L2: %d L3: %d L4: %d L5: %d\n",pressedKey, G15_KEY_L1, G15_KEY_L2, G15_KEY_L3, G15_KEY_L4, G15_KEY_L5);
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

struct G15AppsData_s
{
	G15Screen* screens[MAX_SCREENS];

	pthread_t handlerThread;
	struct threadList *threads;

	int numOfScreens;
	int lastPressedKey;
	unsigned char MKeysState;
	int BrightnessLevel;
};

struct G15Screen_s
{
	int id;
	char* name;				//screen name
	int background;
	int g15screen_fd;
	g15canvas *canvas;

	drawFuncPtrDef drawFunc;
};

G15AppsData *newKeyboard(){
	return malloc(sizeof(G15AppsData));
}
int setKeyBoard(G15AppsData *this){
	this->screens[0] = NULL;
	/*this->screens[0] = newScreen();
	bool ret = setScreen(this->screens[0], "Senia");
	if(ret==EXIT_FAILURE)
		return EXIT_FAILURE;*/

	this->numOfScreens=0;
	this->lastPressedKey=-1;
	this->BrightnessLevel=-1;
	this->MKeysState=-1;

	return EXIT_SUCCESS;
}
G15Screen* createNewScreen(G15AppsData *this, char* name, int (*drawFuncPtr)(G15Screen*)){
	int screenID = this->numOfScreens;
	if(this->screens[screenID]==NULL){
		this->screens[screenID] = newScreen();
		setScreen(this->screens[screenID], name,  drawFuncPtr);
		if(this->screens[screenID]->id > this->numOfScreens)
			this->numOfScreens = this->screens[screenID]->id;
	}
	return this->screens[screenID];
}
G15Screen* getScreen(G15AppsData *this, int screenID){
	if(this->screens[screenID] != NULL)
		return this->screens[screenID];
	else
		return EXIT_FAILURE;
}
void* getHandlerThread(G15AppsData *this){
	return &this->handlerThread;
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
		return this->screens[screenID]->g15screen_fd;
	else
		return -1;
}


int invokeDrawFunc(G15AppsData* this, int screenID){
	drawFuncPtrDef draw = getDrawFunc(this->screens[screenID]);
	G15Screen* scrn = getScreen(this, screenID);
	draw((G15Screen*)scrn);		//Invokation
	return EXIT_SUCCESS;
}
int invokeAllDrawFuncs(G15AppsData *this){
	int i;
	for(i=0; i<this->numOfScreens ; i++){
		int ret = invokeDrawFunc(this, i);
		if(ret == EXIT_FAILURE)
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
void deleteAllScreens(G15AppsData *this){
	int i;
	for(i=0;i<MAX_SCREENS && this->screens[i] == NULL;i++){
		g15r_clearScreen (this->screens[i]->canvas, G15_COLOR_WHITE);
		g15_close_screen (this->screens[i]->g15screen_fd);

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
int lightMKey(int screenFD, int G15_KEY){
	unsigned char sh=NULL;
	int ret1, ret2; //TODO Add timeout error
	ret1 = g15_send_cmd (screenFD, G15DAEMON_MKEYLEDS, (G15_LED_M1 | sh ));
	ret2 = g15_recv_oob_answer(screenFD);

	return (ret1&&ret2);
}


G15Screen *newScreen(){				//Constuctor
	return malloc(sizeof(G15AppsData));
}
int setScreen(G15Screen *this, char* name,  int (*drawFuncPtr)(G15Screen*)){
	static int ID = 1;
	this->id = ID;
	ID++;
	this->name = name;
	this->drawFunc = drawFuncPtr;

	this->background=0;
	this->background = 0;
	this->canvas = (g15canvas *) malloc (sizeof (g15canvas));
	this->g15screen_fd = 0;
	if ((this->g15screen_fd = new_g15_screen (G15_G15RBUF)) < 0)
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
	return (this->g15screen_fd);
}
drawFuncPtrDef getDrawFunc(G15Screen* this){
	return this->drawFunc;
}

void deleteScreen(G15Screen *this){	//De-stractor
	free(this);
}
