#include "e.h"
#include "e_test_case_main.h"

#define ADD_TEST_CASE(type_, num_, test_, name_, expect_, is_stopper_) \
   tc = _e_test_case_add(type_, num_, name_, test_, expect_, is_stopper_)
#define ADD_INNER_TCS(type_, num_, test_, name_, expect_, is_stopper_) \
   _e_test_case_inner_add(tc, type_, num_, name_, test_, expect_, is_stopper_)

Eldbus_Connection *dbus_conn;
Eldbus_Proxy *dbus_proxy;
Eldbus_Object *dbus_obj;

static Eina_List *tcs = NULL;

static void
_e_test_case_inner_add(E_Test_Case* gtc,
                       E_Test_Case_Type type,
                       unsigned int num, const char *name,
                       Eina_Bool (*test)(E_Test_Case*),
                       Eina_Bool expect, Eina_Bool is_stopper)
{
   E_Test_Case *tc;

   tc = E_NEW(E_Test_Case, 1);
   tc->type = type;
   tc->num = num,
   tc->name = name;
   tc->test = test;
   tc->expect = expect;
   tc->is_stopper = is_stopper;

   gtc->inner_tcs = eina_list_append(gtc->inner_tcs, tc);
}

static E_Test_Case *
_e_test_case_add(E_Test_Case_Type type,
                 unsigned int num, const char *name,
                 Eina_Bool (*test)(E_Test_Case*),
                 Eina_Bool expect, Eina_Bool is_stopper)
{
   E_Test_Case *tc;

   tc = E_NEW(E_Test_Case, 1);
   tc->type = type;
   tc->num = num;
   tc->name = name;
   tc->test = test;
   tc->expect = expect;
   tc->is_stopper = is_stopper;

   tcs = eina_list_append(tcs, tc);

   return tc;
}

static void
_e_test_case_setup(void)
{
   E_Test_Case *tc = NULL;

   /* [0000 - 0009] E_TEST_CASE_TYPE_BASE */
#undef T
#define T E_TEST_CASE_TYPE_BASE
#undef T_FUNC
#define T_FUNC(num_, test_) test_case_000##num_##_##test_
   ADD_TEST_CASE(T, 0, T_FUNC(0, easy_pass), "Easy Pass", 1, 1);
   ADD_TEST_CASE(T, 0, T_FUNC(0, easy_fail), "Easy Fail", 0, 1);
   ADD_TEST_CASE(T, 1, T_FUNC(1, introspect), "Introspect", 1, 1);
   ADD_TEST_CASE(T, 2, T_FUNC(2, deregister), "(De)Register", 1, 1);

   /* [0010 - 0099] */
#undef T_FUNC
#define T_FUNC(num_, test_) test_case_00##num_##_##test_

   /* [0100 - 0199] E_TEST_CASE_TYPE_WIN */
#undef T
#define T E_TEST_CASE_TYPE_WIN
#undef T_FUNC
#define T_FUNC(num_, test_) test_case_0##num_##_##test_
   ADD_TEST_CASE(T, 100, T_FUNC(100, basic), "Basic", 1, 0);
   ADD_INNER_TCS(T, 101, T_FUNC(101, basic_stack), "Basic Stack", 1, 0);

   ADD_TEST_CASE(T, 110, T_FUNC(110, transient_for), "Transient For", 1, 0);
   ADD_INNER_TCS(T, 111, T_FUNC(111, transient_for_raise), "Transient For Raise", 1, 0);
   ADD_INNER_TCS(T, 112, T_FUNC(112, transient_for_lower), "Transient For Lower", 1, 0);
   ADD_INNER_TCS(T, 113, T_FUNC(113, transient_for_stack_above), "Transient For Stack Above", 1, 0);
   ADD_INNER_TCS(T, 114, T_FUNC(114, transient_for_stack_below), "Transient For STack Below", 1, 0);

   /* [0200 - 0299] E_TEST_CASE_TYPE_WIN_ALPHA */
#undef T
#define T E_TEST_CASE_TYPE_WIN_ALPHA

   /* [0300 - 0999] E_TEST_CASE_TYPE_NONE */
#undef T
#define T E_TEST_CASE_TYPE_NONE

   /* [1000 - 9999] */
#undef T_FUNC
#define T_FUNC(num_, test_) test_case_##num_##_##test_

#undef T
#undef T_FUNC
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

   eina_strbuf_append(buf, "\n\n==============================================\n");
   eina_strbuf_append(buf, "TEST CASE RESULT\n");
   eina_strbuf_append(buf, "==============================================\n");

   EINA_LIST_FOREACH(tcs, l, tc)
     {
        eina_strbuf_append_printf(buf,
                                  "[%04d] TEST \"%-25.25s\" : %s\n",
                                  tc->num,
                                  tc->name, tc->inner_tcs? "-" : tc->passed? "PASS":"FAIL");

        if (tc->inner_tcs)
          {
             EINA_LIST_FOREACH(tc->inner_tcs, ll, in_tc)
               {
                  eina_strbuf_append_printf(buf,
                                            "[%04d] TEST \"%-25.25s\" : %s\n",
                                            in_tc->num,
                                            in_tc->name, in_tc->passed? "PASS":"FAIL");
                  total ++;
                  in_tc->passed? pass_case++ : fail_case++;
                  if ((in_tc->is_stopper) && (!tc->passed))
                    break;
               }
          }
        else
          {
             total ++;
             tc->passed? pass_case++ : fail_case++;
          }

        if ((tc->is_stopper) && (!tc->passed))
          {
             eina_strbuf_append_printf(buf, "\n!!Test Case failed at \"%s\"\n", tc->name);
             break;
          }
     }

   eina_strbuf_append(buf, "==============================================\n");
   eina_strbuf_append_printf(buf, "TOTAL: %4d Case\n", total);
   eina_strbuf_append_printf(buf, "PASS : %4d Case\n", pass_case);
   eina_strbuf_append_printf(buf, "FAIL : %4d Case\n", fail_case);
   eina_strbuf_append(buf, "==============================================\n");

   printf("%s", eina_strbuf_string_get(buf));
}

EAPI Eina_Bool
e_test_case_inner_do(E_Test_Case *tc)
{
   E_Test_Case *_tc;
   Eina_List *l;
   Eina_Bool pass;

   pass = !!tc->inner_tcs;

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
   E_Test_Case *tc;
   Eina_List *l;

   if (!eldbus_init()) return -1;

   /* connect to dbus */
   dbus_conn = eldbus_connection_get(ELDBUS_CONNECTION_TYPE_SYSTEM);
   dbus_obj = eldbus_object_get(dbus_conn, BUS, PATH);
   dbus_proxy = eldbus_proxy_get(dbus_obj, INTERFACE);

   /* setup testcase list */
   _e_test_case_setup();

   /* do test */
   EINA_LIST_FOREACH(tcs, l, tc)
     {
        if (!e_test_case_do(tc)) break;
     }

   /* print test result */
   _e_test_case_result_print();

   /* shutdown */
   EINA_LIST_FREE(tcs, tc)
     {
        E_FREE_LIST(tc->inner_tcs, free);
        E_FREE(tc);
     }

   eldbus_proxy_unref(dbus_proxy);
   eldbus_object_unref(dbus_obj);
   eldbus_connection_unref(dbus_conn);

   eldbus_shutdown();

   return 0;
}
ELM_MAIN()
