#!/bin/bash
cd /home/marquest/AutoDNS-cpp
g++ AutoDNS.cpp JsonParser/JsonAnalyze.cpp JsonParser/DataContainer.cpp JsonParser/SyntaxCheck.cpp JsonParser/Stack.h -lcurl -ltencentcloud-sdk-cpp-core -ltencentcloud-sdk-cpp-dnspod -o AutoDNS-cpp
