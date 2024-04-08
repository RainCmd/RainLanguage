import * as vscode from 'vscode';
import { RainDebugSession } from './DebugSession';

export class InlineDebugAdapterFactory implements vscode.DebugAdapterDescriptorFactory{
    createDebugAdapterDescriptor(session: vscode.DebugSession, executable: vscode.DebugAdapterExecutable): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
        return new vscode.DebugAdapterInlineImplementation(new RainDebugSession());
    }
}