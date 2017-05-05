var webpack = require("webpack");
var folder = __dirname;

module.exports = {
  entry: ['./app/index.js'],
  output: {
    path: folder+'/build',
    filename: 'bundle.js'
  },
  module: {
    loaders: [ 
      {
      loader: 'babel-loader',
      test: /\.js$/,
      exclude: /node_modules/
      }
    ]
  },
  devServer: {
    port: 3000,
    contentBase: './build',
    inline: true
  }
}
