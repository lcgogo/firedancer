# Unit test only works if there is an accessable rocksdb

ifneq ($(FD_HAS_ROCKSDB),)

$(call add-hdrs,fd_banks_solana.h fd_rocksdb.h fd_executor.h fd_acc_mgr.h fd_hashes.h fd_types.h fd_types_custom.h fd_runtime.h)
$(call add-hdrs,sysvar/fd_sysvar.h sysvar/fd_sysvar_clock.h sysvar/fd_sysvar_slot_history.h sysvar/fd_sysvar_slot_hashes.h sysvar/fd_sysvar_epoch_schedule.h sysvar/fd_sysvar_fees.h sysvar/fd_sysvar_rent.h sysvar/fd_sysvar_stake_history.h)
$(call add-hdrs,program/fd_system_program.h program/fd_vote_program.h program/fd_builtin_programs.h program/fd_stake_program.h program/fd_compute_budget_program.h program/fd_config_program.h)
$(call add-hdrs,tests/fd_tests.h)

$(call add-objs,fd_banks_solana fd_rocksdb fd_executor fd_acc_mgr fd_hashes fd_types fd_runtime tests/fd_tests,fd_ballet)
$(call add-objs,sysvar/fd_sysvar sysvar/fd_sysvar_clock sysvar/fd_sysvar_recent_hashes sysvar/fd_sysvar_slot_history sysvar/fd_sysvar_slot_hashes sysvar/fd_sysvar_epoch_schedule sysvar/fd_sysvar_fees sysvar/fd_sysvar_rent sysvar/fd_sysvar_stake_history,fd_ballet)
$(call add-objs,program/fd_system_program program/fd_nonce_program program/fd_vote_program program/fd_builtin_programs program/fd_stake_program program/fd_compute_budget_program program/fd_config_program,fd_ballet)

$(call make-unit-test,test_runtime,test_runtime,fd_ballet fd_funk fd_util)

else

$(warning runtime disabled due to lack of rocksdb)

endif
