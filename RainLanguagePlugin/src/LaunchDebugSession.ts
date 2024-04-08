import { RainDebugSession } from "./DebugSession";
import { RainLaunchDebugConfiguration } from "./LaunchDebugConfigurationProvider";

export class RainLaunchDebugSession extends RainDebugSession{
    constructor(protected configuration: RainLaunchDebugConfiguration) {
        super();
    }
}