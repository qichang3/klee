//===-- KModule.h -----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_KMODULE_H
#define KLEE_KMODULE_H

#include "klee/Config/Version.h"
#include "klee/Core/Interpreter.h"
#include "klee/Module/KCallable.h"

#include "llvm/ADT/ArrayRef.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace llvm {
  class BasicBlock;
  class Constant;
  class Function;
  class Instruction;
  class Module;
  class DataLayout;
}

namespace klee {
  struct Cell;
  class Executor;
  class Expr;
  class InterpreterHandler;
  class InstructionInfoTable;
  struct KInstruction;
  class KModule;
  template<class T> class ref;

  struct KFunction : public KCallable { //Kfunction应该是klee处理函数的数据结构，包括函数指针、参数数量、寄存器数量、函数中的指令数等
    llvm::Function *function; //函数指针

    unsigned numArgs, numRegisters; //参数数量，寄存器数量

    unsigned numInstructions; //函数中的指令数量
    KInstruction **instructions; //函数中的指令指针，有多个指令所以是二阶指针

    std::map<llvm::BasicBlock*, unsigned> basicBlockEntry; //基本块到unsigned的映射？

    /// Whether instructions in this function should count as
    /// "coverable" for statistics and search heuristics.
    bool trackCoverage; //该函数内的指令覆盖信息是否被用于全局统计和搜索策略

    explicit KFunction(llvm::Function*, KModule*);
    KFunction(const KFunction &) = delete;
    KFunction &operator=(const KFunction &) = delete;

    ~KFunction();

    unsigned getArgRegister(unsigned index) { return index; }

    llvm::StringRef getName() const override { return function->getName(); }

    llvm::FunctionType *getFunctionType() const override {
      return function->getFunctionType();
    }

    llvm::Value *getValue() override { return function; }

    static bool classof(const KCallable *callable) {
      return callable->getKind() == CK_Function;
    }
  };


  class KConstant {
  public:
    /// Actual LLVM constant this represents.
    llvm::Constant* ct;

    /// The constant ID.
    unsigned id;

    /// First instruction where this constant was encountered, or NULL
    /// if not applicable/unavailable.
    KInstruction *ki;

    KConstant(llvm::Constant*, unsigned, KInstruction*);
  };


  class KModule {
  public:
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::DataLayout> targetData;

    // Our shadow versions of LLVM structures.
    std::vector<std::unique_ptr<KFunction>> functions; //kfunction的容器，一个module可以有多个kfunction
    std::map<llvm::Function*, KFunction*> functionMap; //函数的map，从function指向Kfunction

    // Functions which escape (may be called indirectly)
    // XXX change to KFunction
    std::set<llvm::Function*> escapingFunctions;

    std::unique_ptr<InstructionInfoTable> infos;

    std::vector<llvm::Constant*> constants;
    std::map<const llvm::Constant *, std::unique_ptr<KConstant>> constantMap;
    KConstant* getKConstant(const llvm::Constant *c);

    std::unique_ptr<Cell[]> constantTable;

    // Functions which are part of KLEE runtime
    std::set<const llvm::Function*> internalFunctions;

  private:
    // Mark function with functionName as part of the KLEE runtime
    void addInternalFunction(const char* functionName);

  public:
    KModule() = default;

    /// Optimise and prepare module such that KLEE can execute it
    //
    void optimiseAndPrepare(const Interpreter::ModuleOptions &opts,
                            llvm::ArrayRef<const char *>);

    /// Manifest the generated module (e.g. assembly.ll, output.bc) and
    /// prepares KModule
    ///
    /// @param ih
    /// @param forceSourceOutput true if assembly.ll should be created
    ///
    // FIXME: ihandler should not be here
    void manifest(InterpreterHandler *ih, bool forceSourceOutput);

    /// Link the provided modules together as one KLEE module.
    ///
    /// If the entry point is empty, all modules are linked together.
    /// If the entry point is not empty, all modules are linked which resolve
    /// the dependencies of the module containing entryPoint
    ///
    /// @param modules list of modules to be linked together
    /// @param entryPoint name of the function which acts as the program's entry
    /// point
    /// @return true if at least one module has been linked in, false if nothing
    /// changed
    bool link(std::vector<std::unique_ptr<llvm::Module>> &modules,
              const std::string &entryPoint);

    void instrument(const Interpreter::ModuleOptions &opts);

    /// Return an id for the given constant, creating a new one if necessary.
    unsigned getConstantID(llvm::Constant *c, KInstruction* ki);

    /// Run passes that check if module is valid LLVM IR and if invariants
    /// expected by KLEE's Executor hold.
    void checkModule();
  };
} // End klee namespace

#endif /* KLEE_KMODULE_H */
