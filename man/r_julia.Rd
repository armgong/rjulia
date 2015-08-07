\name{r_julia}
\alias{r2j}
\alias{r_julia}
\title{Send R Objects to Julia}
\description{
  Send \R object \code{x} to julia, i.e., translate it to a corresponding julia
  object, and store that as \code{y}.
}
\usage{
r_julia(x, y)  # or shorter
    r2j(x, y)
}
\arguments{
  \item{x}{an \R object.  Currently must fulfill certain properties to
    be translatable to Julia.}
  \item{y}{a \code{\link{character}} string; the variable name on the
    Julia side.}
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
\value{
  on success, \code{\link{NULL}}, invisibly.
}
\seealso{
  \code{\link{j2r}} aka \code{\link{julia_eval}} for the reverse of this.
}
\examples{
## assign R's pi to Julia, as "Rpi"
r2j(pi, "Rpi")
j2r("pi") # Julia's builtin pi
## is there a difference
j2r("Rpi - pi") # typically 0

## integer vector:
r2j(1:10, "i10")
j2r("i10") ; stopifnot(identical(1:10, j2r("i10")))

## matrix, passed to Julia and got back
m <- matrix(exp(-5:6), 3,4)
r2j(m, "m")
stopifnot(identical(m, j2r("m")))

## complex -- not yet _FIXME_
z <- complex(modulus = 4, argument = pi*(0:16)/8)
r2j(z, "z") # does *not* work ! but gives no error
if(FALSE)
  stopifnot(identical(z, j2r("z")))
try( j2r( "z" ) )# ->  "... undefined"
}
\keyword{interface}
