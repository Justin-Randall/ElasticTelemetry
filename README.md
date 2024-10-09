# ElasticTelemetry

An Unreal Engine plugin for sending UE_LOG events to an ElasticSearch server. Also supports custom logs. All in well-formed JSON.

## Getting Started

1. Setup or identify an ElasticSearch endpoint for the plugin to use.

2. On the ElasticSearch server (or through the Kibana front-end), create a "writer" account for the endpoint. There should be no read access for this account. It should be restricted to specific index patterns. 

   ElasticSearch defaults to the use of TLS, so if the installation does not have a publicly valid certificate, be sure to install the cert for Unreal (which uses libcurl, same instructions apply). If a typical browser can reach the ElasticSearch server, then there should not need to be more to do.

## Configuration:
### In the editor: 
`Edit` -> `Project Settings`. Scroll down the the `Elastic Telemetry Settings` section on the left side bar of the project settings window. In the main pane on the right, there is a mapping of "environments" with settings. ElasticTelemetry will provide default values for 3: `Development`, `Staging` and `Production` along with a field for `Active Environment`, which should match the name of one of the settings mapped.

### In the INI: 
Settings are saved in `DefaultEngine.ini` under the section `[/Script/ElasticTelemetry.ElasticTelemetryEnvironmentSettings]`. This is better suited for build systems that are cooking for specific environments. For example, internal developer playtest versions may have `ActiveEnvironment=Development` in the section. If the map is already configured with suitable development environment settings, then simply changing this line prior to the cook as part of the build process can select the appriopriate configuration.

  It should looks something like this:

  ```
[/Script/ElasticTelemetry.ElasticTelemetryEnvironmentSettings]
ActiveEnvironment=Development
Environments=(("Development", (Enabled=True,EndpointURL="https://mygamestrialaccount.us-central1.gcp.cloud.es.io",Username="gamewriter",Password="changeme",EnableVerbose=True,EnableVeryVerbose=True,IncludeCallstacksOnWarning=True)),("Staging", ()),("Production", ()))
  ```

If the ElasticSearch cluster has a Kibana front end, log in, go to `Stack Managegement` and see if the index is populated after running the game with the plugin active and configured with ElasticTelemetry enabled. If it is there, you should be able to create new data views to see the raw logs coming in real-time, as well as apply filters and other queries. The index is also available for creating other dashboards and visualizations or do intersting things enabled by other ElasticSearch plugins.

## Usage in Code

The plugin can include "headers" with each JSON log payload. Some useful defaults already enabled are a SessionID GUID, local computer user and machine name. Other cases may be to also assign mutiplayer session IDs so all players in the same match can be grouped when performing log filtering.

To add a custom header, let's use the SessionID as an example:

```cpp
#include "ETLogger.h"

//...


	// Create a unique session ID guid for this session
	// This is used to group logs together in ElasticSearch
	// and can be used to filter logs by session
	const auto SessionID = FGuid::NewGuid().ToString();
	Herald::addHeader("SessionID", SessionID);

```

This ensures each call to UE_LOG() that is transformed into JSON and sent to ElasticSearch includes a field named "SessionID" and includes a GUID unique to this particular run with the plugin.

There are 4 customizations handy:

```cpp
template <typename KeyType, typename ValueType>
	void addHeader(const KeyType& Key, const ValueType& Value);

template <typename KeyType, typename ValueType>
	void removeHeader(const KeyType& Key);

void log(LogLevels Level, const FString& Message);

template <typename... Args>
	void log(const LogLevels LogLevel, const FString& Message, Args... args);
```

The variadic form of `log()` is likely the most useful for custom instrumentation. `...args` should be in pairs of Key/Value to construct a log entry with custom fields. 

For simple UE_LOG collection and analysis, nothing else needs to be done in code other than ensuring the ElasticTelemetry plugin is included and configured correctly.

## Under the Hood
```
+--------------------------------------+
| Unreal Engine UE_LOG Event Triggered |
+--------------------------------------+
                 |
                 v
   +--------------------------------+
   | ElasticTelemetry FOutputDevice |
   +--------------------------------+
                 |
                 v
+---------------------------------------------+
| JsonLogTransformer: Transforms log to JSON  |
+---------------------------------------------+
                 |
                 v
+--------------------------------------------+
| ILogWriter: Queues JSON payload (Async)    |
|  (Handles queuing for worker thread)       |
+--------------------------------------------+
                 |
                 v
  +-----------------------------------------------+
  | Worker Thread: Monitors queue and sends logs  |
  |      |                                        |
  |      v                                        |
  | +--------------------+   +------------------+ |
  | | Uses libcurl to     |  | Sends JSON to    | |
  | | send JSON payloads  |  | ElasticSearch    | |
  | +--------------------+   +------------------+ |
  +-----------------------------------------------+
                 |
                 v
 +---------------------------------------------------+
 | ElasticSearch Server: Receives and indexes logs   |
 +---------------------------------------------------+
```

Unreal Engine has, for quite some time now, allowed for custom log output devices to be installed. When the engine creates a log, it will call each `FOutputDevice` attached and invoke `Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const class FName& Category)`.

ElasticTelemetry provides a custom `FOutputDevice`. It uses a simple C++ log transfomer and writer library called `Herald` to transform the incoming log text into well-formed JSON, then hand it off to a custom writer that handles the I/O. To avoid blocking the thread creating the UE_LOG event, the transformed JSON payload is queued and returns.

A worker thread grabs the payloads, up to a certain high-watermark to prevent overloading Unreal's version of libcurl, and sends them to the configured ElasticSearch server.

The json serialization is pretty standard C++ (not Unreal's own implementation) built on top of TenCent's very quick rapidjson library. An interface between rapidjson and the logger, called `rapidjsoncpp` handles conversion and variadic invocations. Game-specific types can be enabled for serialization by the JSON transformer as long as a to_json method is in scope. Custom game types can be included in headers, or in custom log messages for later use by other tools that may want to work with the ElasticSearch index for other analytics (design, for example, wondering where players die most often?).

## Differences From the Old, UnrealEngine 4.x Module Version

There are two features missing with the update.

First, real-time updates to the configuration from the editor are not applied. A change to the configuration requires restarting the editor.

Second, the reader/query functionality is not present. There have been some changes over the years to the ElasticSearch API, so there is not (at present) suppoort for a "query" mode to retrieve index data from ElasticSearch for use in the engine (such as for visualizations and design analytics from within the editor).
