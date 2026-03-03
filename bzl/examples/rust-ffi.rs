use std::ffi::CString;
use core::ffi::CStr;
use std::ffi::c_char;

#[no_mangle]
pub extern "C" fn rust_ffi_add(a: i32, b: i32) -> i32 {
    return a + b;
}

#[no_mangle]
pub unsafe extern "C" fn rust_ffi_str_cat(s1: *const c_char, s2: *const c_char) -> *const c_char {
    let s1 = CStr::from_ptr(s1).to_str().unwrap();
    let s2 = CStr::from_ptr(s2).to_str().unwrap();
    let result = format!("{}{}", s1, s2);
    let c_str = CString::new(result).unwrap();
    c_str.into_raw()
}
