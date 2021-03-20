open Relude;
open CoreUtils;

let defaultOptions = [%raw "{
  langPrefix: 'language-',
  highlight: function (/*str, lang*/) { return ''; },
  linkify: true,
  breaks: true,
  xhtmlOut: true,
  html: false,
  quotes: '“”‘’',
  typographer: true,
}"];

module Token = {
  [@bs.deriving accessors]
  type t = {
    [@bs.as "type"] kind: string,
    info: string,
    content: string,
  };
};

module MdIt = {
  type t;
  type options;

  [@bs.module "markdown-it"] external make: unit => t = "default";
  [@bs.send "parse"] external parse': (t, string) => array(Token.t) = "parse";
  let parse = parse';

  type renderer;
  [@bs.get "renderer"] external getRenderer: t => renderer = "renderer";
  [@bs.send "render"]
  external render': (renderer, array(Token.t), options) => string = "render";
  let render: (t, array(Token.t)) => string =
    (t, tokens) => t->getRenderer->render'(_, tokens, defaultOptions);
};

module PState = {
  type t = {annotations: option(string)};
  let empty = {annotations: None};
};

module CodeBlock = {
  [@bs.deriving accessors]
  type t = {
    lang: string,
    code: string,
    annotations: option(string),
  };

  let isAnnotation = Predicate.(
    (String.startsWith(~search="<!--@") << Token.content)
      <&> (String.eq("inline") << Token.kind)
  );

  let isCodeBlock = String.eq("fence") << Token.kind

  let codeBlock: ((PState.t, Token.t)) => t =
    (({annotations}, token)) => {
      lang: token.info,
      code: token.content,
      annotations,
    };

  let fromMdTokens =
    Array.map(codeBlock) << Array.filter(isCodeBlock << snd);
};

let normalizeTokens = ((state, arr), token) => {
  let (nextState, item) =
    switch (token) {
    | t when CodeBlock.isCodeBlock(t) =>
      (PState.empty, (state, t))
    | t when CodeBlock.isAnnotation(t) =>
      ({annotations: Some(t.content)}, (PState.empty, t))
    | t =>
      (state, (PState.empty, t))
    };

  (nextState, Array.append(item, arr));
};

let make = MdIt.make;

let parse = md =>
  snd << Array.foldLeft(normalizeTokens, (PState.empty, [||])) << MdIt.parse(md);

let render = md => MdIt.render(md) << Array.map(snd);
