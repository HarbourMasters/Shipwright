{
  description = "Shipwright development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    pinned.url = "github:NixOS/nixpkgs/e6f23dc08d3624daab7094b701aa3954923c6bbb";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, pinned, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        pinned-pkgs = pinned.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Build tools
            git
            cmake
            ninja
            lsb-release
            pkg-config

            # SDL2 libraries
            SDL2
            SDL2.dev
            SDL2_net

            # Assets pipeline
            python3
            imagemagick

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
            libx11

            # Audio libraries
            libogg
            libogg.dev
            libvorbis
            libvorbis.dev
            libopus
            libopus.dev
            opusfile
            opusfile.dev

            # Runtime dependencies
            zenity
          ] ++ [
            # Version of clang-format used by decomp
            pinned-pkgs.clang_14
          ];
          shellHook = ''
            echo "Shipwright development environment loaded"
            echo "Available tools: clang, git, cmake, ninja, python3"
          '';
        };
      });
}
