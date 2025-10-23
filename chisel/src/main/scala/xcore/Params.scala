package xcore

import xcore.backend.BackendParams

trait HasXCoreParams {
  implicit val p: XCoreParams
}

case class XCoreParams(
    val debug:      Boolean = true,
    val instrWidth: Int = 32,
    val XLEN:       Int = 32,
    val backend:    BackendParams = BackendParams()
)
