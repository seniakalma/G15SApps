#include "G15SApps.h"
#include <assert.h>

int main (int argc, char *argv[])
{
	printf("New, Width: %d, Hight: %d\n", G15_LCD_WIDTH, G15_LCD_HEIGHT);
	G15AppsData *G15Keyboard = newKeyboard();
	bool ret = setKeyBoard(G15Keyboard);
	if(ret == EXIT_FAILURE)
		assert(ret==EXIT_FAILURE);

	//Create first screen(SENIA KALMA)
	G15Screen* logoScreen = createNewScreen(G15Keyboard,"SENIA LOGO", &drawLogo );


	//Create second screen(Test)
	G15Screen* testScreen = (G15Screen*)createNewScreen(G15Keyboard, "Test", &drawTest);


	//	Invoke "drawFunc" from all the screens - Draw all screens
	invokeAllDrawFuncs(G15Keyboard);


	//Start a new thread to listen to button presses
	void* handlThrd = (void*)getHandlerThread(G15Keyboard);
	pthread_create(handlThrd, NULL, (void*)keyboardHandlerThread,(void*)&G15Keyboard);

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
