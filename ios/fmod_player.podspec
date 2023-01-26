#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint fmod_player.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'fmod_player'
  s.version          = '0.3.0'
  s.summary          = 'A new Flutter plugin project.'
  s.description      = <<-DESC
A new Flutter plugin project.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.dependency 'Flutter'
  s.platform = :ios, '9.0'

  s.swift_version = '5.0'

  s.frameworks = ['AudioToolbox', 'AVFoundation']

  s.public_header_files = [
    'Classes/**/*.h',
    'core/inc/*.{h,hpp}',
  ]
  s.source_files = [
    'Classes/**/*',
    'core/inc/*.{c,h}',
  ]
  s.compiler_flags = [
    '-w',
  ]

  s.pod_target_xcconfig = {
      # Enable equivalent of '-Isrc/include' to make '#include <fmod.h>' work
    'HEADER_SEARCH_PATHS' => [
      '$(PODS_TARGET_SRCROOT)/../lib/core/inc',
    ],

    'LIBRARY_SEARCH_PATHS' => [
      '$(PODS_TARGET_SRCROOT)/../fmod/lib/ios',
    ],

    'OTHER_LDFLAGS[config=Debug]' => '$(inherited) -framework Flutter -lstdc++ -lfmodL_$(PLATFORM_NAME)',
    'OTHER_LDFLAGS[config=Release]' => '$(inherited) -framework Flutter -lstdc++ -lfmod_$(PLATFORM_NAME)',

    'CLANG_CXX_LIBRARY' => 'libc++',

    'DEFINES_MODULE' => 'YES',

    # Flutter.framework does not contain a i386 slice.
    # Only x86_64 simulators are supported.
    #'VALID_ARCHS[sdk=iphonesimulator*]' => 'x86_64'
  }
end
