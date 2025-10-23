package xcore.backend

import xcore._

trait HasBackendParams extends HasXCoreParams {
  def XLEN:        Int = p.XLEN
  def instrWidth:  Int = p.instrWidth
  def numArchRegs: Int = p.backend.numArchRegs
}

abstract class BackendBundle(implicit p: XCoreParams)
    extends XCoreBundle with HasBackendParams {}
