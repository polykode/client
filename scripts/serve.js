const { build } = require('esbuild');
const chokidar = require('chokidar');
const liveServer = require('live-server');
const { writeHtml } = require('./utils/build-html');
const { commonConfig, htmlConfig } = require("./utils/common");

const html = () => writeHtml(htmlConfig);

build({
	...commonConfig,
	incremental: true,
}).then(async builder => {
	// Generate html
	await html();

	let isReady = false;

	// Bundle rebuild
	chokidar
		.watch('src/**/*.{js,css}', { interval: 200 })
		.on('ready', () => (isReady = true))
		.on('all', () => {
			if (isReady) {
				console.log(':: Rebuilding...');
				builder.rebuild();
			}
		});

	// html rebuild
	chokidar
		.watch('src/**/*.html', { interval: 0 })
		.on('all', html);

	// Start dev server
	liveServer.start({
		open: false,
		port: +process.env.PORT || 8080,
		root: 'dist',
		middlewares: ['spa'],
		file: '/index.html',
		logLevel: 1,
	});
});

