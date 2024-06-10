{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-22.11";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-darwin" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.buildEnv {
            name = "cmake-gcc-env";
            paths = [
              pkgs.cmake_3_24_3
              pkgs.gcc11
            ];
          };
        }
      );

      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            buildInputs = [
              pkgs.cmake_3_24_3
              pkgs.gcc11
            ];
          };
        }
      );
    };
}
