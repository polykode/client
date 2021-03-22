/*open Relude.Globals;*/
open UiUtils;

let defaultContent = "
# Hello world

```js
const hello = \"world\";

console.log(hello);
```

## Hello
Some content for fun

<!--@
(module hsmain)
-->
```haskell
main = do
  putStrLn \"foobar\"
  let n = 2000
   in print 3000
```

<script>alert(1)</script>
";

[@react.component]
let make = (~id: int) => {
  open MarkdownParser;
  let md = make();
  let tokens = defaultContent |> parse(md);
  let htmlStr = tokens |> render(md);
  /*let codeBlocks = tokens |> CodeBlock.fromMdTokens;*/
  /*Js.log(codeBlocks);*/

  let (state, dispatch) = PolykodeCore.Machine.use(Websockets.Idle);

  React.useEffect0(() => {
    dispatch(Websockets.Connect);
    None;
  });

  React.useEffect1(() => {
    Js.log2(">> State change", state |> Websockets.showState);
    None;
  }, [|state|]);

  <div>
    <button
      className=Tailwind.(bg_blue_800)
      onClick={_ => dispatch(Websockets.SendData("foobar boeyz"))}
    >
      "Run"->text
    </button>
    <div className=Tailwind.(bg_blue_500)> "HEader"->text id->number </div>
    <div
      className=Tailwind.(
        bg_blue_100
        <+> rounded_md
        <+> shadow_md
        <+> p_6
        <+> "markdown-container"
      )
      dangerouslySetInnerHTML={"__html": htmlStr}
    />
  </div>;
};
