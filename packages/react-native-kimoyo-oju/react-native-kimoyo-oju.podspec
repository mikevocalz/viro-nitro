require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

Pod::Spec.new do |s|
  s.name         = "react-native-kimoyo-oju"
  s.version      = package['version']
  s.summary      = package['description']
  s.homepage     = package['homepage']
  s.license      = package['license']
  s.authors      = package['author']

  s.platforms    = { :ios => "13.0" }
  s.source       = { :git => "https://github.com/user/kimoyo-oju.git", :tag => "#{s.version}" }

  s.source_files = [
    "ios/**/*.{h,m,mm,swift}",
    "cpp/**/*.{h,hpp,cpp}",
    "../viro-engine/cpp/include/**/*.{h,hpp}",
    "../viro-engine/cpp/src/**/*.{h,hpp,cpp,mm}"
  ]

  s.exclude_files = [
    "../viro-engine/cpp/src/graphics/GLDevice.cpp",
    "../viro-engine/cpp/src/xr/ARCoreSession.cpp"
  ]

  s.public_header_files = [
    "ios/**/*.h"
  ]

  s.swift_version = "5.0"

  # Dependencies
  s.dependency "React-Core"
  s.dependency "react-native-nitro-modules"

  # Frameworks
  s.frameworks = [
    "Metal",
    "MetalKit", 
    "ARKit",
    "CoreVideo",
    "QuartzCore",
    "Accelerate"
  ]

  # Compiler settings
  s.pod_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++20",
    "CLANG_CXX_LIBRARY" => "libc++",
    "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) KIMOYO_OJU_PLATFORM_APPLE=1",
    "HEADER_SEARCH_PATHS" => [
      "$(PODS_TARGET_SRCROOT)/cpp",
      "$(PODS_TARGET_SRCROOT)/../viro-engine/cpp/include",
      "$(PODS_TARGET_SRCROOT)/../viro-engine/cpp/src"
    ].join(" "),
    "OTHER_CPLUSPLUSFLAGS" => "-fexceptions -frtti"
  }

  s.user_target_xcconfig = {
    "CLANG_CXX_LANGUAGE_STANDARD" => "c++20"
  }

  # Resource bundles (for Metal shaders)
  s.resource_bundles = {
    "KimoyoOjuShaders" => ["ios/Shaders/**/*.metal"]
  }
end
