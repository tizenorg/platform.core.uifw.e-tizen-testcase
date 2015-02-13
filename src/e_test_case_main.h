#ifndef E_MOD_TEST_CASE_H
#define E_MOD_TEST_CASE_H

typedef struct _E_TC_Client E_TC_Client;
typedef struct _E_Test_Case E_Test_Case;

extern Eldbus_Connection *dbus_conn;
extern Eldbus_Proxy *dbus_proxy;
extern Eldbus_Object *dbus_obj;

#define BUS "org.enlightenment.wm"
#define PATH "/org/enlightenment/wm"
#define INTERFACE "org.enlightenment.wm.Test"

struct _E_Test_Case
{
   const char *name;
   const char *desc;

   Eina_Bool (*test) (E_Test_Case *tc);

   Eina_Bool passed;
   Eina_Bool expect;
   Eina_Bool is_stopper;

   Eina_List *inner_tcs;
};

struct _E_TC_Client
{
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
Eina_Bool test_case_0000_easy_pass(E_Test_Case *tc);
Eina_Bool test_case_0000_easy_fail(E_Test_Case *tc);

Eina_Bool test_case_0001_basic(E_Test_Case *tc);
Eina_Bool test_case_0002_basic_stack(E_Test_Case *tc);

/* 0100 - 0199 */
/* 0200 - 0299 */
/* 0300 - 9999 */

/* end of test cases */

#endif
