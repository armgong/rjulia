\name{julia_eval}
\alias{julia_eval}
\alias{j2r}
\alias{julia_void_eval}
\title{Evaluate Julia Code, or simply get a Julia Variable}
\description{
  Evaluate Julia code from \R, or simply get a Julia variable into \R.

  Whereas \code{j2r()} is currently \emph{identical} to
  \code{julia_eval()},
  \code{julia_void_eval()} will evaluate code in the running julia
  process \emph{without} returning a value.
}
\usage{
j2r        (expression)
julia_eval (expression)

julia_void_eval(expression)
}
\arguments{
  \item{expression}{a \code{\link{character}} string.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
\value{
  an \R object, translated from the corresponding Julia object to which
  \code{expression} evaluated.
}
%% \references{
%% }
%% \note{
%% }
\seealso{
  \code{\link{r2j}} to \dQuote{send} \R objects to Julia.
}
\examples{
julia_init()
j2r("rand(2,3)")
}
\keyword{interface}