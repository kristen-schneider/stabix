#include <pybind11.h>
#include "compress_main.h"
#include "decompress_main.h"
#include "index_main.h"


namespace py = pybind11;

PYBIND11_MODULE(stabixcore, m) {
    m.def("compress", &compress_main, "Pipeline for compressing GWAS files.");
    m.def("index", &index_main, "Pipeline for indexing GWAS files.");
    m.def("decompress", &decompress_main, "Pipeline for decompressing GWAS files.");
}