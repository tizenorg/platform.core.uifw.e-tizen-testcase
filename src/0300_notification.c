#include <efl_util.h>
#include "e_test_runner.h"

#define TW_W   400
#define TW_H   400

struct _E_TC_Data
{
   E_TC_Win *tw_normal;
   E_TC_Win *tw_noti1;
   E_TC_Win *tw_noti2;
};

static void
_tc_shutdown(E_TC *tc)
{
   E_TC_Data *data = tc->data;
   EINA_SAFETY_ON_NULL_RETURN(data);

   e_test_runner_req_win_deregister(tc->runner, data->tw_normal);
   e_tc_win_del(data->tw_normal);
   e_tc_win_del(data->tw_noti1);
   e_tc_win_del(data->tw_noti2);

   E_FREE(data);
   tc->data = NULL;
}

static void
_tc_post_run(E_TC *tc)
{
   E_TC_Data *data = tc->data;
   EINA_SAFETY_ON_NULL_RETURN(data);

   e_tc_win_hide(data->tw_normal);
   e_tc_win_hide(data->tw_noti1);
   e_tc_win_hide(data->tw_noti2);
}

static Eina_Bool
_tc_pre_run(E_TC *tc)
{
   Eina_Bool res;
   E_TC_Data *data = NULL;

   data = E_NEW(E_TC_Data, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_FALSE);
   tc->data = data;

   data->tw_normal = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                  EINA_FALSE, "tw_normal",
                                  0, 0, TW_W, TW_H, EINA_TRUE,
                                  200, E_TC_WIN_COLOR_RED);
   EINA_SAFETY_ON_NULL_GOTO(data->tw_normal, cleanup);

   data->tw_noti1 = e_tc_win_add(NULL, ELM_WIN_NOTIFICATION,
                                 EINA_FALSE, "tw_noti1",
                                 0, 0, TW_W, TW_H, EINA_TRUE,
                                 200, E_TC_WIN_COLOR_GREEN);
   EINA_SAFETY_ON_NULL_GOTO(data->tw_noti1, cleanup);

   data->tw_noti2 = e_tc_win_add(NULL, ELM_WIN_NOTIFICATION,
                                 EINA_FALSE, "tw_noti2",
                                 0, 0, TW_W, TW_H, EINA_TRUE,
                                 200, E_TC_WIN_COLOR_BLUE);
   EINA_SAFETY_ON_NULL_GOTO(data->tw_noti2, cleanup);

   res = e_test_runner_req_win_register(tc->runner, data->tw_normal);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_tc_win_geom_update(data->tw_normal);
   e_tc_win_geom_update(data->tw_noti1);
   e_tc_win_geom_update(data->tw_noti2);

   e_tc_win_show(data->tw_normal);
   e_tc_win_show(data->tw_noti1);
   e_tc_win_show(data->tw_noti2);

   res = e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_CHANGED);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_test_runner_work();

   return EINA_TRUE;

cleanup:
   _tc_post_run(tc);
   _tc_shutdown(tc);

   return EINA_FALSE;
}

static Eina_Bool
_tc_noti_level_set(E_TC_Win *tw, efl_util_notification_level_e level)
{
   int ret;

   ret = efl_util_set_notification_window_level(tw->elm_win, level);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(ret == EFL_UTIL_ERROR_NONE, EINA_FALSE);

   return EINA_TRUE;
}

static Eina_Bool
_tc_noti_level_check(E_TC_Win *tw, efl_util_notification_level_e level)
{
   efl_util_notification_level_e value = -1;
   int ret;

   ret = efl_util_get_notification_window_level(tw->elm_win, &value);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(ret == EFL_UTIL_ERROR_NONE, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(value == level, EINA_FALSE);

   return EINA_TRUE;
}

static Eina_Bool
_tc_check_stack(E_TC *tc, E_TC_Win *bottom, E_TC_Win *middle, E_TC_Win *top)
{
   Eina_List *list = NULL, *l;
   int b_layer = 0, m_layer = 0, t_layer = 0;
   E_TC_Win *tw;

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, failed);

   EINA_LIST_FOREACH(list, l, tw)
     {
        if (top->elm_win == tw->elm_win)
        {
           t_layer = tw->layer;
           continue;
        }
        else if (middle->elm_win == tw->elm_win)
        {
           m_layer = tw->layer;
           continue;
        }
        else if (bottom->elm_win == tw->elm_win)
        {
           b_layer = tw->layer;
           continue;
        }
     }

   EINA_SAFETY_ON_FALSE_GOTO(b_layer <= m_layer, failed);
   EINA_SAFETY_ON_FALSE_GOTO(m_layer <= t_layer, failed);

   E_FREE_LIST(list, e_tc_win_del);
   return EINA_TRUE;

failed:
   E_FREE_LIST(list, e_tc_win_del);
   return EINA_FALSE;
}

Eina_Bool
tc_0300_notification_level_1(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;
   E_TC_Data *data;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);
   data = tc->data;

   res = _tc_noti_level_set(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_noti_level_check(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_check_stack(tc, data->tw_normal, data->tw_noti2, data->tw_noti1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

cleanup:
   _tc_post_run(tc);
   _tc_shutdown(tc);

   return res;
}

Eina_Bool
tc_0301_notification_level_2(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;
   E_TC_Data *data;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);
   data = tc->data;

   res = _tc_noti_level_set(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_2);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_noti_level_check(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_2);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_check_stack(tc, data->tw_normal, data->tw_noti2, data->tw_noti1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

cleanup:
   _tc_post_run(tc);
   _tc_shutdown(tc);

   return res;
}

Eina_Bool
tc_0302_notification_level_3(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;
   E_TC_Data *data;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);
   data = tc->data;

   res = _tc_noti_level_set(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_3);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_noti_level_check(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_3);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_check_stack(tc, data->tw_normal, data->tw_noti2, data->tw_noti1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

cleanup:
   _tc_post_run(tc);
   _tc_shutdown(tc);

   return res;
}

Eina_Bool
tc_0303_notification_level_change(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;
   E_TC_Data *data;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);
   data = tc->data;

   res = _tc_noti_level_set(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_3);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_noti_level_set(data->tw_noti2, EFL_UTIL_NOTIFICATION_LEVEL_2);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_check_stack(tc, data->tw_normal, data->tw_noti2, data->tw_noti1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_noti_level_set(data->tw_noti1, EFL_UTIL_NOTIFICATION_LEVEL_1);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_check_stack(tc, data->tw_normal, data->tw_noti1, data->tw_noti2);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

cleanup:
   _tc_post_run(tc);
   _tc_shutdown(tc);

   return res;
}
