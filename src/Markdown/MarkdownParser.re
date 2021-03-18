open Relude
open CoreUtils

/*const md = new MarkdownIt()*/
/*console.log(md.parse(defaultContent));*/

let defaultOptions = [%raw "{
  langPrefix: 'language-',
  highlight: function (/*str, lang*/) { return ''; },
  linkify: true,
  breaks: true,
  xhtmlOut: true,
  html: false,
  quotes: '“”‘’',
  typographer: true,
}"]

module MdIt = {
  type t;
  type token = {
    [@bs.as "type"] kind: string,
    info: string,
    content: string,
  };
  type options;

  [@bs.module "markdown-it"] external make: unit => t = "default";
  [@bs.send "parse"] external parse': (t, string) => array(token) = "parse";
  let parse = parse'; // t => List.fromArray << parse(t)

  type renderer;
  [@bs.get "renderer"] external getRenderer: t => renderer = "renderer";
  [@bs.send "render"] external render': (renderer, array(token), options) => string = "render";
  let render = (t, tokens) => t->getRenderer->render'(_, tokens, defaultOptions);
}

module PState = {
  type t = {
    annotations: option(string)
  }

  let empty = { annotations: None }
}

module CodeBlock = {
  type t = {
    lang: string,
    code: string,
    annotations: option(string),
  }

  let isAnnotation = String.startsWith(~search = "<!--@")

  let codeBlock
    : ((PState.t, MdIt.token)) => t
    = (({ annotations }, token)) => {
      lang: token.info,
      code: token.content,
      annotations: annotations
    }

  let fromMdTokens
    : array((PState.t, MdIt.token)) => array(t)
    = Array.map(codeBlock) << Array.filter(((b: MdIt.token) => b.kind === "fence") << snd)
}

type reducerState = (PState.t, array((PState.t, MdIt.token)))

let normalizeTokens
: reducerState => MdIt.token => reducerState
= ((state, arr), token) => {
  let (nextState, item) = switch token {
  | { kind: "fence" } as t =>
    (PState.empty, (state, t))
  | { kind: "inline" } as t when CodeBlock.isAnnotation(t.content) =>
    ({ annotations: Some(t.content) }, (PState.empty, t))
  | t =>
    (state, (PState.empty, t))
  };

  (nextState, Array.append(item, arr));
};


let make = MdIt.make

let parse = md => snd << Array.foldLeft(normalizeTokens, (PState.empty, [||])) << MdIt.parse(md)

let render = md => MdIt.render(md) << Array.map(snd)

