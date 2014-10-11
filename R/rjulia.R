#Check if Julia exists on the local system and set the home directory if it does
JuliaExists <- function(juliahome) {

 #If a name is provided, user that. If not, go through common system environment names and rely
 #on the first valid one
 if (nchar(juliahome) > 0) {  
   julia_home_dir <- juliahome
 } else {
   
   #Grab all the common system environment variable names
   env_vars <- Sys.getenv(x = c("JULIA_HOME","Julia_Home","JULIAHOME",
                                "JuliaHome","JULIA","julia"), names = FALSE)
   
   #set jhome to the first one that exists
   julia_home_dir <- env_vars[nchar(env_vars) > 0][1]
   
   #If no entry meets that, set julia_found to FALSE
   if(is.na(julia_home_dir)) {
     julia_found <- FALSE
   }
   
 }
 
 #If the terminating character is a slash of some kind, don't add a slash to the full sys.ji address.
 if(grepl(x = julia_home_dir, pattern = "/") || grepl(x = julia_home_dir, pattern = "\\", fixed = TRUE)) {
  sysfile <- paste0(julia_home_dir,"../lib/julia/sys.ji")
 } else {
   sysfile <- paste0(julia_home_dir,"/../lib/julia/sys.ji")
 }
 
 #If julia_found wasn't already set, it's TRUE
 if(!exists("julia_found")){
   julia_found <- TRUE
 }
 
 #Return
 return (list(julia_found, julia_home_dir))
}


julia_init <- function(juliahome, disablegc = FALSE, parallel = TRUE) {
 
 #Check Julia exists on the system. If it doesn't, stop immediately.
 findjl <- JuliaExists(juliahome)	
 if(!findjl[[1]]) {
   
   stop("Julia could not be found on this system. Check the juliahome value you provided is correct.")
   
 }
 
 #Otherwise, initialise Julia using the provided home directory.
 invisible(.Call(.NAME = "initJulia",findjl[[2]], PACKAGE = "rjulia", disablegc))
 
 #If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
 julia_void_eval('@windows_only push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor)))')
 
 #If the intent is for a parallelised session, initialise that.
 if (parallel) {
 
  julia_void_eval("Sys.init_sysinfo()")
  julia_void_eval('if CPU_CORES > 8 && !("OPENBLAS_NUM_THREADS" in keys(ENV)) && !("OMP_NUM_THREADS" in keys(ENV)) ENV["OPENBLAS_NUM_THREADS"] = 8 end')
  #init_parallel
  julia_void_eval("Base.init_parallel()")
  #init_bind_addr(ARGS)
  julia_void_eval("Base.init_bind_addr(ARGS)")
  
 }
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
 if (is.vector(x)||is.factor(x)||is.matrix(x)||is.array(x)||is.data.frame(x))
 {
  if (is.data.frame(x))
   {
    invisible(.Call("R_Julia_NA_DataFrame",x,y,PACKAGE="rjulia"))
   }
  else 
  {
   if (!anyNA(x)&&!is.factor(x))
    {
     invisible(.Call("R_Julia",x,y,PACKAGE="rjulia"))
    }
   else
   {
    if(is.factor(x))
    invisible(.Call("R_Julia_NA_Factor",x,y,PACKAGE="rjulia")) 
    else 
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
