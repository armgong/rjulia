\name{r_julia}
\alias{r2j}
\alias{r_julia}
\title{Send R Objects to Julia}
\description{
  Send \R object \code{x} to julia, i.e., translate it to a corresponding julia
  object, and store that as \code{y}.
}
\usage{
r_julia(x, y)
}
\arguments{
  \item{x}{an \R object.  Currently must fulfill certain properties to
    be translatable to Julia.}
  \item{y}{a \code{\link{character}} string.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
\value{
 FIXME
}
%% \references{
%% %% ~put references to the literature/web site here ~
%% }
\seealso{
  \code{\link{j2r}} aka \code{\link{julia_eval}} for the reverse of this.
}
\examples{
}
\keyword{interface}
