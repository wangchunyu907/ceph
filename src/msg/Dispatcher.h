// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */


#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include "Message.h"

class Messenger;

class Dispatcher {
  Dispatcher *next;

  // how i receive messages
  virtual bool dispatch_impl(Message *m) = 0;
 public:
  virtual ~Dispatcher() { }
  Dispatcher() : next(NULL) { }

  virtual void dispatch(Message *m) { 
    if (!dispatch_impl(m)) {
      if (next) {
        next->dispatch(m);
      } else {
        assert(0);
      }
    }
  }

  virtual void add(Dispatcher *disp) {
    if (!next) {
      next = disp; 
    } else {
      next->add(disp);
    }
  }

  // how i deal with transmission failures.
  virtual void ms_handle_failure(Message *m, const entity_inst_t& inst) {  }

  /*
   * on any connection reset.
   * this indicates that the ordered+reliable delivery semantics have 
   * been violated.  messages may have been lost.
   */
  virtual void ms_handle_reset(const entity_addr_t& peer, entity_name_t last) { }

  // on deliberate reset of connection by remote
  //  implies incoming messages dropped; possibly/probably some of our previous outgoing too.
  virtual void ms_handle_remote_reset(const entity_addr_t& peer, entity_name_t last) { }
};

#endif
