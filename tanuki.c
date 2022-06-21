#include<stdio.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/extensions/Xinerama.h>

#include"types.h"
#include"config.h"

/* XEvent handler */
static void clientmessage(XEvent *e);
static void configurenotify(XEvent *e);
static void configurerequest(XEvent *e);
static void maprequest(XEvent *e);
static void unmapnotify(XEvent *e);
static void expose(XEvent *e);
static void focusin(XEvent *e);
static void enternotify(XEvent *e);

/* Client managment */
static Client * wintoclient(Window w);
static void setfocus(Client *c);
static void focus(Client *c);

/* main */
static void setup(void);
static void run(void);

static int screen;
static int running = 1;
static const XEvent * handler[LASTEvent] = {
  [EnterNotify]      = enternotify,
  [FocusIn]          = focusin,
  [Expose]           = expose,
  [UnmapNotify]      = unmapnotify,
  [MapRequest]       = maprequest,
  [ConfigureRequest] = configurerequest,
  [ConfigureNotify]  = configurenotify,
  [ClientMessage]    = clientmessage,

};
static Client *focused;
static Display *dpy;
static Window root;

/* XEvent handler */
void
clientmessage(XEvent *e)
{
}

void
configurenotify(XEvent *e)
{
}

void
configurerequest(XEvent *e)
{
}

void
maprequest(XEvent *e)
{
}

void
unmapnotify(XEvent *e)
{
}

void
expose(XEvent *e)
{
}

void
focusin(XEvent *e)
{
}

void
enternotify(XEvent *e)
{
}

/* Client managment */
Client *
wintoclient(Window w)
{
  Client *c;
  c = focused;
  while ((c)) {
    if (c->win == w)
      return c;
  }
  return NULL;
}

void
setfocus(Client *c)
{
  if (!c->hidden)
    return;
  XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
  XSetBorderWidth(dpy, c->win, bw);
}

void
focus(Client *c)
{
  c->prev->next = c->next;
  c->next->prev = c->prev;
  focused->prev = c;
  c->next = focused;
  c->prev = NULL;

  focused = c;
  setfocus(c);
}

/* main */
void
setup()
{
  XSetWindowAttributes wa;
  screen = DefaultScreen(dpy);
  root = RootWindow(dpy, screen);

  wa.cursor = XCreateFontCursor(dpy, XC_left_ptr);
  wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask|
    EnterWindowMask|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;

  XDefineCursor(dpy, root, wa.cursor);
  XSelectInput(dpy, root,wa,event_mask);

  XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
  focused = NULL;
}

void
run(void)
{
  XEvent ev;
  XSync(dpy, False);
  while (running && !XNextEvent(dpy, &ev))
    if (handler[ev.type])
      handler[ev.type](&ev);
}

int
main(void)
{
  if (!(dpy = XOpenDisplay(NULL)))
    return 1;
  setup();
  run();
  XCloseDisplay(dpy);
  return 0;
}
