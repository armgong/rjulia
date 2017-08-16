.juliaLib <- function()
    gsub("\"", "",
         system('julia -E "abspath(Libdl.dlpath(\\\"libjulia\\\"))"',
                intern = TRUE))

.onLoad <- function(libname, pkgname)
{
    .ldjulia <<- .juliaLib()
    dyn.load(.ldjulia, local = FALSE)
    julia_init()
}

.onUnload <- function(libpath)
{
    ccall("jl_eval_string","jl_atexit_hook(0)")
    if(!exists(".ldjulia")) .ldjulia <- .juliaLib()
    dyn.unload(.ldjulia)
}
