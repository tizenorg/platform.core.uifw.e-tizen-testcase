#include "e.h"
#include "e_test_case_main.h"

static Eina_Bool pass = EINA_FALSE;

static void
_method_cb_introspect(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   const char *introspect;
   const char *errname, *errmsg;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s\n", errname, errmsg);
        elm_exit();
        return;
     }

   if (!eldbus_message_arguments_get(msg, "s", &introspect))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
        elm_exit();
        return;
     }

   if ((!strstr(introspect, "method name=\"RegisterWindow\"")) ||
       (!strstr(introspect, "method name=\"DeregisterWindow\"")) ||
       (!strstr(introspect, "method name=\"GetClients\""))  ||
       (!strstr(introspect, "method name=\"ChangeStack\""))  ||
       (!strstr(introspect, "signal name=\"ChangeVisibility\"")) ||
       (!strstr(introspect, "signal name=\"Restack\"")) ||
       (!strstr(introspect, "property name=\"Registrant\"")))
     {
        elm_exit();
        return;
     }

   pass = EINA_TRUE;
   elm_exit();
}

Eina_Bool
test_case_0001_introspect(E_Test_Case *tc)
{

   if ((!dbus_conn) || (!dbus_obj) || (!dbus_proxy)) return pass;

   if (!eldbus_object_introspect(dbus_obj, _method_cb_introspect, NULL))
     return pass;

   elm_run();

   return pass;
}
