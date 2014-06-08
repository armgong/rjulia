julia_init <- function(juliahome,disablegc=TRUE)
{
 .Call("initJulia",juliahome,disablegc,PACKAGE="rjulia")
}

julia_eval<-function(expression)
{
 y<-.Call("jl_eval",expression,PACKAGE="rjulia")
 if ((length(dim(y))==1)||(length(y)==1))
  return (as.vector(y))
 else if(length(dim(y))==2)
  return (as.matrix(y))
 else 
  return (y)
}
julia_void_eval<-function(expression)
{
 invisible(.Call("jl_void_eval",expression,PACKAGE="rjulia"))
}

r_julia<-function(x,y)
{
if (is.vector(x)||is.matrix(x)||is.array(x))
 invisible(.Call("R_Julia",x,y,PACKAGE="rjulia"))
else
 warning("only accept vector or matrix or array of string int float")
}

