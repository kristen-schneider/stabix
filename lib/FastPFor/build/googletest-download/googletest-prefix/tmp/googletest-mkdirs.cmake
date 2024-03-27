# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-src"
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-build"
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix"
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix/tmp"
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix/src/googletest-stamp"
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix/src"
  "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix/src/googletest-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix/src/googletest-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/krsc0813/CLionProjects/gwas_local/lib/FastPFor/build/googletest-download/googletest-prefix/src/googletest-stamp${cfgdir}") # cfgdir has leading slash
endif()
