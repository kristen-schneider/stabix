{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
  };

  outputs = { self, nixpkgs }:
    let
      # TODO: generalize flake
      system = "aarch64-darwin";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    with pkgs;
    {
      devShells.${system}.default = mkShell {
        buildInputs = [
          gcc
          cmake
          simde
          libzip # TODO: necessary?
          zlib
          bzip2
          xz
          zstd
        ];
      };
    };
}
