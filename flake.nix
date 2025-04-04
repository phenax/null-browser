{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; config.allowUnsupportedSystem = true; };

        myPkgs = with pkgs; {
          libuv = pkgsStatic.libuv;
          luajit = pkgsStatic.luajit;
          qt = qt6; # pkgsStatic.qt6
          lua-libluv = lua-libluv;
        };

        lua-libluv = pkgs.callPackage (import ./nix/libluv.nix) {
          inherit (myPkgs) libuv luajit;
        };

        dependencies = [
          myPkgs.qt.qtbase
          myPkgs.qt.qtwebengine
          myPkgs.luajit
          myPkgs.libuv
          myPkgs.lua-libluv
        ];
      in {
        devShells.default = pkgs.mkShell rec {
          buildInputs = with pkgs; [
            cmake
            gnumake
            clang-tools
            pkg-config
            gdb
            valgrind
          ] ++ dependencies;

          nativeBuildInputs = with pkgs; [gobject-introspection];

          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
          FOOBARITY = "${lua-libluv}";
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "null-browser";
          version = "0.0.0";
          src = ./.;

          buildInputs = [
            myPkgs.qt.qtbase
          ] ++ dependencies;

          NIX_CFLAGS_COMPILE = "-DRELEASE=1";

          nativeBuildInputs = with pkgs; [
            cmake
            myPkgs.qt.wrapQtAppsHook
            pkg-config
          ];
        };
      });
}
