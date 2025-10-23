package xcore

import chisel3.Bundle
import xcore._
abstract class XCoreBundle(implicit val p: XCoreParams) extends Bundle
    with HasXCoreParams {}
