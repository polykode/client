open Relude;
open CoreUtils;

let text = React.string;
let number = text << String.fromInt;

type effect('a) = IO.t(option('a), Void.t);

type reducerTransition('s, 'a) =
  | Pure('s)
  | Effectful('s, effect('a));

let getState =
  fun
  | Pure(s) => s
  | Effectful(s, _) => s;

let getEffect =
  fun
  | Pure(_) => IO.pureWithVoid(None)
  | Effectful(_, e) => e;

// | Point-free reducer (style choice)
let pfReducer = (fn, s, a) => fn((s, a));

let useMegaReducer = (reducer, init) => {
  let (effState, dispatch) = React.useReducer(reducer << getState, init);

  React.useEffect1(
    () =>
      effState
      |> getEffect
      |> IO.unsafeRunAsync(ignore << Result.map(Option.map(dispatch)))
      |> const(None),
    [|effState|],
  );

  (effState |> getState, dispatch);
};
