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
invisible(.Call("R_Julia",x,y,PACKAGE="rjulia"))
}

