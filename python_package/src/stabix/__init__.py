# INFO: stabixcore SHOULD NOT BE USED DIRECTLY. Use this interface instead.
from typing import Dict, Literal, Union

import stabix.stabixcore as _core

# Codecs are specified for a column by datatype
codecMap = Union[str, Dict[Literal["int", "float", "string"], str]]


class StabixIndex:
    def __init__(self, gwas_file: str, block_size: int, name: str):
        # NOTE: block_size can be -1 which indicates map file;
        # not yet tested via python wrapper
        self.gwas_file = gwas_file
        # block_size is immediately parsed to int again in c++,
        # but needs to be a string to be passed to c++ for type consistency
        self.block_size = str(block_size)
        self.name = name

    def compress(self, codecs: str | codecMap = None):
        if not isinstance(codecs, str):
            int_codec = codecs.get("int", "bz2")
            float_codec = codecs.get("float", "bz2")
            string_codec = codecs.get("string", "bz2")
        else:
            int_codec = codecs or "bz2"
            float_codec = codecs or "bz2"
            string_codec = codecs or "bz2"

        _core.compress(
            {
                "gwas_file": self.gwas_file,
                "block_size": self.block_size,
                "out_name": self.name,
                "int": int_codec,
                "float": float_codec,
                "string": string_codec,
            }
        )

    def add_threshold_index(self, col_idx: int, bins: list[float]):
        """
        bins indicates a series of "cuts" used to define the boundaries between bins.
        for example, [0.3, 0.7] indicates 3 bins: (< 0.3), [0.3 to 0.7), [>= 0.7)
        """
        # TODO: make the bin specification less confusing

        index_name = f"col_{col_idx}"
        _core.index(
            {
                "gwas_file": self.gwas_file,
                "block_size": self.block_size,
                "out_name": self.name,
                "col_idx": str(col_idx),
                "bins": ",".join(map(str, bins)),
                "extra_index": index_name,
            }
        )

    def query(self, bed_file: str, col_idx: int = None, threshold: str = None):
        """
        Such as .query("file.bed", 8, "<= 0.3")
        """

        # TODO: why is there no query_out path??

        if (col_idx is None) != (threshold is None):
            raise StabixError(
                "either both or neither of col_idx & threshold should be specified."
            )

        _core.decompress(
            {
                "gwas_file": self.gwas_file,
                "block_size": self.block_size,
                "out_name": self.name,
                "genomic": bed_file,
                "extra_index": f"col_{col_idx}",
                "col_idx": str(col_idx),
                "threshold": threshold,
            }
        )


class StabixError(Exception):
    pass
