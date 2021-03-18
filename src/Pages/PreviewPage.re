open UiUtils;
/*open CoreUtils*/

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
  let codeBlocks = tokens |> CodeBlock.fromMdTokens;

  Js.log(htmlStr);
  Js.log(codeBlocks);

  <div>
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
