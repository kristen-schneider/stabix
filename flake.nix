{
  # description = "A simple flake for direnv";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    with pkgs;
    {
      devShells.${system}.default = mkShell {
        buildInputs = [
          # Add your desired packages here
          nodejs
          python3
        ];
      };
    };
}
