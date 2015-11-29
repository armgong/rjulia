
## Initialise Julia
julia_init <- function(juliahome="", disablegc = FALSE, parallel = TRUE)
{
  ## Check Julia exists on the system. If it doesn't, stop immediately.
  juliabindir <- if (nchar(juliahome) > 0) juliahome else {
    gsub("\"", "", system('julia -E JULIA_HOME', intern=TRUE))
  }
  ## Otherwise, initialise Julia using the provided home directory.
  .Call("initJulia", juliabindir, disablegc, PACKAGE = "rjulia")

  ## If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
  julia_void_eval('@windows_only push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor)))')
  julia_void_eval('@windows_only ENV["HOME"]=joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]))')
}

isJuliaOk <- function() .Call("Julia_is_running", PACKAGE="rjulia")

.julia_init_if_necessary <- function() {
  if (!isJuliaOk()) {
    message("Julia not yet running. Calling julia_init() ...")
    julia_init()
    if (!isJuliaOk())
      stop("Julia *still* not running. Giving up.")
  }
}

j2r <- julia_eval <- function(expression)
{
  .julia_init_if_necessary()
  ## Evaluate the expression and return the results of that evaluation. If it's appropriate
  ## to provide it to the user as a vector, do so - otherwise provide it raw.
  eval_result <- .Call("jl_eval", expression, PACKAGE="rjulia")
  if((length(dim(eval_result)) == 1)||(length(eval_result) == 1)) {
    as.vector(eval_result)
  } else {
    eval_result
  }
}

jDo <- julia_void_eval <- function(expression)
{
  .julia_init_if_necessary()
  invisible(.Call("jl_void_eval",expression, PACKAGE="rjulia"))
}

r2j <- r_julia <- function(x,y)
{
  .julia_init_if_necessary()

  if (is.vector(x) || is.factor(x) || is.matrix(x) || is.array(x) || is.data.frame(x)) {
    proc <-
      if (is.data.frame(x)) "R_Julia_NA_DataFrame"
      else if (!anyNA(x) && !is.factor(x)) "R_Julia"
      else if(is.factor(x)) "R_Julia_NA_Factor"
      else "R_Julia_NA"

    invisible(.Call(proc, x,y, PACKAGE="rjulia"))
  }
  else
    warning("only support vector, matrix, array, list(withoug NAs), factor and
 data frames (with simple string, int, float, logical)")
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
