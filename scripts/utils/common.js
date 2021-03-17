const PostCssPlugin = require('./postcss.plugin');
const tailwindConfig = require('../../tailwind.config');

const ENV = process.env.NODE_ENV || 'development';

const SOURCEDIR = 'src';
const DISTDIR = 'dist';

const outfile = 'bundle.js';

const dev = ENV === 'development';
const optimized = !dev;

//const bucklescriptPlugin = {
	//name: "bucklescript",
	//setup(build) {
		//const extRe = /\.(ml|re|res)$/
		//build.onResolve({ filter: extRe }, args => ({
			//path: console.log(args.path.replace(extRe, '.bs.js'), args) || args.path.replace(extRe, '.bs.js'),
			//namespace: 'module',
		//}));
	//},
//};

module.exports = {
	ENV,
	SOURCEDIR,
	DISTDIR,
	htmlConfig: {
		title: 'Polykode',
		scripts: {
			[`/${outfile}?v=1`]: '',
		},
		styles: {
			[`/bundle.css?v=1`]: '',
		},
	},
	commonConfig: {
		entryPoints: [`${SOURCEDIR}/index.js`],
		bundle: true,
		minify: optimized,
		sourcemap: true,
		define: {
			"process.env.NODE_ENV": JSON.stringify(ENV),
		},
		outfile: `./${DISTDIR}/${outfile}`,
		plugins: [
			//bucklescriptPlugin,
			PostCssPlugin({
				plugins: [
					require('postcss-import'),
					require('tailwindcss')(tailwindConfig),
					require('autoprefixer'),
				],
			}),
		],
	},
};

