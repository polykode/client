// Copyright 2007 Gage Peterson
// Copyright 2007 Murphy Randle

// Copied from + Licensed under : https://github.com/afidegnum/elm-tailwind

const postcss = require('postcss');
const _ = require('lodash');

const fs = require('fs');
const read = (fn) => fs.readFileSync(fn, 'utf8');
const tailwindBaseWithDarkPath = './node_modules/tailwindcss/dist/tailwind-dark.css';

const outputFile = './src/vendor/Tailwind.re';

const objToMinimalElmTailwindClassFunction = obj => `
let ${obj.elmName} = "${obj.name}"
`;


const tailwindBaseWithDarkTemplate = objects =>
`
let group = "group"
let darkClass = "dark"

let (<+>) = (c1, c2) => c1 ++ " " ++ c2;

let (<?>) = (cond, cls) => if (cond) cls else "";

${objects.map(objToMinimalElmTailwindClassFunction).join('')}`;


// minified css introduces problems with compound selectors
const contexts = {
  'dark': {
    'cssIn': postcss.parse(read(tailwindBaseWithDarkPath)),
    'template': tailwindBaseWithDarkTemplate,
    'objects': {},
    'elmOut': outputFile,
  },
}

const ruleFormatter = (rule) => {
  let def = rule.toString().replace('{-', '{ -');
  def = setCorrectIndentation(def);
  return def;
};

const defaultIndentation = ' '.repeat(4);

const setCorrectIndentation = (text) => {
  // normalize indentation
  if (/ {4}/.test(text)) {
    text = text.replace(/\n {2}/g, '\n');
  }
  // set indentation
  text = text.replace(/\n/g, `\n${defaultIndentation}`);
  return text;
};

/**
 * This will walk through each of the css rules in tailwind
 * and pull out the relevant information.
 */
_.forOwn(contexts, ((ctx, ctxId) => { ctx.cssIn.walkRules((rule) => {
  //
  // Ignore anything that's not a class
  //
  if (!rule.selector.startsWith('.')) return;

  //
  // Ignore group-hover class
  //
  if (rule.selector.startsWith('.group:hover ')) return;

  //
  // Ignore dark class
  //
  if (rule.selector.startsWith('.dark ')) return;

  //
  // Ignore rules that select an :after or :before
  //
  if (rule.selector.search(/:after$/) != -1 || rule.selector.search(/:before$/) != -1) {
    return;
  }

  //
  // Ignore rules with :: modifies (for custom-forms)
  //
  if (rule.selector.search(/::/) !== -1) return;  // -1 indicates no match

  //
  // Ignore rules that select :checked or modifiers starting with -ms- (for custom-forms)
  //
  if (rule.selector.indexOf(':checked') != -1 || rule.selector.indexOf(':-ms') != -1) {
    return;
  }


  let name = rule.selector;

  //
  // Remove the leading dot including leading whitespace
  //
  name = name.replace(/^\s*\./, '');

  //
  // Throw away anything after a comma or a space
  //
  name = name.split(/[, ]/)[0];

  //
  // Replace the "\32xl" with "2xl"
  //
  name = name.replace(/\\32xl/g, '2xl');

  //
  // Replace the "\\\:" with ":"
  //
  name = name.replace(/\\\\:/g, ':');

  //
  // Replace the "\:" with ":"
  //
  name = name.replace(/\\:/g, ':');

  //
  // Remove :focus
  //
  name = name.replace(/:focus$/, '');

  //
  // Remove :focus
  //
  name = name.replace(/:focus-within$/, '');

  //
  // Remove :hover
  //
  name = name.replace(/:hover$/, '');
  
  //
  // Cleanup escaping from fractions
  //
  name = name.replace(/(\d+)\\\/(\d+)/g, '$1/$2');
  
  //
  // Cleanup escaping from decimal dots
  //
  name = name.replace(/(\d+)\\\.(\d+)/g, '$1.$2');


  let elmName = name;

  //
  // Replace the negative margin with a 'neg'
  //
  elmName = elmName.replace(/-m([lrtbxy])/g, 'neg_m$1');

  //
  // Change a leading dash to a 'neg'
  //
  elmName = elmName.replace(/^-/, 'neg');
  elmName = elmName.replace(/:-/, ':neg');

  //
  // Replace dashes with underscores
  //
  elmName = elmName.replace(/-/g, '_');

  //
  // Replace dashes with underscores
  //
  elmName = elmName.replace(/2xl:/g, 'xxl:');

  //
  // Change decimal dot (only in .5) to 'dot'
  //
  elmName = elmName.replace(/(\d+)\.(\d)/, '$1dot$2');

  //
  // Change the \/ in fractions to `over`
  //
  elmName = elmName.replace(/\//g, 'over');

  //
  // Replace the ":" with a "__"
  //
  elmName = elmName.replace(/:/g, '__');

  const obj = {
    name,
    elmName,
    def: ruleFormatter(rule),
  };

  // console.log(obj);

  if (name in contexts[ctxId]['objects']) { contexts[ctxId]['objects'][name].def += `\n${defaultIndentation}${obj.def}`; } // class has been already registered, only append new def
  else contexts[ctxId]['objects'][name] = obj;
})}));


// sort the output alphabetically
_.forOwn(contexts, ((ctx, ctxId) => { contexts[ctxId]['objects'] = _.sortBy(ctx['objects'], 'name'); }));


// render templates and write files
_.forOwn(contexts, ((ctx, ctxId) => {
  fs.writeFile(ctx['elmOut'], ctx['template'](ctx['objects']), (err) => {
    if (err) return console.log(err);
    console.log('\nSuccessfully generated reasonml file: ', ctx['elmOut']);
  })
}));
