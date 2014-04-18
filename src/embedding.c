#include <R.h>
#include <julia.h>
#include <stdio.h>
#include <math.h>

void initJulia()
{
  jl_init(NULL);
  JL_SET_STACK_BASE;
}

void jl_eval(char* str)
{
 jl_value_t *ret = jl_eval_string(str);
 double retDouble = jl_unbox_float64(ret);
 Rprintf("test value is %f",retDouble);
 R_FlushConsole();
}

/*jl_value_t* jl_eval(char* str)
{
 jl_value_t *ret = jl_eval_string(str);
  if (jl_is_float64(ret)) {
            double retDouble = jl_unbox_float64(ret);
            Rprintf("test value is %e",retDouble);
        }

 return ret;
}
*/

