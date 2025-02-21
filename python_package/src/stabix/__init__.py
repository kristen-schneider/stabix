# index, compress, decompress
# INFO: stabixcore SHOULD NOT BE USED DIRECTLY. Use this interface instead.
import stabix.stabixcore as _core


# NOTE: block_size can be -1


class StabixIndex:
    def __init__(self, gwas_file, block_size, name):
        self.gwas_file = gwas_file
        self.block_size = block_size
        self.name = name

    def compress(self, codecs):
        _core.compress({
            "gwas_file": self.gwas_file,
            "block_size": self.block_size,
            "out_name": self.name,
            "codecs": codecs
        })

    def add_threshold_index(self, col_idx, bins):
        index_name = f"col_{col_idx}"
        _core.index({
            "gwas_file": self.gwas_file,
            "block_size": self.block_size,
            "out_name": self.name,
            "col_idx": col_idx,
            "bins": bins,
            "extra_index": index_name
        })

    def query(self, bed_file, col_idx=None, threshold=None):
        # TODO: why is there no query_out path??
        if col_idx is None:
            threshold = None

        _core.decompress({
            "gwas_file": self.gwas_file,
            "block_size": self.block_size,
            "out_name": self.name,
            "genomic": bed_file,
            "extra_index": f"col_{col_idx}",
            "col_idx": col_idx,
            "threshold": threshold
        })


# def compress():
    # gwas_file
    # out_name
    # block_size
    # codecs

# def threshold_index():
    # gwas file
    # block size
    # out _name  # NOT USED: _directory

    # extra index  ( name of second index to generate )
    # col idx (pVal col idx)
    # bins

# def decompress():
    # gwas file
    # block size
    # out

    # genomic ( query bed file )
    # extra index
    # col idx ( used for filtering :( )
    # threshold ( for pval idx )