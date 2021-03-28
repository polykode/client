open Relude.Globals;

let (^$) = (f, x) => f(x);

let noop = unit => unit;

let tap = fn => x => fn(x) |> const(x)

let debug = (txt: string, any: 'a): 'a => Js.log2(txt, any) |> const(any);
let debug2 = (txt: string, a: 'a, b: 'b): 'b => Js.log3(txt, a, b) |> const(b);

