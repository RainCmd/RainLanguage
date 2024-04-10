import { LoggingDebugSession } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import { RainDebugConfiguration } from './DebugConfigurationProvider';

export class RainDebugSession extends LoggingDebugSession {
	constructor(protected configuration: RainDebugConfiguration) {
		super();
	}
	
	protected attachRequest(response: DebugProtocol.AttachResponse, args: DebugProtocol.AttachRequestArguments, request?: DebugProtocol.Request): void {
		console.log("attach")
	}
	protected launchRequest(response: DebugProtocol.LaunchResponse, args: DebugProtocol.LaunchRequestArguments, request?: DebugProtocol.Request): void {
		console.log("lanuch")
	}
}