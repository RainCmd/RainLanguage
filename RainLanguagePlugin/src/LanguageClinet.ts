
import { LanguageClient, LanguageClientOptions, ServerOptions, StreamInfo } from 'vscode-languageclient/node'
import * as vscode from 'vscode'
import * as fs from 'fs'
import * as net from 'net'
import { RegistRainLanguagePreviewDoc, extensionDebug } from './extension';

let client: LanguageClient;

export async function RestartServer(context: vscode.ExtensionContext) {
    StopServer()
    await StartServer(context)
}

async function GetProjectName(): Promise<string | null> {
    if (vscode.workspace.workspaceFolders.length > 0) {
        const projectPath = vscode.workspace.workspaceFolders[0].uri.fsPath
        const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
        const cfgs = JSON.parse(data).configurations
        if (cfgs && cfgs.length > 0) {
            for (let index = 0; index < cfgs.length; index++) {
                const element = cfgs[index];
                if (element.ProjectName) {
                    return (String)(element.ProjectName)
                }
            }
        }
    }
    return null
}

async function CollectImports(): Promise<string[]> {
    //todo 收集当前工作区可用的dll路径
    return []
}
async function LoadRely(rely: string): Promise<string> {
    //todo 参数rely是CollectImports传给服务的内容，返回类似kernel.rain的字符串内容
    return ""
}

function GetCPServerOptions(context: vscode.ExtensionContext): ServerOptions {
    const binPath = `${context.extension.extensionUri.fsPath}/bin/`
    let serverArgs: string[] = []
    serverArgs.push('-logPath')
    serverArgs.push(`${binPath}server.log`)
    
    return {
        command: `${binPath}server/RainLanguageServer.exe`,
        args: serverArgs
    }
}

function GetSocketServerOperation() {
    return new Promise<StreamInfo>((resolve, reject) => {
        const client = net.connect({
            port: 14567,
            host: "127.0.0.1"
        })
        client.on('connect', () => {
            resolve({
                reader: client,
                writer: client
            })
        })
        client.on('error', err => {
            reject(err)
        })
    })
}

export async function StartServer(context: vscode.ExtensionContext) {

    const serverOptions = extensionDebug ? GetSocketServerOperation : GetCPServerOptions(context)
    
    const projectName = await GetProjectName()
    const imports = await CollectImports()
    const clientOptions: LanguageClientOptions = {
        documentSelector: [{
            language: "雨言"
        }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher("**/*.rain")
        },
        initializationOptions: {
            kernelDefinePath: `${context.extension.extensionUri.fsPath}/kernel.rain`,
            projectName: projectName,
            imports: imports
        }
    }
    client = new LanguageClient("雨言", "雨言服务客户端", serverOptions, clientOptions)
    client.onNotification("rainlanguage/regPreviewDoc", doc => {
        RegistRainLanguagePreviewDoc(doc.path, doc.content)
    })
    client.onRequest("rainlanguage/loadRely", LoadRely)
    client.start().then(() => {
        console.log("雨言服务客户端：启动")
    }).catch((error) => {
        console.log("雨言服务客户端：启动失败:" + error)
        client = null
    })
}

export async function GetSemanticTokens(document: vscode.TextDocument, token: vscode.CancellationToken) {
    if (client) {
        return await client.sendRequest<any>("rainlanguage/getSemanticTokens", { uri: document.uri.toString() }, token)
    }
    return undefined
}

export function StopServer() {
    if (client) {
        console.log("雨言服务客户端：终止")
        client.stop()
        client = null
    }
}