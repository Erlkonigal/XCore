package xcore.backend.decode

import xcore._

trait HasDecodeParams extends HasXCoreParams {
  def instrWidth: Int = p.instrWidth
}

abstract class DecodeBundle(implicit p: XCoreParams)
    extends XCoreBundle with HasDecodeParams {}
