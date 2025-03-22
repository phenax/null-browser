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
          qt6.full
          luajit
          libuv
          luajitPackages.libluv
          # libcef
          # nss
        ];
      in {
        devShells.default = pkgs.mkShell rec {
          buildInputs = with pkgs; [
            cmake
            gnumake
            clang-tools
            pkg-config
            gdb
            # vcpkg
          ] ++ dependencies;

          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "null-browser";
          version = "0.0.0";
          src = ./.;

          buildInputs = with pkgs; [
            qt6.qtbase
          ] ++ dependencies;
          nativeBuildInputs = with pkgs; [ cmake qt6.wrapQtAppsHook pkg-config ];
        };
      });
}
