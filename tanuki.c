#include<stdio.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/extensions/Xinerama.h>

#include"types.h"
#include"config.h"

static Client * wintoclient(Window w);
static void setfocus(Client *c);
static void focus(Client *c);
static void setup(void);
static void run(void);

static int screen;
static int running = 1;
static Client *focused;
static Display *dpy;
static Window root;

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
