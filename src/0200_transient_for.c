#include "e_tc_main.h"

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

   res = e_tc_win_register(tc->data->tw);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_tc_win_geom_update(tc->data->tw_parent);
   e_tc_win_show(tc->data->tw_parent);

   e_tc_win_geom_update(tc->data->tw);
   e_tc_win_show(tc->data->tw);

   e_tc_win_geom_update(tc->data->tw_child);
   e_tc_win_show(tc->data->tw_child);

   res = e_tc_ev_wait(E_TC_EVENT_TYPE_VIS_ON);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   return EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   return EINA_FALSE;
}

static Eina_Bool
_tc_post_run(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;

   e_tc_win_hide(tc->data->tw_child);
   e_tc_win_hide(tc->data->tw);
   e_tc_win_hide(tc->data->tw_parent);

   res = e_tc_ev_wait(E_TC_EVENT_TYPE_VIS_OFF);
   EINA_SAFETY_ON_FALSE_RETURN_VAL(res, EINA_FALSE);

   return EINA_TRUE;
}

static void
_tc_shutdown(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   e_tc_win_deregister(tc->data->tw);
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
   Eina_List *clients, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   clients = e_tc_req_clients_info_get(tc->main_data);
   EINA_SAFETY_ON_NULL_GOTO(clients, cleanup);

   tw = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   EINA_LIST_FOREACH(clients, l, tw2)
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
   E_FREE_LIST(clients, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0201_transient_for_raise(E_TC *tc)
{
   E_TC_Win *tw, *tw2;
   Eina_Bool res = EINA_FALSE;
   Eina_List *clients;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;

   res = e_tc_win_stack_change(tw, NULL, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = e_tc_ev_wait(E_TC_EVENT_TYPE_STACK_RAISE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   clients = e_tc_req_clients_info_get(tc->main_data);
   EINA_SAFETY_ON_NULL_GOTO(clients, cleanup);

   tw2 = eina_list_nth(clients, 0);
   EINA_SAFETY_ON_NULL_GOTO(tw2, cleanup);
   EINA_SAFETY_ON_FALSE_GOTO(tw2->native_win == tw->native_win, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(clients, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0202_transient_for_lower(E_TC *tc)
{
   E_TC_Win *tw, *tw2;
   Eina_Bool res = EINA_FALSE;
   Eina_List *clients;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;

   res = e_tc_win_stack_change(tw, NULL, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = e_tc_ev_wait(E_TC_EVENT_TYPE_STACK_LOWER);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   clients = e_tc_req_clients_info_get(tc->main_data);
   EINA_SAFETY_ON_NULL_GOTO(clients, cleanup);

   tw2 = eina_list_last_data_get(clients);
   EINA_SAFETY_ON_NULL_GOTO(tw2, cleanup);
   EINA_SAFETY_ON_FALSE_GOTO(tw2->native_win == tw->native_win, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);
   E_FREE_LIST(clients, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0203_transient_for_stack_above(E_TC *tc)
{
   E_TC_Win *tw, *tw2, *tw_parent, *tw_child;
   E_TC_Win *tw_above = NULL;
   Eina_Bool res = EINA_FALSE;
   Eina_List *clients, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   res = e_tc_win_stack_change(tw_parent, tw, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = e_tc_ev_wait(E_TC_EVENT_TYPE_STACK_ABOVE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   clients = e_tc_req_clients_info_get(tc->main_data);
   EINA_SAFETY_ON_NULL_GOTO(clients, cleanup);

   EINA_LIST_FOREACH(clients, l, tw2)
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
   E_FREE_LIST(clients, e_tc_win_del);

   return tc->passed;
}

Eina_Bool
tc_0204_transient_for_stack_below(E_TC *tc)
{
   E_TC_Win *tw, *tw2, *tw_parent, *tw_child;
   E_TC_Win *tw_above = NULL;
   Eina_Bool res = EINA_FALSE;
   Eina_List *clients, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;
   tw_parent = tc->data->tw_parent;
   tw_child = tc->data->tw_child;

   res = e_tc_win_stack_change(tw_parent, tw, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = e_tc_ev_wait(E_TC_EVENT_TYPE_STACK_BELOW);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   clients = e_tc_req_clients_info_get(tc->main_data);
   EINA_SAFETY_ON_NULL_GOTO(clients, cleanup);

   EINA_LIST_FOREACH(clients, l, tw2)
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
   E_FREE_LIST(clients, e_tc_win_del);

   return tc->passed;
}
