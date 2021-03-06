/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <folly/SocketAddress.h>
#include <folly/init/Init.h>
#include <folly/io/async/EventBase.h>
#include <thrift/lib/cpp2/async/RocketClientChannel.h>

#include "EchoService.h"
#include "echo/gen-cpp2/Echo.h"

DEFINE_string(host, "::1", "EchoServer host");
DEFINE_int32(port, 7778, "EchoServer port");

using example::echo::EchoAsyncClient;

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  folly::init(&argc, &argv);

  // create eventbase first so no dangling stack refs on 'client' dealloc
  folly::EventBase evb;

  // Create a thrift client based on RocketClientChannel
  auto socket = 
    folly::AsyncSocket::UniquePtr(new folly::AsyncSocket(&evb, FLAGS_host, FLAGS_port));
  auto channel =
    apache::thrift::RocketClientChannel::newChannel(std::move(socket));
  auto client =
    std::make_unique<EchoAsyncClient>(std::move(channel));

  // Prepare thrift request
  std::string message = "Ping this back";
  std::string response;

  // Get an echo'd message
  try {
    client->sync_echo(response, message);
    LOG(INFO) << response;
  } catch (apache::thrift::transport::TTransportException& ex) {
    LOG(ERROR) << "Request failed " << ex.what();
  }

  return 0;
}
