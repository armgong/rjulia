\name{julia_init}
\alias{julia_init}
\title{Start Julia from R, properly initializing the  R--Julia Interface}
\description{
  Start Julia from \R, properly initializing the  \R--Julia interface.

}
\usage{
julia_init(juliahome = "", disablegc = FALSE, parallel = TRUE)
}
\arguments{
  \item{juliahome}{the home directory of the julia installation; should
    rarely be needed.}
  \item{disablegc}{logical indicating if garbage collection is
    % ....
    disabled.}
  \item{parallel}{logical indicating if Julia should be initialized to
    run several parallel threads.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
\value{
  \code{\link{NULL}} if successful.
}
\seealso{
  \code{\link{r2j}}, \code{\link{j2r}}, \code{\link{julia_eval}}.
}
\examples{
try( julia_init() ) # should work if julia is properly installed
}
\keyword{interface}
