// Copyright (c) 2015, the Fletch project authors. Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE.md file.

#include "generated/myapi.h"

#include <stdio.h>
#include <stdlib.h>


static void Setup(char* path);
static void TearDown();

int main(int argc, char** argv) {
  Setup(argv[1]);

  MyApi api = MyApi::create();
  MyObject m0 = api.foo();
  MyObject m1 = api.foo();
  m0.funk(m1);
  m1.funk(m0);
  api.destroy();

  TearDown();
}


// -----------------------------------------------------------------------

#include <pthread.h>

#include "include/fletch_api.h"
#include "include/service_api.h"

static const int kDone = 1;

static pthread_mutex_t mutex;
static pthread_cond_t cond;
static int status = 0;

static void ChangeStatusAndNotify(int new_status) {
  pthread_mutex_lock(&mutex);
  status = new_status;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

static void WaitForStatus(int expected) {
  pthread_mutex_lock(&mutex);
  while (expected != status) pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);
}

static void* FletchThreadEntry(void* arg) {
  const char* path = static_cast<char*>(arg);
  FletchSetup();
  FletchRunSnapshotFromFile(path);
  FletchTearDown();
  ChangeStatusAndNotify(kDone);
  return NULL;
}

static void RunSnapshotInNewThread(char* path) {
  pthread_t thread;
  int result = pthread_create(&thread, NULL, FletchThreadEntry, path);
  if (result != 0) {
    perror("Failed to start thread");
    exit(1);
  }
}

void Setup(char* path) {
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  ServiceApiSetup();
  RunSnapshotInNewThread(path);
}

void TearDown() {
  WaitForStatus(kDone);
  ServiceApiTearDown();
}

