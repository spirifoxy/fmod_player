import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';

final DynamicLibrary fmodPlayerLib = Platform.isAndroid
    ? DynamicLibrary.open('libfmod_player.so')
    : DynamicLibrary.process();

typedef DisposeNative = Pointer<Void> Function();
typedef Dispose = Pointer<Void> Function();

typedef MixerSuspendNative = Pointer<Void> Function();
typedef MixerSuspend = Pointer<Void> Function();
typedef MixerResumeNative = Pointer<Void> Function();
typedef MixerResume = Pointer<Void> Function();

typedef CreateStreamNative = Pointer<Void> Function(
  Pointer<Utf8> path,
  Double volume,
  Bool looped,
);
typedef CreateStream = Pointer<Void> Function(
  Pointer<Utf8> path,
  double volume,
  bool looped,
);

typedef SetVolumeNative = Void Function(
  Pointer<Void> handle,
  Double volume,
);
typedef SetVolume = void Function(
  Pointer<Void> handle,
  double volume,
);

typedef PlayNative = Void Function(Pointer<Void> handle);
typedef Play = void Function(Pointer<Void> handle);

typedef StopNative = Pointer<Void> Function(Pointer<Void> handle);
typedef Stop = Pointer<Void> Function(Pointer<Void> handle);

typedef PauseNative = Void Function(Pointer<Void> handle);
typedef Pause = void Function(Pointer<Void> handle);
