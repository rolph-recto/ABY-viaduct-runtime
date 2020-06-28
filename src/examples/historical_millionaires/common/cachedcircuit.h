#ifndef __CACHEDCIRCUIT_H__
#define __CACHEDCIRCUIT_H__

#include <vector>
#include <stack>

#include "historical_millionaires.h"
#include "../../../abycore/circuit/arithmeticcircuits.h"
#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/sharing/sharing.h"

struct CircuitBuilders {
  int bitlen;
  e_role role;
  ArithmeticCircuit* arith;
  BooleanCircuit* boolean;
  BooleanCircuit* yao;
};

class CircuitGate {
  public:
  virtual void AddChildrenToTraversal(std::vector<CircuitGate*>& children)=0;
  virtual share* BuildGate(std::stack<share*>& shareStack, CircuitBuilders& builders)=0;
  virtual ~CircuitGate() {}
};

class DummyInGate: public CircuitGate {
  public:
  DummyInGate() {};

  void AddChildrenToTraversal(std::vector<CircuitGate*>& children) {}

  share* BuildGate(std::stack<share*>& shareStack, CircuitBuilders& builders) {
    return builders.yao->PutDummyINGate(builders.bitlen);
  }
};

class InGate: public CircuitGate {
  uint32_t input;
  public:
  InGate(uint32_t input) {
    this->input = input;
  }

  void AddChildrenToTraversal(std::vector<CircuitGate*>& children) {}

  share* BuildGate(std::stack<share*>& shareStack, CircuitBuilders& builders) {
    return builders.yao->PutINGate(input, builders.bitlen, builders.role);
  }
};

class InvGate: public CircuitGate {
  CircuitGate* input;
  public:
  InvGate(CircuitGate* input) {
    this->input = input;
  }

  void AddChildrenToTraversal(std::vector<CircuitGate*>& children) {
    children.push_back(this->input);
  }

  share* BuildGate(std::stack<share*>& shareStack, CircuitBuilders& builders) {
    share* inputShare = shareStack.top();
    return builders.boolean->PutINVGate(inputShare);
  }
};

class BinaryOpGate: public CircuitGate {
  protected:
  CircuitGate* lhs;
  CircuitGate* rhs;

  virtual share* BuildGate(share* lhsShare, share* rhsShare, CircuitBuilders& builders)=0;

  public:

  BinaryOpGate(CircuitGate* lhs, CircuitGate* rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
  }

  void AddChildrenToTraversal(std::vector<CircuitGate*>& children) {
    children.push_back(this->lhs);
    children.push_back(this->rhs);
  }

  share* BuildGate(std::stack<share*>& shareStack, CircuitBuilders& builders) {
    share* lhsShare = shareStack.top();
    shareStack.pop();
    share* rhsShare = shareStack.top();
    shareStack.pop();
    return BuildGate(lhsShare, rhsShare, builders);
  }
};

class AddGate: public BinaryOpGate {
  protected:
  share* BuildGate(share* lhsShare, share* rhsShare, CircuitBuilders& builders) {
    return builders.yao->PutADDGate(lhsShare, rhsShare);
  }

  public:
  AddGate(CircuitGate* lhs, CircuitGate* rhs) : BinaryOpGate(lhs, rhs) {}
};

class MulGate: public BinaryOpGate {
  protected: 
  share* BuildGate(share* lhsShare, share* rhsShare, CircuitBuilders& builders) {
    return builders.yao->PutMULGate(lhsShare, rhsShare);
  }

  public:
  MulGate(CircuitGate* lhs, CircuitGate* rhs) : BinaryOpGate(lhs, rhs) {}
};

class ConstGate: public CircuitGate {
  uint32_t value;

  public:
  ConstGate(uint32_t value) {
    this->value = value;
  }

  void AddChildrenToTraversal(std::vector<CircuitGate*>& children) {}

  share* BuildGate(std::stack<share*>& shareStack, CircuitBuilders& builders) {
    return builders.yao->PutCONSGate(this->value, builders.bitlen);
  }
};

class GtGate: public BinaryOpGate {
  protected: 
  share* BuildGate(share* lhsShare, share* rhsShare, CircuitBuilders& builders) {
    return builders.yao->PutGTGate(lhsShare, rhsShare);
  }

