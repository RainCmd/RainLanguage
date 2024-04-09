import { LoggingDebugSession } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import { RainDebugConfiguration } from './DebugConfigurationProvider';
import * as net from 'net'

export class RainDebugSession extends LoggingDebugSession {
	constructor(protected configuration: RainDebugConfiguration) {
		super();
	}
	protected disconnectRequest(response: DebugProtocol.DisconnectResponse, args: DebugProtocol.DisconnectArguments, request?: DebugProtocol.Request): void {
		this.configuration.client.end()
	}
}