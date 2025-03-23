{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        lua-libluv = with pkgs; luajitPackages.libluv.overrideAttrs (self: rec {
          version = "1.50.0-1";
          knownRockspec = (fetchurl {
            url    = "mirror://luarocks/luv-${version}.rockspec";
            sha256 = "sha256-IL2EejtmT0pw0cAupMz0gvP3a19NPsc45W1RaoeGJgY=";
          }).outPath;
          src = fetchurl {
            url    = "https://github.com/luvit/luv/releases/download/${version}/luv-${version}.tar.gz";
            sha256 = "sha256-2GfDAk2cmB1U8u3YPhP9bcEVjwYIY197HA9rVYa1vDQ=";
          };
        });

        dependencies = with pkgs; [
          qt6.full
          luajit
          libuv
          lua-libluv
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
            valgrind
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

          nativeBuildInputs = with pkgs; [
            cmake
            qt6.wrapQtAppsHook
            pkg-config
          ];
        };
      });
}
