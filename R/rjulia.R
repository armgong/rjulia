## Initialise Julia
julia_init <- function(juliahome="", disablegc = FALSE, parallel = TRUE)
{
  ## Check Julia exists on the system. If it doesn't, stop immediately.
  juliabindir <- if (nzchar(juliahome)) juliahome else {
    gsub("\"", "", system('julia -E JULIA_HOME', intern=TRUE))
  }
  ## Otherwise, initialise Julia using the provided home directory.
  .Call("initJulia", juliabindir, disablegc, PACKAGE = "rjulia")

  ## If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
  julia_void_eval('@static if is_windows()  push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor))) end')
  julia_void_eval('@static if is_windows()  ENV["HOME"]=joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"])) end')

  jloaddf()

  if (julia_eval('VERSION < v"0.5.0"'))
      stop("Julia version must be 0.5 or higher.")
  
  return(invisible(TRUE))
}

######### From: rjulia2

cstrnull<-function(orgstr)
{
  return (c(charToRaw(orgstr),as.raw(0)))
}

ccall<-function(fname,cmdstr)
{
  functionsym<-getNativeSymbolInfo(fname)
  invisible(.C(functionsym,cstrnull(cmdstr)))
}


.julia_init_if_necessary <- function() {
  if (!.isJuliaOk) {
    message("Julia not yet running. Calling julia_init() ...")
    julia_init()
    if (!.isJuliaOk)
      stop("Julia *still* not running. Giving up.")
    else
      message("julia_init complete successfully")
  }
}

jDo<-julia_void_eval<-julia_eval<-function(cmdstr) {
  .julia_init_if_necessary()
                                        #first clear julia exception
  ccall("jl_eval_string",'ccall(:jl_exception_clear,Void,());')
                                        #second eval julia expression
  ccall("jl_eval_string",cmdstr)
                                        #then,try catch and show julia execption
  ccall("jl_eval_string",
        'if ccall(:jl_exception_occurred,Ptr{Void},())!=C_NULL
            rjuliaexception=ccall(:jl_exception_occurred,Any,());
            showerror(STDERR,rjuliaexception);
            println("");
            ccall(:jl_exception_clear,Void,());
       end')
}

Init<-julia_init <- function(juliahome="")
       {
           ##force change HOME env variable in R, R change HOME to c:\user\username\Documents
           ##but on window 7+ this should be c:\user\username, and it not change it, julia could not
           ##find its package and compiled package, so let us change it
           if (Sys.info()[['sysname']]=="Windows")
           {
               Sys.setenv(HOME=paste0(Sys.getenv("HOMEDRIVE"),Sys.getenv("HOMEPATH")))
           }

           juliabindir <- if (nchar(juliahome) > 0) juliahome else {
                                                                      gsub("\"", "",
                                                                      system('julia -E JULIA_HOME',
                                                                             intern=TRUE))
                                                              }
           ccall("jl_init",juliabindir)
           .isJuliaOk<<-T
           ## If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
             ##jDo('@windows_only
           ##push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor)))')
           ##jDo('@windows_only ENV["HOME"]=joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]))')
           ## Loading julia packages
           jDo("using DataFrames")
           jDo("using RCall")
       }


########## End: from rjulia2

j2r <- julia_eval <- function(expression)
{
  .julia_init_if_necessary()
  ## Evaluate the expression and return the results of that evaluation.
  .Call("jl_eval", expression, PACKAGE="rjulia")
}

r2j <- r_julia <- function(x,y)
{
  .julia_init_if_necessary()

  if (is.vector(x) || is.array(x)) {  # Covers list and matrix too

    if (anyNA(x)) {
      na = is.na(x)
      invisible(.Call("R_Julia_NA", x, na, y, PACKAGE="rjulia"))
    } else {
      invisible(.Call("R_Julia", x, y, PACKAGE="rjulia"))
    }
  } else if (is.data.frame(x)) {
    na = lapply(x, is.na)
    if (is.null(names(x))) {
      names(x) = as.character(seq_len(length(x)))
    }
    invisible(.Call("R_Julia_NA_DataFrame", x, na, y, PACKAGE="rjulia"))
  } else if (is.factor(x)) {
    invisible(.Call("R_Julia_NA_Factor", x, y, PACKAGE="rjulia"))
  } else {
    warning("rjulia supports only vector, matrix, array, list(withoug NAs), factor and data frames (with simple string, int, float, logical) classes")
  }
}

jdfinited <- julia_DataArrayFrameInited <- function()
{
  .Call("Julia_DataArrayFrameInited", PACKAGE="rjulia")
}

jloaddf <- julia_LoadDataArrayFrame <- function()
{
  .julia_init_if_necessary()

  invisible(.Call("Julia_LoadDataArrayFrame", PACKAGE="rjulia"))
  if (!julia_DataArrayFrameInited()) warning(
 "DataArray and DataFrame Julia packages have not been loaded.
  Please install or check installation directory.")
}


julia_BigintToDouble <- function(mode = FALSE)
{
  invisible(.Call("Julia_BigintToDouble", mode, PACKAGE="rjulia"))
}

###  MM: (ess-set-style 'DEFAULT)  ==> only indent by 2
## Local Variables:
## eval: (ess-set-style 'DEFAULT 'quiet)
## delete-old-versions: never
## End:
