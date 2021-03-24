// open CoreUtils;
open Relude.Globals;

module State = {
  [@bs.deriving accessors]
  type t('a, 'd) = Next('a) | Complete('d) | Cancelled;
}
open State;

type cleanupFn = unit => unit;

type t('a, 'd) =
  (~next: 'a => unit, ~complete: 'd => unit, ~cancel: unit => unit)
    => option(cleanupFn);

let make
: t('a, 'd) => t('a, 'd)
= id;

let fork
: (State.t('a, 'd) => unit) => t('a, 'd) => cleanupFn
= (handler, stream) => {
  let isComplete = ref(false);
  let cleanupFn = ref(None);
  let cleanup = () =>
    cleanupFn.contents |> Option.map(f => f()) |> ignore;

  let onTransition = fun
    | Complete(_) => {
      isComplete.contents = true;
      // TODO: Should cleanup on complete?
    }
    | Cancelled => {
      isComplete.contents = true;
      cleanup();
    }
    | Next(_) => ();

  let transition = ignore
    << Option.map(handler)
    << Option.tap(onTransition)
    << Predicate.toOption(_ => !isComplete.contents);

  let next = transition << State.next;
  let complete = transition << State.complete;
  let cancel = () => transition(Cancelled);

  cleanupFn.contents =
    stream(~next = next, ~complete = complete, ~cancel = cancel);

  cancel
};

let map
: ('a => 'b) => t('a, 'c) => t('b, 'c)
= (fn, stream) => make((~next, ~complete, ~cancel) =>
  stream |> fork(fun
    | Next(x) => x |> fn |> next |> ignore
    | Complete(x) => x |> complete |> ignore
    | Cancelled => cancel()
  ) |> Option.some);

let const = x => map(Relude.Globals.const(x));

