#include <efl_util.h>
#include "e_tc_main.h"

#if 0
#define TC_NWIN_W   400
#define TC_NWIN_H   400

typedef struct
{
   Evas_Object *obj;
   Ecore_Window win;
} E_TC_Win;

static E_TC_Win *_tc_normal_win;
static E_TC_Win *_tc_noti_win1;
static E_TC_Win *_tc_noti_win2;

static Eina_Bool registered;
static Eina_Bool loop_running;

static Eina_Bool
_notification_level_windows_show(E_Test_Case *tc)
{
   Eldbus_Pending *ret;

   if (!(e_test_case_util_register_window(_tc_normal_win->win)))
     return EINA_FALSE;

   registered = EINA_TRUE;
   evas_object_show(_tc_normal_win->obj);
   evas_object_show(_tc_noti_win1->obj);
   evas_object_show(_tc_noti_win2->obj);

   if (!(e_test_case_util_wait_visibility_change(E_TEST_CASE_WAIT_VIS_TYPE_CHANGED)))
     return EINA_FALSE;

   return EINA_TRUE;
}

static void
_notification_level_windows_hide(E_Test_Case *tc)
{
   if (!registered) return;

   evas_object_hide(_tc_noti_win2->obj);
   evas_object_hide(_tc_noti_win1->obj);
   evas_object_hide(_tc_normal_win->obj);

   if (e_test_case_util_deregister_window(_tc_normal_win->win))
     return;

   if (e_test_case_util_wait_visibility_change(E_TEST_CASE_WAIT_VIS_TYPE_OFF))
     return;
}

static void
_notification_level_windows_destroy(E_Test_Case *tc)
{
   if (_tc_noti_win2)
     {
        evas_object_del(_tc_noti_win2->obj);
        E_FREE(_tc_noti_win2);
     }
   if (_tc_noti_win1)
     {
        evas_object_del(_tc_noti_win1->obj);
        E_FREE(_tc_noti_win1);
     }
   if (_tc_normal_win)
     {
        evas_object_del(_tc_normal_win->obj);
        E_FREE(_tc_normal_win);
     }
}

static Eina_Bool
_notification_level_windows_create(E_Test_Case *tc)
{
   if (!_tc_normal_win)
     {
        Evas_Object *bg;

        _tc_normal_win = E_NEW(E_TC_Win, 1);
        EINA_SAFETY_ON_NULL_GOTO(_tc_normal_win, create_fail);

        _tc_normal_win->obj = elm_win_add(NULL, "_tc_normal_win", ELM_WIN_BASIC);
        _tc_normal_win->win = elm_win_xwindow_get(_tc_normal_win->obj);
        elm_win_title_set(_tc_normal_win->obj, "_tc_normal_win");
        elm_win_autodel_set(_tc_normal_win->obj, EINA_FALSE);

        bg = elm_bg_add(_tc_normal_win->obj);
        evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(_tc_normal_win->obj, bg);
        elm_bg_color_set(bg, 0xff, 0, 0);
        evas_object_show(bg);
     }

   if (!_tc_noti_win1)
     {
        Evas_Object *bg;

        _tc_noti_win1 = E_NEW(E_TC_Win, 1);
        EINA_SAFETY_ON_NULL_GOTO(_tc_noti_win1, create_fail);

        _tc_noti_win1->obj = elm_win_add(NULL, "_tc_noti_win1", ELM_WIN_NOTIFICATION);
        _tc_noti_win1->win = elm_win_xwindow_get(_tc_noti_win1->obj);
        elm_win_title_set(_tc_noti_win1->obj, "_tc_noti_win1");
        elm_win_autodel_set(_tc_noti_win1->obj, EINA_FALSE);

        bg = elm_bg_add(_tc_noti_win1->obj);
        evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(_tc_noti_win1->obj, bg);
        elm_bg_color_set(bg, 0, 0, 0xff);
        evas_object_show(bg);
     }

   if (!_tc_noti_win2)
     {
        Evas_Object *bg;

        _tc_noti_win2 = E_NEW(E_TC_Win, 1);
        EINA_SAFETY_ON_NULL_GOTO(_tc_noti_win2, create_fail);

        _tc_noti_win2->obj = elm_win_add(NULL, "_tc_noti_win2", ELM_WIN_NOTIFICATION);
        _tc_noti_win2->win = elm_win_xwindow_get(_tc_noti_win2->obj);
        elm_win_title_set(_tc_noti_win2->obj, "_tc_noti_win2");
        elm_win_autodel_set(_tc_noti_win2->obj, EINA_FALSE);

        bg = elm_bg_add(_tc_noti_win2->obj);
        evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(_tc_noti_win2->obj, bg);
        elm_bg_color_set(bg, 0, 0, 0xff);
        evas_object_show(bg);
     }

   return EINA_TRUE;

create_fail:
   _notification_level_windows_destroy(tc);

   return EINA_FALSE;
}

static void
_notification_level_tc_finish(E_Test_Case *tc)
{
   _notification_level_windows_destroy(tc);

   registered = EINA_FALSE;
}

static Eina_Bool
_notification_level_tc_prepare(E_Test_Case *tc)
{
   if (!_notification_level_windows_create(tc)) return EINA_FALSE;

   registered = EINA_FALSE;

   return EINA_TRUE;
}

static Eina_Bool
_notification_level_check_stack(E_Test_Case *tc,
                                E_TC_Win *bottom,
                                E_TC_Win *middle,
                                E_TC_Win *top)
{
   E_TC_Data tc_data = {0,};
   E_TC_Client *client = NULL;
   Eina_List *l;
   int t_layer = 0, m_layer = 0, b_layer = 0;

   e_test_case_util_get_clients(&tc_data);

   EINA_LIST_FOREACH(tc_data.clients, l, client)
     {
        if (top->win == client->win)
        {
           t_layer = client->layer;
           continue;
        }
        else if (middle->win == client->win)
        {
           m_layer = client->layer;
           continue;
        }
        else if (bottom->win == client->win)
        {
           b_layer = client->layer;
           continue;
        }
     }

   if (b_layer < m_layer && m_layer < t_layer)
      return EINA_TRUE;

   return EINA_FALSE;
}

Eina_Bool
test_case_0120_notification_level(E_Test_Case *tc)
{
   Eina_Bool passed = EINA_TRUE;

   passed = passed && e_test_case_inner_do(tc);

   return passed;
}

Eina_Bool
test_case_0121_notification_level_1(E_Test_Case *tc)
{
   efl_util_notification_level_e level = -1;
   int ret;
   Eina_Bool result = EINA_FALSE;

   EINA_SAFETY_ON_NULL_GOTO(tc, test_shutdown);

   if (!_notification_level_tc_prepare(tc)) goto test_shutdown;

   ret = efl_util_set_notification_window_level(_tc_noti_win1->obj,
                                                EFL_UTIL_NOTIFICATION_LEVEL_1);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);

   ret = efl_util_get_notification_window_level(_tc_noti_win1->obj, &level);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);
   EINA_SAFETY_ON_FALSE_GOTO(level == EFL_UTIL_NOTIFICATION_LEVEL_1, test_shutdown);

   if (!_notification_level_windows_show(tc)) goto test_shutdown;

   result = _notification_level_check_stack (tc, _tc_normal_win, _tc_noti_win2, _tc_noti_win1);
   EINA_SAFETY_ON_FALSE_GOTO(result, test_shutdown);

test_shutdown:
   _notification_level_windows_hide(tc);
   _notification_level_tc_finish(tc);

   return result;
}

Eina_Bool
test_case_0122_notification_level_2(E_Test_Case *tc)
{
   efl_util_notification_level_e level = -1;
   int ret;
   Eina_Bool result = EINA_FALSE;

   EINA_SAFETY_ON_NULL_GOTO(tc, test_shutdown);

   if (!_notification_level_tc_prepare(tc)) goto test_shutdown;

   ret = efl_util_set_notification_window_level(_tc_noti_win1->obj,
                                                EFL_UTIL_NOTIFICATION_LEVEL_2);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);

   ret = efl_util_get_notification_window_level(_tc_noti_win1->obj, &level);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);
   EINA_SAFETY_ON_FALSE_GOTO(level == EFL_UTIL_NOTIFICATION_LEVEL_2, test_shutdown);

   if (!_notification_level_windows_show(tc)) goto test_shutdown;

   result = _notification_level_check_stack (tc, _tc_normal_win, _tc_noti_win2, _tc_noti_win1);
   EINA_SAFETY_ON_FALSE_GOTO(result, test_shutdown);

test_shutdown:
   _notification_level_windows_hide(tc);
   _notification_level_tc_finish(tc);

   return result;
}

Eina_Bool
test_case_0123_notification_level_3(E_Test_Case *tc)
{
   efl_util_notification_level_e level = -1;
   int ret;
   Eina_Bool result = EINA_FALSE;

   EINA_SAFETY_ON_NULL_GOTO(tc, test_shutdown);

   if (!_notification_level_tc_prepare(tc)) goto test_shutdown;

   ret = efl_util_set_notification_window_level(_tc_noti_win1->obj,
                                                EFL_UTIL_NOTIFICATION_LEVEL_3);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);

   ret = efl_util_get_notification_window_level(_tc_noti_win1->obj, &level);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);
   EINA_SAFETY_ON_FALSE_GOTO(level == EFL_UTIL_NOTIFICATION_LEVEL_3, test_shutdown);

   if (!_notification_level_windows_show(tc)) goto test_shutdown;

   result = _notification_level_check_stack (tc, _tc_normal_win, _tc_noti_win2, _tc_noti_win1);
   EINA_SAFETY_ON_FALSE_GOTO(result, test_shutdown);

test_shutdown:
   _notification_level_windows_hide(tc);
   _notification_level_tc_finish(tc);

   return result;
}

Eina_Bool
test_case_0124_notification_level_change(E_Test_Case *tc)
{
   int ret;
   Eina_Bool result = EINA_FALSE;

   EINA_SAFETY_ON_NULL_GOTO(tc, test_shutdown);

   if (!_notification_level_tc_prepare(tc)) goto test_shutdown;

   ret = efl_util_set_notification_window_level(_tc_noti_win2->obj,
                                                EFL_UTIL_NOTIFICATION_LEVEL_2);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);

   ret = efl_util_set_notification_window_level(_tc_noti_win1->obj,
                                                EFL_UTIL_NOTIFICATION_LEVEL_3);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);

   if (!_notification_level_windows_show(tc)) goto test_shutdown;

   result = _notification_level_check_stack (tc, _tc_normal_win, _tc_noti_win2, _tc_noti_win1);
   EINA_SAFETY_ON_FALSE_GOTO(result, test_shutdown);

   ret = efl_util_set_notification_window_level(_tc_noti_win1->obj,
                                                EFL_UTIL_NOTIFICATION_LEVEL_1);
   EINA_SAFETY_ON_FALSE_GOTO(ret == EFL_UTIL_ERROR_NONE, test_shutdown);

   result = _notification_level_check_stack (tc, _tc_normal_win, _tc_noti_win1, _tc_noti_win2);
   EINA_SAFETY_ON_FALSE_GOTO(result, test_shutdown);

test_shutdown:
   _notification_level_windows_hide(tc);
   _notification_level_tc_finish(tc);

   return result;
}
#endif
