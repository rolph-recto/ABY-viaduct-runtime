/**
 \file 		historical_millionaires.cpp
 */

#include <thread>

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
//ABY Party class
#include "../../abycore/aby/abyparty.h"

#include "common/cachedcircuit.h"
#include "common/syncqueue.h"

#include "common/viaduct_runtime.h"

class LocalProcess: public ViaductProcess {
  void (*procFunction)(ViaductProcessRuntime& runtime);

public:
  LocalProcess(void (*procFunction)(ViaductProcessRuntime&))
    : procFunction(procFunction)
  {}

  void run(ViaductProcessRuntime& runtime) {
    (*this->procFunction)(runtime);
  }
};

class ReplicatedProcess: public ViaductProcess {
  void (*procFunction)(ViaductProcessRuntime& runtime);

public:
  ReplicatedProcess(void (*procFunction)(ViaductProcessRuntime& runtime)) {
    this->procFunction = procFunction;
  }

  void run(ViaductProcessRuntime& runtime) {
    (*this->procFunction)(runtime);
  }
};

struct ABYInfo {
  e_role role;
  std::string* address;
  uint16_t port;
  seclvl sec_level;
  uint32_t bitlen;
  uint32_t nthreads;
  e_mt_gen_alg mt_alg;
};

class MPCProcess: public ViaductProcess {
  ABYParty party;
  e_role role;
  uint32_t bitlen;
  void (*procFunction)(ViaductProcessRuntime& runtime, CachedCircuit& circuit);

public:
  MPCProcess(
      ABYInfo info,
      void (*procFunction)(ViaductProcessRuntime&, CachedCircuit&)
  ) : party(info.role, *info.address, info.port, info.sec_level,
            info.bitlen, info.nthreads, info.mt_alg),
      role(info.role), bitlen(info.bitlen), procFunction(procFunction)
  {}

  void run(ViaductProcessRuntime& runtime) {
    CachedCircuit circuit(this->bitlen, this->role, this->party);
    (*this->procFunction)(runtime, circuit);
  }
};

#include "prodsum.c"

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* nvals, uint32_t* secparam, std::string* address,
		uint16_t* port, int32_t* test_op) {

	uint32_t int_role = 0, int_port = 0;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false }, {
					(void*) nvals, T_NUM, "n",
					"Number of parallel operation elements", false, false }, {
					(void*) bitlen, T_NUM, "b", "Bit-length, default 32", false,
					false }, { (void*) secparam, T_NUM, "s",
					"Symmetric Security Bits, default: 128", false, false }, {
					(void*) address, T_STR, "a",
					"IP-address, default: localhost", false, false }, {
					(void*) &int_port, T_NUM, "p", "Port, default: 7766", false,
					false }, { (void*) test_op, T_NUM, "t",
					"Single test (leave out for all operations), default: off",
					false, false } };

	if (!parse_options(argcp, argvp, options,
			sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}

	assert(int_role < 2);
	*role = (e_role) int_role;

	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}

	//delete options;

	return 1;
}

int main(int argc, char** argv) {
  // set up ABY
	uint32_t nvals = 31, secparam = 128;
	std::string address = "127.0.0.1";
	int32_t test_op = -1;

  ABYInfo abyInfo;
  abyInfo.nthreads = 1;
	abyInfo.port = 7766;
  abyInfo.address = &address;
	abyInfo.mt_alg = MT_OT;

	read_test_options(&argc, &argv, &abyInfo.role, &abyInfo.bitlen, &nvals,
      &secparam, abyInfo.address, &abyInfo.port, &test_op);

	abyInfo.sec_level = get_sec_lvl(secparam);

  // set up viaduct runtime
  ViaductRuntime runtime(abyInfo.role);
  start(runtime, abyInfo);
}

