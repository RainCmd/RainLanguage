import * as vscode from 'vscode'
import * as fs from 'fs'
import * as path from 'path'
import * as cp from 'child_process'
import * as iconv from 'iconv-lite';
import * as net from 'net'

interface ProcessInfoItem extends vscode.QuickPickItem {
    pid: number;
}

export interface RainAttachDebugConfiguration extends vscode.DebugConfiguration{
    detectorPath: string
    detectorName: string
    projectPath: string
    projectName: string
    socket: net.Socket
}

function IsNUllOrEmpty(value: string): boolean{
    return value == undefined || value == null || value == ""
}

async function GetProjectName(projectPath: string, name: string): Promise<string> {
    if (IsNUllOrEmpty(name)) {
        try {
            const data = await fs.promises.readFile(projectPath + "/.vscode/launch.json", 'utf-8')
            const cfgs = JSON.parse(data).configurations
            cfgs.forEach((element: { type: string; projectName: string }) => {
                if (element.type == "雨言附加到进程") {
                    name = element.projectName
                }
            });
        } catch (error) { }
        if (IsNUllOrEmpty(name)) {
            vscode.window.showInformationMessage("获取当前工程名失败，将使用工作空间文件夹名作为工程名")
            name = path.dirname(projectPath)
        }
    }
    return name
}

async function pickPID(injector: string) {
    return new Promise<number>((resolve, reject) => {
        cp.exec(injector, { encoding: "buffer" }, (error, stdout, stderr) => {
            const arr = iconv.decode(stdout, "cp936").split("\r\n");
            const items: ProcessInfoItem[] = [];
            let cur: ProcessInfoItem = null
            for (let index = 0; index < arr.length; index++) {
                const element = arr[index];
                if (element.startsWith("id")) {
                    if (cur != null) items.push(cur);
                    cur = {
                        pid: Number(element.substring(2)),
                        label: null
                    }
                } else if (element.startsWith("path")) {
                    cur.detail = element.substring(4)
                    cur.label = `[${cur.pid}] ${path.basename(cur.detail)}`
                } else if (element.startsWith("title")) {
                    cur.description = element.substring(5)
                } else if (element.startsWith("name")) {
                    // 这里的name大多时候和cur.label相同，所以就不加了
                    // if (IsNUllOrEmpty(cur.description)) {
                    //     cur.description = element.substring(4);
                    // }
                }
            }
            if (cur != null) items.push(cur);
            if (items.length > 1) {
                vscode.window.showQuickPick(items, {
                    matchOnDescription: true,
                    matchOnDetail: true,
                    placeHolder: "选择要附加的进程"
                }).then((item: ProcessInfoItem | undefined) => {
                    if (item) {
                        resolve(item.pid)
                    } else {
                        reject()
                    }
                })
            } else if (items.length > 0) {
                resolve(items[0].pid)
            } else {
                vscode.window.showErrorMessage("没有可以附加的进程")
                reject();
            }
        }).on("error", error => reject)
    })
}

async function GetDetectorPort(injector: string, pid: number, detectorPath: string, detectorName: string, projectPath: string, projectName: string) {
    return new Promise<number>((resolve, reject) => {
        cp.exec(`${injector} ${pid} ${detectorPath} ${detectorName} ${projectPath} ${projectName}`, { encoding: "buffer" }, (error, stdout, stderr) => {
            const port = Number(iconv.decode(stdout, "cp936").trim())
            if (port > 0) {
                resolve(port)
            } else {
                reject(port)
            }
        }).on('error', error => reject)
    })
}

async function Connect(port:number) {
    return new Promise<net.Socket>((resolve, reject) => {
        const client = net.connect({
            port: port,
            host: "localhost",
            family: 4
        })
        client.on('connect', () => resolve(client));
        client.on('error', reject)
    })
}

export class RainAttachDebugConfigurationProvider implements vscode.DebugConfigurationProvider {
    constructor(protected context: vscode.ExtensionContext) { }
    async resolveDebugConfiguration(folder: vscode.WorkspaceFolder, configuration: RainAttachDebugConfiguration): Promise<vscode.DebugConfiguration> {
        if (vscode.workspace.workspaceFolders.length > 0) {
            const workspaceFolder = vscode.workspace.workspaceFolders[0];
            configuration.projectPath = workspaceFolder.uri.fsPath;
        } else if (folder != undefined && folder != null) {
            configuration.projectPath = folder.uri.fsPath;
        } else return undefined;
        configuration.projectName = await GetProjectName(configuration.projectPath, configuration.projectName)
        configuration.detectorPath = this.context.extensionUri.fsPath;
        configuration.detectorName = "RainDebuggerDetector.dll";
        const injector = this.context.extensionUri.fsPath + "/RainDebuggerInjector.exe"
        const pid = await pickPID(injector)
        const port = await GetDetectorPort(injector, pid, configuration.detectorPath, configuration.detectorName, configuration.projectPath, configuration.projectName)
        configuration.socket = await Connect(port)
        return configuration
    }
}