pub use crate::generated::{
    fd_pod_cnt_subpod,
    fd_pod_query,
    fd_pod_info_t,
    FD_POD_VAL_TYPE_BUF,
    FD_POD_VAL_TYPE_CHAR,
    FD_POD_VAL_TYPE_CSTR,
    FD_POD_VAL_TYPE_DOUBLE,
    FD_POD_VAL_TYPE_FLOAT,
    FD_POD_VAL_TYPE_INT,
    FD_POD_VAL_TYPE_INT128,
    FD_POD_VAL_TYPE_LONG,
    FD_POD_VAL_TYPE_SCHAR,
    FD_POD_VAL_TYPE_SHORT,
    FD_POD_VAL_TYPE_SUBPOD,
    FD_POD_VAL_TYPE_UCHAR,
    FD_POD_VAL_TYPE_UINT,
    FD_POD_VAL_TYPE_UINT128,
    FD_POD_VAL_TYPE_ULONG,
    FD_POD_VAL_TYPE_USHORT,
};

pub unsafe fn fd_pod_query_subpod( pod: *const u8, path: *const i8 ) -> *const u8 {
    let mut info = std::mem::zeroed::<fd_pod_info_t>();
    if fd_pod_query(pod, path, &mut info) != 0 || info.val_type != FD_POD_VAL_TYPE_SUBPOD as i32 {
        return std::ptr::null();
    }
    info.val as *const u8
}
