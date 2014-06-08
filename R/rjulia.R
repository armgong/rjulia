julia_init <- function(juliahome,disablegc=TRUE)
{
 .Call("initJulia",juliahome,disablegc,PACKAGE="rjulia")
}

julia_eval<-function(expression)
{
 .Call("jl_eval",expression,PACKAGE="rjulia")
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

