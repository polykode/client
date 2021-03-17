{}:
with import <nixpkgs> {};
let
  ocaml-version = "4.06.1";
  bsCompiler = let
    version = "9.0.2";
  in
    pkgs.stdenv.mkDerivation {
      name = "rescript-compiler--${version}";
      inherit version;
      src = pkgs.fetchFromGitHub {
        owner = "rescript-lang";
        repo = "rescript-compiler";
        rev = version;
        sha256 = "1hql7sxps1k17zmwyha6idq6nw20abpq770l55ry722birclmsmf";
        fetchSubmodules = true;
      };

      buildInputs = [ nodejs-15_x python3 gnumake gcc ninja ocaml ];

      BS_RELEASE_BUILD = "true";
      BS_TRAVIS_CI = "1";

      prePatch = ''
        sed -i 's:./configure.py --bootstrap:python3 ./configure.py --bootstrap:' ./scripts/install.js
        mkdir -p ./native/${pkgs.ocaml.version}/bin
        ln -sf ${pkgs.ocaml.version}/bin/*  ./native/${pkgs.ocaml.version}/bin
      '';

      dontConfigure = true;

      buildPhase = ''
        node scripts/buildocaml.js;
        pwd;ls linux;
        exit 1;
        npm run postinstall;
        ./scripts/ninja.js config;
        ./scripts/ninja.js build;
      '';

      installPhase = ''
        mkdir -p $out/bin
        cp -rf jscomp lib linux vendor odoc_gen native bsb bsc bsrefmt $out
        mkdir -p $out/lib/ocaml
        cp jscomp/runtime/js.* jscomp/runtime/*.cm* $out/lib/ocaml
        cp jscomp/others/*.ml jscomp/others/*.mli jscomp/others/*.cm* $out/lib/ocaml
        cp jscomp/stdlib-406/*.ml jscomp/stdlib-406/*.mli jscomp/stdlib-406/*.cm* $out/lib/ocaml
        cp bsconfig.json package.json $out
        ln -s $out/bsb $out/bin/bsb
        ln -s $out/bsc $out/bin/bsc
        ln -s $out/bsrefmt $out/bin/bsrefmt
      '';
    };

  ocamlDeps = with ocamlPackages; with nodePackages; [
    ocaml
    opam
    merlin
    ocp-indent
    ocaml-language-server
    #bsCompiler
    #bs-platform
  ];
  systemDeps = with pkgs; [
    nodejs-15_x
    yarn
    docker-compose
  ];
in
stdenv.mkDerivation {
  name = "bs-optics";
  buildInputs = systemDeps ++ ocamlDeps;
}
