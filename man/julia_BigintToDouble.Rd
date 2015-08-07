\name{julia_BigintToDouble}
\alias{julia_BigintToDouble}
\title{Option for Converting Julia Types Uint32, Int64, and Uint64 to R}
\description{
  We need to pay attention on the julia types \code{UInt32}, \code{Int64}
  and \code{UInt64} are mapped to \R.  As the \code{\link{integer}} type
  in \R is (only) signed 32 bit, the above three types contain values out
  of \R's integer range.
  \code{julia_BigintToDouble()} allows to determine what happens when
  such variables are converted to \R objects.
}
\usage{
julia_BigintToDouble(mode = FALSE)
}
\arguments{
  \item{mode}{\code{\link{logical}} indicating how to convert Uint32,
    Int64 and Uint64 julia objects to \R.  If \code{mode} is true, convert to
    double.  If it is false, the julia object will be converted to
    integer or double depending on the object's value.  If all values of
    the object stay within the range of signed int32, then the object will be
    converted to integer, otherwise to double.  The default \code{mode}
    is \code{FALSE}.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
%% \value{
%% }
%% \references{
%% %% ~put references to the literature/web site here ~
%% }
\seealso{
 \code{\link{r2j}},
 \code{\link{julia_eval}}.
}
\examples{
julia_BigintToDouble(TRUE)
y <- j2r("jvar=convert(Uint64, 12)")
julia_void_eval("println(typeof(jvar))") # UInt64
str(y) # num 12 -- double ("TRUE" above)

julia_BigintToDouble(FALSE)
y <- j2r("jvar=convert(Uint64, 31)")
julia_void_eval("println(typeof(jvar))") # UInt64
str(y) # int 31 -- integer, not double  ("FALSE" above)

## Show maximal values of the 64-bit integer types:
y[1] <- j2r("jvar= convert(Uint64, typemax(Uint64))")
y[2] <- j2r("jv2 = convert(Int64,  typemax( Int64))")
julia_void_eval("println(typeof(jvar),' ', typeof(jv2))")
str(y) # num 1.84e+19 9.22e+18
## they are 2^64 and 2^63  respectively:
stopifnot(log2(y) == 64:63)
}
\keyword{interface}
