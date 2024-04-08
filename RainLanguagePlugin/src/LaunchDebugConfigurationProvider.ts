import * as vscode from 'vscode'
export interface RainLaunchDebugConfiguration extends vscode.DebugConfiguration{
    
}


export class RainLaunchDebugConfigurationProvider implements vscode.DebugConfigurationProvider {
    constructor(protected context: vscode.ExtensionContext) { }
    resolveDebugConfiguration(folder: vscode.WorkspaceFolder, configuration: RainLaunchDebugConfiguration, token?: vscode.CancellationToken): vscode.ProviderResult<vscode.DebugConfiguration> {
        vscode.window.showInformationMessage("该功能还在开发中")
        return undefined
    }
}