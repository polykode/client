open Belt

let text = React.string

type effect ('a) = unit => Js.Promise.t(option('a));
type reducer ('s, 'a) = 's => 'a => ('s, effect('a));

let noop: effect('a) = () => Js.Promise.resolve(None)

let pfReducer
: ((('s, 'a)) => 'r) => 's => 'a => 'r
= (fn, s, a) => fn((s, a))

let useMegaReducer
: reducer('s, 'a) => 's => ('s, 'a => unit)
= (reducer, init) => {
  let reducer' = ((state, _), msg) => reducer(state, msg);
  let ((state, effect), dispatch) = React.useReducer(reducer', (init, noop));

  React.useEffect1(() => {
    let _ = effect()
      |> Js.Promise.then_(d => {
        let _ = d->Option.map(dispatch);
        ()->Js.Promise.resolve;
      })
    None;
  }, [|effect|]);

  (state, dispatch)
};

