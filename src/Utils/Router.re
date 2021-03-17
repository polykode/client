open Relude;
open CoreUtils;
open ReasonReactRouter;

type page =
  | Home
  | About
  | Editor(int)
  | Preview(int)
  | NotFound;

let toPage = path => {
  let route =
    switch (path) {
    | [] => Some(Home)
    | ["about"] => Some(About)
    | ["edit", id] => id |> Int.fromString |> Option.map(s => Editor(s))
    | ["preview", id] => id |> Int.fromString |> Option.map(s => Preview(s))
    | _ => None
    };

  route |> Option.getOrElse(NotFound);
};

let fromPage =
  fun
  | Home => "/"
  | About => "/about"
  | Editor(id) => "/editor/" ++ String.fromInt(id)
  | Preview(id) => "/preview/" ++ String.fromInt(id)
  | NotFound => "/404"

let gotoPage = push << fromPage

let useRouter = () => {
  let url = useUrl();
  url.path |> toPage;
};
