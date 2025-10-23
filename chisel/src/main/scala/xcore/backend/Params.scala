package main.scala.xcore.backend

import main.scala.xcore.backend.decode.DecodeParams

case class BackendParams(
    val numArchRegs: Int = 32,
    val decodeParams: DecodeParams = DecodeParams()
)