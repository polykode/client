module.exports = {
  plugins: [
    require('postcss-import'),
    require('postcss-preset-env'),
    require('postcss-nested'),
    require('tailwindcss')(require('./tailwind.config')),
    require('autoprefixer'),
  ],
}
