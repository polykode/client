
module MonacoEditor = {
  [@bs.module "@monaco-editor/react"] [@react.component]
  external make: (
    ~height: string,
    ~defaultLanguage: string,
    ~defaultValue: string,
    ~theme: string,
  ) => React.element = "default";
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

