import { render } from 'react-dom';
import { createElement } from 'react';

import { make as App } from './App.bs';

import './style.css';

//import 'hypermd/mode/hypermd';
//import 'codemirror/mode/javascript/javascript';
//import 'codemirror/mode/haskell/haskell';
//import 'codemirror/mode/htmlmixed/htmlmixed';

//import 'hypermd/keymap/hypermd';

const $root = document.createElement('div');
document.body.appendChild($root);

render(createElement(App, {}), $root);

