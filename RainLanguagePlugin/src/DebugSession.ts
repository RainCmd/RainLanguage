import { LoggingDebugSession } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import { RainDebugConfiguration } from './DebugConfigurationProvider';

export class RainDebugSession extends LoggingDebugSession {
	constructor(protected configuration: RainDebugConfiguration) {
		super();
	}
	protected async attachRequest(response: DebugProtocol.AttachResponse, args: DebugProtocol.AttachRequestArguments) {
		
	}
}