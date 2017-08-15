.juliaLib <- function()
    gsub("\"", "",
         system('julia -E "abspath(Libdl.dlpath(\\\"libjulia\\\"))"',
                intern = TRUE))

.onLoad <- function(libname, pkgname)
{
    .ldjulia <<- .juliaLib()
    dyn.load(.ldjulia, local = FALSE)
}

.onUnload <- function(libpath)
{
    if(!exists(".ldjulia")) .ldjulia <- .juliaLib()
    dyn.unload(.ldjulia)
}
