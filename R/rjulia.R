JuliaIsExist<-function(juliahome) 
{  
 if (nchar(juliahome)>0)
 {  
   jhome<-juliahome
 }
 else
 {
   jhome<-""
   if (nchar(Sys.getenv("JULIA_HOME"))>0)
     jhome<-Sys.getenv("JULIA_HOME")
   if  (nchar(Sys.getenv("Julia_Home"))>0)
     jhome<-Sys.getenv("Julia_Home")
   if  (nchar(Sys.getenv("JULIAHOME"))>0)
     jhome<-Sys.getenv("JULIAHOME")
   if  (nchar(Sys.getenv("JuliaHome"))>0)
     jhome<-Sys.getenv("JuliaHome")
   if  (nchar(Sys.getenv("JULIA"))>0)
     jhome<-Sys.getenv("JULIA")
   if  (nchar(Sys.getenv("Julia"))>0)
     jhome<-Sys.getenv("Julia")
 }
  
 if (nchar(jhome)>0)
 { 
   if ((jhome[length(jhome)]!="/")||(jhome[length(jhome)]!="\\"))
     sysfile<-paste(jhome,"/../lib/julia/sys.ji",sep="")
   else
     sysfile<-paste(jhome,"../lib/julia/sys.ji",sep="")
   ret<-file.exists(sysfile)
 }  
 else
  {
  	ret<-FALSE
  }
  return (list(ret,jhome))
}
 

julia_init <- function(juliahome,disablegc=TRUE)
{
 findjl<-JuliaIsExist(juliahome)	
 if (findjl[[1]])	
  .Call("initJulia",findjl[[2]],disablegc,PACKAGE="rjulia")
 else
  stop("Could't Find Julia,Besure juliahome your passed is right")
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
{
  invisible(.Call("R_Julia",x,y,PACKAGE="rjulia"))
}
else
 warning("only accept vector or matrix or array of string int float")
}

