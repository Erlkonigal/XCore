package main.scala.xcore.backend

import main.scala.xcore.XCoreBundle
import main.scala.xcore.XCoreParams

trait HasBackendParams {
    implicit val p: XCoreParams
    def XLEN: Int = p.XLEN
    def instrWidth: Int = p.instrWidth
    def numArchRegs: Int = p.backend.numArchRegs
}

abstract class BackendBundle(implicit p: XCoreParams) 
    extends XCoreBundle with HasBackendParams {}