#ifndef E_TC_MAIN_H
#define E_TC_MAIN_H

#include <Eldbus.h>
#include <Elementary.h>

extern int _log_dom;

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)

#ifdef INF
# undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_log_dom, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif
#define WRN(...) EINA_LOG_DOM_WARN(_log_dom, __VA_ARGS__)

#ifdef CRI
# undef CRI
#endif
#define CRI(...) EINA_LOG_DOM_CRIT(_log_dom, __VA_ARGS__)

#ifdef E_NEW
# undef E_NEW
#endif
# define E_NEW(s, n) (s *)calloc(n, sizeof(s))

#ifdef E_FREE
# undef E_FREE
#endif
# define E_FREE(p) do { free(p); p = NULL; } while (0)

#ifdef E_FREE_LIST
# undef E_FREE_LIST
#endif
# define E_FREE_LIST(list, free)   \
  do                               \
    {                              \
       void *_tmp_;                \
       EINA_LIST_FREE(list, _tmp_) \
         {                         \
            free(_tmp_);           \
         }                         \
    }                              \
  while (0)

typedef enum _E_TC_Event_Type
{
   E_TC_EVENT_TYPE_NONE = 0,
   E_TC_EVENT_TYPE_TIMEOUT,
   E_TC_EVENT_TYPE_VIS_ON,
   E_TC_EVENT_TYPE_VIS_OFF,
   E_TC_EVENT_TYPE_STACK_RAISE,
   E_TC_EVENT_TYPE_STACK_LOWER,
   E_TC_EVENT_TYPE_STACK_ABOVE,
   E_TC_EVENT_TYPE_STACK_BELOW,
   E_TC_EVENT_TYPE_MAX
} E_TC_Event_Type;

typedef struct _E_Test_Runner E_Test_Runner;
typedef struct _E_TC_Win      E_TC_Win;
typedef struct _E_TC          E_TC;
typedef struct _E_TC_Data     E_TC_Data;

struct _E_Test_Runner
{
   Eina_List   *tc_list; // test suite

   struct
   {
      Ecore_Timer       *expire_timer;
      E_TC_Event_Type    expect;
      E_TC_Event_Type    response;
   } ev;

   struct
   {
      Eldbus_Connection *conn;
      Eldbus_Proxy      *proxy;
      Eldbus_Object     *obj;
   } dbus;
};

struct _E_TC_Win
{
   Evas_Object *elm_win;    // elm_win evas object
   Ecore_Window native_win; // native window id
   const char  *name;       // name of client window
   int          x, y, w, h; // geometry
   int          layer;      // value of E_Layer
   int          vis;        // visibility
   int          alpha;      // alpha window
};

struct _E_TC
{
   const char    *name;
   unsigned int   num;
   Eina_Bool      (*func) (E_TC *tc);
   Eina_Bool      passed;
   Eina_Bool      expect;
   E_Test_Runner *runner;
   E_TC_Data     *data;
};

Eina_Bool  e_test_runner_req_win_register(E_Test_Runner *runner, E_TC_Win *tw);
Eina_Bool  e_test_runner_req_win_deregister(E_Test_Runner *runner, E_TC_Win *tw);
Eina_List *e_test_runner_req_win_info_list_get(E_Test_Runner *runner);
Eina_Bool  e_test_runner_ev_wait(E_Test_Runner *runner, E_TC_Event_Type ev);

E_TC_Win  *e_tc_win_add(E_TC_Win *parent, Elm_Win_Type type, Eina_Bool alpha, const char *name, int x, int y, int w, int h, int layer);
E_TC_Win  *e_tc_win_info_add(Ecore_Window native_win, Eina_Bool alpha, const char *name, int x, int y, int w, int h, int layer);
void       e_tc_win_del(E_TC_Win *tw);
void       e_tc_win_geom_update(E_TC_Win *tw);
void       e_tc_win_show(E_TC_Win *tw);
void       e_tc_win_hide(E_TC_Win *tw);
void       e_tc_win_stack_change(E_TC_Win *tw, E_TC_Win *sibling, Eina_Bool above);

/* test cases */
Eina_Bool  tc_0000_base_pass(E_TC *tc);
Eina_Bool  tc_0001_base_fail(E_TC *tc);
Eina_Bool  tc_0010_introspect(E_TC *tc);
Eina_Bool  tc_0011_win_register(E_TC *tc);

Eina_Bool  tc_0100_win_show(E_TC *tc);
Eina_Bool  tc_0101_win_stack(E_TC *tc);
Eina_Bool  tc_0110_alpha_win_show(E_TC *tc);
Eina_Bool  tc_0111_alpha_win_stack(E_TC *tc);

Eina_Bool  tc_0200_transient_for_basic(E_TC *tc);
Eina_Bool  tc_0201_transient_for_raise(E_TC *tc);
Eina_Bool  tc_0202_transient_for_lower(E_TC *tc);
Eina_Bool  tc_0203_transient_for_stack_above(E_TC *tc);
Eina_Bool  tc_0204_transient_for_stack_below(E_TC *tc);

#endif
