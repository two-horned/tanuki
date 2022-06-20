typedef struct Client Client;
typedef struct Monitor Monitor;
typedef struct Layout Layout;
typedef struct Geometrie Geometrie;
typedef struct Tag Tag;

struct Layout {
  const char *symbol;
};

struct Tag {
  Tag *next, *prev;
  Client *clients;
};

struct Client {
  Window win;
  Geometrie *now, *old;
  int isfullscreen, isfloating, hidden;
  Client *next, *prev;
  char name[256];
};

struct Geometrie {
  int x, y, w, h;
};

struct Monitor {
  Tag *tags;
  int screen;
  Geometrie geom;
  Monitor *next, *prev;
  Layout *layout;
};
