julia_init <- function(juliahome)
{
 .Call("initJulia",juliahome,PACKAGE="rjulia")
}

julia_eval<-function(expression)
{
 .Call("jl_eval",expression,PACKAGE="rjulia")
}
r_julia<-function(x,y)
{
 .Call("R_Julia",x,y,PACKAGE="rjulia")
} 
