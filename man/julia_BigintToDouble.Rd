\name{julia_BigintToDouble}
\alias{julia_BigintToDouble}
\title{Options How to convert julia uin32 int64 uint64 type to R}
\description{
 we need pay some attention to mapping types of UInt32, Int64 and UInt64 from Julia to R. this is because Integer type in R is signed 32 bit, while the above three types’ value may be out of signed 32 bit integer's range. To deal with this, the julia_BigintToDouble function is created.
}
\usage{
julia_BigintToDouble(mode = FALSE)
}
\arguments{
  \item{mode}{logical indicating how to convert uint32 int64 and Uint64 julia object to r. if mode is TRUE then convert to dobule type. if mode is FALSE, julia object will be converted to integer or double, which depends on the object's value. if all value in the object within signed int32 range, then the object will be converted to integer type, otherwise it will be converted to double type. default value of mode is FALSE.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
%% \value{
%% %%  ~Describe the value returned
%% %%  If it is a LIST, use
%% %%  \item{comp1 }{Description of 'comp1'}
%% %%  \item{comp2 }{Description of 'comp2'}
%% %% ...
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
y<-j2r("jvar=convert(Uint64,1)")
julia_void_eval("println(typeof(jvar))")
typeof(y)

julia_BigintToDouble(FALSE)
y<-j2r("jvar=convert(Uint64,1)")
julia_void_eval("println(typeof(jvar))")
typeof(y)

y<-j2r("jvar=convert(Uint64,typemax(Uint64))")
julia_void_eval("println(typeof(jvar))")
Uint64
typeof(y)
}
\keyword{interface}
