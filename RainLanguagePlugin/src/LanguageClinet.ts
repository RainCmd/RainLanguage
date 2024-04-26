
import { LanguageClient, LanguageClientOptions, ServerOptions, StreamInfo } from 'vscode-languageclient/node'
import * as vscode from 'vscode'
import * as fs from 'fs'

let client: LanguageClient;

export async function RestartServer(context: vscode.ExtensionContext) {
    StopServer()
    await StartServer(context)
}

async function TryPushProjectName(serverArgs: string[]) {
    if (vscode.workspace.workspaceFolders.length > 0) {
        const projectPath = vscode.workspace.workspaceFolders[0].uri.fsPath
        const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
        const cfgs = JSON.parse(data).configurations
        if (cfgs && cfgs.length > 0 && cfgs[0].ProjectName) {
            serverArgs.push('-projectRoot')
            serverArgs.push(projectPath)
            serverArgs.push('-projectName')
            serverArgs.push((String)(cfgs[0].ProjectName))
            return true
        }
    }
    return false
}

export async function StartServer(context: vscode.ExtensionContext) {
    const binPath = `${context.extension.extensionUri.fsPath}/bin/`
    let serverArgs: string[] = []
    serverArgs.push('-logPath')
    serverArgs.push(`${binPath}server.log`)
    serverArgs.push('-kernelDefinePath')
    serverArgs.push(`${context.extension.extensionUri.fsPath}/kernel.rain`)
    if (!await TryPushProjectName(serverArgs)) {
        //todo 没有打开工作区，看是否有必要传当前打开的文档什么的，单个文档指令参数 -filePath
    }
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
            kernelDefinePath: `${context.extension.extensionUri.fsPath}/kernel.rain`,
            
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