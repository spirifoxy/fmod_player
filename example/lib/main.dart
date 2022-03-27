import 'dart:async';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:fmod_player/fmod_player.dart';
import 'package:path_provider/path_provider.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  FMODPlayer? nl1;
  FMODPlayer? nl2;
  late String path;

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  Future<void> initPlatformState() async {
    const asset = 'heavyRain.mp3';
    final fullPath = "assets/$asset";

    ByteData data = await rootBundle.load(fullPath);
    List<int> bytes =
        data.buffer.asUint8List(data.offsetInBytes, data.lengthInBytes);

    Directory appDocDir = await getApplicationDocumentsDirectory();
    path = "${appDocDir.path}$asset";
    await File(path).writeAsBytes(bytes);
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Column(
          children: [
            Row(
              children: [
                const Spacer(),
                Column(
                  children: [
                    ElevatedButton(
                        onPressed: () {
                          nl1 = FMODSystem().createStream(
                              filePath: path, looped: true, volume: 0.1);
                        },
                        child: Text("Load")),
                    ElevatedButton(
                        onPressed: () => nl1!.play(), child: Text("Play")),
                    ElevatedButton(
                        onPressed: () => nl1!.stop(), child: Text("Stop")),
                    ElevatedButton(
                        onPressed: () => nl1!.pause(), child: Text("Pause")),
                    ElevatedButton(
                        onPressed: () => nl1!.setVolume(0.1),
                        child: Text("Vol 0.1")),
                    ElevatedButton(
                        onPressed: () => nl1!.setVolume(0.5),
                        child: Text("Vol 0.5")),
                    ElevatedButton(
                        onPressed: () => nl1!.setVolume(1.0),
                        child: Text("Vol 1.0")),
                  ],
                ),
                const Spacer(),
                Column(
                  children: [
                    ElevatedButton(
                        onPressed: () {
                          nl2 = FMODSystem().createStream(
                              filePath: path, looped: true, volume: 0.1);
                        },
                        child: Text("Load")),
                    ElevatedButton(
                        onPressed: () => nl2!.play(), child: Text("Play")),
                    ElevatedButton(
                        onPressed: () => nl2!.stop(), child: Text("Stop")),
                    ElevatedButton(
                        onPressed: () => nl2!.pause(), child: Text("Pause")),
                    ElevatedButton(
                        onPressed: () => nl2!.setVolume(0.1),
                        child: Text("Vol 0.1")),
                    ElevatedButton(
                        onPressed: () => nl2!.setVolume(0.5),
                        child: Text("Vol 0.5")),
                    ElevatedButton(
                        onPressed: () => nl2!.setVolume(1.0),
                        child: Text("Vol 1.0")),
                  ],
                ),
                const Spacer(),
              ],
            ),
            Row(
              children: [
                const Spacer(),
                ElevatedButton(
                    onPressed: () => FMODSystem().dispose(),
                    child: Text("Dispose 'em all!")),
                const Spacer(),
              ],
            ),
            const Spacer(),
          ],
        ),
      ),
    );
  }
}
