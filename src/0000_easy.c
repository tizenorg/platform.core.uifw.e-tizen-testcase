#include "e_test_runner.h"

static void
_cb_introspect(void *data,
               const Eldbus_Message *msg,
               Eldbus_Pending *pending EINA_UNUSED)
{
   E_TC *tc = data;
   const char *arg, *name = NULL, *text = NULL;
   Eina_Bool res;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_TRUE_GOTO(res, finish);

   res = eldbus_message_arguments_get(msg, "s", &arg);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   if ((!strstr(arg, "method name=\"RegisterWindow\""   )) ||
       (!strstr(arg, "method name=\"DeregisterWindow\"" )) ||
       (!strstr(arg, "method name=\"SetWindowStack\""   )) ||
       (!strstr(arg, "method name=\"GetWindowInfo\""    )) ||
       (!strstr(arg, "signal name=\"VisibilityChanged\"")) ||
       (!strstr(arg, "signal name=\"StackChanged\""     )) ||
       (!strstr(arg, "property name=\"Registrant\""     )))
     {
        ERR("missing mehod, signal or property:%s\n", arg);
        goto finish;
     }

   tc->passed = EINA_TRUE;

finish:
   if (!tc->passed) ERR("errname:%s errmsg:%s\n", name, text);

   elm_exit();
}

#define _WIN_ID 0x123456

static void
_cb_prop_registrant(void *data,
                    const Eldbus_Message *msg,
                    Eldbus_Pending *p EINA_UNUSED)
{
   E_TC *tc = data;
   const char *name = NULL, *text = NULL;
   Eldbus_Message_Iter *variant = NULL;
   Ecore_Window win;
   Eina_Bool res;
   char *type;

   if (!tc)
     {
        elm_exit();
        return;
     }

   tc->passed = EINA_FALSE;

   res = eldbus_message_error_get(msg, &name, &text);
   EINA_SAFETY_ON_TRUE_GOTO(res, finish);

   res = eldbus_message_arguments_get(msg, "v", &variant);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);
   EINA_SAFETY_ON_NULL_GOTO(variant, finish);

   type = eldbus_message_iter_signature_get(variant);
   EINA_SAFETY_ON_NULL_GOTO(type, finish);
   EINA_SAFETY_ON_FALSE_GOTO((type[0] == 'u'), finish);

   res = eldbus_message_iter_arguments_get(variant, "u", &win);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);
   EINA_SAFETY_ON_FALSE_GOTO((win == _WIN_ID), finish);

   tc->passed = EINA_TRUE;

finish:
   if (!tc->passed)
     {
        ERR("errname:%s errmsg:%s\n", name, text);
     }

   elm_exit();
}

Eina_Bool
tc_0000_base_pass(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   tc->passed = EINA_TRUE;

   return tc->passed;
}

Eina_Bool
tc_0001_base_fail(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   tc->passed = EINA_FALSE;

   return tc->passed;
}

Eina_Bool
tc_0010_introspect(E_TC *tc)
{
   Eldbus_Pending *p;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   p = eldbus_object_introspect(tc->runner->dbus.obj,
                                _cb_introspect,
                                tc);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p, EINA_FALSE);

   elm_run();

   return tc->passed;
}

Eina_Bool
tc_0011_win_register(E_TC *tc)
{
   Eldbus_Pending *p;
   Eina_Bool res;
   E_TC_Win *tw = NULL;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   tw = e_tc_win_info_add(_WIN_ID, EINA_FALSE, EINA_FALSE, "tc", 0, 0, 1, 1, 0);
   EINA_SAFETY_ON_NULL_GOTO(tw, finish);

   /* check RegisterWindow method */
   res = e_test_runner_req_win_register(tc->runner, tw);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   /* check whether registered window id is same */
   p = eldbus_proxy_property_get(tc->runner->dbus.proxy,
                                 "Registrant",
                                 _cb_prop_registrant,
                                 tc);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p, EINA_FALSE);

   elm_run();
   EINA_SAFETY_ON_FALSE_RETURN_VAL(tc->passed, EINA_FALSE);

   /* check DeregisterWindow method */
   res = e_test_runner_req_win_deregister(tc->runner, tw);
   EINA_SAFETY_ON_FALSE_GOTO(res, finish);

   tc->passed = EINA_TRUE;

finish:
   e_tc_win_del(tw);
   return tc->passed;
}
