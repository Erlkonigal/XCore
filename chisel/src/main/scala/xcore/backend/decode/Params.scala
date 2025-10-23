package xcore.backend.decode

case class InstrSetParams(
    val extensions: List[String] = List("I")
)

case class DecodeParams(
    val instrSetParams: InstrSetParams = InstrSetParams()
)
