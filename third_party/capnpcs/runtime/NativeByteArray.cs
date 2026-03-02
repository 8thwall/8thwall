using System;
using System.Runtime.InteropServices;

namespace Capnp {

[StructLayout(LayoutKind.Sequential)]
public struct NativeByteArray {
  public IntPtr bytes;
  public Int32 size;
}

}
