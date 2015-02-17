#include "e.h"
#include "e_test_case_main.h"
#include "e_test_case_util.h"

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
        return;
     }

   if (!eldbus_message_arguments_get(msg, "ua(usiiiiibb)", &target_win, &array_of_ec))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
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


EAPI void
e_test_case_util_get_clients(E_TC_Data *tc_data)
{
   eldbus_proxy_call(dbus_proxy, "GetClients",
                    _method_cb_get_clients, tc_data, -1, "");

   elm_run();
}
