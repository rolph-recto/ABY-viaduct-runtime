#include <iostream>
#include <stdarg.h>
#include "viaduct_runtime.h"

// ViaductProcessRuntime
ViaductProcessRuntime::ViaductProcessRuntime(
    ViaductRuntime& runtime,
    pid self_id,
    hostid host,
    ViaductProcess& process
) : runtime(runtime), self_id(self_id), host(host), process(process) {}

int ViaductProcessRuntime::receive(pid sender) {
  return this->runtime.receive(this->self_id, sender);
}

void ViaductProcessRuntime::send(pid receiver, int val) {
  runtime.send(this->self_id, receiver, val);
}

pid ViaductProcessRuntime::getId() { return this->self_id; }

hostid ViaductProcessRuntime::getHost() { return this->host; }

int ViaductProcessRuntime::input() {
  int in;
  std::cin >> in;
  std::cout << "got input " << in << std::endl;
  return in;
}

void ViaductProcessRuntime::output(int value) {
  std::cout << "out: " << value << "\n";
}

void ViaductProcessRuntime::operator()() {
  this->process.run(*this);
}


// ViaductRuntime

ViaductRuntime::ViaductRuntime(hostid host)
  : hostMap(), channelMap(), processMap(), host(host) {}

ViaductRuntime::~ViaductRuntime() {}

void ViaductRuntime::registerHost(hostid id, std::string& ip, uint16_t port) {}

void ViaductRuntime::registerProcess(
    pid proc_id, hostid host, ViaductProcess& process
) {
  if (processMap.count(proc_id) == 0) {
    auto procRuntime = std::make_unique<ViaductProcessRuntime>(*this, proc_id, host, process);
    processMap[proc_id] = std::move(procRuntime);

    // create channels
    std::vector<pid> otherProcesses;
    for (channel_map::iterator it = this->channelMap.begin(); it != this->channelMap.end(); it++)
    {
      auto queue = std::make_unique<SyncQueue<int>>();
      it->second[proc_id] = std::move(queue);
      otherProcesses.push_back(it->first);
    }

    // create empty map
    this->channelMap[proc_id];
    for (std::vector<pid>::iterator it = otherProcesses.begin();
         it != otherProcesses.end(); it++)
    {
      this->channelMap[proc_id][*it];
    }
  }
}

uint32_t ViaductRuntime::receive(pid self, pid sender) {
  return this->channelMap[sender][self]->dequeue();
}

void ViaductRuntime::send(pid self, pid receiver, uint32_t val) {
  ViaductProcessRuntime* receiverProcess = this->processMap[receiver].get();

  if (receiverProcess->getHost() == this->host) { // local communication
    this->channelMap[self][receiver]->enqueue(val);

  } else { // remote communication
    // TODO: fill this in
    std::cout << "remote communication between process " << self << " " << receiver << std::endl;
  }
}

hostid ViaductRuntime::getHost() { return this->host; }

void ViaductRuntime::createRemoteConnections() {}

void ViaductRuntime::run() {
  std::vector<std::thread> processThreads;

  for (process_map::iterator it = processMap.begin(); it != processMap.end(); it++) {
    if (it->second->getHost() == this->host) {
      processThreads.push_back(std::thread(*it->second));
    }
  }

  for (uint32_t i = 0; i < processThreads.size(); i++) {
    processThreads[i].join();
  }
}


