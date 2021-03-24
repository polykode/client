// Snowpack Configuration File
// See all supported options: https://www.snowpack.dev/reference/configuration

/** @type { import("snowpack").SnowpackUserConfig } */
module.exports = {
  mount: {
    src: '/',
  },
  //scripts: { 'build:css': 'postcss' },
  plugins: [
    '@snowpack/plugin-postcss',
    '@snowpack/plugin-webpack',
  ],
  packageOptions: {
    polyfillNode: true,
    sourcemap: true,
  },
  devOptions: {
    hostname: '0.0.0.0',
    port: 8080,
    open: 'none',
  },
  routes: [
    { match: 'routes', src: '.*', dest: '/index.html' }
  ],
  buildOptions: {},
};
