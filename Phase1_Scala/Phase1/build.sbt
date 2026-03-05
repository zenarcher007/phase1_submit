name := "Phase1"
version := "1.0"
scalaVersion := "2.13.17"
Compile / mainClass := Some("SparkApp")

// https://mvnrepository.com/artifact/org.apache.spark/spark-sql_2.13/4.1.1
libraryDependencies += "org.apache.spark" % "spark-sql_2.13" % "4.1.1"