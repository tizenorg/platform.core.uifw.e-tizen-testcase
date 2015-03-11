#include "e_test_case_main.h"

Eina_Bool
test_case_0000_easy_fail(E_Test_Case *tc EINA_UNUSED)
{
   return EINA_FALSE;
}

Eina_Bool
test_case_0000_easy_pass(E_Test_Case *tc EINA_UNUSED)
{
   return EINA_TRUE;
}
