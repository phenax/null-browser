{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    let
      shell = { pkgs, ... }:
        pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            gnumake
            pkgs.qt6.full
            clang-tools
          ];
        };
    in flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = shell { inherit pkgs system; };
      });
}
