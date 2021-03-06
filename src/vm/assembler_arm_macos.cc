// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#if defined(FLETCH_TARGET_ARM)

#include <stdio.h>
#include "src/vm/assembler.h"

namespace fletch {

#if defined(FLETCH_TARGET_ANDROID)
static const char* kPrefix = "";
#else
static const char* kPrefix = "_";
#endif

void Assembler::Bind(const char* name) {
  putchar('\n');
  printf("\t.align 4\n");
  printf("\t.code 32\n");
  printf("\t.global %s%s\n%s%s:\n", kPrefix, name, kPrefix, name);
}

void Assembler::DefineLong(const char* name) {
  printf("\t.long %s%s\n", kPrefix, name);
}

const char* Assembler::LabelPrefix() {
  return kPrefix;
}

}  // namespace fletch

#endif  // defined(FLETCH_TARGET_ARM)
