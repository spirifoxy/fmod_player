import 'dart:async';
import 'dart:ffi';

import 'package:ffi/ffi.dart';
import 'package:flutter/foundation.dart' as foundation;
import 'package:fmod_player/exceptions.dart';
import 'package:fmod_player/util.dart';

/// Debug print function is passed to the cpp code since it's not possible to
/// output to console natively
void wrappedPrint(Pointer<Utf8> arg, bool isError) {
  if (foundation.kDebugMode) {
    print(arg.toDartString());
  }

  if (isError && FMODSystem.errorLoggerCallback != null) {
    FMODSystem.errorLoggerCallback!(arg.toDartString());
  }
}

typedef WrappedPrintNative = Void Function(Pointer<Utf8> arg, Bool isError);
final wrappedPrintPointer =
    Pointer.fromFunction<WrappedPrintNative>(wrappedPrint);

class FMODSystem {
  static FMODSystem? _instance;

  static void Function(String)? errorLoggerCallback;

  late Dispose _dispose;
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
    final _initialize = fmodPlayerLib
        .lookup<NativeFunction<Void Function(Pointer)>>("initialize")
        .asFunction<void Function(Pointer)>();
    _initialize(wrappedPrintPointer);

    _dispose = fmodPlayerLib.lookupFunction<DisposeNative, Dispose>('dispose');

    _createStream = fmodPlayerLib
        .lookupFunction<CreateStreamNative, CreateStream>('createStream');
    _setVolume =
        fmodPlayerLib.lookupFunction<SetVolumeNative, SetVolume>('setVolume');
    _play = fmodPlayerLib.lookupFunction<PlayNative, Play>('play');
    _stop = fmodPlayerLib.lookupFunction<StopNative, Stop>('stop');
    _pause = fmodPlayerLib.lookupFunction<PauseNative, Pause>('pause');
  }

  static FMODPlayer createStream({
    required String filePath,
    volume = 1,
    looped = false,
  }) {
    final path = filePath.toNativeUtf8();
    final nativePlayerRef = FMODSystem()._createStream(path, volume, looped);

    if (nativePlayerRef == nullptr) {
      throw FMODUnitializedException();
    }

    return FMODPlayer._(nativePlayerRef, filePath, volume, looped);
  }

  static void dispose() {
    final call = _instance != null
        ? FMODSystem()._dispose
        : fmodPlayerLib.lookupFunction<DisposeNative, Dispose>('dispose');

    call();
    _instance = null;
  }
}

class FMODPlayer {
  Pointer<Void> _nativePlayerRef;

  String filePath;
  double volume;
  bool looped;

  FMODPlayer._(
    this._nativePlayerRef,
    this.filePath,
    this.volume,
    this.looped,
  );

  Future<void> play() async {
    FMODSystem()._play(_nativePlayerRef);
  }

  Future<void> stop() async {
    _nativePlayerRef = FMODSystem()._stop(_nativePlayerRef);
  }

  Future<void> pause() async {
    FMODSystem()._pause(_nativePlayerRef);
  }

  Future<void> setVolume(double volume) async {
    FMODSystem()._setVolume(_nativePlayerRef, volume);
  }
}
