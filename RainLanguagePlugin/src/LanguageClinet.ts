
import { LanguageClient, LanguageClientOptions, ServerOptions, StreamInfo } from 'vscode-languageclient/node'
import * as vscode from 'vscode'
import * as cp from 'child_process'

let client: LanguageClient;

export function StartServer(context: vscode.ExtensionContext) {
    const binPath = `${context.extension.extensionUri.fsPath}/bin/`
    const serverOptions: ServerOptions = {
        command: `${binPath}server/RainLanguageServer.exe`,
        args: [binPath]
    }

    const clientOptions: LanguageClientOptions = {
        documentSelector: [{
            language: "雨言"
        }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher("**/*.rain")
        }
    }
    client = new LanguageClient("雨言", "雨言服务客户端", serverOptions, clientOptions);
    client.start().then(() => {
        console.log("雨言服务客户端：启动")
    }).catch((error) => {
        console.log("雨言服务客户端：启动失败:" + error)
        client = null
    })
}

export function StopServer() {
    if (client) {
        console.log("雨言服务客户端：终止")
        client.stop()
    }
}