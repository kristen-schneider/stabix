#include <pybind11.h>
#include <pybind11/stl.h>
#include "compress_main.h"
#include "decompress_main.h"
#include "index_main.h"
#include "stabix_except.h"


namespace py = pybind11;

PYBIND11_MODULE(stabixcore, m) {
    // Register the custom exception
    py::register_exception<StabixExcept>(m, "StabixError");

    m.def("compress", &compress_main_by_map, "Pipeline for compressing GWAS files.");
    m.def("index", &index_main_by_map, "Pipeline for indexing GWAS files.");
    m.def("decompress", &decompress_main_by_map, "Pipeline for decompressing GWAS files.");
}
