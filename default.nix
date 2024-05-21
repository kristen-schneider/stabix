with import <nixpkgs> {};
stdenv.mkDerivation {
    name = "dev";
    buildInputs = [
      # pkg-config
      zlib
      libzip
      gcc
      cmake
      simde
      bzip2
    ];
}
