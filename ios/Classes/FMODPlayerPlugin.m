#import "FMODPlayerPlugin.h.back"
#if __has_include(<fmod_player/fmod_player-Swift.h>)
#import <fmod_player/fmod_player-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "fmod_player-Swift.h"
#endif

@implementation FMODPlayerPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftFMODPlayerPlugin registerWithRegistrar:registrar];
}
@end
