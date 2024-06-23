$(call add-hdrs,fd_deque.c fd_deque_dynamic.c fd_dlist.c fd_heap.c fd_map.c fd_map_chain.c fd_map_dynamic.c fd_map_giant.c fd_pool.c fd_prq.c fd_queue.c fd_queue_dynamic.c fd_set.c fd_set_dynamic.c fd_smallset.c fd_sort.c fd_stack.c fd_treap.c fd_vec.c fd_voff.c)
$(call make-unit-test,test_deque,test_deque,fd_util)
$(call make-unit-test,test_deque_dynamic,test_deque_dynamic,fd_util)
$(call make-unit-test,test_dlist,test_dlist,fd_util)
$(call make-unit-test,test_heap,test_heap,fd_util)
$(call make-unit-test,test_map,test_map,fd_util)
$(call make-unit-test,test_map_chain,test_map_chain,fd_util)
$(call make-unit-test,test_map_chain_multi,test_map_chain_multi,fd_util)
$(call make-unit-test,test_map_dynamic,test_map_dynamic,fd_util)
$(call make-unit-test,test_map_giant,test_map_giant,fd_util)
$(call make-unit-test,test_map_giant_mem,test_map_giant_mem,fd_util)
ifdef FD_HAS_HOSTED
$(call make-unit-test,test_map_giant_concur,test_map_giant_concur,fd_util)
endif
$(call make-unit-test,test_map_perfect,test_map_perfect,fd_util)
$(call make-unit-test,test_pool,test_pool,fd_util)
$(call make-unit-test,test_prq,test_prq,fd_util)
$(call make-unit-test,test_queue,test_queue,fd_util)
$(call make-unit-test,test_queue_dynamic,test_queue_dynamic,fd_util)
$(call make-unit-test,test_redblack,test_redblack,fd_util)
$(call make-unit-test,test_redblack2,test_redblack2,fd_util)
$(call make-unit-test,test_set,test_set,fd_util)
$(call make-unit-test,test_set_dynamic,test_set_dynamic,fd_util)
$(call make-unit-test,test_smallset,test_smallset,fd_util)
$(call make-unit-test,test_sort,test_sort,fd_util)
$(call make-unit-test,test_stack,test_stack,fd_util)
$(call make-unit-test,test_treap,test_treap,fd_util)
$(call make-unit-test,test_vec,test_vec,fd_util)
$(call make-unit-test,test_voff,test_voff,fd_util)
$(call run-unit-test,test_deque,)
$(call run-unit-test,test_deque_dynamic,)
$(call run-unit-test,test_dlist,)
$(call run-unit-test,test_heap,)
$(call run-unit-test,test_map,)
$(call run-unit-test,test_map_chain,)
$(call run-unit-test,test_map_chain_multi,)
$(call run-unit-test,test_map_dynamic,)
$(call run-unit-test,test_map_giant,)
$(call run-unit-test,test_map_giant_mem,)
$(call run-unit-test,test_pool,)
$(call run-unit-test,test_prq,)
$(call run-unit-test,test_queue,)
$(call run-unit-test,test_queue_dynamic,)
$(call run-unit-test,test_redblack,)
$(call run-unit-test,test_redblack2,)
$(call run-unit-test,test_set,)
$(call run-unit-test,test_set_dynamic,)
$(call run-unit-test,test_smallset,)
$(call run-unit-test,test_sort,)
$(call run-unit-test,test_stack,)
$(call run-unit-test,test_treap,)
$(call run-unit-test,test_vec,)
$(call run-unit-test,test_voff,)
