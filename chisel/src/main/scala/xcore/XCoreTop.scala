package main.scala.xcore

import chisel3._
import chisel3.util._
import _root_.circt.stage.ChiselStage

object XCoreSimTop extends App {
    ChiselStage.emitSystemVerilogFile(
        new Arbiter(UInt(8.W), 4),
        firtoolOpts = Array(
            "-disable-all-randomization", 
            "-strip-debug-info", 
            "-default-layer-specialization=enable"
        ),
        args = args
    )
}
