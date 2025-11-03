{
  description = "Shipwright development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Build tools
            clang
            git
            cmake
            ninja
            lsb-release
            pkg-config

            # SDL2 libraries
            SDL2
            SDL2.dev
            SDL2_net

            # Other libraries
            libpng
            libzip
            nlohmann_json
            tinyxml-2
            spdlog
            libGL
            libGL.dev
            bzip2

            # X11 libraries
            xorg.libX11

            # Audio libraries
            libogg
            libogg.dev
            libvorbis
            libvorbis.dev
            libopus
            libopus.dev
            opusfile
            opusfile.dev
          ];

          shellHook = ''
            echo "Shipwright development environment loaded"
            echo "Available tools: clang, git, cmake, ninja"
          '';
        };
      });
}
