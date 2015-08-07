\name{julia_eval}
\title{Evaluate Julia Code or Get a Julia Variable}
\alias{j2r}
\alias{julia_eval}
\alias{jDo}
\alias{julia_void_eval}
\description{
  Evaluate Julia code from \R, or simply get a Julia variable into \R.

  Whereas \code{j2r()} is currently \emph{identical} to
  \code{julia_eval()},
  \code{julia_void_eval()} will evaluate code in the running julia
  process \emph{without} returning a value.
}
\usage{
j2r        (expression) # short for
julia_eval (expression)

jDo            (expression) # short for
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
\seealso{
  \code{\link{r2j}()} to \dQuote{send} \R objects to Julia.
}
\examples{
j2r("cos(pi * (1:7))")

j2r("map(x -> x^2, (7,17,47))") # tuple in Julia ==>  list (with no names!) in R

j2r("rand(2,3)")# or rather use R's RNG
}
\keyword{interface}
