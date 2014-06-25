/*
Copyright (C) 2014 by Yu Gong
*/
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <R.h>
#define USE_RINTERNALS
#include <Rinternals.h>
#include <Rmath.h>
#include <julia.h>
#include "R_Julia.h"
#define pkgdebug

#define UTF8_MASK (1<<3)
#define IS_UTF8(x) ((x)->sxpinfo.gp & UTF8_MASK)
jl_array_t* CreateArray(jl_datatype_t* type,size_t ndim,jl_tuple_t* dims)
{
  return jl_new_array(jl_apply_array_type(type,ndim),dims);;
}

jl_tuple_t* RDims_JuliaTuple(SEXP Var)
{
 jl_tuple_t* d;
 SEXP dims = getAttrib(Var, R_DimSymbol);
 //array or matrix
 if (dims!=R_NilValue)
 {
   int ndims = LENGTH(dims);
   d = jl_alloc_tuple(ndims);
   JL_GC_PUSH1(&d);
   size_t i;
   for(i=0; i < ndims; i++)
   {
    jl_tupleset(d, i, jl_box_long(INTEGER(dims)[i]));
  }
  JL_GC_POP();
}
  else //vector
  {
   d = jl_alloc_tuple(1);
   JL_GC_PUSH1(&d);
   jl_tupleset(d, 0, jl_box_long(LENGTH(Var)));
   JL_GC_POP();
 }
 return d;
}

