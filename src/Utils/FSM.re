open Relude;
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

let getStateFromTransition = fun | Pure(s) | EffIO(s, _) | EffStream(s, _) => s;

module MakeStateMachine(M: StateChart) = {
  let transition = (state, msg) => (state, msg) |> M.update;
  let execute = fn => fun
    | Pure(_) => Some(noop)
    | EffIO(_, eff) => eff |> Effect.IOEff.fork(fn) |> Option.some
    | EffStream(_, eff) => eff |> Effect.StreamEff.fork(fn) |> const(None);

  let make = (init, onStateChange) => {
    let state = ref(init);

    let getState = () => state.contents;
    let rec dispatch = act => {
      let setState = s => {
        let hasUpdated = s !== state.contents;
        state.contents = s;
        if (hasUpdated) onStateChange(dispatch, s) |> ignore;
        s;
      };

      act
      |> transition(state.contents)
      |> tap(setState << getStateFromTransition)
      |> execute(ignore << Relude.Option.map(dispatch))
      |> ignore;
    };

    (getState, dispatch);
  }

  let use = init => {
    let (state, setState) = React.useState(_ => init);
    let dispatch = React.useRef(_ => ());

    React.useEffect0(() => {
      let (_, disp) = make(init, (_, s) => setState(_ => s))
      dispatch.current = disp;
      None; // TODO: Cleanup?
    });
    
    (state, dispatch);
  };
}

