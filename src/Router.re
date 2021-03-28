open Relude.Globals;
open ReasonReactRouter;

type page =
  | Home
  | About
  | Editor(int)
  | Preview(int)
  | NotFound;

let toPage =
  Option.getOrElse(NotFound)
  << (
    fun
    | [] => Some(Home)
    | ["about"] => Some(About)
    | ["edit", id] => id |> Int.fromString |> Option.map(s => Editor(s))
    | ["preview", id] => id |> Int.fromString |> Option.map(s => Preview(s))
    | _ => None
  );

let fromPage =
  fun
  | Home => "/"
  | About => "/about"
  | Editor(id) => "/edit/" ++ String.fromInt(id)
  | Preview(id) => "/preview/" ++ String.fromInt(id)
  | NotFound => "/404";

let gotoPage = ignore << push << fromPage;

module Link = {
  [@react.component]
  let make = (~page: page, ~children) => <a
    href=""
    onClick={e => {
      ReactEvent.Mouse.preventDefault(e);
      gotoPage(page);
    }}
  >{children}</a>
}

let usePage = toPage << (u => u.path) << useUrl;