jl_value_t* R_Julia_MD(SEXP Var,const char* VarName)
{
  
  if ((LENGTH(Var))!= 0) {
    jl_tuple_t* dims=RDims_JuliaTuple(Var);
    switch (TYPEOF( Var)) {
      case LGLSXP:
      {
        jl_array_t* ret= CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH1(&ret);
        char* retData = (char*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =LOGICAL(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        return (jl_value_t*) ret;            
        JL_GC_POP(); 
        break;
      };
      case INTSXP:
      {
        jl_array_t* ret= CreateArray(jl_int32_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH1(&ret);
        int* retData = (int*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =INTEGER(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        return (jl_value_t*) ret;            
        JL_GC_POP(); 
        break;
      }
      case REALSXP:
      {
        jl_array_t* ret= CreateArray(jl_float64_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH1(&ret);
        double* retData = (double*)jl_array_data(ret);
        for(size_t i=0; i<jl_array_len(ret); i++)
          retData[i] =REAL(Var)[i];
        jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
        JL_GC_POP();
        return (jl_value_t*) ret;        
        break;
      }
      case STRSXP:
      {
        jl_array_t* ret;
        if (IS_UTF8(Var))
          ret= CreateArray(jl_utf8_string_type,jl_tuple_len(dims),dims);
       else
        ret= CreateArray(jl_ascii_string_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH1(&ret);
        jl_value_t** retData=jl_array_data(ret); 
        for(size_t i=0; i<jl_array_len(ret); i++)
         if (IS_UTF8(Var))
          retData[i] =jl_cstr_to_string(translateChar(STRING_ELT(Var, i)));  
         else 
          retData[i] =jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
       jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
       JL_GC_POP();
       return (jl_value_t*) ret;
       break;
     }
     case VECSXP:
     {
      char eltcmd[1024];
      jl_tuple_t* ret=jl_alloc_tuple(length(Var));
       JL_GC_PUSH1(&ret);
      for (int i=0;i<length(Var);i++)
      {
        sprintf(eltcmd,"%selement%d",VarName,i);
        jl_tupleset(ret,i,R_Julia_MD(VECTOR_ELT(Var,i),eltcmd));
      }        
      jl_set_global(jl_main_module, jl_symbol(VarName), (jl_value_t*)ret); 
      JL_GC_POP();
      return (jl_value_t*) ret;
     }
     default:
      {
      return (jl_value_t*) jl_nothing;
      }
     break; 
   }
 return (jl_value_t*) jl_nothing;  
 }  
 return (jl_value_t*) jl_nothing; 
}


//first pass creat array then convert it to DataArray
//second pass assign NA to element
jl_value_t* TransArrayToDataArray(jl_array_t* mArray,jl_array_t* mboolArray,const char* VarName)
{
  char evalcmd[4096]; 
  jl_set_global(jl_main_module, jl_symbol("TransVarName"), (jl_value_t*)mArray);
  jl_set_global(jl_main_module, jl_symbol("TransVarNamebool"), (jl_value_t*)mboolArray);
  sprintf(evalcmd,"%s=DataArray(TransVarName,TransVarNamebool)",VarName);
  jl_value_t* ret=jl_eval_string(evalcmd);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
   return (jl_value_t*) jl_nothing;
  }
  return ret;
}
jl_value_t* R_Julia_MD_NA(SEXP Var,const char* VarName)
{
  if ((LENGTH(Var))!= 0) {
    jl_tuple_t* dims=RDims_JuliaTuple(Var);

    switch (TYPEOF(Var)) {
      case LGLSXP:
      {
        jl_array_t* ret= CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH(&ret,&ret1);
        
        char* retData = (char*)jl_array_data(ret);
        bool* retData1 = (bool*)jl_array_data(ret1);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
          if (LOGICAL(Var)[i]==NA_LOGICAL)
          { 
            retData[i] =1;
            retData1[i] =true;
          }
          else
          {
            retData[i] =LOGICAL(Var)[i];
            retData1[i] =false;
          }
        }   
        JL_GC_POP();
        return TransArrayToDataArray(ret,ret1,VarName);
        break;
      };
      case INTSXP:
      {
        jl_array_t* ret= CreateArray(jl_int32_type,jl_tuple_len(dims),dims);
        jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH(&ret,&ret1);
        
        int* retData = (int*)jl_array_data(ret);
        bool* retData1 = (bool*)jl_array_data(ret1);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
          if (INTEGER(Var)[i]==NA_INTEGER)
          {
            retData[i] =999;
            retData1[i] =true;
          }
          else
          {
            retData[i] =INTEGER(Var)[i];
            retData1[i] =false;
          }
        }  
        JL_GC_POP();   
        return TransArrayToDataArray(ret,ret1,VarName);
        break;
      }
      case REALSXP:
      {
        jl_array_t* ret= CreateArray(jl_float64_type,jl_tuple_len(dims),dims);
        jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);
        JL_GC_PUSH(&ret,&ret1);
        double* retData = (double*)jl_array_data(ret);
        bool* retData1 = (bool*)jl_array_data(ret1);
        for(size_t i=0; i<jl_array_len(ret); i++)
        {
          if (ISNAN(REAL(Var)[i]))
          {
           retData[i] =999.01;
           retData1[i] =true;
         }
         else
         {
          retData[i] =REAL(Var)[i]; 
          retData1[i] =false;
        }
      }   
      JL_GC_POP();
      return TransArrayToDataArray(ret,ret1,VarName);
      break;
    }
    case STRSXP:
    { jl_array_t* ret;
      if (IS_UTF8(Var))
       ret= CreateArray(jl_utf8_string_type,jl_tuple_len(dims),dims);
      else
       ret= CreateArray(jl_ascii_string_type,jl_tuple_len(dims),dims);

      jl_array_t* ret1=CreateArray(jl_bool_type,jl_tuple_len(dims),dims);

      JL_GC_PUSH(&ret,&ret1);
      jl_value_t** retData=jl_array_data(ret); 
      bool* retData1 = (bool*)jl_array_data(ret1);
      for(size_t i=0; i<jl_array_len(ret); i++)
      {
        if (STRING_ELT(Var, i)==NA_STRING)
        {
         retData[i] =jl_cstr_to_string("999");
         retData1[i] =true;
       }
       else
       {
        if (IS_UTF8(Var))
         retData[i] =jl_cstr_to_string(translateChar(STRING_ELT(Var, i)));  
        else 
         retData[i] =jl_cstr_to_string(CHAR(STRING_ELT(Var, i)));
        retData1[i] =false;
      } 
    }   
    JL_GC_POP(); 
    return TransArrayToDataArray(ret,ret1,VarName);
    break;
  }
  default:
  return (jl_value_t*) jl_nothing;
  break; 
  }//case end
  return (jl_value_t*) jl_nothing;
 }//if length !=0
 return (jl_value_t*) jl_nothing;
} 

jl_value_t* R_Julia_MD_NA_DataFrame(SEXP Var,const char* VarName)
{
 SEXP names=getAttrib(Var,R_NamesSymbol); 
 size_t len=LENGTH(Var);
 if (TYPEOF(Var)!=VECSXP||len==0||names==R_NilValue)
  return (jl_value_t*) jl_nothing;
 char evalcmd[4096]; 
 char eltcmd[1024];
 const char* onename;
 for (size_t i=0;i<len;i++)
 {
  sprintf(eltcmd,"%sdfelt%d",VarName,i+1);
  R_Julia_MD_NA(VECTOR_ELT(Var,i),eltcmd);
  onename=CHAR(STRING_ELT(names, i));
  if (i==0) 
   sprintf(evalcmd,"%s=DataFrame(%s =%s)",VarName,onename,eltcmd);
  else
   sprintf(evalcmd,"%s[symbol(\"%s\")]=%s",VarName,onename,eltcmd);
  //Rprintf("%s\n",evalcmd);
  jl_eval_string(evalcmd);
  if (jl_exception_occurred())
  {
    jl_show(jl_stderr_obj(), jl_exception_occurred());
    Rprintf("\n");
    jl_exception_clear();
   return (jl_value_t*) jl_nothing;
  }  
 }
 return (jl_value_t*) jl_nothing;;
}