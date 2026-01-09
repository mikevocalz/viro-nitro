module.exports = {
  dependency: {
    platforms: {
      android: {
        componentDescriptors: [],
        cmakeListsPath: 'src/main/cpp/CMakeLists.txt',
        packageImportPath: 'import com.margelo.nitro.kimoyooju.KimoyoOjuPackage;',
        packageInstance: 'new KimoyoOjuPackage()',
      },
    },
  },
};
