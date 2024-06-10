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
    {
      devShells.${system}.default = pkgs.mkShell with pkgs {
        buildInputs = [
          # Add your desired packages here
          nodejs
          python3
        ];
      };
    };
}
