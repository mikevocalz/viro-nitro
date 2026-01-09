const path = require('path');

module.exports = {
  project: {
    android: {
      sourceDir: './android',
    },
    ios: {
      sourceDir: './ios',
    },
  },
  dependencies: {
    'react-native-kimoyo-oju': {
      root: path.resolve(__dirname, '../../packages/react-native-kimoyo-oju'),
    },
  },
};
