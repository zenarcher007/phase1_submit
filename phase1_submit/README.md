# Phase 1 - Hashtag/URL Extraction + Hadoop WordCount (FABRIC)

## Environment
- Platform: FABRIC VM (Rocky 8)
- Tools: C++17 extractor, Apache Hadoop MapReduce (wordcount example)

## Input
- Tweets dataset: (Canvas provided) `tweets.json` or `tweets.jsonl`

## extract hashtags and URLs (C++)
Compile:
```bash
g++ -O2 -std=c++17 extract.cpp -o extract


## extract and get tokens.txt
./extract tweets.jsonl tokens.txt

## run hadoop wordcount and get results
hadoop jar ~/hadoop/share/hadoop/mapreduce/hadoop-mapreduce-examples-*.jar wordcount tokens.txt wc_out

