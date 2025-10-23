package xcore.backend.decode

import chisel3._
import chisel3.util._
import xcore._
import xcore.backend._
import xcore.backend.decode._

object FUType extends ChiselEnum {
  val ALU = Value
  val BRU = Value
  val LSU = Value
  val MDU = Value
  val CSR = Value
}

object ExecASel extends ChiselEnum {
  val RS1  = Value
  val PC   = Value
  val ZERO = Value
}

object ExecBSel extends ChiselEnum {
  val RS2    = Value
  val IMM    = Value
  val XBYTES = Value
  val ZERO   = Value
}

object ImmType extends ChiselEnum {
  val IType = Value
  val SType = Value
  val BType = Value
  val JType = Value
  val UType = Value
  val RType = Value
}

object HasWriteBack extends ChiselEnum {
  val HasWB = Value
  val NoWB  = Value
}

class InstrAttribute(implicit p: XCoreParams) extends BackendBundle {
  val fuType       = FUType()
  val execASel     = ExecASel()
  val execBSel     = ExecBSel()
  val immType      = ImmType()
  val hasWriteBack = HasWriteBack()
}

object InstrAttribute {
  def apply()(implicit p: XCoreParams): InstrAttribute =
    new InstrAttribute()
  def apply(
      fuType:       FUType.Type,
      execASel:     ExecASel.Type,
      execBSel:     ExecBSel.Type,
      immType:      ImmType.Type,
      hasWriteBack: HasWriteBack.Type
  )(implicit p: XCoreParams): InstrAttribute = {
    val attr = InstrAttribute()
    attr.fuType       := fuType
    attr.execASel     := execASel
    attr.execBSel     := execBSel
    attr.immType      := immType
    attr.hasWriteBack := hasWriteBack
    attr
  }
}
