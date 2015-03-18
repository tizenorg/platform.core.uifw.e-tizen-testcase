#include "e_test_case_main.h"
#include "e_test_case_util.h"

static E_Test_Case *_test_case = NULL;

E_TC_Data *_tc_data = NULL;
E_TC_Client *_parent, *_child;

static void test(void);

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

   evas_object_hide(_parent->obj);
   evas_object_hide(_child->obj);
   evas_object_hide(_tc_data->client->obj);

   if (e_test_case_util_deregister_window(_tc_data->client->win))
     return;

   if (e_test_case_util_wait_visibility_change(E_TEST_CASE_WAIT_VIS_TYPE_OFF))
     return;
}

Eina_Bool
test_case_0110_transient_for(E_Test_Case *tc)
{
   Evas_Object *bg, *win;
   E_TC_Client client = { NULL, // evas object
                          "0110_transient_for", // name
                          0, 0, 320, 320, // geometry(x,y,w,h)
                          200, //E_LAYER_CLIENT_NORMAL, //layer
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

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   elm_bg_color_set(bg, 120,0, 0);
   evas_object_show(bg);

   client.obj = win;

   _tc_data->client = E_NEW(E_TC_Client, 1);
   memcpy(_tc_data->client, &client, sizeof(E_TC_Client));

   if (e_test_case_util_register_window(client.win))
     {
        evas_object_move(_parent->obj, 0, 0);
        evas_object_resize(_parent->obj, 400, 400);
        evas_object_show(_parent->obj);

        evas_object_move(win, client.x, client.y);
        evas_object_resize(win, client.w, client.h);
        evas_object_show(win);

        evas_object_move(_child->obj, 0, 0);
        evas_object_resize(_child->obj, 200, 200);
        evas_object_show(_child->obj);
     }
   else
     goto test_shutdown;

   if (e_test_case_util_wait_visibility_change(E_TEST_CASE_WAIT_VIS_TYPE_CHANGED))
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

        if (_parent)
          {
             evas_object_del(_parent->obj);
             _parent->obj = NULL;
             E_FREE(_parent);
          }

        if (_child)
          {
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

        E_FREE(_tc_data);
     }

   return _test_case->passed;
}

Eina_Bool
test_case_0111_transient_for_raise(E_Test_Case *tc)
{
   E_TC_Client *client = NULL, *above = NULL;
   Eina_List *l;
   Eina_Bool passed = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);

   if (!e_test_case_util_change_stack(_parent->win, 0, E_TEST_CASE_STACK_TYPE_RAISE))
     return EINA_FALSE;

   if (!e_test_case_util_wait_restack())
     return EINA_FALSE;

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
        else if (client->win != _child->win)
          break;

        above = client;
     }

   return passed;

}

Eina_Bool
test_case_0112_transient_for_lower(E_Test_Case *tc)
{
   E_TC_Client *client = NULL, *above = NULL;
   Eina_List *l;
   Eina_Bool passed = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);

   if (!e_test_case_util_change_stack(_parent->win, 0, E_TEST_CASE_STACK_TYPE_LOWER))
     return EINA_FALSE;

   if (!e_test_case_util_wait_restack())
     return EINA_FALSE;

   e_test_case_util_get_clients(_tc_data);
   EINA_LIST_FOREACH(_tc_data->clients, l, client)
     {
        if (client->layer > _tc_data->client->layer)
          continue;
        if (client->layer < _tc_data->client->layer)
          break;

        if ((passed) && (above) && (above->win == _parent->win))
          {
             passed = EINA_FALSE;
             break;
          }

        if (client->win == _parent->win)
          {
             if ((above) && (above->win == _child->win))
               passed = EINA_TRUE;
          }

        above = client;
     }

   return passed;

}

Eina_Bool
test_case_0113_transient_for_stack_above(E_Test_Case *tc)
{
   E_TC_Client *client = NULL, *above = NULL;
   Eina_List *l;
   Eina_Bool passed = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);

   if (!e_test_case_util_change_stack(_parent->win, _tc_data->client->win, E_TEST_CASE_STACK_TYPE_ABOVE))
     return EINA_FALSE;

   if (!e_test_case_util_wait_restack())
     return EINA_FALSE;

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

   return passed;

}

Eina_Bool
test_case_0114_transient_for_stack_below(E_Test_Case *tc)
{
   E_TC_Client *client = NULL, *above = NULL;
   Eina_List *l;
   Eina_Bool passed = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(_tc_data, EINA_FALSE);

   if (!e_test_case_util_change_stack(_parent->win, _tc_data->client->win, E_TEST_CASE_STACK_TYPE_BELOW))
     return EINA_FALSE;

   if (!e_test_case_util_wait_restack())
     return EINA_FALSE;

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
        else if (client->win == _child->win)
          {
             if ((!above) || (above->win != _tc_data->client->win))
               {
                  passed = EINA_FALSE;
                  break;
               }
          }


        above = client;
     }

   return passed;

}
