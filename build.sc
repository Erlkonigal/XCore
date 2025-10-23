import os._
import mill._
import scalalib._
import scalafmt._

object chisel extends SbtModule with ScalafmtModule { m =>
  def chiselVersion         = "7.2.0"
  def scalaTestVersion      = "3.2.19"
  override def scalaVersion = "2.13.16"
  override def scalacOptions = Seq(
    "-language:reflectiveCalls",
    "-deprecation",
    "-feature",
    "-Xcheckinit",
    "-Ymacro-annotations"
  )
  override def ivyDeps = Agg(
    ivy"org.chipsalliance::chisel:$chiselVersion"
  )
  override def scalacPluginIvyDeps = Agg(
    ivy"org.chipsalliance:::chisel-plugin:$chiselVersion"
  )
  object test extends SbtTests with TestModule.ScalaTest {
    override def ivyDeps = m.ivyDeps() ++ Agg(
      ivy"org.scalatest::scalatest::$scalaTestVersion"
    )
  }
}
