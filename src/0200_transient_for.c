#include "e_test_runner.h"

struct _E_TC_Data
{
   E_TC_Win *tw;
   E_TC_Win *tw_parent;
   E_TC_Win *tw_child;
};

static Eina_Bool _tc_pre_run(E_TC *tc);
static Eina_Bool _tc_post_run(E_TC *tc);
static void      _tc_shutdown(E_TC *tc);

static Eina_Bool
_tc_pre_run(E_TC *tc)
{
   Eina_Bool res;

   tc->data = E_NEW(E_TC_Data, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc->data, EINA_FALSE);

   tc->data->tw_parent = e_tc_win_add(NULL, ELM_WIN_BASIC,
                                      EINA_FALSE, "parent",
                                      0, 0, 400, 400,
                                      200);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_parent, cleanup);

   tc->data->tw_child = e_tc_win_add(tc->data->tw_parent, ELM_WIN_BASIC,
                                     EINA_FALSE, "child",
                                     0, 0, 320, 320,
                                     200);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw_child, cleanup);

   tc->data->tw = e_tc_win_add(NULL, ELM_WIN_BASIC,
                               EINA_FALSE, "tc",
                               0, 0, 200, 200,
                               200);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw, cleanup);

   res = e_test_runner_req_win_register(tc->runner, tc->data->tw);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_tc_win_geom_update(tc->data->tw_parent);
   e_tc_win_show(tc->data->tw_parent);

   e_tc_win_geom_update(tc->data->tw);
   e_tc_win_show(tc->data->tw);

   e_tc_win_geom_update(tc->data->tw_child);
   e_tc_win_show(tc->data->tw_child);

   res = e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_VIS_ON);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   return EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   return EINA_FALSE;
}

static Eina_Bool
_tc_post_run(E_TC *tc)
{
   e_tc_win_hide(tc->data->tw_child);
   e_tc_win_hide(tc->data->tw);
   e_tc_win_hide(tc->data->tw_parent);

   return EINA_TRUE;
}

static void
_tc_shutdown(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   e_test_runner_req_win_deregister(tc->runner, tc->data->tw);
   e_tc_win_del(tc->data->tw_parent);
   e_tc_win_del(tc->data->tw);
   e_tc_win_del(tc->data->tw_child);

   E_FREE(tc->data);
}

Eina_Bool
tc_0200_transient_for_basic(E_TC *tc)
{
   E_TC_Win *tw, *tw2, *tw_parent, *tw_child;
   E_TC_Win *tw_above = NULL;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   tw = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   EINA_LIST_FOREACH(list, l, tw2)
     {
        if (tw2->layer > tw->layer) continue;
        if (tw2->layer < tw->layer) break;

        if (tw2->native_win == tw_parent->native_win)
          {
             if ((tw_above) &&
                 (tw_above->native_win == tw_child->native_win))
               res = EINA_TRUE;
             break;
          }

        tw_above = tw2;
     }

   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0201_transient_for_raise(E_TC *tc)
{
   E_TC_Win *tw, *tw2;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;

   e_tc_win_stack_change(tw, NULL, EINA_TRUE);

   res = e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_RAISE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   tw2 = eina_list_nth(list, 0);
   EINA_SAFETY_ON_NULL_GOTO(tw2, cleanup);
   EINA_SAFETY_ON_FALSE_GOTO(tw2->native_win == tw->native_win, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0202_transient_for_lower(E_TC *tc)
{
   E_TC_Win *tw, *tw2;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;

   e_tc_win_stack_change(tw, NULL, EINA_FALSE);

   res = e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_LOWER);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   tw2 = eina_list_last_data_get(list);
   EINA_SAFETY_ON_NULL_GOTO(tw2, cleanup);
   EINA_SAFETY_ON_FALSE_GOTO(tw2->native_win == tw->native_win, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0203_transient_for_stack_above(E_TC *tc)
{
   E_TC_Win *tw, *tw2, *tw_parent, *tw_child;
   E_TC_Win *tw_above = NULL;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   e_tc_win_stack_change(tw_parent, tw, EINA_TRUE);

   res = e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_ABOVE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw2)
     {
        if (tw2->layer > tw->layer) continue;
        if (tw2->layer < tw->layer) break;

        if (tw2->native_win == tw_parent->native_win)
          {
             if ((tw_above) &&
                 (tw_above->native_win == tw_child->native_win))
               res = EINA_TRUE;
             break;
          }

        tw_above = tw2;
     }

   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0204_transient_for_stack_below(E_TC *tc)
{
   E_TC_Win *tw, *tw2, *tw_parent, *tw_child;
   E_TC_Win *tw_above = NULL;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   e_tc_win_stack_change(tw_parent, tw, EINA_FALSE);

   res = e_test_runner_ev_wait(tc->runner, E_TC_EVENT_TYPE_STACK_BELOW);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw2)
     {
        if (tw2->layer > tw->layer) continue;
        if (tw2->layer < tw->layer) break;

        if (tw2->native_win == tw_parent->native_win)
          {
             if ((tw_above) &&
                 (tw_above->native_win == tw_child->native_win))
               res = EINA_TRUE;
             break;
          }
        else if (tw2->native_win == tw_child->native_win)
          {
             if ((!tw_above) ||
                 (tw_above->native_win != tw->native_win))
               {
                  res = EINA_FALSE;
                  break;
               }
          }

        tw_above = tw2;
     }

   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(list, e_tc_win_del);

   return tc->passed;
}