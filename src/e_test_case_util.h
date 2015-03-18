#ifndef E_TEST_CASE_UTIL_H
#define E_TEST_CASE_UTIL_H

typedef enum _E_Test_Case_Stack_Type
{
   E_TEST_CASE_STACK_TYPE_RAISE,
   E_TEST_CASE_STACK_TYPE_LOWER,
   E_TEST_CASE_STACK_TYPE_ABOVE,
   E_TEST_CASE_STACK_TYPE_BELOW
} E_Test_Case_Stack_Type;

typedef enum _E_Test_Case_Wait_Vis_Type
{
   E_TEST_CASE_WAIT_VIS_TYPE_TIMEOUT,
   E_TEST_CASE_WAIT_VIS_TYPE_CHANGED,
   E_TEST_CASE_WAIT_VIS_TYPE_ON,
   E_TEST_CASE_WAIT_VIS_TYPE_OFF
} E_Test_Case_Wait_Vis_Type;

EAPI void e_test_case_util_get_clients(E_TC_Data *tc_data);
EAPI Eina_Bool e_test_case_util_register_window(Ecore_Window win);
EAPI Eina_Bool e_test_case_util_deregister_window(Ecore_Window win);
EAPI Eina_Bool e_test_case_util_wait_visibility_change(E_Test_Case_Wait_Vis_Type expect);
EAPI Eina_Bool e_test_case_util_change_stack(Ecore_Window target, Ecore_Window pic, int type);
EAPI Eina_Bool e_test_case_util_wait_restack(void);

#endif
