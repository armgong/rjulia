library(rjulia)
julia_init()
julia_LoadDataArrayFrame()
x <- matrix(1:9, 3,3)
x[c(1,5,8)] <- NA
x
r2j(x,"xy")
str( j2r("xy") )
str( j2r("length(xy)") )
str( j2r("length(xy.na)") )
str( j2r("length(xy.data)") )
