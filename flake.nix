{
  description = "Nix flake for cppevent";

  inputs.nixpkgs.url = github:NixOS/nixpkgs;

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in {
    packages.${system} = {
      default = pkgs.gcc13Stdenv.mkDerivation {
        src = builtins.path {
          path = ./.;
        };
        name = "cppevent-1.0";
        inherit system;
        nativeBuildInputs = [pkgs.cmake pkgs.doctest];
        buildInputs = [pkgs.liburing];
      };
    };
  };
}