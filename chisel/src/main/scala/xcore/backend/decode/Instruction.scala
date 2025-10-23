package xcore.backend.decode

import chisel3._
import chisel3.util._
import xcore.XCoreBundle
import xcore.XCoreParams

class Instruction(implicit p: XCoreParams) extends DecodeBundle {
  val opcode = UInt(7.W)
  val rd     = UInt(5.W)
  val funct3 = UInt(3.W)
  val rs1    = UInt(5.W)
  val rs2    = UInt(5.W)
  val funct7 = UInt(7.W)

  def bits: UInt = Cat(funct7, rs2, rs1, funct3, rd, opcode)
  def fromUInt(b: UInt): Unit = {
    opcode := b(6, 0)
    rd     := b(11, 7)
    funct3 := b(14, 12)
    rs1    := b(19, 15)
    rs2    := b(24, 20)
    funct7 := b(31, 25)
  }
  def immI: UInt = Cat(Fill(20, bits(31)), bits(31, 20))
  def immS: UInt = Cat(Fill(20, bits(31)), bits(31, 25), bits(11, 7))
  def immB: UInt =
    Cat(Fill(19, bits(31)), bits(31), bits(7), bits(30, 25), bits(11, 8), 0.B)
  def immU: UInt = Cat(bits(31, 12), Fill(12, 0.B))
  def immJ: UInt =
    Cat(Fill(11, bits(31)), bits(31), bits(19, 12), bits(20), bits(30, 21), 0.B)
}

object Instruction {
  def apply(bits: UInt)(implicit p: XCoreParams): Instruction = {
    val inst = Wire(new Instruction())
    inst.fromUInt(bits)
    inst
  }
  def apply()(implicit p: XCoreParams): Instruction =
    Wire(new Instruction())
}
