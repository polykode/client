open Relude.Globals;
open UiUtils;

let codeBlocks = [|
  {
    "lang": "javascript",
    "code": "
const pow = 3;
console.log(8 ** pow);",
  },
  {
    "lang": "shell",
    "code": "
echo hello world beoeeye;
echo 2 3 41",
  }
|];


[@react.component]
let make = (~id: int) => {

  <div>
    <div className=Tailwind.(bg_blue_500)> "Header "->text id->number </div>
    <div className=Tailwind.(bg_blue_100 <+> rounded_md <+> shadow_md <+> p_6)>
      {codeBlocks |> Array.map(block => {
        <div className=Tailwind.(p_5) key={block##lang ++ block##code}>
          <CodeBlock
            height="300px"
            value={block##code}
            lang={block##lang}
          />
        </div>
      }) |> React.array}
    </div>
  </div>;
};
