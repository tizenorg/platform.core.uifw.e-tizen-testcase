#include "e_test_runner.h"

struct _E_TC_Data
{
   E_TC_Win *tw;
};

static Eina_Bool _tc_pre_run(E_TC *tc, Eina_Bool alpha);
static Eina_Bool _tc_post_run(E_TC *tc);
static void      _tc_shutdown(E_TC *tc);

static Eina_Bool
_tc_pre_run(E_TC *tc, Eina_Bool alpha)
{
   Eina_Bool res;

   tc->data = E_NEW(E_TC_Data, 1);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tc->data, EINA_FALSE);

   tc->data->tw = e_tc_win_add(NULL, ELM_WIN_BASIC,
                               alpha, "tc",
                               0, 0, 320, 320,
                               200);
   EINA_SAFETY_ON_NULL_GOTO(tc->data->tw, cleanup);

   res = e_test_runner_req_win_register(tc->runner, tc->data->tw);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   e_tc_win_geom_update(tc->data->tw);
   e_tc_win_show(tc->data->tw);

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
   Eina_Bool res = EINA_FALSE;

   e_tc_win_hide(tc->data->tw);

   return EINA_TRUE;
}

static void
_tc_shutdown(E_TC *tc)
{
   EINA_SAFETY_ON_NULL_RETURN(tc->data);

   e_test_runner_req_win_deregister(tc->runner, tc->data->tw);
   e_tc_win_del(tc->data->tw);

   E_FREE(tc->data);
}

Eina_Bool
tc_0100_win_show(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);

   return tc->passed;
}

Eina_Bool
tc_0101_win_stack(E_TC *tc)
{
   E_TC_Win *tw, *tw2;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc, EINA_FALSE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw2)
     {
        if (tw2->layer > tw->layer) continue;
        if (tw2->layer < tw->layer) break;

        if (!strncmp(tw2->name, tw->name, strlen(tw2->name)))
          res = EINA_TRUE;

        break;
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
tc_0110_alpha_win_show(E_TC *tc)
{
   Eina_Bool res = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   res = _tc_post_run(tc);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tc->passed = EINA_TRUE;

cleanup:
   _tc_shutdown(tc);

   return tc->passed;
}

Eina_Bool
tc_0111_alpha_win_stack(E_TC *tc)
{
   E_TC_Win *tw, *tw2;
   Eina_Bool res = EINA_FALSE;
   Eina_List *list = NULL, *l;

   EINA_SAFETY_ON_NULL_RETURN_VAL(tc, EINA_FALSE);

   res = _tc_pre_run(tc, EINA_TRUE);
   EINA_SAFETY_ON_FALSE_GOTO(res, cleanup);

   tw = tc->data->tw;

   list = e_test_runner_req_win_info_list_get(tc->runner);
   EINA_SAFETY_ON_NULL_GOTO(list, cleanup);

   EINA_LIST_FOREACH(list, l, tw2)
     {
        if (tw2->layer > tw->layer) continue;
        if (tw2->layer < tw->layer) break;

        if (!strncmp(tw2->name, tw->name, strlen(tw2->name)))
          res = EINA_TRUE;

        break;
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
