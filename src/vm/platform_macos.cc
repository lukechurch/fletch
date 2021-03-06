// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "src/vm/platform.h"

#include <CoreFoundation/CFTimeZone.h>

namespace fletch {

int Platform::GetLocalTimeZoneOffset() {
  CFTimeZoneRef tz = CFTimeZoneCopySystem();
  // Even if the offset was 24 hours it would still easily fit into 32 bits.
  int offset = CFTimeZoneGetSecondsFromGMT(tz, CFAbsoluteTimeGetCurrent());
  CFRelease(tz);
  // Note that Unix and Dart disagree on the sign.
  return static_cast<int>(-offset);
}

}  // namespace fletch

