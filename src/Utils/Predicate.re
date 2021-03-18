
type t('a) = 'a => bool;

let (<|>) = (a, b, x) => a(x) || b(x);

let (<&>) = (a, b, x) => a(x) && b(x);
