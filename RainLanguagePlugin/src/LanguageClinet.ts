
import { LanguageClient, LanguageClientOptions, ServerOptions, StreamInfo } from 'vscode-languageclient/node'
import * as vscode from 'vscode'

let client: LanguageClient;

export function RestartServer(context: vscode.ExtensionContext) {
    StopServer()
    StartServer(context)
}

export function StartServer(context: vscode.ExtensionContext) {
    const binPath = `${context.extension.extensionUri.fsPath}/bin/`
    let serverArgs: string[] = []
    serverArgs.push('-logPath')
    serverArgs.push(`${binPath}server.log`)
    serverArgs.push('-kernelDefinePath')
    serverArgs.push(`${context.extension.extensionUri.fsPath}/kernel.rain`)
    const serverOptions: ServerOptions = {
        command: `${binPath}server/RainLanguageServer.exe`,
        args: serverArgs
    }

    const clientOptions: LanguageClientOptions = {
        documentSelector: [{
            language: "雨言"
        }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher("**/*.rain")
        },
        initializationOptions: {
            
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
        client = null
    }
}