{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        dependencies = with pkgs; [
          pkg-config
          qt6.full
          qt6.qtbase
          luajit
          # libcef
          # nss
        ];
      in {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "web-browser";
          version = "0.0.0";
          src = ./.;

          buildInputs = dependencies;
          nativeBuildInputs = with pkgs; [ cmake qt6.wrapQtAppsHook ];
        };

        devShells.default = pkgs.mkShell rec {
          buildInputs = with pkgs; [
            cmake
            gnumake
            clang-tools
          ] ++ dependencies;

          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
        };
      });
}
