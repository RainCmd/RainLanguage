import * as vscode from 'vscode';
import { RainDebugSession } from './DebugSession';
import { RainLaunchDebugSession } from './LaunchDebugSession';
import { RainAttachDebugSession } from './AttachDebugSession';
import { RainLaunchDebugConfiguration } from './LaunchDebugConfigurationProvider';
import { RainAttachDebugConfiguration } from './AttachDebugConfigurationProvider';

export class InlineDebugAdapterFactory implements vscode.DebugAdapterDescriptorFactory{
    createDebugAdapterDescriptor(session: vscode.DebugSession, executable: vscode.DebugAdapterExecutable): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
        switch (session.type) {
            case "雨言调试运行": {
                return new vscode.DebugAdapterInlineImplementation(new RainLaunchDebugSession(session.configuration as RainLaunchDebugConfiguration));
            }
            case "雨言附加到进程": {
                return new vscode.DebugAdapterInlineImplementation(new RainAttachDebugSession(session.configuration as RainAttachDebugConfiguration));
            }
        }
    }
}