open CoreUtils
module Option = Relude.Option

type state('a, 'd) = Next('a) | Complete('d) | Cancelled;

type unsubFn = unit => unit;

type t('a, 'd) =
  (~next: 'a => unit, ~complete: 'd => unit, ~cancel: unit => unit)
    => option(unsubFn);

let make: t('a, 'd) => t('a, 'd) = id;

let subscribe = (handler, stream) => {
  let unsubfn = ref(None);
  let cleanup = () =>
    unsubfn.contents |> Option.map(f => f()) |> ignore;

  let isComplete = ref(false);

  let transition = ignore << Option.map(handler) << fun
    | (Complete(_) | Cancelled) as state => {
      isComplete.contents = true;
      cleanup();
      Some(state)
    }
    | state => if (isComplete.contents) None else Some(state);

  let next = value => transition(Next(value))
  let complete = value => transition(Complete(value));
  let cancel = () => transition(Cancelled);

  unsubfn.contents =
    stream(~next = next, ~complete = complete, ~cancel = cancel);

  cancel
};

let map = (fn, stream) => make((~next, ~complete, ~cancel) =>
  stream |> subscribe(fun
    | Next(v) => v |> fn |> next |> ignore
    | Complete(x) => x |> complete |> ignore
    | Cancelled => cancel()
  ) |> Option.some);

