package xcore.backend

import chisel3._
import chisel3.util._
import xcore._
import xcore.backend._
import xcore.backend.decode._
class DataPath(implicit p: XCoreParams) extends BackendBundle {
  val pc        = UInt(XLEN.W)
  val instr     = Instruction()
  val attribute = InstrAttribute()
  val src1      = UInt(XLEN.W)
  val src2      = UInt(XLEN.W)
}
