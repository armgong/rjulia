
.onLoad <- function(libname, pkgname)
{
   library.dynam("rjulia",pkgname,libname,local=FALSE)

}

.onUnload <- function(libpath)
 {
 library.dynam.unload("rjulia", libpath)
}
