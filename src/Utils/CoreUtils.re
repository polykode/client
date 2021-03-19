let (<<) = (f, g, x) => f(g(x));
let (^$) = (f, x) => f(x);

let const: ('a, 'b) => 'a = (x, _) => x;

let id = x => x;

// NOTE: Don't open Relude here
let debug = (txt: string, any: 'a): 'a => Js.log2(txt, any) |> const(any);