  public:
  GtGate(CircuitGate* lhs, CircuitGate* rhs) : BinaryOpGate(lhs, rhs) {}
};

/* Builds a representation of a circuit, which can be built into an ABY circuit
 *
 * CachedCircuit is needed because ABY supports building only one circuit at
 * a time, which makes certain Viaduct programs impossible to compile.
 * CachedCircuit represents all possible computations that will be executed,
 * and allows the user to build an ABY circuit whenever some output is needed,
 * bypassing this limitation.
 */
class CachedCircuit {
  uint32_t bitlen;
  e_role role;
  std::vector<CircuitGate*> gates;
  ABYParty& party;

public:
  CachedCircuit(uint32_t bitlen, e_role role, ABYParty& party)
    : bitlen(bitlen), role(role), gates(), party(party)
    {}

  ~CachedCircuit() {
    for (uint32_t i = 0; i < this->gates.size(); i++) {
      delete this->gates[i];
    }
  }

  CircuitGate* PutDummyINGate() {
    DummyInGate* gate = new DummyInGate();
    this->gates.push_back(gate);
    return gate;
  }

  CircuitGate* PutINGate(uint32_t value) {
    InGate* gate = new InGate(value);
    this->gates.push_back(gate);
    return gate;
  }

  CircuitGate* PutCONSTGate(uint32_t value) {
    ConstGate* gate = new ConstGate(value);
    this->gates.push_back(gate);
    return gate;
  }

  CircuitGate* PutADDGate(CircuitGate* lhs, CircuitGate* rhs) {
    AddGate* gate = new AddGate(lhs, rhs);
    this->gates.push_back(gate);
    return gate;
  }

  CircuitGate* PutMULGate(CircuitGate* lhs, CircuitGate* rhs) {
    MulGate* gate = new MulGate(lhs, rhs);
    this->gates.push_back(gate);
    return gate;
  }

  CircuitGate* PutGTGate(CircuitGate* lhs, CircuitGate* rhs) {
    GtGate* gate = new GtGate(lhs, rhs);
    this->gates.push_back(gate);
    return gate;
  }

  /*
  CircuitGate* PutINVGate(CircuitGate* input) {
    InvGate* gate = new InvGate(input);
    this->gates.push_back(gate);
    return gate;
  }
  */

  // build an ABY circuit from (some part of) the circuit,
  // ending with the `out` param gate
  //
  // perform iterative traversal instead of recursive traversal because
  // C++ doesn't do tail-call optimization
  share* BuildCircuit(CircuitGate* out) {
    CircuitBuilders builders;
    std::vector<Sharing*>& sharings = this->party.GetSharings();
    builders.arith = (ArithmeticCircuit*)sharings[S_ARITH]->GetCircuitBuildRoutine();
    builders.boolean = (BooleanCircuit*)sharings[S_BOOL]->GetCircuitBuildRoutine();
    builders.yao = (BooleanCircuit*)sharings[S_YAO]->GetCircuitBuildRoutine();
    builders.bitlen = this->bitlen;
    builders.role = this->role;

    // use this as an pre-order traversal
    std::stack<CircuitGate*> traverseStack = std::stack<CircuitGate*>();

    // contains postorder traversal of the expr tree
    std::stack<CircuitGate*> exprStack = std::stack<CircuitGate*>();

    std::stack<share*> shareStack = std::stack<share*>();
    traverseStack.push(out);

    while (!traverseStack.empty()) {
      CircuitGate* curGate = traverseStack.top();
      traverseStack.pop();
      exprStack.push(curGate);

      std::vector<CircuitGate*> children = std::vector<CircuitGate*>();
      curGate->AddChildrenToTraversal(children);
      for (uint32_t i = 0; i < children.size(); i++) {
        traverseStack.push(children[i]);
      }
    }

    // "evaluates" the stack as an reverse Polish expr by building the circuit
    while (!exprStack.empty()) {
      CircuitGate* curGate = exprStack.top();
      exprStack.pop();
      share* curShare = curGate->BuildGate(shareStack, builders);
      shareStack.push(curShare);
    }

    // at the end, the only node in the stack should be the top-level expr node
    assert(shareStack.size() == 1);
    return builders.yao->PutOUTGate(shareStack.top(), ALL);
  }

  void ExecCircuit() {
    this->party.ExecCircuit();
  }

  void Reset() {
    this->party.Reset();
  }
};

#endif
