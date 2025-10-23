package main.scala.xcore.backend.decode

import chisel3.ChiselEnum

import main.scala.xcore._
import main.scala.xcore.backend._
import main.scala.xcore.backend.decode._


object FUType extends ChiselEnum {
    val ALU = Value
    val BRU = Value
    val LSU = Value
    val MDU = Value
    val CSR = Value
}