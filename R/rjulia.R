#Initialise Julia
julia_init <- function(juliahome="", disablegc = FALSE, parallel = TRUE) {
 
  #Check Julia exists on the system. If it doesn't, stop immediately.
  if (nchar(juliahome) > 0) {
   juliabindir <- juliahome
  } else {
   juliabindir<-gsub(pattern="\"", replacement="",system('julia -E JULIA_HOME',intern=T))
  } 
  #Otherwise, initialise Julia using the provided home directory.
  invisible(.Call("initJulia",juliabindir,disablegc, PACKAGE = "rjulia"))
  
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

julia_eval <- function(expression) {
  
  #Check if Julia is running. Obviously if it's not, stop.
  if (!.Call("Julia_is_running", PACKAGE="rjulia")) {
    stop("Julia is not running. Call julia_init() to start it.")
  }
  
  #Otherwise, evaluate the expression and return the results of that evaluation. If it's appropriate
  #to provide it to the user as a vector, do so - otherwise provide it raw.
  eval_result <- .Call("jl_eval", expression, PACKAGE="rjulia")
  if((length(dim(eval_result)) == 1)||(length(eval_result) == 1)) {
    return (as.vector(eval_result))
  }
  else
   {
    return (eval_result)
   }
}

julia_void_eval<-function(expression)
{
  #Check if Julia is running. Obviously if it's not, stop.
  if (!.Call("Julia_is_running", PACKAGE="rjulia")) {
    stop("Julia is not running. Call julia_init() to start it.")
  }
  
  invisible(.Call("jl_void_eval",expression,PACKAGE="rjulia"))
}
r_julia<-function(x,y)
{
  #Check if Julia is running. Obviously if it's not, stop.
  if (!.Call("Julia_is_running", PACKAGE="rjulia")) {
    stop("Julia is not running. Call julia_init() to start it.")
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
  warning("only support vector,matrix ,array,list(contain no NA values),factor and data frame which data type must be string int float boolean")
}

julia_DataArrayFrameInited<-function()
{
  y<-.Call("Julia_DataArrayFrameInited",PACKAGE="rjulia")
  return (y)
}

julia_LoadDataArrayFrame<-function()
{
  #Check if Julia is running. Obviously if it's not, stop.
  if (!.Call("Julia_is_running", PACKAGE="rjulia")) {
    stop("Julia is not running. Call julia_init() to start it.")
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

julia_BigintToDouble<-function(mode =FALSE)
{
 invisible(.Call("Julia_BigintToDouble",mode,PACKAGE="rjulia"))
}
