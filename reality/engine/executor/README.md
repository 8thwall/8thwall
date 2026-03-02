# //reality/engine/executor

This is the main internal entry point for executing 8th Wall's reality engine.
8th Wall Clients and 8th Wall First Party apps should interact with this code
indirectly through code in //reality/app/deprecated.

This directory contains an XREngine which manages state across
calls and serves as the main interface. The XREngine passes all
state explicitly to a RealityRequestExecutor, which is stateless.
