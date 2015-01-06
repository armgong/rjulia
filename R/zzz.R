.onLoad <- function(libname, pkgname)
{

ldjulia<-gsub(pattern="\"", replacement="",system('julia -E "abspath(Sys.dlpath(\\\"libjulia\\\"))"',intern=T))

 #Sys.setenv("LD_LIBRARY_PATH"=paste(ldjulia,Sys.getenv("LD_LIBRARY_PATH"),sep=':'))
  # print(Sys.getenv("LD_LIBRARY_PATH"))
  # print(pkgname)
  # print(libname)
   dyn.load(ldjulia,local=F)
   library.dynam("rjulia",pkgname,libname,local=F)

}

.onUnload <- function(libpath)
 {

 ldjulia<-gsub(pattern="\"", replacement="",system('julia -E "abspath(Sys.dlpath(\\\"libjulia\\\"))"',intern=T))

 dyn.unload(ldjulia)

 library.dynam.unload("rjulia", libpath)
}
