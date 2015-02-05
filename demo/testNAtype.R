library(rjulia)

julia_init()
## needed for NA's (!)
julia_void_eval("using DataArrays")
## ?? julia_void_eval("using DataArrays, DataFrames")

x <- 1:10
x[2] <- NA
r2j(x,"ttt")
y <- j2r("ttt[10]")
y
y <- j2r("ttt[2]")
y
y <- j2r("ttt")
y

x <- c(TRUE, NA, TRUE,TRUE,FALSE,FALSE,FALSE)
r2j(x,"ttt")
y <- j2r("ttt[3]")
y
y <- j2r("ttt[2]")
y
y <- j2r("ttt")
y

x <- 1.1:10.1
x[2] <- NA
r2j(x,"ttt")
y <- j2r("ttt[10]")
y
y <- j2r("ttt[2]")
y
y <- j2r("ttt")
y

x <- c("x", NA, "z","u","v","w","a")
r2j(x,"ttt")
y <- j2r("ttt[3]")
y
y <- j2r("ttt[2]")
y
y <- j2r("ttt")
y
