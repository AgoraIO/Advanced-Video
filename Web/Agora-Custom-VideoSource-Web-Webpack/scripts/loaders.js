const path = require("path");

const env = process.env;
const MiniCssExtractPlugin = require("mini-css-extract-plugin");

const styleLoader = env.NODE_ENV !== 'production' ? "style-loader" : MiniCssExtractPlugin.loader;

const babelLoader = {
  test: /\.m?js$/,
  exclude: /(node_modules)/,
  use: {
    loader: 'babel-loader',
    options: {
      presets: ['@babel/preset-env']
    }
  }
};

const cssLoader = {
  test: /\.css$/,
  use: [
    styleLoader,
    "css-loader",
  ],
};

const fileLoader = {
  test: /\.(png|svg|jpg|gif|m4a|mp3)$/,
  use: "file-loader",
};

const fontLoader = {
  test: /\.(woff|woff2|eot|ttf|otf)$/,
  use: "file-loader",
};

const eslintLoader = {
  test: /\.js$/,
  include: /src/,
  use: {
    loader: "eslint-loader"
  }
};

module.exports = {
  rules: [
    cssLoader,
    fileLoader,
    fontLoader,
    babelLoader,
    eslintLoader,
  ],
};
