/*
 * SGateG15.h
 *
 *  Created on: Feb 9, 2016
 *      Author: Senia
 */

#ifndef G15SAPPS_H_
#define G15SAPPS_H_

#define TTF_SUPPORT 1

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdbool.h"
#include <pthread.h>
#include <libgen.h>
#include <libg15.h>
#include <libg15render.h>
#include <g15daemon_client.h>

#include "SYSINFO.h"

#define SEC2MICRO 1000000
#define MAX_SCREENS 5
//		###  "Class" G15AppsData  ###
typedef struct G15AppsData_s G15AppsData;
typedef struct G15Screen_s G15Screen;
typedef int (*drawFuncPtrDef)(G15Screen*);
typedef int (*lightMKeyPtrDef)(G15Screen*, int);
G15AppsData *newKeyboard();				//Constuctor
int setKeyBoard(G15AppsData *this);
void* getHandlerThread(G15AppsData *this);
G15Screen* getScreen(G15AppsData *this, int screenID);
int getScreenFDByID(G15AppsData *this, int screenID);
g15canvas* getCanvasByID(G15AppsData *this, int screenID);
int invokeDrawFunc(G15AppsData* this, int screenID);
int invokeAllDrawFuncs(G15AppsData *this);

void deleteKeyBoard(G15AppsData *this);	//De-stractor


//		###  "Class" Screen  ###
G15Screen *newScreen();				//Constuctor
G15Screen* createNewScreen(G15AppsData *this,char* name, int (*drawFuncPtr)(G15Screen*));
int setScreen(G15Screen *this, char* name,  int (*drawFuncPtr)(G15Screen*));

int getScreenFD(G15Screen *this);
g15canvas* getCanvas(G15Screen *this);
drawFuncPtrDef getDrawFunc(G15Screen* this);
void deleteScreen(G15Screen *this);	//De-stractor
void deleteAllScreens(G15AppsData *this);

void updateNClearScreen(int g15screen_fd, g15canvas* canvas);
int drawLogo(G15Screen* screen);
int drawTest(G15Screen* screen);
void keyboardHandlerThread(G15AppsData * Keyboard);


//won't show from libg15render.h somewhy..
int g15r_ttfLoad (g15canvas * canvas, char *fontname, int fontsize, int face_num);
void g15r_ttfPrint (g15canvas * canvas, int x, int y, int fontsize, int face_num, int color, int center, char *print_string);

#endif /* G15SAPPS_H_ */
