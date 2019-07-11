const webpack = require('webpack');
const path = require('path');
const CleanWebpackPlugin = require("clean-webpack-plugin");
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const { HashedModuleIdsPlugin } = require('webpack');

module.exports = {
    entry: {
        main: path.resolve(__dirname, "src/assets/js/main.js")
    },
    output: {
        path: path.resolve(__dirname, 'dist'),
        publicPath: "/",
        filename: 'assets/js/[name].[chunkhash].js',
        chunkFilename: 'assets/js/[name].[chunkhash].js'
    },
    optimization: {
        runtimeChunk: {
            name: 'manifest'
        },
        splitChunks: {
            cacheGroups: {
                vendor: {
                    test: /node_modules\/(.*)\.js/,
                    name: 'vendor',
                    chunks: "all"
                }
            }
        }
    },
    module: {
        rules: [{
            test: /\.js$/,
            use: {
                loader: 'babel-loader',
                options: {
                    presets: ['@babel/preset-env']
                }
            },
            exclude: /node_modules/,
            include: '/src/'
        }, {
            test: /(\.css|\.scss|\.sass)$/,
            use: [MiniCssExtractPlugin.loader, 'css-loader', 'sass-loader', {
                loader: 'postcss-loader',
                options: {
                    plugins: () => [require('autoprefixer')({
                        'browsers': ['> 1%', 'last 2 versions']
                    })]
                }

            }]
        },{
            test: /\.html$/,
            use: {
                loader: 'html-loader',
                options: {
                    minimize: true,
                    removeComments: false,
                    collapseWhitespace: false
                }
            }
        }]
    },
    plugins: [
        //清空dist
        new HashedModuleIdsPlugin(),
        new CleanWebpackPlugin(["dist"], {
            root: '',
            verbose: true,
            dry: false
        }),
        new MiniCssExtractPlugin({
            filename: 'assets/css/[name].[chunkhash].min.css',
            chunkFilename: 'assets/css/[name].[chunkhash].css'
        }),
        new HtmlWebpackPlugin({
            template: './src/index.html',
            inject: 'body',
            hash: false,
            minify: {
                removeComments: true,
                collapseWhitespace: true
            }
        })

    ]
};