import { LoggingDebugSession } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';

export class RainDebugSession extends LoggingDebugSession {
	private async attach(): Promise<void>{
		
	}
	protected async attachRequest(response: DebugProtocol.AttachResponse, args: DebugProtocol.AttachRequestArguments) {
		await this.attach()
	}
}