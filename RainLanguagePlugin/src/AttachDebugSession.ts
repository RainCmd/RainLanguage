import { RainAttachDebugConfiguration } from "./AttachDebugConfigurationProvider";
import { RainDebugSession } from "./DebugSession";

export class RainAttachDebugSession extends RainDebugSession{
    constructor(protected configuration: RainAttachDebugConfiguration) {
        super();
    }
}