open Relude;
open CoreUtils;

let text = React.string;
let number = text << String.fromInt;

type reducerTransition('s, 'a) =
  | Pure('s)
  | EffIO('s, Effect.IOEff.t(option('a)))
  | EffStream('s, Effect.StreamEff.t(option('a)))

let getState = fun
  | Pure(s) => s
  | EffIO(s, _) | EffStream(s, _) => s

let forkEffect = fn => fun
  | Pure(_) => noop
  | EffIO(_, eff) => eff |> Effect.IOEff.fork(fn)
  | EffStream(_, eff) => eff |> Effect.StreamEff.fork(fn)

// | Point-free reducer (style choice)
let pfReducer = (fn, s, a) => fn((s, a));

let useMegaReducer = (reducer, init) => {
  let (effState, dispatch) = React.useReducer(reducer << getState, init);

  React.useEffect1(
    () => effState
      |> forkEffect(ignore << Option.map(dispatch))
      |> Option.some,
    [|effState|],
  );

  (effState |> getState, dispatch);
};
