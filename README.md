
# Online Allocation of Reusable Services

This repository contains code used to develop intuition about algorithms used to
solve a certain class of online matching problem, described below.


**This is not an officially supported Google product.**

# Problem Statement
Requests arrive online to be served. There are n types of resources available offline, with resource type i having capacity C_i. As soon as a request arrives, it has to be served with one of the acceptable services. A service simultaneously consumes one unit (we could generalize it later) each of a certain subset of resources, and the subset consumed could vary across services. One could impose a certain structure on what subset of resources each service consumes. For example, in the nested model there are n services, with service-i requiring resources {i, i+1, …, n}. In the Tree-1 model the services are the internal nodes of a rooted tree and the leaves could be resources; offering a service requires consuming all resources which are part of the subtree rooted at the service. In another model, say Tree-2, the services are the leaves of a rooted tree and the internal nodes are resources; offering a service requires consuming all the resources in the path from the leaf corresponding to the service to the root. A special case of both Tree-1 and Tree-2 is the nested model.  

The time element in the problem is that each resource involved is a reusable resource, i.e., it gets back to being available after being used for some time. When a service s is being used, all the resources it consumes are being used for that time period d_s.

Let's call this problem “Online Allocation of Reusable Services” (OARS). The nested structure can be called OARS-N, the two tree models can be called OARS-T1 and OARS-T2, and the general arbitrary structure problem can be called OARS-G.

The question we ask is what competitive ratio can one achieve in this setting.

# Compile

Compile using the following command:
    g++ ranking-ort.cpp -std=c++14 -lortools -pthread -O2

# Source code headers

Every file containing source code must include copyright and license
information. This includes any JS/CSS files that you might be serving out to
browsers. (This is to help well-intentioned people avoid accidental copying that
doesn't comply with the license.)

Apache header:

    Copyright 2020 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
