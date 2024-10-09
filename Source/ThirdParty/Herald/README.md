# Herald
![Build Status](https://github.com/Justin-Randall/herald/actions/workflows/cmake-multi-platform.yml/badge.svg) [![codecov](https://codecov.io/github/Justin-Randall/herald/graph/badge.svg?token=4HYUF98OJ4)](https://codecov.io/github/Justin-Randall/herald)


Herald is a logging library for C++. It provides interfaces for log transformers and writers to structure and deliver log messages.

It currently includes simple line and json tranformers with additional formatting for key-value variadics to include in structured log output, as well as global "headers" of key-value pairs to include in all log output.

Typical usage calls a static "Herald::log()" function that either takes a pointer to the ILogTransformer interface, a LogLevel and a message, or those 3 plus any number of key-value pairs to include with the log. The transformed log output is then delivered to all attached Writer's and writer callback objects. Multiple writers may be attached to the transformer. For example a JSON transformer logging to stderr, a file and perhaps a custom HTTP writer that the well-formed JSON object to ElasticSearch.

A basic FileLogWriter is included with a worker thread to decouple file I/O from loglines. It will batch as it can for moments when an extraordinary amount of log messages are produced, but during which the logging thread should not be blocked on I/O. 

## Usage:

Create a Writer, give it to a transformer somewhere during startup:

```cpp
auto fileLogBuilder = Herald::createFileWriterBuilder();
auto fileLogWriter = fileLogBuilder->addConfigPair("filename", "stdout").build();

auto simpleBuilder = Herald::createSimpleLogTransformerBuilder();
auto simpleTransformer = simpleBuilder->attachLogWriter(fileLogWriter).build();
```

Later, use `Herald::log()` to create a log message:

```cpp
// ... use it anywhere the transformer is handy
Herald::log(simpleTransformer, LogLevels::Info, "Informational log message");
```

Headers can be added at any time, or chained in the build call:

```cpp
// ... set headers to include for each log
auto fileLogWriter = fileLogBuilder->addConfigPair("filename", "stdout")
	.addHeader("userId", userId)
	.addHeader("machineId", hostName)
	.build();

Herald::log(simpleTransformer, LogLevels::Debug, "Something happening");
// output will include userId=<userId> machineId=<hostName>
// or if a JsonTransformer is used, the headers will be listed as part of the well-formed json, suitable for ingestion by endpoints like ElasticSearch
```
