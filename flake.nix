{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
  };

  outputs = { self, nixpkgs }:
    let
      system = "aarch64-darwin";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    with pkgs;
    {
      devShells.${system}.default = mkShell {
        buildInputs = [
          # Add your desired packages here
          zlib
          libzip
          gcc
          cmake
          simde
          bzip2
        ];
      };
    };
}
