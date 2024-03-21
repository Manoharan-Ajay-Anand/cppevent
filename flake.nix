{
  description = "Nix flake for cppevent";

  inputs.nixpkgs.url = github:NixOS/nixpkgs;

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
    outpkgs = self.packages.${system}; 
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      src = builtins.path {
        path = ./.;
      };
      name = "cppevent-1.0";
      inherit system;
      nativeBuildInputs = [pkgs.cmake pkgs.doctest];
      buildInputs = [pkgs.liburing];
    };
    devShells.${system}.default = pkgs.mkShell {
      packages = [pkgs.gdb];
      inputsFrom = [outpkgs.default];
      shellHook = ''
        cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCPPEVENT_DEVEL=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build -S .
      '';
    };
  };
}
