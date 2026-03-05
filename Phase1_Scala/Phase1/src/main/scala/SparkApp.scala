import org.apache.spark._
import org.apache.spark.sql.functions._
import org.apache.spark.sql.SparkSession
import org.apache.spark.rdd.RDD
import org.apache.spark.sql.functions.col


class SparkApp {

  def run(): Unit = {
    val spark = SparkSession.builder()
      .appName("phase1")
      .master("spark://127.0.0.1:7077")
      .getOrCreate()
    val sc = spark.sparkContext // Get sparkContext from SparkSession

    val json = spark.read.json("hdfs://127.0.0.1:9000/out.json")

    // === I will run wordcount upon each tag and each URL separately. ===

    // Explode: generates a new row for each hashtag and url
    // hashtags.text works because col is smart enough to get the parameter "text" for each hashtag in "hashtags"
    val tags = json.select(explode( col("entities.hashtags.text").alias("tag") ) )
    val urls = json.select(explode( col("entities.urls.expanded_url").alias("url") ) )

    val tags_rdd = tags.rdd // Convert to distributed RDD
    val urls_rdd = urls.rdd

    // Map each tag and url to key value pairs - in this case, the tag name and appearence count
    val tags_mapped = tags_rdd.map(s => (String.valueOf(s),1)) // Apply function to each; will be a table of "key | 1"
    val urls_mapped = urls_rdd.map(s => (String.valueOf(s),1))

    // Apply reduction function to each, which will group equivalent keys together and apply this function to each pair.
    val tags_counts = tags_mapped.reduceByKey((a,b) => a + b) // For each key in column 1, group similar keys and apply function "a+b"
    val urls_counts = urls_mapped.reduceByKey((a,b) => a + b)

    // Save each as CSV
    // https://spark.apache.org/docs/latest/api/scala/org/apache/spark/sql/DataFrameWriter.html
    // Note: .toDF worked in the Spark shell, but not in this code. The Gemini AI from Google search revealed an alternative way of doing this using createDataFrame()
    spark.createDataFrame(tags_counts).write.mode("overwrite").csv("/tags_wordcount.csv")
    spark.createDataFrame(urls_counts).write.mode("overwrite").csv("/urls_wordcount.csv")

    spark.stop() 
  }
}

object SparkApp {
  def main(args: Array[String]): Unit = {
    val app = new SparkApp() // Instantiate the class
    app.run()
  }
}