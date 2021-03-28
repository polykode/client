open Relude.Globals;

module MonacoEditor = {
  [@bs.module "@monaco-editor/react"] [@react.component]
  external make: (
    ~height: string,
    ~defaultLanguage: string,
    ~defaultValue: string,
    ~theme: string,
  ) => React.element = "default";
};

module Block = {
  type result = string;

  [@bs.deriving accessors]
  type t = {
    name: string,
    lang: string,
    code: string,
    result: option(result),
  }

  module BlockResult = {
    [@bs.deriving accessors]
    type blockResult = {
      stdout: string,
    };

    [@bs.deriving accessors]
    type t = {
      result: blockResult,
    };
  }

  module Response = {
    [@bs.deriving accessors]
    type t = {
      result: array(BlockResult.t),
    };

    [@bs.scope "JSON"] [@bs.val]
    external parseJson: string => t = "parse"
  }

  let getResults =
    Array.map(BlockResult.(stdout << result)) << Response.result << Response.parseJson;

  let addResults = (blocks, results) =>
    Array.zip(blocks, results) |> Array.map(((block, res)) => {
      ...block,
      result: Some(res)
    });
};

[@react.component]
let make = (
  ~height,
  ~lang,
  ~value,
) => <MonacoEditor
  height
  defaultLanguage=lang
  defaultValue=value
  theme="vs-dark"
/>;

