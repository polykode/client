open Relude.Globals;

let log2 = Js.log2;

let (^$) = (f, x) => f(x);

let noop = unit => unit;

let tap = fn => x => fn(x) |> const(x)

let debug = (txt: string, any: 'a): 'a => log2(txt, any) |> const(any);

