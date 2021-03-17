import { render } from 'react-dom';
import { createElement } from 'react';

import { make as App } from './App.bs';

const $root = document.createElement('root');
document.body.appendChild($root);

render(createElement(App, {}), $root);

