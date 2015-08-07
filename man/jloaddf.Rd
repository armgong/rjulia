\name{jloaddf}
\title{Loads the DataArray and DataFrame Julia Packages or Checks them}
\alias{julia_LoadDataArrayFrame}
\alias{julia_DataArrayFrameInited}
\alias{jloaddf}
\alias{jdfinited}
\description{
  \code{julia_LoadDataArrayFrame()} loads the \code{DataArray} and
  \code{DataFrames} julia packages,
  and
  \code{julia_DataArrayFrameInited()} checks if they have been loaded
  and work.

  \code{jloaddf} is identical to \code{julia_LoadDataArrayFrame}, and
  \code{jdfinited} is identical to \code{julia_DataArrayFrameInited},
  just short forms for convenience.
}
\usage{
julia_LoadDataArrayFrame()
jloaddf()

julia_DataArrayFrameInited()
jdfinited()
}
%% \details{
%% %%  ~~ If necessary, more details than the description above ~~
%% }
\value{
  \code{julia_DataArrayFrameInited()} (and hence \code{jdfinited()})
  return a \code{\link{logical}}, \code{TRUE} if the julia packages are there.

%% FIXME the other:  none (?)

}
\seealso{
  \code{\link{julia_init}}
}
\examples{
julia_DataArrayFrameInited() # TRUE or FALSE in any case

if(julia_DataArrayFrameInited())
   julia_LoadDataArrayFrame()
## ^^^^^^^^^^^^^^^^^^^^^^^^^^  fails if  not installed in Julia
}
\keyword{interface}
