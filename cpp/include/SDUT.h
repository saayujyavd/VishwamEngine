#include <windows.h>

// OGL related headers
#include <gl/GL.h>
#include <GL/GLU.h>

#ifndef SDUT_H
#define SDUT_H

#ifdef __cplusplus
extern "C" {
#endif

// macros
#define SDUT_RIGHT_BUTTON  WM_RBUTTONDOWN
#define SDUT_LEFT_BUTTON   WM_LBUTTONDOWN
#define SDUT_MIDDLE_BUTTON WM_MBUTTONDOWN

#define SDUT_MOUSE  0x0000
#define SDUT_DOUBLE 0x0001
#define SDUT_RGBA   0x0002
#define SDUT_DEPTH  0x0004

#define SDUT_CURRENT_TIME 0x0003

#define SDUT_DOWN 0x0005
#define SDUT_UP   0x0006
#define SDUT_MOVE 0x0007

// function prototypes
	void sdutInit(int, char* argv[]);
	void sdutInitDisplayMode(unsigned int mode);
	void sdutInitWindowSize(unsigned int, unsigned int);
	void sdutInitWindowPosition(unsigned int, unsigned int);
	void sdutCreateWindow(char* wndname);

	void sdutReshapeFunc(void (*func)(int, int));
	void sdutRenderFunc(void (*func)(void));
	void sdutKeyboardFunc(void (*func)(unsigned char, int, int));
	void sdutKeyUpFunc(void (*func)(unsigned char, int, int));
	void sdutMouseFunc(void (*func)(int, int, int, int));
	void sdutUpdateFunc(void (*func)(void));
	void sdutCloseFunc(void (*func)(void));

	void sdutMainLoop(void);
	void sdutLeaveMainLoop(void);
	void sdutSwapBuffers(void);
	void sdutFullscreen(void);
	void sdutLeaveFullscreen(void);

	GLfloat sdutGet(unsigned int);

#ifdef __cplusplus
}
#endif

#endif
