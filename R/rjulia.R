#' @name rjulia package
#' @title Integrating R and Julia -- Calling Julia from R
#' @description
#' RJulia (R package name \pkg{rjulia}) provides an interface between \R
#' and Julia by converting \R and Julia objects mutually,  and running
#' Julia scripts from \R.
#'
#' \R is a sophisticated feature-rich statistical software but a bit
#' slow,  julia is new advanced computing language which is fast and
#' LLVM-based JIT. So, combining \R and julia will allow us to do better
#' statistical computing.
#' @docType package
#' @useDynLib rjulia
#' @family rjulia
NULL

#' Start Julia from R,  properly initializing the  R--Julia Interface
#'
#' Start Julia from R,  properly initializing the  R--Julia Interface
#' @param disablegc single logical, turn of garbage collection?
#' @return TRUE, invisibly
#' @export julia_init
#' @family rjulia
julia_init <- function(disablegc = FALSE) {
  ## Otherwise, initialise Julia using the provided home directory.
  .Call("initJulia", disablegc, PACKAGE = "rjulia")

  ## If on Windows, run a specific push to compensate for R not handling pkg.dir() correctly.
  jDo('@static if is_windows()  push!(LOAD_PATH,joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"]),".julia",string("v",VERSION.major,".",VERSION.minor))) end')
  jDo('@static if is_windows()  ENV["HOME"]=joinpath(string(ENV["HOMEDRIVE"],ENV["HOMEPATH"])) end')

  if (j2r('VERSION < v"0.6.0"'))
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
#' @export jDo
#' @family rjulia
jDo <- function(cmdstr) {
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

#' Evaluate Julia Code or Get a Julia Variable
#'
#'  Evaluate Julia code from \R, or simply get a Julia variable into \R. Whereas
#' \code{j2r()} is currently \emph{identical} to \code{j2r()},
#' \code{jDo()} will evaluate code in the running julia
#' process \emph{without} returning a value.
#' @param expression single character
#' @return single character
#' @examples
#' j2r("cos(pi * (1:7))")
#' j2r("map(x -> x^2,  (7, 17, 47))") # tuple in Julia ==>  list (with no names!) in R
#' j2r("rand(2, 3)")# or rather use R's RNG
#' @export
#' @family rjulia
j2r <- function(expression) {
  .Call("jl_eval", expression, PACKAGE="rjulia")
}

#' Send data to juila
#'
#' R data types will be translated to the appropriate julia types. For now,
#' names will be lost.
#' @param x R variable: vector, array, factor, list, data.frame
#' @param y single character, symbol to use in julia
#' @return nothing
#' @examples
#' r2j(pi,  "Rpi")
#' j2r("pi") # Julia's builtin pi
#' ## is there a difference
#' j2r("Rpi - pi") # typically 0
#'
#' ## integer vector:
#' r2j(1:10,  "i10")
#' j2r("i10")
#'
#' ## matrix,  passed to Julia and got back
#' m <- matrix(exp(-5:6),  3, 4)
#' r2j(m,  "m")
#' @export
#' @family rjulia
r2j <- function(x,y) {
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
#' @export jdfinited
#' @family rjulia
jdfinited  <- function() {
  .Call("Julia_DataArrayFrameInited", PACKAGE="rjulia")
}

#' Load the DataFrames and DataArrays packages in juila
#'
#' These packages are loaded any time r2j is used to pass data.frames or
#' any objects containing NAs, but you can load them early if you want.
#' @return single logical for success or failure
#' @export jloaddf
#' @family rjulia
jloaddf  <- function() {
  invisible(.Call("Julia_LoadDataArray", PACKAGE="rjulia"))
  if (!jdfinited())
      warning("DataArray and DataFrame Julia packages could not be loaded.")
}

#' Option for Converting Julia Types Uint32,  Int64,  and Uint64 to R
#'
#' We need to pay attention on the julia types \code{UInt32}, \code{Int64}
#'   and \code{UInt64} are mapped to \R. As the \code{\link{integer}} type
#'   in \R is (only) signed 32 bit, the above three types contain values out
#'   of \R's integer range.
#'   \code{julia_BigintToDouble()} allows to determine what happens when
#'   such variables are converted to \R objects.
#' @param mode single logical, indicates how to convert Uint32,
#'   Int64 and Uint64 julia objects to \R. If \code{mode} is true, convert to
#'   double. If it is false, the julia object will be converted to
#'   integer or double depending on the object's value. If all values of
#'   the object stay within the range of signed int32, then the object will be
#'   converted to integer, otherwise to double. The default \code{mode}
#'   is \code{FALSE}.
#' @return NULL
#' @examples
#' julia_BigintToDouble(TRUE)
#' y <- j2r("jvar=convert(Uint64,  12)")
#' jDo("println(typeof(jvar))") # UInt64
#' str(y) # num 12 -- double ("TRUE" above)
#'
#' julia_BigintToDouble(FALSE)
#' y <- j2r("jvar=convert(Uint64,  31)")
#' jDo("println(typeof(jvar))") # UInt64
#' str(y) # int 31 -- integer,  not double  ("FALSE" above)
#' @export
#' @family rjulia
julia_BigintToDouble <- function(mode = FALSE) {
  invisible(.Call("Julia_BigintToDouble", mode, PACKAGE="rjulia"))
}
