#include "e_test_case_main.h"
#include "e_test_case_util.h"

static Ecore_Timer *vis_exit_timer = NULL;

static void
_method_cb_get_clients(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   const char *errname, *errmsg;
   Eldbus_Message_Iter *array_of_ec, *struct_of_ec;
   Ecore_Window target_win = 0;
   E_TC_Data *tc_data = data;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
        elm_exit();
        return;
     }

   if (!eldbus_message_arguments_get(msg, "ua(usiiiiibb)", &target_win, &array_of_ec))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
        elm_exit();
        return;
     }

   if (tc_data->clients)
     tc_data->clients = eina_list_free(tc_data->clients);

   while (eldbus_message_iter_get_and_next(array_of_ec, 'r', &struct_of_ec))
     {
        const char  *name;
        int x, y, w, h, layer;
        Eina_Bool visible, argb;
        Ecore_Window win;
        E_TC_Client *client = NULL;

        if (!eldbus_message_iter_arguments_get(struct_of_ec, "usiiiiibb", &win, &name, &x, &y, &w, &h, &layer, &visible, &argb))
          {
             WRN("Error on eldbus_message_arguments_get()\n");
             continue;
          }

        client = E_NEW(E_TC_Client, 1);
        client->x = x;
        client->y = y;
        client->w = w;
        client->h = h;
        client->layer = layer;
        client->win = win;
        client->name = eina_stringshare_add(name);
        tc_data->clients = eina_list_append(tc_data->clients, client);
     }

   elm_exit();
}

static void
_method_cb_register_window(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   const char *errname, *errmsg;
   Eina_Bool *accepted = data;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s\n", errname, errmsg);
     }

   if (!eldbus_message_arguments_get(msg, "b", accepted))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
     }

   elm_exit();
}

static void
_method_cb_deregister_window(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   const char *errname, *errmsg;
   Eina_Bool *allowed = data;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s\n", errname, errmsg);
     }

   if (!eldbus_message_arguments_get(msg, "b", allowed))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
     }

   elm_exit();
}

static Eina_Bool
_timeout_wait_change_visibility(void *data)
{
   E_Test_Case_Wait_Vis_Type *type = data;

   *type = E_TEST_CASE_WAIT_VIS_TYPE_TIMEOUT;
   vis_exit_timer = NULL;
   elm_exit();

   return ECORE_CALLBACK_DONE;
}

static void
_signal_cb_change_visibility(void *data, const Eldbus_Message *msg)
{
   const char *errname, *errmsg;
   E_Test_Case_Wait_Vis_Type *type = data;
   int vis = 0;
   Ecore_Window id;

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
     }

   if (!eldbus_message_arguments_get(msg, "ub", &id, &vis))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
     }

   if ((*type == E_TEST_CASE_WAIT_VIS_TYPE_CHANGED) ||
       ((*type == E_TEST_CASE_WAIT_VIS_TYPE_ON) && (vis)) ||
       ((*type == E_TEST_CASE_WAIT_VIS_TYPE_OFF) && (!vis)))
     elm_exit();
}

static void
_signal_cb_restack(void *data EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *errname, *errmsg;
   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
     }

   elm_exit();
}

EAPI void
e_test_case_util_get_clients(E_TC_Data *tc_data)
{
   if (!eldbus_proxy_call(dbus_proxy, "GetClients",
                          _method_cb_get_clients, tc_data, -1, ""))
     return;

   elm_run();
}

EAPI Eina_Bool
e_test_case_util_register_window(Ecore_Window win)
{
   Eina_Bool accepted = EINA_FALSE;

   if (!eldbus_proxy_call(dbus_proxy,
                          "RegisterWindow", _method_cb_register_window,
                          &accepted, -1, "u", win))
     return EINA_FALSE;

   elm_run();

   return accepted;
}

EAPI Eina_Bool
e_test_case_util_deregister_window(Ecore_Window win)
{
   Eina_Bool allowed = EINA_FALSE;

   if (!eldbus_proxy_call(dbus_proxy,
                          "DeregisterWindow", _method_cb_deregister_window,
                          &allowed, -1, "u", win))
     return EINA_FALSE;

   elm_run();

   return allowed;
}

EAPI Eina_Bool
e_test_case_util_wait_visibility_change(E_Test_Case_Wait_Vis_Type expect)
{
   Eldbus_Signal_Handler *sh;
   E_Test_Case_Wait_Vis_Type type;

   type = expect;
   if (!(sh = eldbus_proxy_signal_handler_add(dbus_proxy,
                                              "ChangeVisibility",
                                              _signal_cb_change_visibility,
                                              &type)))
     return EINA_FALSE;

   vis_exit_timer = ecore_timer_add(2.0, _timeout_wait_change_visibility, &type);
   elm_run();

   if (vis_exit_timer)
     {
        ecore_timer_del(vis_exit_timer);
        vis_exit_timer = NULL;
     }
   eldbus_signal_handler_del(sh);

   return type == expect;
}

EAPI Eina_Bool
e_test_case_util_change_stack(Ecore_Window target, Ecore_Window pic, int type)
{
   if (eldbus_proxy_call(dbus_proxy, "ChangeStack", NULL,
                         NULL, -1, "uiu", target, type, pic))
     return EINA_FALSE;

   return EINA_TRUE;
}

EAPI Eina_Bool
e_test_case_util_wait_restack(void)
{
   Eldbus_Signal_Handler *sh;

   if (!(sh = eldbus_proxy_signal_handler_add(dbus_proxy,
                                              "Restack",
                                              _signal_cb_restack,
                                              NULL)))
     return EINA_FALSE;

   elm_run();

   eldbus_signal_handler_del(sh);
   sh = NULL;

   return EINA_TRUE;
}
