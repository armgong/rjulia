\name{julia_init}
\title{Start Julia from R, properly initializing the  R--Julia Interface}
\alias{julia_init}
\alias{isJuliaOk}
\description{
  Start Julia from \R, properly initializing the  \R--Julia interface.
}
\usage{
julia_init(juliahome = "", disablegc = FALSE, parallel = TRUE)

isJuliaOk()
}
\arguments{
  \item{juliahome}{the home directory of the julia installation; should
    rarely be needed.}
  \item{disablegc}{logical indicating if garbage collection is
    % ....
    disabled.}
  \item{parallel}{logical indicating if Julia should be initialized to
    run several parallel processes.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
\value{
  \code{julia_init()} returns \code{\link{NULL}} if successful, an error
  otherwise

  \code{isJuliaOk()} returns \code{\link{TRUE}} or \code{\link{FALSE}}
  (and should never give an error).
}
\seealso{
  \code{\link{r2j}}, \code{\link{j2r}}, \code{\link{julia_eval}}.
}
\examples{
isJuliaOk() # TRUE, (almost ?) always if the 'rjulia' package has been loaded properly
try( julia_init() ) # should work if julia is properly installed
}
\keyword{interface}
