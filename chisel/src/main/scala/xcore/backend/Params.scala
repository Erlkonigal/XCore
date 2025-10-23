package xcore.backend

import xcore.backend.decode.DecodeParams

case class BackendParams(
    val numArchRegs:  Int = 32,
    val decodeParams: DecodeParams = DecodeParams()
)
