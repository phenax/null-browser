{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";

    lua-luv-source = {
      url = "git+https://github.com/luvit/luv?submodules=1";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, lua-luv-source, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        dependencies = with pkgs; [
          qt6.full
          luajit
          libuv
          luajitPackages.libluv
          # luajitPackages.luv
          # libcef
          # nss
        ];
      in {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "null-browser";
          version = "0.0.0";
          src = ./.;

          buildInputs = with pkgs; [
            qt6.qtbase
          ] ++ dependencies;
          nativeBuildInputs = with pkgs; [ cmake qt6.wrapQtAppsHook pkg-config ];
        };

        devShells.default = pkgs.mkShell rec {
          buildInputs = with pkgs; [
            cmake
            gnumake
            clang-tools
            pkg-config
            # vcpkg
          ] ++ dependencies;

          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
        };
      });
}
