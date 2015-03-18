#include "e_test_case_main.h"
#include "e_test_case_util.h"

E_TC_Data *_tc_data;

static E_Test_Case *_test_case = NULL;
static void test(void);

/* Main test */
static void
test(void)
{
   _test_case->passed = e_test_case_inner_do(_test_case);

   evas_object_hide(_tc_data->client->obj);

   if (e_test_case_util_deregister_window(_tc_data->client->win))
     return;

   if (e_test_case_util_wait_visibility_change(E_TEST_CASE_WAIT_VIS_TYPE_OFF))
     return;
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

   if (e_test_case_util_register_window(client.win))
     {
        evas_object_move(win, client.x, client.y);
        evas_object_resize(win, client.w, client.h);
        evas_object_show(win);
     }
   else
     goto test_shutdown;

   if (e_test_case_util_wait_visibility_change(E_TEST_CASE_WAIT_VIS_TYPE_ON))
     test();

test_shutdown:
   if (_tc_data)
     {
        E_TC_Client *_client;

        if (_tc_data->client)
          {
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

        E_FREE(_tc_data);
     }

   return _test_case->passed;
}

Eina_Bool
test_case_0101_basic_stack(E_Test_Case *tc EINA_UNUSED)
{
   E_TC_Client *client;
   Eina_Bool passed = EINA_FALSE;
   Eina_List *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data->client, EINA_FALSE);

   e_test_case_util_get_clients(_tc_data);

   EINA_LIST_FOREACH(_tc_data->clients, l, client)
     {
        if (client->layer > _tc_data->client->layer)
          continue;
        if (client->layer < _tc_data->client->layer)
          break;

        if (!strncmp(client->name, _tc_data->client->name, strlen(client->name)))
          passed = EINA_TRUE;

        break;
     }

   return passed;
}
