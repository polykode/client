const path = require('path');
const glob = require('glob');
const TerserPlugin = require('terser-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const PurgeCSSPlugin = require('purgecss-webpack-plugin');
const { CleanWebpackPlugin: Cleanup } = require('clean-webpack-plugin');
const HtmWebpackPlugin = require('html-webpack-plugin');

const buildDir = path.resolve('./build');
const srcDir = path.resolve('./src');

const ENV = process.env.NODE_ENV || 'development';

const production = ENV === 'production';

module.exports = {
  entry: path.join(srcDir, 'index.js'),
  mode: ENV,
  output: {
    path: buildDir,
    publicPath: '/',
    filename: '[name].[contenthash].js',
    chunkFilename: '[name].[contenthash].js',
  },
  optimization: {
    minimize: production,
    moduleIds: 'natural',
    minimizer: [
      new TerserPlugin({
        parallel: true,
        terserOptions: {
          mangle: true,
          compress: {},
        },
      }),
    ],
  },
  module: {
    rules: [
      {
        test: /\.js$/i,
        use: [
          {
            loader: 'babel-loader',
          }
        ],
      },
      {
        test: /\.css$/i,
        use: [
          !production
            ? 'style-loader'
            : MiniCssExtractPlugin.loader,
          'css-loader',
          'postcss-loader',
        ],
      },
      {
        test: /\.(png|jpg|gif|ttf|eot|woff|woff2)$/i,
        use: [
          {
            loader: 'url-loader',
            options: { limit: 8192 }
          }
        ]
      },
    ],
  },
  plugins: [
    new HtmWebpackPlugin({ title: 'Loading... | Polykode' }),
    new Cleanup(),
  ].concat(!production ? [] : [
    new PurgeCSSPlugin({
      paths: glob.sync(path.join(buildDir, './**/*.js'), { nodir: true }).concat([
        path.join(srcDir, './index.html'),
      ]),
    }),
    new MiniCssExtractPlugin({
      filename: '[name].[contenthash].css',
      chunkFilename: '[name].[contenthash].css',
    }),
  ]),
  devServer: {
    compress: true,
    host: '0.0.0.0',
    port: 8080,
    historyApiFallback: true,
    publicPath: '/',
  },
};
