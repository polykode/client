open Relude.Globals;
open CoreUtils;

[@bs.deriving accessors]
type transitionType('s, 'm) =
  | Pure('s)
  | EffIO('s, Effect.IOEff.t(option('m)))
  | EffStream('s, Effect.StreamEff.t(option('m)))

module type StateChart = {
  type state;
  type msg;

  let update: ((state, msg)) => transitionType(state, msg);
};

module MakeStateMachine(M: StateChart) = {
  let transition = (state, msg) => (state, msg) |> M.update;
  let getStateFromTransition = fun | Pure(s) | EffIO(s, _) | EffStream(s, _) => s;

  let execute = fn => fun
    | Pure(_) => Some(noop)
    | EffIO(_, eff) => eff |> Effect.IOEff.fork(fn) |> Option.some
    | EffStream(_, eff) => eff |> Effect.StreamEff.fork(fn) |> const(None);

  let use = init => {
    let (state, setState) = React.useState(_ => init);
    let dispatchRef = React.useRef((_) => ());

    dispatchRef.current = React.useCallback1(
      ignore
      << execute(s => s |> Relude.Option.map(dispatchRef.current) |> ignore) // Note: pointful for a reason
      << tap(setState << const << getStateFromTransition)
      << transition(state)
    , [|state|]);

    (state, dispatchRef.current);
  };
}

