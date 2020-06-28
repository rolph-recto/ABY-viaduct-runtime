#ifndef __VIADUCT_RUNTIME_H__
#define __VIADUCT_RUNTIME_H__

#include <map>
#include <set>
#include <thread>
#include <vector>

#include "syncqueue.h"

using hostid = uint32_t;
using pid = uint32_t;

class ViaductRuntime;
class ViaductProcess;

class ViaductProcessRuntime {
  ViaductRuntime& runtime;
  pid self_id;
  hostid host;
  ViaductProcess& process;
public:
  ViaductProcessRuntime(
      ViaductRuntime& runtime,
      pid self_id,
      hostid host,
      ViaductProcess& process
  );

  hostid getHost();

  int receive(pid sender);

  void send(pid receiver, int val);

  hostid getParty();

  pid getId();

  int input();

  void output(int value);

  void operator()();
};

class ViaductProcess {
public:
  virtual void run(ViaductProcessRuntime& runtime)=0;
};

struct Message {
  pid sender;
  pid receiver;
  int value;
};

struct HostInfo {
  hostid id;
  std::string& ip_address;
  uint16_t port;
  std::unique_ptr<SyncQueue<Message>> sendQueue;
};

using host_map = std::map<hostid, HostInfo>;
using channel_map = std::map<pid, std::map<pid, std::unique_ptr<SyncQueue<int>>>>;
using process_map = std::map<pid, std::unique_ptr<ViaductProcessRuntime>>;

class ViaductRuntime {
  hostid host;
  host_map hostMap;
  channel_map channelMap;
  process_map processMap;

  void createRemoteConnections();

public:
  ViaductRuntime(hostid party);

  ~ViaductRuntime();

  void registerHost(hostid id, std::string& ip, uint16_t port);

  void registerProcess(pid proc_id, hostid host, ViaductProcess& process);

  uint32_t receive(pid self, pid sender);

  void send(pid self, pid receiver, uint32_t val);

  hostid getHost();

  void run();
};


#endif
