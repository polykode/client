open CoreUtils
open Relude

module State = {
  [@bs.deriving accessors]
  type t('a, 'd) = Next('a) | Complete('d) | Cancelled;
}
open State;

type cleanupFn = unit => unit;

type t('a, 'd) =
  (~next: 'a => unit, ~complete: 'd => unit, ~cancel: unit => unit)
    => option(cleanupFn);
type stream('a, 'd) = t('a, 'd) // alias for t

let make: t('a, 'd) => t('a, 'd) = id;

let fork = (handler, stream) => {
  let isComplete = ref(false);
  let cleanupFn = ref(None);
  let cleanup = () =>
    cleanupFn.contents |> Option.map(f => f()) |> ignore;

  let transition = ignore << Option.map(handler) << fun
    | (Complete(_) | Cancelled) as state => {
      isComplete.contents = true;
      cleanup();
      Some(state)
    }
    | state => if (isComplete.contents) None else Some(state);

  let next = transition << State.next;
  let complete = transition << State.complete;
  let cancel = () => transition(Cancelled);

  cleanupFn.contents =
    stream(~next = next, ~complete = complete, ~cancel = cancel);

  cancel
};

let map = (fn, stream) => make((~next, ~complete, ~cancel) =>
  stream |> fork(fun
    | Next(v) => v |> fn |> next |> ignore
    | Complete(x) => x |> complete |> ignore
    | Cancelled => cancel()
  ) |> Option.some);

