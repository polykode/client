module Option = Relude.Option;

type state('a) = Next('a) | Complete('a) | Cancelled;

type unsubFn = unit => unit;

type t('a) = (~next: 'a => unit, ~complete: 'a => unit, ~cancel: unit => unit) => option(unsubFn);

let make = cb: t('a) => cb;

let subscribe = (handler, stream) => {
  let unsubfn = ref(None);
  let next = value => handler(Next(value))
  let complete = value => handler(Complete(value))
  let cancel = () => {
    unsubfn.contents |> Option.map(f => f()) |> ignore;
    handler(Cancelled);
  };

  unsubfn.contents = stream(~next = next, ~complete = complete, ~cancel = cancel);

  cancel
};

let map = (fn, stream) => make((~next, ~complete, ~cancel) =>
  stream |> subscribe(fun
    | Next(v) => v |> fn |> next |> ignore
    | Complete(x) => x |> complete |> ignore
    | Cancelled => cancel()
  ) |> Option.some);

