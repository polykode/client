open Relude;
open CoreUtils;

let text = React.string;
let number = text << String.fromInt;

[@bs.deriving accessors]
type reducerTransition('s, 'a) =
  | Pure('s)
  | EffIO('s, Effect.IOEff.t(option('a)))
  | EffStream('s, Effect.StreamEff.t(option('a)))

let getState = fun
  | Pure(s) => s
  | EffIO(s, _) | EffStream(s, _) => s

// Note:
// IO will be cancelled if state or effect updates
// Stream will not be cancelled
let forkEffect = fn => fun
  | Pure(_) => Some(noop)
  | EffIO(_, eff) => eff |> Effect.IOEff.fork(fn) |> Option.some
  | EffStream(_, eff) => eff |> Effect.StreamEff.fork(fn) |> const(None)

// | Point-free reducer (style choice)
let pf = (fn, s, a) => fn((s, a));

let useMegaReducer = (reducer, init) => {
  let (effState, dispatch) = React.useReducer(pf(reducer) << getState, init);

  React.useEffect1(
    () => effState
      |> debug(">> Update")
      |> forkEffect(ignore << Option.map(dispatch)),
    [|effState|],
  );

  (effState |> getState, dispatch);
};
