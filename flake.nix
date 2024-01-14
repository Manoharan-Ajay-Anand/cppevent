{
  description = "Nix flake for cppevent";

  inputs.nixpkgs.url = github:NixOS/nixpkgs;

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      src = builtins.path {
        path = ./.;
      };
      name = "cppevent-1.0";
      inherit system;
      nativeBuildInputs = [pkgs.cmake];
      buildInputs = [pkgs.liburing];
    };
    devShells.${system}.default = pkgs.mkShell {
      packages = [pkgs.cgdb];
      inputsFrom = [self.packages.${system}.default];
    };
  };
}
