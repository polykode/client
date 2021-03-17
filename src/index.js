import { render } from 'react-dom';
import { createElement } from 'react';

import { make as App } from './App.bs';

import './style.css';

const $root = document.createElement('div');
document.body.appendChild($root);

render(createElement(App, {}), $root);

