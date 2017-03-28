/*
 * compile & link: gcc -pipe -O2 -o xsendevent{,.c} -L /usr/X11R6/lib -lX11
 * run: ./xsendevent "$WINDOWID" 1 2 3 4 Left
 * # sends keys 1, 2, 3, 4 and Left to window $WINDOWID
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
int main(int argc, char *argv[])
{
  Display *dpy;
  Window window;
  char *prog;
  char *s;
  int i;
  if ((prog=strrchr(argv[0],'/'))==NULL)
      prog=argv[0];
  else
      prog++;
  if (argc < 2) {
    printf("Usage: %s windowid\n", prog);
    return 1;
  }
  dpy = XOpenDisplay(NULL);
  if (dpy == NULL) {
    fprintf(stderr,"%s: Cannot open display\n",prog);
    return 1;
  }
  window = (Window)strtol(argv[1], &s, 0);
  if(*s) {
      fprintf(stderr, "%s: bad windowid: `%s'\n%*c\n",
          prog,argv[1],strlen(prog)+17+1+(s-argv[1]),'^');
      XCloseDisplay(dpy);
      return 1;
  }

  printf ("Lowering window %d\n", window);
  XLowerWindow(dpy,window);

/*
  for(i = 2; i < argc; i++) {
      XEvent ev;
      KeySym key;
      ev.xkey.type = KeyPress;
      ev.xkey.window = window;
      ev.xkey.root = ev.xkey.subwindow = None;
      ev.xkey.time = 0;
      ev.xkey.x = ev.xkey.y = ev.xkey.x_root = ev.xkey.y_root = 0;
      ev.xkey.state = 0;
      key = XStringToKeysym(argv[i]);
      ev.xkey.keycode = XKeysymToKeycode(dpy,key);
      ev.xkey.same_screen = True;
      XSendEvent(dpy, window, True, KeyPressMask, &ev);
      ev.type = KeyRelease;
      XSendEvent(dpy, window, True, KeyReleaseMask, &ev);
  }
*/

/*
  Atom wm_delete=XInternAtom(dpy, "WM_DELETE_WINDOW", False);
//  Atom wm_take_focus=XInternAtom(dpy, "WM_TAKE_FOCUS", False);
  Atom protos=XInternAtom(dpy, "WM_PROTOCOLS", False);  
  
  if (wm_delete) {
    XEvent ev;
    ev.type = ClientMessage;
    ev.xclient.window = window;
    ev.xclient.message_type = protos;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = wm_delete;
    ev.xclient.data.l[1] = CurrentTime;

    XSendEvent(dpy, window, False, NoEventMask, &ev);
  }
  else {
    printf("ATOM not found!\n");  
  }
*/
/*
    XEvent ev;
    printf ("Sending event of type Expose\n");
    memset(&ev, 0, sizeof(ev));
    ev.type = Expose;
    ev.xexpose.window = window;
    XSendEvent(dpy,window,False,ExposureMask,&ev);
    XFlush(dpy);
*/                                                                    
/*
  XSetWindowAttributes xswa;
  xswa.override_redirect = True;
  XChangeWindowAttributes(dpy, window, CWOverrideRedirect, &xswa);
  XRaiseWindow(dpy, window);
  xswa.override_redirect = False;
  XChangeWindowAttributes(dpy, window, CWOverrideRedirect, &xswa);
  XSetInputFocus(dpy, window, RevertToNone, CurrentTime);
*/

  XCloseDisplay(dpy);
  return 0;
}


/*

static void
setup_standard_atoms(void)
{
Atom state, protos, delete, cmapwins;

	arwm.atoms.wm.state=GETATOM("WM_STATE");
	arwm.atoms.wm.protos=GETATOM("WM_PROTOCOLS");
	arwm.atoms.wm.delete=GETATOM("WM_DELETE_WINDOW");
}

static int 
send_xmessage(Window w, Atom a, long x)
{
	XEvent ev;

	ev.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = a;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = x;
	ev.xclient.data.l[1] = CurrentTime;

	return XSendEvent(arwm.X.dpy, w, False, NoEventMask, &ev);
}


                
void 
send_wm_delete(Client * c, int kill_client)
{
	if(kill_client)
		send_xmessage(c->window, arwm.atoms.wm.protos, 
			arwm.atoms.wm.delete);
	else
		XKillClient(arwm.X.dpy, c->window);
}
*/

