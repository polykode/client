
type t('a) = 'a => bool;

let (<|>) = (a, b, x) => a(x) || b(x);

let (<&>) = (a, b, x) => a(x) && b(x);

let toOption = (p, v) => if (p(v)) Some(v) else None

