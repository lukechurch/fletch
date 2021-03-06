// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

library fletchc;

import 'dart:async';
import 'dart:io';

import 'compiler.dart' show
    FletchCompiler;

import 'commands.dart';

import 'fletch_vm.dart';

import 'fletch_system.dart';

import 'session.dart';

const COMPILER_CRASHED = 253;
const DART_VM_EXITCODE_COMPILE_TIME_ERROR = 254;
const DART_VM_EXITCODE_UNCAUGHT_EXCEPTION = 255;

main(List<String> arguments) async {
  String script;
  String snapshotPath;
  bool debugging = false;
  bool testDebugger = false;
  String testDebuggerCommands = "";
  bool connectToExistingVm = false;
  int existingVmPort = 0;

  for (int i = 0; i < arguments.length; i++) {
    String argument = arguments[i];
    switch (argument) {
      case '-o':
      case '--out':
        snapshotPath = arguments[++i];
        break;

      case '-d':
      case '--debug':
        debugging = true;
        break;

      case '--test-debugger':
        testDebugger = true;
        break;

      default:
        if (argument.startsWith('--test-debugger=')) {
          testDebugger = true;
          testDebuggerCommands = argument.substring(16);
          break;
        }

        if (argument.startsWith('--port=')) {
          connectToExistingVm = true;
          existingVmPort = int.parse(argument.substring(7));
          break;
        }

        if (script != null) throw "Unknown option: $argument";
        script = argument;
        break;
    }
  }

  if (script == null) throw "No script supplied";

  exitCode = COMPILER_CRASHED;

  List<String> options = const bool.fromEnvironment("fletchc-verbose")
      ? <String>['--verbose'] : <String>[];
  // TODO(ajohnsen): packageRoot should be a command line argument.
  FletchCompiler compiler = new FletchCompiler(
      options: options,
      script: script,
      packageRoot: "package/");
  FletchDelta fletchDelta = await compiler.run();

  FletchVm vm;
  if (connectToExistingVm) {
    var socket = await Socket.connect("127.0.0.1", existingVmPort);
    vm = new FletchVm.existing(socket);
  } else {
    vm = await FletchVm.start(compiler);
  }

  var session = new Session(vm.socket, compiler, fletchDelta.system,
                            vm.stdoutSyncMessages, vm.stderrSyncMessages,
                            vm.process != null ? vm.process.exitCode : null);

  await session.runCommands(fletchDelta.commands);
  if (snapshotPath != null) {
    await session.writeSnapshot(snapshotPath);
  } else if (debugging) {
    if (testDebugger) {
      await session.testDebugger(testDebuggerCommands);
    } else {
      await session.debug();
    }
  } else {
    await session.run();
  }
  await session.shutdown();

  if (!connectToExistingVm) {
    exitCode = await vm.process.exitCode;
    if (exitCode != 0) {
      print("Non-zero exit code from "
            "'${compiler.fletchVm.toFilePath()}' ($exitCode).");
    }
    if (exitCode < 0) {
      // TODO(ahe): Is there a better value for reporting a Vm crash?
      exitCode = COMPILER_CRASHED;
    }
  }
}
