//===-- SolverStats.h -------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_SOLVERSTATS_H
#define KLEE_SOLVERSTATS_H

#include "klee/Statistics/Statistic.h"

namespace klee {
namespace stats { //包含对klee的统计信息进行跟踪的各种函数和变量

  extern Statistic cexCacheTime;
  extern Statistic solverQueries;
  extern Statistic queries;
  extern Statistic queriesInvalid;
  extern Statistic queriesValid;
  extern Statistic queryCacheHits;
  extern Statistic queryCacheMisses;
  extern Statistic queryCexCacheHits;
  extern Statistic queryCexCacheMisses;
  extern Statistic queryConstructs;
  extern Statistic queryCounterexamples;
  extern Statistic queryTime;
  
#ifdef KLEE_ARRAY_DEBUG
  extern Statistic arrayHashTime;
#endif

}
}

#endif /* KLEE_SOLVERSTATS_H */
