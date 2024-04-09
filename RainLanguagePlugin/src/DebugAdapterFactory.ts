import * as vscode from 'vscode';
import { RainDebugSession } from './DebugSession';
import { RainDebugConfiguration } from './DebugConfigurationProvider';

export class InlineDebugAdapterFactory implements vscode.DebugAdapterDescriptorFactory{
    createDebugAdapterDescriptor(session: vscode.DebugSession, executable: vscode.DebugAdapterExecutable): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
        return new vscode.DebugAdapterInlineImplementation(new RainDebugSession(session.configuration as RainDebugConfiguration));
    }
}