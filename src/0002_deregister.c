#include "e.h"
#include "e_test_case_main.h"

static const Ecore_Window custom_win = 0x123456;
static Eina_Bool pass = EINA_FALSE;

static void
_method_cb_deregister_window(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   Eina_Bool allowed = EINA_FALSE;
   const char *errname, *errmsg;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s\n", errname, errmsg);
     }

   if (!eldbus_message_arguments_get(msg, "b", &allowed))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
     }

   pass = allowed;
   elm_exit();
}

static void
_property_cb_get_registrant(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   Ecore_Window win;
   Eldbus_Message_Iter *variant = NULL;
   Eldbus_Pending *pend = NULL;
   char *type;
   const char *errname, *errmsg;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("Message error %s - %s\n\n", errname, errmsg);
     }

   if (!eldbus_message_arguments_get(msg, "v", &variant))
     {
        ERR("Error getting arguments.");
     }

   type = eldbus_message_iter_signature_get(variant);
   if (type[0] == 'u')
     {
        if (eldbus_message_iter_arguments_get(variant, "u", &win))
          {
             if (win == custom_win)
               {
                  pend = eldbus_proxy_call(dbus_proxy, "DeregisterWindow",
                                           _method_cb_deregister_window, NULL, -1,
                                           "u", custom_win);
                  if (pend) return;
               }
          }
     }

   elm_exit();
}

static void
_method_cb_register_window(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   Eina_Bool  accepted = EINA_FALSE;
   const char *errname, *errmsg;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s\n", errname, errmsg);
        elm_exit();
        return;
     }

   if (!eldbus_message_arguments_get(msg, "b", &accepted))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
        elm_exit();
        return;
     }

   if (!eldbus_proxy_property_get(dbus_proxy, "Registrant", _property_cb_get_registrant, NULL))
       elm_exit();
}

Eina_Bool
test_case_0002_deregister(E_Test_Case *tc)
{

   if ((!dbus_conn) || (!dbus_obj) || (!dbus_proxy)) return pass;
   if (!eldbus_proxy_call(dbus_proxy, "RegisterWindow", _method_cb_register_window, NULL, -1, "u", custom_win))
     return pass;

   elm_run();

   return pass;
}
