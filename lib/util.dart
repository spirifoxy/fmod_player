import 'dart:ffi';

import 'package:ffi/ffi.dart';
import 'package:flutter/foundation.dart' as foundation;

/// Debug print function is passed to the cpp code since it's not possible to
/// output to console natively
void wrappedPrint(Pointer<Utf8> arg) {
  if (!foundation.kDebugMode) {
    return;
  }
  print(arg.toDartString());
}

typedef WrappedPrintNative = Void Function(Pointer<Utf8> a);
final wrappedPrintPointer =
    Pointer.fromFunction<WrappedPrintNative>(wrappedPrint);

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

typedef StopNative = Void Function(Pointer<Void> handle);
typedef Stop = void Function(Pointer<Void> handle);

typedef PauseNative = Void Function(Pointer<Void> handle);
typedef Pause = void Function(Pointer<Void> handle);
