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
 {
  invisible(.Call("initJulia",findjl[[2]],disablegc,PACKAGE="rjulia"))
  #pacth on windows for pkg.dir() not correct because R 
  julia_void_eval('@windows_only push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor)))')
 }
 else
 stop("Could't Find Julia,Besure juliahome your passed is right")
}

Julia_is_running<-function()
{
  y<-.Call("Julia_is_running",PACKAGE="rjulia")
  return (y)
}

julia_eval<-function(expression)
{
 if (!Julia_is_running())
 {
  stop("Julia not running,use julia_init to start it")
 } 
 y<-.Call("jl_eval",expression,PACKAGE="rjulia")
 if ((length(dim(y))==1)||(length(y)==1))
 return (as.vector(y))
 #else if(length(dim(y))==2)
 #return (as.matrix(y))
 #else if (inherits(y,"data.frame"))
 # {
 #   print("this is a data frame") 
 #   return (as.data.frame(y))
 #  } 
 else  
  return (y)
}

julia_void_eval<-function(expression)
{
  if (!Julia_is_running())
  {
    stop("Julia not running,use julia_init to start it")
  } 
  invisible(.Call("jl_void_eval",expression,PACKAGE="rjulia"))
}
r_julia<-function(x,y)
{
 if (!Julia_is_running())
 {
  stop("Julia not running,use julia_init to start it")
 }   
 if (is.vector(x)||is.matrix(x)||is.array(x)||is.data.frame(x))
 {
  if (is.data.frame(x))
   {
    invisible(.Call("R_Julia_NA_DataFrame",x,y,PACKAGE="rjulia"))
   }
  else 
  {
   if (!anyNA(x))
    {
     invisible(.Call("R_Julia",x,y,PACKAGE="rjulia"))
    }
   else
   {
    #r_julia_na(x,y)
   invisible(.Call("R_Julia_NA",x,y,PACKAGE="rjulia"))
   }
  }
 }
 else
  warning("only accept vector or matrix or array of string int float")
}

julia_DataArrayFrameInited<-function()
{
  y<-.Call("Julia_DataArrayFrameInited",PACKAGE="rjulia")
  return (y)
}

julia_LoadDataArrayFrame<-function()
{
 if (!Julia_is_running())
 {
  stop("Julia not running,use julia_init to start it")
 } 
 invisible(.Call("Julia_LoadDataArrayFrame",PACKAGE="rjulia"))
 if (julia_DataArrayFrameInited()==F)
 {
  warning("DataArray and DataFrame not load,please install or check dir")
 }
}
#short and precise name for object type mapping
r2j<-function(x,y)
{
  r_julia(x,y)
}
j2r<-function(expression)
{
  return (julia_eval(expression))
}
jdfinited<-function()
{
  return (julia_DataArrayFrameInited())
}
jloaddf<-function()
{
 julia_LoadDataArrayFrame()
}