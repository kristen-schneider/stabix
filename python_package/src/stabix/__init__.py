from dataclasses import dataclass
from typing import Dict, Literal, Union

# INFO: stabixcore SHOULD NOT BE USED DIRECTLY. Use this interface instead.
import stabix.stabixcore as _core

# Codecs are specified for a column by datatype
codecMap = Union[str, Dict[Literal["int", "float", "string"], str]]


@dataclass
class Stabix:
    """
    Stabix class members help make usage more concise.
    The gwas_file parameter is sometimes required depending
    on the methods called.
    """

    index_dir: str
    gwas_file: str = None

    def compress(
        self,
        block_size: int = None,
        map_file: str = None,
        codecs: str | codecMap = None,
    ):
        if self.gwas_file is None:
            raise StabixError("gwas_file must be provided")

        if (block_size is None) == (map_file is None):
            raise StabixError("only one of block_size or map_file should be provided")

        # block_size is immediately parsed to int again in c++,
        # but needs to be a string to be passed to c++ for type consistency
        block_size = map_file or str(block_size)

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
                "index_dir": self.index_dir,
                "block_size": block_size,
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

        if self.gwas_file is None:
            raise StabixError("gwas_file must be provided")

        index_name = f"col_{col_idx}"
        _core.index(
            {
                "gwas_file": self.gwas_file,
                "index_dir": self.index_dir,
                "col_idx": str(col_idx),
                "bins": ",".join(map(str, bins)),
                "extra_index": index_name,
            }
        )

    def query(
        self, bed_file: str, out_path: str, col_idx: int = None, threshold: str = None
    ):
        """
        Such as .query("file.bed", 8, "<= 0.3")
        """

        if (col_idx is None) != (threshold is None):
            raise StabixError(
                "either both or neither of col_idx & threshold should be specified"
            )

        _core.decompress(
            {
                "index_dir": self.index_dir,
                "out_path": out_path,
                "genomic": bed_file,
                "extra_index": f"col_{col_idx}",
                "col_idx": str(col_idx),
                "threshold": threshold,
            }
        )


class StabixError(Exception):
    pass
