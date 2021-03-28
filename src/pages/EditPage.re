open Relude.Globals;
open UiUtils;

let defaultCodeBlocks: array(CodeBlock.Block.t) = [|
  {
    name: "1",
    lang: "javascript",
    result: None,
    code: "const pow = 3; console.log(8 ** pow);",
  },
  {
    name: "2",
    lang: "shell",
    result: None,
    code: "echo hello world beoeeye; echo 2 3 41",
  }
|];


[@react.component]
let make = (~id: int) => {
  let (state, dispatch) = PolykodeCore.Machine.use(Websockets.Idle);
  let (codeBlocks, setCodeBlocks) = React.useState(_ => defaultCodeBlocks);

  React.useEffect0(() => {
    dispatch(Websockets.Connect);
    None;
  });

  React.useEffect1(() => {
    Js.log2("CurrentState", state |> Websockets.showState);

    switch state {
      | ConnectedRxData(_, data) => {
        setCodeBlocks(codeBlocks => data
          |> CodeBlock.Block.getResults
          |> CodeBlock.Block.addResults(codeBlocks)
        );
      }
      | _ => ()
    }
    None;
  }, [|state|]);

  let executeBlocks = _ => {
    let data = Js.Json.stringifyAny({
      "action": "blocks/execute",
      "id": "test",
      "blocks": codeBlocks,
    });
    data |> Option.map(dispatch << Websockets.sendData) |> ignore;
  };

  <div>
    <div>
      <div className=Tailwind.(bg_blue_500)>
        {"Header "->text} {id->number}
        <button onClick={executeBlocks} className=Tailwind.(bg_red_300 <+> p_2)>
          "EXECUTE BLOCKS"->text
        </button>
      </div>
      <div className=Tailwind.(bg_blue_100 <+> rounded_md <+> shadow_md <+> p_6)>
        {codeBlocks |> Array.map((block: CodeBlock.Block.t) => {
          <div className=Tailwind.(p_5) key={block.name}>
            <CodeBlock
              height="300px"
              value={block.code}
              lang={block.lang}
            />
            <div>
              <pre>
                <code>
                  {block.result |> Option.getOrElse("") |> text}
                </code>
              </pre>
            </div>
          </div>
        }) |> React.array}
      </div>
    </div>
  </div>;
};
