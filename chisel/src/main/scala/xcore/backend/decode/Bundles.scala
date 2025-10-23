package main.scala.xcore.backend.decode

import main.scala.xcore.XCoreParams
import main.scala.xcore.XCoreBundle

trait HasDecodeParams {
    implicit val p: XCoreParams
    def instrWidth: Int = p.instrWidth
}

abstract class DecodeBundle(implicit p: XCoreParams) 
    extends XCoreBundle with HasDecodeParams {}