## Initialise Julia
julia_init <- function(disablegc = FALSE) {
  ## Otherwise, initialise Julia using the provided home directory.
  .Call("initJulia", disablegc, PACKAGE = "rjulia")

  ## If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
  julia_void_eval('@static if is_windows()  push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor))) end')
  julia_void_eval('@static if is_windows()  ENV["HOME"]=joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"])) end')

  jloaddf()

  if (julia_eval('VERSION < v"0.6.0"'))
      stop("Julia version must be 0.6 or higher.")

  return(invisible(TRUE))
}

cstrnull <- function(orgstr) {
  return (c(charToRaw(orgstr),as.raw(0)))
}

ccall <- function(fname,cmdstr) {
  functionsym<-getNativeSymbolInfo(fname)
  invisible(.C(functionsym,cstrnull(cmdstr)))
}

jDo <- julia_void_eval <- julia_eval <- function(cmdstr) {
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

j2r <- julia_eval <- function(expression) {
  ## Evaluate the expression and return the results of that evaluation.
  .Call("jl_eval", expression, PACKAGE="rjulia")
}

r2j <- r_julia <- function(x,y) {
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

jdfinited <- julia_DataArrayFrameInited <- function() {
  .Call("Julia_DataArrayFrameInited", PACKAGE="rjulia")
}

jloaddf <- julia_LoadDataArrayFrame <- function() {
  invisible(.Call("Julia_LoadDataArrayFrame", PACKAGE="rjulia"))
  if (!julia_DataArrayFrameInited()) warning(
 "DataArray and DataFrame Julia packages have not been loaded.
  Please install or check installation directory.")
}


julia_BigintToDouble <- function(mode = FALSE) {
  invisible(.Call("Julia_BigintToDouble", mode, PACKAGE="rjulia"))
}

###  MM: (ess-set-style 'DEFAULT)  ==> only indent by 2
## Local Variables:
## eval: (ess-set-style 'DEFAULT 'quiet)
## delete-old-versions: never
## End:
