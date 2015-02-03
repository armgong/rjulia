\name{rjulia-package}
\alias{rjulia-package}
\alias{rjulia}
\docType{package}
\title{rjulia helps integrating R and Julia by calling Julia from R}
\description{
  RJulia (R package name \pkg{rjulia}) provides an interface between R
  and Julia by converting \R and Julia objects mutually, and running
  Julia scripts from \R.

  R is a sophisticated feature-rich statistical software but a bit
  slow, julia is new advanced computing language which is fast and
  LLVM-based JIT.  So, combining R and julia will allow us to do better
  statistical computing.
}
\details{
  \tabular{ll}{
    Package: \tab rjulia\cr
    Type: \tab Package\cr
    Version: \tab 1.0\cr
%%  Date: \tab 2015-07-02\cr -- don't want to update all the time
    License: \tab GPL 2\cr
  }

  TODO:
  An overview of how to use the package, including the most important functions.
}
\author{Yu Gong \email{armgong@yahoo.com}
}
%% \references{
%%   ~~ Literature or other references for background information ~~
%% }
\seealso{
  \code{\link{julia_init}}, \code{\link{julia_eval}},
  \code{\link{j2r}}, \code{\link{r2j}}.
  %% Optional links to other man pages, e.g. \code{\link[<pkg>:<pkg>-package]{<pkg>}
}
\examples{
 if(is(T1 <- tryCatch(julia_init(), error=function(e)e), "error")) {
    ## try harder
    juliaDIR <- dirname(system("which julia", intern=TRUE))
    message("Trying Julia directory ", juliaDIR)
    if(is(T2 <- tryCatch(julia_init(juliaDIR), error=function(e)e), "error")) {
      ## TODO: show error messages of  T1 and T2
      warning("Did not succeed to initialize  Rjulia with correct julia directory yet")
    }
    else {
      message(" Success !!")
      julia_eval("1+1")
      ## julia_eval("2^20")
    }
 }
}
\keyword{package}
