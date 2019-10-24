let
    nixpkgs = import <nixpkgs> {};
in 
with nixpkgs;

stdenv.mkDerivation {
    name = "textedit";
    buildInputs = [
        gcc
        gnumake
        xlibsWrapper
        pkgconfig
        gdb
        qtcreator
    ];
}
