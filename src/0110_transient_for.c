#include "e.h"
#include "e_test_case_main.h"
#include "e_test_case_util.h"
#include "0110_transient_for.h"

static E_Test_Case *_test_case = NULL;

E_TC_Data *_tc_data = NULL;
E_TC_Client *_parent, *_child;

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
        evas_object_move(_parent->obj, 0, 0);
        evas_object_resize(_parent->obj, 400, 400);
        evas_object_show(_parent->obj);

        evas_object_move(_tc_data->client->obj,
                         _tc_data->client->x, _tc_data->client->y);
        evas_object_resize(_tc_data->client->obj,
                           _tc_data->client->w, _tc_data->client->h);
        evas_object_show(_tc_data->client->obj);

        evas_object_move(_child->obj, 0, 0);
        evas_object_resize(_child->obj, 200, 200);
        evas_object_show(_child->obj);
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
   E_TC_Client *client = NULL, *above = NULL;
   Eina_List *l;
   Eina_Bool passed = EINA_FALSE;

   e_test_case_util_get_clients(_tc_data);
   EINA_LIST_FOREACH(_tc_data->clients, l, client)
     {
        if (client->layer > _tc_data->client->layer)
          continue;
        if (client->layer < _tc_data->client->layer)
          break;

        if (client->win == _parent->win)
          {
             if ((above) && (above->win == _child->win))
               passed = EINA_TRUE;

             break;
          }

        above = client;
     }

   _test_case->passed = passed && e_test_case_inner_do(_test_case);

   //request for quitting this test
   if (eldbus_proxy_call(dbus_proxy,
                         "DeregisterWindow", _method_cb_deregister_window,
                         NULL, -1, "u", _tc_data->client->win))
     elm_run();
}

Eina_Bool
test_case_0110_transient_for(E_Test_Case *tc)
{
   Evas_Object *bg, *win;
   E_TC_Client client = { NULL, // evas object
                          "0110_transient_for", // name
                          0, 0, 320, 320, // geometry(x,y,w,h)
                          E_LAYER_CLIENT_NORMAL, //layer
                          -1, -1, 0 // visible, arg, win
   };

   if (!tc) goto test_shutdown;

   _test_case = tc;

   if (_tc_data) E_FREE(_tc_data);
   _tc_data = E_NEW(E_TC_Data, 1);

   _parent = E_NEW(E_TC_Client, 1);
   _parent->obj = elm_win_add(NULL, "parent", ELM_WIN_BASIC);
   _parent->win = elm_win_xwindow_get(_parent->obj);
   elm_win_title_set(_parent->obj, "parent");
   elm_win_autodel_set(_parent->obj, EINA_FALSE);

   bg = elm_bg_add(_parent->obj);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(_parent->obj, bg);
   elm_bg_color_set(bg, 0, 120, 100);
   evas_object_show(bg);

   _child = E_NEW(E_TC_Client, 1);
   _child->obj = elm_win_add(_parent->obj, "child", ELM_WIN_BASIC);
   _child->win = elm_win_xwindow_get(_child->obj);
   elm_win_title_set(_child->obj, "child");
   elm_win_autodel_set(_child->obj, EINA_FALSE);

   bg = elm_bg_add(_child->obj);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(_child->obj, bg);
   elm_bg_color_set(bg, 0, 0, 150);
   evas_object_show(bg);

   win = elm_win_add(NULL, client.name, ELM_WIN_BASIC);
   client.win = elm_win_xwindow_get(win);
   elm_win_title_set(win, client.name);
   elm_win_autodel_set(win, EINA_FALSE);

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

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   elm_bg_color_set(bg, 120,0, 0);
   evas_object_show(bg);

   client.obj = win;

   _tc_data->client = E_NEW(E_TC_Client, 1);
   memcpy(_tc_data->client, &client, sizeof(E_TC_Client));

   elm_run();

   test();

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

        if (_parent)
          {
             evas_object_hide(_parent->obj);
             evas_object_del(_parent->obj);
             _parent->obj = NULL;
             E_FREE(_parent);
          }

        if (_child)
          {
             evas_object_hide(_child->obj);
             evas_object_del(_child->obj);
             _child->obj = NULL;
             E_FREE(_child);
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
