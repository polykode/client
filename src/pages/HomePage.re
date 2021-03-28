/*open Relude.Globals;*/
open UiUtils;
/*open CoreUtils;*/
/*open Webapi.Dom;*/

[@react.component]
let make = () => {
  <div>
    <div className=Tailwind.(bg_green_300)> "Home is where home is"->text </div>
    <Router.Link page={Router.Editor(200)}>
      "Goto editor"->text
    </Router.Link>
  </div>
};
