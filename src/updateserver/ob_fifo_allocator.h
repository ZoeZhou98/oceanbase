////===================================================================
 //
 // ob_fifo_allocator.h updateserver / Oceanbase
 //
 // Copyright (C) 2010 Taobao.com, Inc.
 //
 // Created on 2012-08-30 by Yubai (yubai.lk@taobao.com) 
 //
 // -------------------------------------------------------------------
 //
 // Description
 //
 //
 // -------------------------------------------------------------------
 // 
 // Change Log
 //
////====================================================================

#ifndef  OCEANBASE_UPDATESERVER_FIFO_ALLOCATOR_H_
#define  OCEANBASE_UPDATESERVER_FIFO_ALLOCATOR_H_
#include "common/ob_define.h"
#include "common/ob_fixed_queue.h"
#include "common/page_arena.h"
#include "common/ob_allocator.h"
#include "ob_id_map.h"

namespace oceanbase
{
  namespace updateserver
  {
    class FIFOAllocator : public common::ObIAllocator
    {
      struct Page
      {
        volatile uint32_t ref_cnt;
        uint32_t pos;
        char buf[0];
      };

      struct ThreadNode
      {
        uint64_t id;
        volatile ThreadNode *next;
      };

      public:
        FIFOAllocator();
        ~FIFOAllocator();
      public:
        int init(const int64_t total_limit,
                const int64_t hold_limit,
                const int64_t page_size);
        void destroy();
      public:
        void set_mod_id(const int32_t mod_id);
        void *alloc(const int64_t size);
        void free(void *ptr);
        int64_t allocated() const {return allocated_size_;};
        int64_t hold() const {return page_size_ * free_list_.get_total();};
      private:
        inline Page *get_page_(const int64_t require_size, uint64_t &id);
        inline void revert_page_(const uint64_t id, void *ptr);
        inline Page *alloc_page_();
        inline void free_page_(Page *ptr);
      private:
        bool inited_;
        int32_t mod_id_;
        ObIDMap<Page> id_map_;
        common::ObFixedQueue<Page> free_list_;
        pthread_key_t thread_node_key_;
        pthread_spinlock_t thread_node_lock_;
        common::PageArena<ThreadNode> thread_node_allocator_;
        volatile ThreadNode *thread_node_list_;
        int64_t total_limit_;
        int64_t hold_limit_;
        int64_t page_size_;
        volatile int64_t allocated_size_;
    };
  }
}

#endif //OCEANBASE_UPDATESERVER_FIFO_ALLOCATOR_H_

