// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_REPL_CLIENT_CONN_H_
#define PIKA_REPL_CLIENT_CONN_H_

#include "net/include/pb_conn.h"

#include <memory>

#include "include/pika_conf.h"
#include "pika_inner_message.pb.h"

class SyncMasterPartition;
class SyncSlavePartition;

class PikaReplClientConn : public net::PbConn {
 public:
  PikaReplClientConn(int fd, const std::string& ip_port, net::Thread* thread, void* worker_specific_data,
                     net::NetMultiplexer* mpx);
  virtual ~PikaReplClientConn() = default;

  static void HandleMetaSyncResponse(void* arg);
  static void HandleDBSyncResponse(void* arg);
  static void HandleTrySyncResponse(void* arg);
  static void HandleRemoveSlaveNodeResponse(void* arg);

  static Status TrySyncConsensusCheck(const InnerMessage::ConsensusMeta& consensus_meta,
                                      const std::shared_ptr<SyncMasterPartition>& partition,
                                      const std::shared_ptr<SyncSlavePartition>& slave_partition);
  static bool IsTableStructConsistent(const std::vector<TableStruct>& current_tables,
                                      const std::vector<TableStruct>& expect_tables);
  int DealMessage() override;

 private:
  // dispatch binlog by its table_name + partition
  void DispatchBinlogRes(const std::shared_ptr<InnerMessage::InnerResponse> response);

  struct ReplRespArg {
    std::shared_ptr<InnerMessage::InnerResponse> resp;
    std::shared_ptr<net::PbConn> conn;
    ReplRespArg(std::shared_ptr<InnerMessage::InnerResponse> _resp, std::shared_ptr<net::PbConn> _conn)
        : resp(_resp), conn(_conn) {}
  };
};

#endif
