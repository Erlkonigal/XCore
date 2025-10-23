import mill._, scalalib._
import os.Path

object chisel extends ScalaModule {
    def chiselVersion = "7.2.0"
    def scalaTestVersion = "3.2.19"
    override def scalaVersion = "2.13.16"
    override def millSourcePath: Path = super.millSourcePath
    override def scalacOptions = Seq(
        "-language:reflectiveCalls",
        "-deprecation",
        "-feature",
        "-Xcheckinit",
        "-Ymacro-annotations",
    )
    override def ivyDeps = Agg(
        ivy"org.chipsalliance::chisel:$chiselVersion",
    )
    override def scalacPluginIvyDeps = Agg(
		ivy"org.chipsalliance:::chisel-plugin:$chiselVersion",
  	)
	object test extends ScalaTests with TestModule.ScalaTest {
		override def ivyDeps = Agg (
      		ivy"org.scalatest::scalatest::$scalaTestVersion"
    	)
	}
}
