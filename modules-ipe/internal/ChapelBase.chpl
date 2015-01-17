/*
 * Copyright 2004-2015 Cray Inc.
 * Other additional copyright holders may be indicated within.
 *
 * The entirety of this work is licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License.
 *
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

// ChapelBase.chpl
//

module ChapelBase
{
  param bitsPerByte = 8;

  proc numBits(type t)  param where t == bool(64) return 64;
  proc numBits(type t)  param where t == int(64)  return 64;
  proc numBits(type t)  param where t == real(64) return 64;

  proc numBytes(type t) param return numBits(t) / 8;

  //
  // Cond test
  //
  inline proc _cond_test (x : bool)     return x;
  inline proc _cond_test (x : int )     return x != 0;

  //
  // Assignment operators
  //
  inline proc =  (ref a : bool, b : bool) : void { __primitive("=", a, b); }
  inline proc =  (ref a : int,  b : int ) : void { __primitive("=", a, b); }
  inline proc =  (ref a : real, b : real) : void { __primitive("=", a, b); }

  //
  // Basic arithmetic operators
  //
  inline proc +  (a: int,  b: int)        : int  return __primitive("+", a, b);
  inline proc -  (a: int,  b: int)        : int  return __primitive("-", a, b);
  inline proc *  (a: int,  b: int)        : int  return __primitive("*", a, b);
  inline proc /  (a: int,  b: int)        : int  return __primitive("/", a, b);

  inline proc +  (a: real, b: real)       : real return __primitive("+", a, b);
  inline proc -  (a: real, b: real)       : real return __primitive("+", a, b);
  inline proc *  (a: real, b: real)       : real return __primitive("+", a, b);
  inline proc /  (a: real, b: real)       : real return __primitive("+", a, b);


  //
  // equality comparison on primitive types
  //
  inline proc == (a: bool, b: bool)       : bool return __primitive("==", a, b);
  inline proc == (a: int,  b: int)        : bool return __primitive("==", a, b);
  inline proc == (a: real, b: real)       : bool return __primitive("==", a, b);

  inline proc != (a: bool, b: bool)       : bool return __primitive("!=", a, b);
  inline proc != (a: int,  b: int)        : bool return __primitive("!=", a, b);
  inline proc != (a: real, b: real)       : bool return __primitive("!=", a, b);

  //
  // ordered comparison on primitive types
  //
  inline proc <  (a: int,  b: int)        : bool return __primitive("<",  a, b);
  inline proc <  (a: real, b: real)       : bool return __primitive("<",  a, b);

  inline proc >  (a: int,  b: int)        : bool return __primitive(">",  a, b);
  inline proc >  (a: real, b: real)       : bool return __primitive(">",  a, b);

  inline proc <= (a: int,  b: int)        : bool return __primitive("<=", a, b);
  inline proc <= (a: real, b: real)       : bool return __primitive("<=", a, b);

  inline proc >= (a: int,  b: int)        : bool return __primitive(">=", a, b);
  inline proc >= (a: real, b: real)       : bool return __primitive(">=", a, b);
}
