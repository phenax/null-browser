{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; config.allowUnsupportedSystem = true; };
        inherit (pkgs) pkgsStatic;

        myPkgs = {
          libuv = pkgsStatic.libuv;
          luajit = pkgsStatic.luajit;
          qt = pkgs.qt6; # pkgsStatic.qt6
          lua-libluv = pkgs.callPackage (import ./nix/libluv.nix) {
            inherit (myPkgs) libuv luajit;
          };
          lua-inspect = pkgs.luajitPackages.inspect; # TODO: include with build?
        };

        dependencies = [
          myPkgs.qt.qtbase
          myPkgs.qt.qtwebengine
          myPkgs.luajit
          myPkgs.libuv
          myPkgs.lua-libluv
          myPkgs.lua-inspect
        ];
      in {
        devShells.default = pkgs.mkShell rec {
          buildInputs = with pkgs; [
            cmake
            ninja
            gnumake
            clang-tools
            pkg-config
            ccache
            gdb
            valgrind
          ] ++ dependencies;

          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "null-browser";
          version = "0.0.0";
          src = ./.;

          buildInputs = dependencies;
          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            myPkgs.qt.wrapQtAppsHook
            pkg-config
          ];
          dontUseCmakeConfigure = true;
          buildPhase = ''make build PREFIX=$out'';
          installPhase = ''make install PREFIX=$out'';

          meta.mainProgram = "null-browser";
        };
      });
}
