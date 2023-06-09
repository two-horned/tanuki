#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/cursorfont.h>
#include<X11/extensions/Xinerama.h>

#include"types.h"
#include"config.h"

/* XEvent handler */
//static void clientmessage(XEvent *e);
static void configurenotify(XEvent *e);
static void configurerequest(XEvent *e);
static void maprequest(XEvent *e);
static void unmapnotify(XEvent *e);
static void expose(XEvent *e);
static void focusin(XEvent *e);
static void enternotify(XEvent *e);

/* Client managment */
static Client * wintoclient(Window w);
static void configure(Client *c);
static void unmanage(Client *c);
static void manage(Window w, XWindowAttributes wa);
static void setfocus(Client *c);
static void age(Client *c);
static void mark(Client *c);
static void setfocus(Client *c);
static void focus(Client *c);

/* Monitor managment */
static void setmon(void);

/* main */
static void setup(void);
static void run(void);

static int screen, sw, sh;
static int running = 1;
static void (*handler[LASTEvent]) (XEvent *e) = {
  [EnterNotify]      = enternotify,
  [FocusIn]          = focusin,
  [Expose]           = expose,
  [UnmapNotify]      = unmapnotify,
  [MapRequest]       = maprequest,
  [ConfigureRequest] = configurerequest,
  [ConfigureNotify]  = configurenotify,
//  [ClientMessage]    = clientmessage,
};
static Cursor cursor, mcursor;
static Client *focused = NULL;
static Monitor *viewed;
static Display *dpy;
static Window root, check;

/* XEvent handler */

/*
void
clientmessage(XEvent *e)
{
}*/

void
configurenotify(XEvent *e)
{
  XConfigureEvent *ev = &e->xconfigure;

  if (ev->window == root) {
    sw = ev->width;
    sh = ev->height;
  }
  free(viewed);
  viewed = NULL;
  setmon();
}

void
configurerequest(XEvent *e)
{
  Client *c;
  XConfigureRequestEvent *ev = &e->xconfigurerequest;
  XWindowChanges wc;

  wc.x = ev->x;
  wc.y = ev->y;
  wc.width = ev->width;
  wc.height = ev->height;
  wc.border_width = ev->border_width;
  wc.sibling = ev->above;
  wc.stack_mode = ev->detail;

  XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);

  if (!(c = wintoclient(ev->window)))
    return;
  if (c->isfullscreen)
    return;
  age(c);
  c->x = ev->x;
  c->x = ev->y;
  c->x = ev->width;
  c->x = ev->height;
  configure(c);
}

void
maprequest(XEvent *e)
{
  static XWindowAttributes wa;
  XMapRequestEvent *ev = &e->xmaprequest;
  Client *c;

  if (!XGetWindowAttributes(dpy, ev->window, &wa))
    return;
  if (wa.override_redirect)
    return;

  manage(ev->window, wa);
}

void
unmapnotify(XEvent *e)
{
  XUnmapEvent *ev = &e->xunmap;
  Client *c;

  if (!(c = wintoclient(ev->window)))
      return;
  unmanage(c);
}

void
expose(XEvent *e)
{
  XExposeEvent *ev = &e->xexpose;
  Client *c;

  if(!(c = wintoclient(ev->window)))
    return;
  mark(c);
}

void
focusin(XEvent *e)
{
  XFocusChangeEvent *ev = &e->xfocus;
  setfocus(focused);
}

void
enternotify(XEvent *e)
{
  XEnterWindowEvent *ev = &e->xcrossing;
  Client *c;

  if(!(c = wintoclient(ev->window)))
      return;
  if (c == focused)
    return;
  focus(c);
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
manage(Window w, XWindowAttributes wa)
{
  Client *c;
  c = malloc(sizeof(Client));
  
  c->win = w;
  c->x = wa.x;
  c->y = wa.y;
  c->w = wa.width;
  c->h = wa.height;
  c->ishidden = 0;
  c->isfloating = 0;
  c->isfullscreen = 0;
  age(c);

  XSetWindowBorderWidth(dpy, w, 0);

  XMapWindow(dpy, c->win);
  focus(c);
  XMoveWindow(dpy, w, 0, 0);
}

void
unmanage(Client *c)
{
  if (c == focused) {
    focused = c->next;
    focused->prev = NULL;
  } else if ((c->next)) {
    c->prev->next = c->next;
    c->next->prev = c->prev;
  } else
    c->prev->next = NULL;
  free(c);
}

void
age(Client *c)
{
  c->oldx = c->x;
  c->oldy = c->y;
  c->oldw = c->w;
  c->oldh = c->h;
}

void unfocus(Client *c)
{
  XSetWindowBorderWidth(dpy, c->win, 0);
}

void
mark(Client *c)
{
  XSetWindowBorderWidth(dpy, c->win, bw);
  //XSetWindowBorder(dpy, c->win, mcol);
}

void
setfocus(Client *c)
{
  if (!c->ishidden)
    return;
  XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
  XSetWindowBorderWidth(dpy, c->win, bw);
  //XSetWindowBorder(dpy, c->win, fcol);
}

void
focus(Client *c)
{
  if((focused)) {
    unfocus(focused);
    if ((c->next)) {
      c->prev->next = c->next;
      c->next->prev = c->prev;
    } else
      c->prev->next = NULL;
    focused->prev = c;
    c->next = focused;
    c->prev = NULL;
  }

  focused = c;
  setfocus(c);
}

/* Montitor managment */
void
setmon(void)
{
  XineramaScreenInfo *s;
  int n;
  
  if (!XineramaIsActive(dpy))
    return;
  if (!(s = XineramaQueryScreens(dpy, &n)))
    return;

  viewed = malloc(sizeof(Monitor) * n);
  for (int i = 0; i < n; i++) {
    viewed[i].screen = s[i].screen_number;
    viewed[i].x = s[i].x_org;
    viewed[i].y = s[i].y_org;
    viewed[i].w = s[i].width;
    viewed[i].h = s[i].height;
  }
}

void
killclient(void)
{
  Client *c = focused;
  focused = focused->next;
  focused->prev = NULL;
  XDestroyWindow(dpy, c->win);
}

void
configure(Client *c)
{
  XMoveWindow(dpy, c->win, c->x, c->y);
}

/* main */
void
setup(void)
{
  XSetWindowAttributes wa;
  screen = DefaultScreen(dpy);
  sw = DisplayWidth(dpy, screen);
  sh = DisplayWidth(dpy, screen);
  root = RootWindow(dpy, screen);

  cursor = wa.cursor = XCreateFontCursor(dpy, XC_left_ptr);
  mcursor = XCreateFontCursor(dpy, XC_crosshair);
  wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask|
    EnterWindowMask|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;

  XDefineCursor(dpy, root, wa.cursor);

  check = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
  XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
  XSelectInput(dpy, root, wa.event_mask);
}

void
run(void)
{
  XEvent ev;
  XSync(dpy, False);
  while (running)
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
