{
  description = "Nix flake for cppevent";

  inputs.nixpkgs.url = github:NixOS/nixpkgs;

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in {
    devShells.${system}.default = (pkgs.mkShell.override { stdenv = pkgs.gcc13Stdenv; }) {
      packages = [pkgs.gdb pkgs.valgrind pkgs.cmake pkgs.doctest pkgs.liburing pkgs.openssl];
      shellHook = ''
        cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build -S .
      '';
    };
  };
}