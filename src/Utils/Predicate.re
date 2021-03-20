
type t('a) = 'a => bool;

let (<|>) = (a, b, x) => a(x) || b(x);

let (<&>) = (a, b, x) => a(x) && b(x);

let boolToOption = (p, v) => if (p) Some(v) else None

