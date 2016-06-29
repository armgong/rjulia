#!/usr/bin/env Rscript
#####
#####  A script to run tests on the rjulia package

suppressMessages(library(rjulia))
suppressMessages(library(RUnit))
julia_init()

testsuite.rjulia <- defineTestSuite("rjulia.test.suite",
                                      dirs = "unit",
                                      testFileRegexp = "^test_.+\\.R",
                                      testFuncRegexp = "^test_.+",
                                      rngKind = "default",
                                      rngNormalKind = "default")

results <- capture.output(runTestSuite(testsuite.rjulia))

## Finish up
quit(runLast=FALSE)
