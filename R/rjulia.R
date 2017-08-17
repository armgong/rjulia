#' @name rjulia package
#' @title Integrating R and Julia -- Calling Julia from R
#' @description
#' Provides a mechanism to send data to create a running julia proess,
#' send and receive data to/from that process and run arbitrary code in
#' the process.
#' @docType package
#' @useDynLib rjulia
NULL

#' Initialise Julia
#'
#' Initialise Julia
#' @param disablegc single logical, turn of garbage collection?
#' @return TRUE, invisibly
#' @export julia_init
julia_init <- function(disablegc = FALSE) {
  ## Otherwise, initialise Julia using the provided home directory.
  .Call("initJulia", disablegc, PACKAGE = "rjulia")

  ## If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
  julia_void_eval('@static if is_windows()  push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor))) end')
  julia_void_eval('@static if is_windows()  ENV["HOME"]=joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"])) end')

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

#' Execute julia code
#'
#' Execute julia code
#' @param cmdstr single character
#' @return nothing
#' @export jDo julia_void_eval
jDo <- julia_void_eval <- function(cmdstr) {
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

#' Evaluate julia code and return result to R
#'
#' Evaluate julia code and return result to R
#' @param expression single character
#' @return single character
#' @export j2r julia_eval
j2r <- julia_eval <- function(expression) {
  .Call("jl_eval", expression, PACKAGE="rjulia")
}

#' Send data to juila
#'
#' R data types will be translated to the appropriate julia types. For now,
#' names will be lost.
#' @param x R variable: vector, array, factor, list, data.frame
#' @param y single character, symbol to use in julia
#' @return nothing
#' @export r2j r_julia
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

#' Check if DataFrames and DataArrays packages have been loaded in julia
#'
#' These packages are loaded any time r2j is used to pass data.frames or
#' any objects containing NAs.
#' @return single logical
#' @export jdfinited julia_DataArrayFrameInited
jdfinited <- julia_DataArrayFrameInited <- function() {
  .Call("Julia_DataArrayFrameInited", PACKAGE="rjulia")
}

#' Load the DataFrames and DataArrays packages in juila
#'
#' These packages are loaded any time r2j is used to pass data.frames or
#' any objects containing NAs, but you can load them early if you want.
#' @return single logical for success or failure
#' @export jloaddf julia_LoadDataArrayFrame
jloaddf <- julia_LoadDataArrayFrame <- function() {
  invisible(.Call("Julia_LoadDataArrayFrame", PACKAGE="rjulia"))
  if (!julia_DataArrayFrameInited()) warning(
 "DataArray and DataFrame Julia packages have not been loaded.
  Please install or check installation directory.")
}

#' Set option for converting julia BigInt values to doubles
#'
#' Set option for converting julia BigInt values to doubles
#' @param mode single logical
#' @return NULL
#' @export julia_BigintToDouble
julia_BigintToDouble <- function(mode = FALSE) {
  invisible(.Call("Julia_BigintToDouble", mode, PACKAGE="rjulia"))
}
