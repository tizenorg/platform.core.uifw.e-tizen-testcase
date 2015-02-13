#include "e.h"
#include "e_test_case_main.h"

#define ADD_TEST_CASE(test_, name_, desc_, expect_, is_stopper_) \
   tc = _e_test_case_add(name_, desc_, test_case_##test_, expect_, is_stopper_)
#define ADD_INNER_TCS(test_, name_, desc_, expect_, is_stopper_) \
   _e_test_case_inner_add(tc, name_, desc_, test_case_##test_, expect_, is_stopper_)

Eldbus_Connection *dbus_conn;
Eldbus_Proxy *dbus_proxy;
Eldbus_Object *dbus_obj;

static Eina_List *signal_hdlrs;
static Eina_List *tcs = NULL;

static void
_e_test_case_inner_add(E_Test_Case* gtc,
                       const char *name, const char* desc,
                       Eina_Bool (*test)(E_Test_Case*),
                       Eina_Bool expect, Eina_Bool is_stopper)
{
   E_Test_Case *tc;

   tc = E_NEW(E_Test_Case, 1);
   tc->name = name;
   tc->desc = desc;
   tc->test = test;
   tc->expect = expect;
   tc->is_stopper = is_stopper;

   gtc->inner_tcs = eina_list_append(gtc->inner_tcs, tc);
}

static E_Test_Case *
_e_test_case_add(const char *name, const char* desc,
                 Eina_Bool (*test)(E_Test_Case*),
                 Eina_Bool expect, Eina_Bool is_stopper)
{
   E_Test_Case *tc;

   tc = E_NEW(E_Test_Case, 1);
   tc->name = name;
   tc->desc = desc;
   tc->test = test;
   tc->expect = expect;
   tc->is_stopper = is_stopper;

   tcs = eina_list_append(tcs, tc);

   return tc;
}

static void
_e_test_case_result_print(void)
{
   Eina_Strbuf *buf;
   Eina_List *l, *ll;
   E_Test_Case *tc, *in_tc;
   int pass_case = 0;
   int fail_case = 0;
   int total = 0;

   if (!(buf = eina_strbuf_new())) return;

   eina_strbuf_append(buf, "\n\n====================================\n");
   eina_strbuf_append(buf, "TEST CASE RESULT\n");
   eina_strbuf_append(buf, "====================================\n");

   EINA_LIST_FOREACH(tcs, l, tc)
     {
        eina_strbuf_append_printf(buf,
                                  "[%04d] TEST \"%15s\" : %s\n",
                                  total++,
                                  tc->name, tc->passed? "PASS":"FAIL");
        tc->passed? pass_case++ : fail_case++;
        EINA_LIST_FOREACH(tc->inner_tcs, ll, in_tc)
          {
             eina_strbuf_append_printf(buf,
                                       "[%04d] TEST \"%15s\" : %s\n",
                                       total++,
                                       in_tc->name, in_tc->passed? "PASS":"FAIL");
             in_tc->passed? pass_case++ : fail_case++;
             if ((in_tc->is_stopper) && (!tc->passed))
               break;
          }

        if ((tc->is_stopper) && (!tc->passed))
          {
             eina_strbuf_append_printf(buf, "\n!!Test Case failed at \"%s\"\n", tc->name);
             break;
          }
     }

   eina_strbuf_append(buf, "====================================\n");
   eina_strbuf_append_printf(buf, "TOTAL: %4d Case\n", total);
   eina_strbuf_append_printf(buf, "PASS : %4d Case\n", pass_case);
   eina_strbuf_append_printf(buf, "FAIL : %4d Case\n", fail_case);
   eina_strbuf_append(buf, "====================================\n");

   printf("%s", eina_strbuf_string_get(buf));
}


EAPI Eina_Bool
e_test_case_inner_do(E_Test_Case *tc)
{
   E_Test_Case *_tc;
   Eina_List *l;
   Eina_Bool pass;

   pass == !!tc->inner_tcs;

   EINA_LIST_FOREACH(tc->inner_tcs, l, _tc)
     {
        _tc->passed = (_tc->test(_tc) == _tc->expect);
        printf("TEST \"%s\" : %s\n", _tc->name, _tc->passed?"PASS":"FAIL");
        pass = pass && _tc->passed;

        if ((!_tc->passed) && (_tc->is_stopper))
          {
             tc->is_stopper = EINA_TRUE;
             break;
          }
     }

   return pass;
}

EAPI Eina_Bool
e_test_case_do(E_Test_Case *tc)
{
   Eina_Bool pass = EINA_FALSE;

   pass = tc->test(tc);
   tc->passed = (pass == tc->expect);

   printf("TEST \"%s\" : %s\n", tc->name, tc->passed?"PASS":"FAIL");

   if ((!tc->passed) && (tc->is_stopper))
     return EINA_FALSE;

   return EINA_TRUE;
}

EAPI_MAIN int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{
   E_Test_Case *tc, *in_tc;
   Eina_List *l, *ll;
   Eldbus_Signal_Handler *_sh;

   if (!eldbus_init()) return -1;

   /* connect to dbus */
   dbus_conn = eldbus_connection_get(ELDBUS_CONNECTION_TYPE_SYSTEM);
   dbus_obj = eldbus_object_get(dbus_conn, BUS, PATH);
   dbus_proxy = eldbus_proxy_get(dbus_obj, INTERFACE);

   /* add basic signal handler */

   /* make test case list */
   ADD_TEST_CASE(0000_easy_pass, "Easy Pass", "Always passed", 1, 1);
   ADD_TEST_CASE(0000_easy_fail, "Easy Fail", "Always failed", 0, 1);

   ADD_TEST_CASE(0001_basic, "Basic", "Create basic window and do inner tcs after it shows completely", 1, 0);
   ADD_INNER_TCS(0002_basic_stack, "Basic Stack", "Check stack of the window", 1, 0);

   /* do test */
   EINA_LIST_FOREACH(tcs, l, tc)
     {
        if (!e_test_case_do(tc))
          break;
     }

   /* print test result */
   _e_test_case_result_print();

   /* shutdown */
   EINA_LIST_FREE(tcs, tc)
     {
        EINA_LIST_FREE(tc->inner_tcs, in_tc)
           E_FREE(in_tc);

        E_FREE(tc);
     }

   EINA_LIST_FREE(signal_hdlrs, _sh)
     {
        eldbus_signal_handler_del(_sh);
     }

   eldbus_proxy_unref(dbus_proxy);
   eldbus_object_unref(dbus_obj);
   eldbus_connection_unref(dbus_conn);

   eldbus_shutdown();

   return 0;
}
ELM_MAIN()
