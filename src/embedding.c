#include <R.h>
#include <julia.h>
#include <stdio.h>
#include <math.h>

void initJulia(char* JuliaHome)
{
  jl_init(JuliaHome);
  JL_SET_STACK_BASE;
}

jl_value_t jl_eval(char* str)
{
 jl_value_t *ret = jl_eval_string(str);
}

