open UiUtils;

let defaultContent = "
# Hello world

```js
const hello = \"world\";

console.log(hello);
```
";

[@react.component]
let make = (~id: int) => {
  <div>
    <div className=Tailwind.(bg_blue_500)> "HEader"->text id->number </div>
    <div
      className=Tailwind.(bg_blue_100 <+> rounded_md <+> shadow_md <+> p_6)
      contentEditable=true>
      defaultContent->text
    </div>
  </div>;
};
