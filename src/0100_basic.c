#include "e_test_case_main.h"
#include "0100_basic.h"

E_TC_Data *_tc_data;

static E_Test_Case *_test_case = NULL;
static void test(void);

/* Method callbacks */
static void
_method_cb_register_window(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   Eina_Bool accepted = EINA_FALSE;
   const char *errname, *errmsg;

   EINA_SAFETY_ON_NULL_RETURN(_tc_data);

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

   if (!accepted)
     {
        if ((_tc_data) && (_tc_data->client))
          ERR("WM rejected the request for register!!(0x%08x)\n", _tc_data->client->win);

        elm_exit();
     }
   else if ((_tc_data->client) && (_tc_data->client->obj))
     {
        evas_object_move(_tc_data->client->obj,
                         _tc_data->client->x, _tc_data->client->y);
        evas_object_resize(_tc_data->client->obj,
                           _tc_data->client->w, _tc_data->client->h);
        evas_object_show(_tc_data->client->obj);
     }
}

static void
_method_cb_deregister_window(void *data EINA_UNUSED, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   const char *errname, *errmsg;
   Eina_Bool allowed = EINA_TRUE;

   EINA_SAFETY_ON_NULL_RETURN(_tc_data);

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s\n", errname, errmsg);
        return;
     }

   if (!eldbus_message_arguments_get(msg, "b", &allowed))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
        return;
     }

   if (!allowed)
     {
        _tc_data->wait_close = EINA_TRUE;
        return;
     }

   elm_exit();
}

/* Signal Callbacks */
static void
_signal_cb_change_visibility(void *data EINA_UNUSED, const Eldbus_Message *msg)
{
   const char *errname, *errmsg;
   Eina_Bool vis;
   Ecore_Window id;

   EINA_SAFETY_ON_NULL_RETURN(_tc_data);
   EINA_SAFETY_ON_NULL_RETURN(_tc_data->client);

   if (eldbus_message_error_get(msg, &errname, &errmsg))
     {
        ERR("%s %s", errname, errmsg);
     }

   if (!eldbus_message_arguments_get(msg, "ub", &id, &vis))
     {
        ERR("Error on eldbus_message_arguments_get()\n");
     }

   if (_tc_data->client->win != id) return;

   _tc_data->client->visible = vis;

   if (vis) elm_exit();
   if ((_tc_data->wait_close) && (!vis)) elm_exit();
}

/* Main test */
static void
test(void)
{
   _test_case->passed = e_test_case_inner_do(_test_case);

   //request for quitting this test
   eldbus_proxy_call(dbus_proxy,
                     "DeregisterWindow", _method_cb_deregister_window,
                     NULL, -1, "u", _tc_data->client->win);
   elm_run();

}

Eina_Bool
test_case_0100_basic(E_Test_Case *tc)
{
   Evas_Object *bg, *win;
   E_TC_Client client = { NULL, // evas object
                          "0100_basic", // name
                          0, 0, 320, 320, // geometry(x,y,w,h)
                          200, //E_LAYER_CLIENT_NORMAL, //layer
                          -1, -1, 0 // visible, arg, win
   };

   if (!tc) goto test_shutdown;

   _test_case = tc;

   if (_tc_data) E_FREE(_tc_data);
   _tc_data = E_NEW(E_TC_Data, 1);

   win = elm_win_add(NULL, client.name, ELM_WIN_BASIC);
   client.win = elm_win_xwindow_get(win);

   if (!eldbus_proxy_call(dbus_proxy,
                          "RegisterWindow", _method_cb_register_window,
                          NULL, -1, "u", client.win))
     goto test_shutdown;


   _tc_data->sig_hdlrs =
      eina_list_append(_tc_data->sig_hdlrs,
                       eldbus_proxy_signal_handler_add(dbus_proxy,
                                                       "ChangeVisibility",
                                                       _signal_cb_change_visibility,
                                                       NULL));

   elm_win_title_set(win, client.name);
   elm_win_autodel_set(win, EINA_FALSE);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   elm_bg_color_set(bg, 0, 120, 100);
   evas_object_show(bg);

   client.obj = win;

   _tc_data->client = E_NEW(E_TC_Client, 1);
   memcpy(_tc_data->client, &client, sizeof(E_TC_Client));

   elm_run();

   if (_tc_data->client->visible) test();

test_shutdown:
   if (_tc_data)
     {
        E_TC_Client *_client;

        if (_tc_data->client)
          {
             evas_object_hide(_tc_data->client->obj);
             evas_object_del(_tc_data->client->obj);
             _tc_data->client->obj = NULL;
             E_FREE(_tc_data->client);
          }

        EINA_LIST_FREE(_tc_data->clients, _client)
          {
             eina_stringshare_del(_client->name);
             E_FREE(_client);
             _client = NULL;
          }

        E_FREE_LIST(_tc_data->sig_hdlrs, eldbus_signal_handler_del);

        E_FREE(_tc_data);
     }

   return _test_case->passed;
}
