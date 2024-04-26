
import { LanguageClient, LanguageClientOptions, ServerOptions, StreamInfo } from 'vscode-languageclient/node'
import * as vscode from 'vscode'
import * as fs from 'fs'
import * as net from 'net'

let client: LanguageClient;

export async function RestartServer(context: vscode.ExtensionContext) {
    StopServer()
    await StartServer(context)
}

async function GetProject(): Promise<{ projectPath: string, projectName: string }> {
    if (vscode.workspace.workspaceFolders.length > 0) {
        const projectPath = vscode.workspace.workspaceFolders[0].uri.fsPath
        const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
        const cfgs = JSON.parse(data).configurations
        if (cfgs && cfgs.length > 0) {
            for (let index = 0; index < cfgs.length; index++) {
                const element = cfgs[index];
                if (element.ProjectName) {
                    return {
                        projectPath: projectPath,
                        projectName: (String)(element.ProjectName)
                    }
                }
            }
        }
    }
    return { projectPath: null, projectName: null }
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

    //const serverOptions = GetCPServerOptions(context)
    const serverOptions = GetSocketServerOperation

    const project = await GetProject()
    const clientOptions: LanguageClientOptions = {
        documentSelector: [{
            language: "雨言"
        }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher("**/*.rain")
        },
        initializationOptions: {
            kernelDefinePath: `${context.extension.extensionUri.fsPath}/kernel.rain`,
            projectPath: project.projectPath,
            projectName: project.projectName,
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