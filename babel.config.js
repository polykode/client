module.exports = {
  presets: [
    ['@babel/preset-env', {
      modules: 'cjs',
      targets: {
        node: 11,
      }
    }]
  ]
};
