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

let make = MdIt.make

type pState = {
  annotation: MdIt.token
}

type reducerState = (option(pState), array((option(pState), MdIt.token)))

let normalizeTokens
: reducerState => MdIt.token => reducerState
= ((state, arr), token) => {
  let (nextState, group) = switch token {
  | { kind: "fence" } as t =>
    (None, (state, t))
  | { kind: "inline" } as t when String.startsWith(~search = "<!--@", t.content) =>
    (Some({ annotation: t }), (None, t))
  | t =>
    (state, (None, t))
  };

  (nextState, Array.append(group, arr));
};

let parse = md => snd << Array.foldLeft(normalizeTokens, (None, [||])) << MdIt.parse(md)

let render = md => MdIt.render(md) << Array.map(snd)

