package main.scala.xcore.backend

import chisel3._
import chisel3.util._

import main.scala.xcore._
import main.scala.xcore.backend._
import main.scala.xcore.backend.decode._

class DataPath(implicit p: XCoreParams) extends BackendBundle {
    val pc = UInt(XLEN.W)
    val instr = Instruction()
    val src1 = UInt(XLEN.W)
    val src2 = UInt(XLEN.W)
}
