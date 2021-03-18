open UiUtils
open CoreUtils

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
  let md = MarkdownParser.make();
  let htmlStr = MarkdownParser.(defaultContent |> parse(md) |> render(md));

  Js.log(htmlStr);

  <div>
    <div className={Tailwind.(bg_blue_500)}>
      {"HEader"->text}
      {id->number}
    </div>
    <div
      className={Tailwind.(
        bg_blue_100
        <+> rounded_md
        <+> shadow_md
        <+> p_6
        <+> "markdown-container"
      )}
      dangerouslySetInnerHTML={{ "__html": htmlStr }}
    />
  </div>
};
