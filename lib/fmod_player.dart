import 'dart:async';
import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:fmod_player/util.dart';

class FMODSystem {
  static FMODSystem? _instance;

  late CreateStream _createStream;
  late SetVolume _setVolume;
  late Play _play;
  late Stop _stop;
  late Pause _pause;

  factory FMODSystem() {
    _instance ??= FMODSystem._();
    return _instance!;
  }

  FMODSystem._() {
    final DynamicLibrary fmodPlayerLib = Platform.isAndroid
        ? DynamicLibrary.open('libfmod_player.so')
        : DynamicLibrary.process();

    final _initialize = fmodPlayerLib
        .lookup<NativeFunction<Void Function(Pointer)>>("initialize")
        .asFunction<void Function(Pointer)>();
    _initialize(wrappedPrintPointer);

    _createStream = fmodPlayerLib
        .lookupFunction<CreateStreamNative, CreateStream>('createStream');
    _setVolume =
        fmodPlayerLib.lookupFunction<SetVolumeNative, SetVolume>('setVolume');
    _play = fmodPlayerLib.lookupFunction<PlayNative, Play>('play');
    _stop = fmodPlayerLib.lookupFunction<StopNative, Stop>('stop');
    _pause = fmodPlayerLib.lookupFunction<PauseNative, Pause>('pause');
  }

  Future<void> dispose() async {}
}

class FMODPlayer {
  Pointer<Void>? _nativePlayerRef;

  String filePath;
  double volume;
  bool looped;

  FMODPlayer({
    required this.filePath,
    this.volume = 1,
    this.looped = false,
  });

  Future<void> createStream() async {
    final path = filePath.toNativeUtf8();
    _nativePlayerRef = FMODSystem()._createStream(path, volume, looped);
  }

  Future<void> play() async {
    FMODSystem()._play(_nativePlayerRef!);
  }

  Future<void> stop() async {
    FMODSystem()._stop(_nativePlayerRef!);
  }

  Future<void> pause() async {
    FMODSystem()._pause(_nativePlayerRef!);
  }

  Future<void> setVolume(double volume) async {
    FMODSystem()._setVolume(_nativePlayerRef!, volume);
  }
}
