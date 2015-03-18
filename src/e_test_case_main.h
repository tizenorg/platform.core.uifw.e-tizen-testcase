#ifndef E_MOD_TEST_CASE_H
#define E_MOD_TEST_CASE_H

# include <Eo.h>
# include <Eina.h>
# include <Eet.h>
# include <Evas.h>
# include <Evas_Engine_Buffer.h>
# include <Ecore.h>
# include <Ecore_Getopt.h>
# include <Ecore_Evas.h>
# include <Ecore_Input.h>
# include <Ecore_Input_Evas.h>
# include <Ecore_Con.h>
# include <Ecore_Ipc.h>
# include <Ecore_File.h>
# include <Efreet.h>
# include <Efreet_Mime.h>
# include <Edje.h>
# include <Eldbus.h>
# include <Eio.h>
# include <Emotion.h>
# include <Elementary.h>

extern int _e_tizen_testcase_log_dom;

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_e_tizen_testcase_log_dom, __VA_ARGS__)

#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_e_tizen_testcase_log_dom, __VA_ARGS__)

#ifdef INF
# undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_e_tizen_testcase_log_dom, __VA_ARGS__)

#ifdef WRN
# undef WRN
#endif
#define WRN(...) EINA_LOG_DOM_WARN(_e_tizen_testcase_log_dom, __VA_ARGS__)

#ifdef CRI
# undef CRI
#endif
#define CRI(...) EINA_LOG_DOM_CRIT(_e_tizen_testcase_log_dom, __VA_ARGS__)

#ifdef E_REALLOC
# undef E_REALLOC
#endif
# define E_REALLOC(p, s, n)   p = (s *)realloc(p, sizeof(s) * n)

#ifdef E_NEW
# undef E_NEW
#endif
# define E_NEW(s, n)          (s *)calloc(n, sizeof(s))

#ifdef E_NEW_RAW
# undef E_NEW_RAW
#endif
# define E_NEW_RAW(s, n)      (s *)malloc(n * sizeof(s))

#ifdef E_FREE
# undef E_FREE
#endif
# define E_FREE(p)            do { free(p); p = NULL; } while (0)

#ifdef E_FREE_LIST
# undef E_FREE_LIST
#endif
# define E_FREE_LIST(list, free)    \
  do                                \
    {                               \
       void *_tmp_;                 \
       EINA_LIST_FREE(list, _tmp_) \
         {                          \
            free(_tmp_);            \
         }                          \
    }                               \
  while (0)

extern Eldbus_Connection *dbus_conn;
extern Eldbus_Proxy *dbus_proxy;
extern Eldbus_Object *dbus_obj;

#define BUS "org.enlightenment.wm"
#define PATH "/org/enlightenment/wm"
#define INTERFACE "org.enlightenment.wm.Test"

typedef enum _E_Test_Case_Type
{
   E_TEST_CASE_TYPE_BASE = 0,
   E_TEST_CASE_TYPE_WIN = 100,
   E_TEST_CASE_TYPE_WIN_ALPHA = 200,
   E_TEST_CASE_TYPE_NONE = 300
} E_Test_Case_Type;

typedef struct _E_TC_Client E_TC_Client;
typedef struct _E_TC_Data E_TC_Data;
typedef struct _E_Test_Case E_Test_Case;

struct _E_Test_Case
{
   const char *name;
   E_Test_Case_Type type;
   unsigned int num;

   Eina_Bool (*test) (E_Test_Case *tc);

   Eina_Bool passed;
   Eina_Bool expect;
   Eina_Bool is_stopper;

   Eina_List *inner_tcs;
};

struct _E_TC_Data
{
   E_TC_Client *client;
   Eina_List *clients;
};

struct _E_TC_Client
{
   Evas_Object *obj;

   const char *name;
   int x, y, w, h;
   int layer;
   int visible;
   int argb;
   Ecore_Window win;
};

EAPI Eina_Bool e_test_case_inner_do(E_Test_Case *tc);
EAPI Eina_Bool e_test_case_do(E_Test_Case *tc);

/* test cases  */
/* 0000 - 0099 */
//force true/false
Eina_Bool test_case_0000_easy_pass(E_Test_Case *tc);
Eina_Bool test_case_0000_easy_fail(E_Test_Case *tc);
//dbus check
Eina_Bool test_case_0001_introspect(E_Test_Case *tc);
Eina_Bool test_case_0002_deregister(E_Test_Case *tc);

/* 0100 - 0199 */
//normal state
Eina_Bool test_case_0100_basic(E_Test_Case *tc);
Eina_Bool test_case_0101_basic_stack(E_Test_Case *tc);

//transient_for
Eina_Bool test_case_0110_transient_for(E_Test_Case *tc);
Eina_Bool test_case_0111_transient_for_raise(E_Test_Case *tc);
Eina_Bool test_case_0112_transient_for_lower(E_Test_Case *tc);
Eina_Bool test_case_0113_transient_for_stack_above(E_Test_Case *tc);
Eina_Bool test_case_0114_transient_for_stack_below(E_Test_Case *tc);

//notification_level
Eina_Bool test_case_0120_notification_level(E_Test_Case *tc);
Eina_Bool test_case_0121_notification_level_1(E_Test_Case *tc);
Eina_Bool test_case_0122_notification_level_2(E_Test_Case *tc);
Eina_Bool test_case_0123_notification_level_3(E_Test_Case *tc);
Eina_Bool test_case_0124_notification_level_change(E_Test_Case *tc);

/* 0200 - 0299 */
/* 0300 - 9999 */

/* end of test cases */

#endif
