import * as vscode from 'vscode'
export interface RainAttachDebugConfiguration extends vscode.DebugConfiguration{
    detectorPath: string,
    detectorName: string,
    projectPath: string
}

function GetProjectName(name:string): string{
    if (name == undefined || name == null || name == "") {
        
    }
    return name
}

export class RainAttachDebugConfigurationProvider implements vscode.DebugConfigurationProvider {
    constructor(protected context: vscode.ExtensionContext) { }
    resolveDebugConfiguration(folder: vscode.WorkspaceFolder, configuration: RainAttachDebugConfiguration, token?: vscode.CancellationToken): vscode.ProviderResult<vscode.DebugConfiguration> {
        if (vscode.workspace.workspaceFolders.length > 0) {
            const workspaceFolder = vscode.workspace.workspaceFolders[0];
            configuration.projectPath = workspaceFolder.uri.path;
        } else if (folder != undefined && folder != null) {
            configuration.projectPath = folder.uri.path;
        } else return undefined;
        configuration.projectName = GetProjectName(configuration.projectName)
        configuration.detectorPath = this.context.extensionUri.path;
        configuration.detectorName = "RainDebuggerDetector.dll";
        vscode.window.showInformationMessage("AAA")
        return undefined
    }
}