#ifndef E_MOD_TEST_CASE_H
#define E_MOD_TEST_CASE_H

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

   Eina_List *sig_hdlrs;

   Eina_Bool wait_close;
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

/* 0200 - 0299 */
/* 0300 - 9999 */

/* end of test cases */

#endif
