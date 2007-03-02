/* (C)opyright MMVI-MMVII Anselm R. Garbe <garbeam at gmail dot com>
 * See LICENSE file for license details.
 */
#define _XOPEN_SOURCE 500
#if HAVE_SHADOW_H
#include <shadow.h>
#endif

#include <ctype.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

const char *
get_password() { /* only run as root */
	const char *rval;
	struct passwd *pw;

	if(geteuid() != 0) {
		fputs("slock: cannot retrieve password entry (make sure to suid slock)\n", stderr);
		exit(EXIT_FAILURE);
	}
	pw = getpwuid(getuid());
	endpwent();
	rval =  pw->pw_passwd;

#if HAVE_SHADOW_H
	{
		struct spwd *sp;
		sp = getspnam(getenv("USER"));
		endspent();
		rval = sp->sp_pwdp;
	}
#endif
	/* drop privileges */
	if(setgid(pw->pw_gid) < 0 || setuid(pw->pw_uid) < 0) {
		fputs("slock: cannot drop privileges\n",stdout);
		exit(EXIT_FAILURE);
	}
	return rval;
}

int
main(int argc, char **argv) {
	char curs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	char buf[32], passwd[256];
	int num, screen;
	const char *pws;
	unsigned int len;
	Bool running = True;
	Cursor invisible;
	Display *dpy;
	KeySym ksym;
	Pixmap pmap;
	Window root, w;
	XColor black, dummy;
	XEvent ev;
	XSetWindowAttributes wa;

	if((argc > 1) && !strncmp(argv[1], "-v", 3)) {
		fputs("slock-"VERSION", (C)opyright MMVI Anselm R. Garbe\n", stdout);
		exit(EXIT_SUCCESS);
	}
	pws = get_password();
	if(!(dpy = XOpenDisplay(0))) {
		fputs("slock: cannot open display\n", stderr);
		exit(EXIT_FAILURE);
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	/* init */
	wa.override_redirect = 1;
	wa.background_pixel = BlackPixel(dpy, screen);
	w = XCreateWindow(dpy, root, 0, 0, DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
			0, DefaultDepth(dpy, screen), CopyFromParent,
			DefaultVisual(dpy, screen), CWOverrideRedirect | CWBackPixel, &wa);

	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), "black", &black, &dummy);
	pmap = XCreateBitmapFromData(dpy, w, curs, 8, 8);
	invisible = XCreatePixmapCursor(dpy, pmap, pmap, &black, &black, 0, 0);
	XDefineCursor(dpy, w, invisible);
	XMapRaised(dpy, w);
	for(len = 1000; len; len--) {
		if(XGrabPointer(dpy, w, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
			GrabModeAsync, GrabModeSync, None, invisible, CurrentTime) == GrabSuccess)
			break;
		usleep(1000);
	}
	if((running = running && (len > 0))) {
		for(len = 1000; len; len--) {
			if(XGrabKeyboard(dpy, w, True, GrabModeAsync, GrabModeAsync, CurrentTime)
				== GrabSuccess)
				break;
			usleep(1000);
		}
		running = (len > 0);
	}
	len = 0;
	XSync(dpy, False);

	/* main event loop */
	while(running && !XNextEvent(dpy, &ev))
		if(ev.type == KeyPress) {
			buf[0] = 0;
			num = XLookupString(&ev.xkey, buf, sizeof buf, &ksym, 0);
			if(IsFunctionKey(ksym) || IsKeypadKey(ksym)
					|| IsMiscFunctionKey(ksym) || IsPFKey(ksym)
					|| IsPrivateKeypadKey(ksym))
				continue;
			switch(ksym) {
			case XK_Return:
				passwd[len] = 0;
				if((running = strcmp(crypt(passwd, pws), pws)) != 0)
					XBell(dpy, 100);
				len = 0;
				break;
			case XK_Escape:
				len = 0;
				break;
			case XK_BackSpace:
				if(len)
					--len;
				break;
			default:
				if(num && !iscntrl((int) buf[0]) && (len + num < sizeof passwd)) { 
					memcpy(passwd + len, buf, num);
					len += num;
				}
				break;
			}
		}
	XUngrabPointer(dpy, CurrentTime);
	XFreePixmap(dpy, pmap);
	XDestroyWindow(dpy, w);
	XCloseDisplay(dpy);
	return 0;
}
