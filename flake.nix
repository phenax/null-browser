{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    let
      shell = { pkgs, ... }:
        pkgs.mkShell rec {
          buildInputs = with pkgs; [
            cmake
            gnumake
            pkg-config
            clang-tools

            pkgs.qt6.full
            luajit
            # lua51Packages.lua
            # libcef
            # nss
          ];
          nativeBuildInputs = [];

          # CEF_PACKAGE_PATH = "${pkgs.libcef}";
          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath (buildInputs ++ nativeBuildInputs)}";
        };
    in flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = shell { inherit pkgs system; };
      });
}
