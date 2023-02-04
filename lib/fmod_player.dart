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

  late MixerSuspend _mixerSuspend;
  late MixerResume _mixerResume;

  late CreateStream _createStream;
  late SetVolume _setHVolume;
  late Play _playH;
  late Stop _stopH;
  late Pause _pauseH;

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

    _mixerSuspend = fmodPlayerLib.lookupFunction<MixerSuspendNative, MixerSuspend>('mixerSuspend');
    _mixerResume = fmodPlayerLib.lookupFunction<MixerSuspendNative, MixerSuspend>('mixerResume');

    _createStream = fmodPlayerLib
        .lookupFunction<CreateStreamNative, CreateStream>('createStream');
    _setHVolume =
        fmodPlayerLib.lookupFunction<SetVolumeNative, SetVolume>('setHVolume');
    _playH = fmodPlayerLib.lookupFunction<PlayNative, Play>('playH');
    _stopH = fmodPlayerLib.lookupFunction<StopNative, Stop>('stopH');
    _pauseH = fmodPlayerLib.lookupFunction<PauseNative, Pause>('pauseH');
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

  static void mixerSuspend() {
    if (_instance == null) {
      throw FMODUnitializedException();
    }

    FMODSystem()._mixerSuspend();
  }

  static void mixerResume() {
    if (_instance == null) {
      throw FMODUnitializedException();
    }

    FMODSystem()._mixerResume();
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
    FMODSystem()._playH(_nativePlayerRef);
  }

  Future<void> stop() async {
    _nativePlayerRef = FMODSystem()._stopH(_nativePlayerRef);
  }

  Future<void> pause() async {
    FMODSystem()._pauseH(_nativePlayerRef);
  }

  Future<void> setVolume(double volume) async {
    FMODSystem()._setHVolume(_nativePlayerRef, volume);
  }
}
