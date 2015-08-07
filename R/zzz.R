.juliaLib <- function()
    gsub("\"", "",
         system('julia -E "abspath(Libdl.dlpath(\\\"libjulia\\\"))"',
                intern = TRUE))

.onLoad <- function(libname, pkgname)
{

    .ldjulia <<- .juliaLib()
 ## Sys.setenv("LD_LIBRARY_PATH"=paste(ldjulia,Sys.getenv("LD_LIBRARY_PATH"),sep=':'))
 ## print(Sys.getenv("LD_LIBRARY_PATH"))
 ## print(pkgname)
 ## print(libname)
    dyn.load(.ldjulia, local = FALSE)

    ## or via ../NAMESPACE  useDynlib()
    library.dynam("rjulia", pkgname, libname, local = FALSE)
}

.onUnload <- function(libpath)
{
    if(!exists(".ldjulia")) .ldjulia <- .juliaLib()
    dyn.unload(.ldjulia)
    library.dynam.unload("rjulia", libpath)
}
