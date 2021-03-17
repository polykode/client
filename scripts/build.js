process.env.NODE_ENV = process.env.NODE_ENV || 'production';

const { build } = require('esbuild');
const { writeHtml } = require('./utils/build-html');
const { commonConfig, htmlConfig } = require('./utils/common');

build({ ...commonConfig })
	.then(_ => writeHtml(htmlConfig))
	.catch(_e => process.exit(1));
