open Relude.Globals;

type cleanup = unit => unit

module type EFF = {
  type t('a)
  let fork: ('a => unit) => t('a) => cleanup;
}

module StreamEff: EFF with type t('a) = Stream.t('a, Void.t) = {
  type t('a) = Stream.t('a, Void.t);

  let fork = fn => Stream.fork(fun | Next(d) => fn(d) | _ => ());
};

module IOEff: EFF with type t('a) = IO.t('a, Void.t) = {
  type t('a) = IO.t('a, Void.t);

  let fork = fn => const(() => ()) << IO.unsafeRunAsync(ignore << Result.map(fn));
};
