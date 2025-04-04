{ luajitPackages, luajit, libuv, fetchurl, ... }:
let
  lua-libluv = luajitPackages.libluv.overrideAttrs (oldAttrs: rec {
    version = "1.50.0-1";
    buildInputs = [luajit libuv];
    knownRockspec = (fetchurl {
      url    = "mirror://luarocks/luv-${version}.rockspec";
      sha256 = "sha256-IL2EejtmT0pw0cAupMz0gvP3a19NPsc45W1RaoeGJgY=";
    }).outPath;
    src = fetchurl {
      url    = "https://github.com/luvit/luv/releases/download/${version}/luv-${version}.tar.gz";
      sha256 = "sha256-2GfDAk2cmB1U8u3YPhP9bcEVjwYIY197HA9rVYa1vDQ=";
    };
  });

  # TODO: Figure out static linking
  # lua-libluv = with pkgs; stdenv.mkDerivation rec {
  #   version = "1.50.0-1";
  #   pname = "libluv";
  #   src = fetchgit {
  #     url = "https://github.com/luvit/luv";
  #     tag = version;
  #     sha256 = "sha256-pmp/lUX2lAFMZA8LiL21G0fNFLEOpNWLp5UPBubTQeM=";
  #     fetchSubmodules = true;
  #     leaveDotGit = true;
  #   };
  #   cmakeFlags = [
  #     "-DBUILD_MODULE=OFF"
  #     "-DBUILD_STATIC_LIBS=ON"
  #     "-DWITH_LUA_ENGINE=LuaJIT"
  #     "-DWITH_SHARED_LIBUV=OFF"
  #     "-DLUA_BUILD_TYPE=Static"
  #   ];
  #   postInstall = ''
  #     mkdir -p $out/lib/pkgconfig;
  #     cp $src/libluv.pc.in $out/lib/pkgconfig/libluv.pc;
  #     substituteInPlace $out/lib/pkgconfig/libluv.pc \
  #       --replace "@prefix@" "$out" \
  #       --replace "@libdir@" "$out/lib" \
  #       --replace "@includedir@" "$out/include" \
  #       --replace "@LUV_VERSION@" ${version} \
  #       --replace "@LIBS" "";
  #   '';
  #   buildInputs = [pkgs.luajit pkgs.libuv];
  #   nativeBuildInputs = [cmake git];
  # };
in
  lua-libluv
