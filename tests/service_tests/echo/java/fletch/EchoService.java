// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

package fletch;

public class EchoService {
  public static native void Setup();
  public static native void TearDown();
  public static native int Echo(int arg);
}