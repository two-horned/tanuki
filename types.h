typedef struct Client Client;
typedef struct Monitor Monitor;
typedef struct Layout Layout;
typedef struct Tag Tag;

struct Layout {
  const char *symbol;
};

struct Tag {
  Client *clients;
};

struct Client {
  Window win;
  int x, y, w, h, oldx, oldy, oldw, oldh;
  int isfullscreen, isfloating, ishidden;
  Client *next, *prev;
};

struct Monitor {
  Tag *tags;
  int screen, x, y, w, h;
};
