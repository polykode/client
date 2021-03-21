module Js' = Js
open UiUtils
open Relude
/*open CoreUtils*/
module Js = Js'

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

let executeMarkdown = (_id, _mdContent) => Stream.make((~next, ~complete as _, ~cancel as _) => {
  let m = ref(0);
  let t = Js.Global.setInterval(() => {
    m.contents = m.contents + 1;
    next("foobar::" ++ String.fromInt(m.contents))
  }, 3000)

  // TODO: Call cancel when stream ends
  Some(() => Js.Global.clearInterval(t))
});

[@react.component]
let make = (~id: int) => {
  open MarkdownParser;
  let md = make();
  let tokens = defaultContent |> parse(md);
  let htmlStr = tokens |> render(md);
  let codeBlocks = tokens |> CodeBlock.fromMdTokens;

  Js.log(htmlStr);
  Js.log(codeBlocks);

  <div>
    <button
      className=Tailwind.(bg_blue_800)
      /*onClick={_ => dispatch(Run("1", defaultContent))}*/
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
