class FMODUnitializedException implements Exception {
  @override
  String toString() {
    return Error.safeToString('FMOD player was not initialized');
  }
}

class FMODLibraryException implements Exception {
  @override
  String toString() {
    return Error.safeToString('FMOD player unexpected library exception');
  }
}
